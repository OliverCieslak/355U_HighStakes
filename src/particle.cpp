#include <random>
#include <algorithm>
#include "particle.hpp"

ParticleFilter::ParticleFilter(size_t num_particles, const std::vector<SensorMount>& sensorMounts) 
    : sensors(sensorMounts), 
      numParticles(num_particles),
      lastOdomPosition(0, 0, 0),
      bestParticle(lemlib::Pose(0, 0, 0), 0) {
    particles.reserve(numParticles);
    
    // Initialize sensor reading buffers
    sensorReadingsBuffer.resize(sensors.size(), std::vector<double>(READING_BUFFER_SIZE, MAX_RANGE_MM));
    bufferIndices.resize(sensors.size(), 0);
}

lemlib::Pose ParticleFilter::generateNoisyPose(const lemlib::Pose& reference, 
                                              double positionNoise, 
                                              double angleNoise,
                                              std::mt19937& gen) {
    std::normal_distribution<> poseDist(0, positionNoise);
    std::normal_distribution<> angleDist(0, angleNoise);
    
    return lemlib::Pose(
        reference.x + poseDist(gen),
        reference.y + poseDist(gen),
        normalizeAngle(reference.theta + angleDist(gen))
    );
}

void ParticleFilter::initialize(const lemlib::Pose& startPose, lemlib::Chassis* chassis, 
                              double positionNoise, double angleNoise) {
    this->chassis = chassis;
    lastOdomPosition = startPose;
    particles.clear();
    
    // Reset all sensor reading buffers to max range
    for (size_t i = 0; i < sensors.size(); i++) {
        std::fill(sensorReadingsBuffer[i].begin(), sensorReadingsBuffer[i].end(), MAX_RANGE_MM);
        bufferIndices[i] = 0;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    particles.reserve(numParticles);
    for (size_t i = 0; i < numParticles; i++) {
        particles.emplace_back(
            generateNoisyPose(startPose, positionNoise, angleNoise, gen),
            1.0/numParticles
        );
    }
    initialized = true;
}

void ParticleFilter::update(const lemlib::Pose& newOdomPosition) {
    // Calculate the change in position
    lemlib::Pose odomDelta = newOdomPosition - lastOdomPosition;
    lastOdomPosition = newOdomPosition;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> noise_xy(0, 0.5); // 0.5 inch std dev
    std::normal_distribution<> noise_theta(0, lemlib::degToRad(10)); // 10 degrees std dev

    for (auto& particle : particles) {
        // Apply odometry motion
        particle.pose = particle.pose + odomDelta;
        
        // Add noise
        particle.pose.x += noise_xy(gen);
        particle.pose.y += noise_xy(gen);
        particle.pose.theta = normalizeAngle(particle.pose.theta + noise_theta(gen));
    }
}

double ParticleFilter::getExpectedWallDistance(const lemlib::Pose& sensorPose) const {
    // Field boundaries (in inches)
    const double left = -FIELD_SIZE/2;
    const double right = FIELD_SIZE/2;
    const double bottom = -FIELD_SIZE/2;
    const double top = FIELD_SIZE/2;
    
    // Store walls as line segments: {x1,y1,x2,y2}
    const std::array<std::array<double,4>,4> walls = {{
        {left, bottom, left, top},    // Left wall
        {right, bottom, right, top},  // Right wall
        {left, bottom, right, bottom},// Bottom wall
        {left, top, right, top}      // Top wall
    }};
    
    // Sensor's ray: starting point (px,py) and direction vector (dx,dy)
    double px = sensorPose.x;
    double py = sensorPose.y;
    // Update direction vector calculation for clockwise rotation
    double dx = std::cos(sensorPose.theta);  // No change needed
    double dy = -std::sin(sensorPose.theta); // Negative for clockwise convention
    
    double minDist = MAX_RANGE_MM * MM_TO_INCH;
    
    for (const auto& wall : walls) {
        // Wall line segment points
        double x1 = wall[0], y1 = wall[1];
        double x2 = wall[2], y2 = wall[3];
        
        // Calculate intersection using parametric equations
        // Ray: p + t*d, where p is start point, d is direction
        // Wall segment: p1 + s*(p2-p1), where p1 and p2 are endpoints
        double denominator = (x2-x1)*dy - (y2-y1)*dx;
        
        // Skip if lines are parallel
        if (std::abs(denominator) < 1e-6) continue;
        
        // Calculate intersection parameters
        double s = ((px-x1)*dy - (py-y1)*dx) / denominator;
        double t = ((x2-x1)*(py-y1) - (y2-y1)*(px-x1)) / denominator;
        
        // Check if intersection lies within wall segment and in front of sensor
        if (s >= 0 && s <= 1 && t >= 0) {
            double dist = t; // Since direction vector is normalized, t is the distance
            minDist = std::min(minDist, dist);
        }
    }
    
    return minDist;
}

void ParticleFilter::addReading(size_t sensorIndex, double reading) {
    sensorReadingsBuffer[sensorIndex][bufferIndices[sensorIndex]] = reading;
    bufferIndices[sensorIndex] = (bufferIndices[sensorIndex] + 1) % READING_BUFFER_SIZE;
}

double ParticleFilter::getSmoothedReading(size_t sensorIndex) const {
    const auto& readings = sensorReadingsBuffer[sensorIndex];
    double sum = 0;
    
    // Simple average of all readings in buffer
    for (double reading : readings) {
        sum += reading;
    }
    
    return sum / READING_BUFFER_SIZE;
}

void ParticleFilter::measurementUpdate() {
    // Collect and smooth all sensor readings first
    std::vector<double> smoothedReadings;
    smoothedReadings.reserve(sensors.size());
    
    for (size_t i = 0; i < sensors.size(); i++) {
        // Add new reading to buffer
        //addReading(i, sensors[i].sensor->get());
        // Get smoothed reading
        //smoothedReadings.push_back(getSmoothedReading(i));
        int currentReading = sensors[i].sensor->get();
        smoothedReadings.push_back(currentReading);
    }

    double maxWeight = -1;
    double sumWeights = 0;
    cumulativeWeights.clear();
    cumulativeWeights.reserve(particles.size());
    
    for (auto& particle : particles) {
        double probability = 1.0;
        
        for (size_t i = 0; i < sensors.size(); i++) {
            double actualDistance_mm = smoothedReadings[i];
            
            if (actualDistance_mm <= 20) {
                continue;
            }

            // Calculate sensor's world position with clockwise-positive angles
            // Note: For clockwise rotation, we flip the sin terms
            double rotatedX = sensors[i].relativePose.x * std::cos(particle.pose.theta) + 
                            sensors[i].relativePose.y * std::sin(particle.pose.theta);
            double rotatedY = -sensors[i].relativePose.x * std::sin(particle.pose.theta) + 
                            sensors[i].relativePose.y * std::cos(particle.pose.theta);
            
            // 2. Create sensor's world pose by adding rotated offset to particle position
            lemlib::Pose sensorWorldPose(
                particle.pose.x + rotatedX,
                particle.pose.y + rotatedY,
                normalizeAngle(particle.pose.theta + sensors[i].relativePose.theta)
            );

            // Debug print
            printf("Robot at (%.1f, %.1f, %.1f°), Sensor offset (%.1f, %.1f, %.1f°) -> Sensor at (%.1f, %.1f, %.1f°)\n",
                   particle.pose.x, particle.pose.y, lemlib::radToDeg(particle.pose.theta),
                   sensors[i].relativePose.x, sensors[i].relativePose.y, lemlib::radToDeg(sensors[i].relativePose.theta),
                   sensorWorldPose.x, sensorWorldPose.y, lemlib::radToDeg(sensorWorldPose.theta));

            // Get expected distance to nearest wall in inches
            double expectedDistance_in = getExpectedWallDistance(sensorWorldPose);
            double expectedDistance_mm = expectedDistance_in / MM_TO_INCH;
            
            printf("\tSensor %d: Actual: %.2f, Expected: %.2f from pose (%d, %d, %d)\n", i, actualDistance_mm, expectedDistance_mm, (int)sensorWorldPose.x,(int)sensorWorldPose.y,int(lemlib::radToDeg(sensorWorldPose.theta)));

            // Handle different measurement cases
            if (expectedDistance_mm >= MAX_RANGE_MM) {
                if (actualDistance_mm < MAX_RANGE_MM) {
                    probability *= actualDistance_mm / MAX_RANGE_MM;
                }
                continue;
            }
            
            if (actualDistance_mm >= MAX_RANGE_MM) {
                probability *= expectedDistance_mm / MAX_RANGE_MM;
                continue;
            }
            
            // Calculate measurement probability using appropriate variance
            double variance = getSensorVariance(expectedDistance_mm);
            double diff = actualDistance_mm - expectedDistance_mm;
            double scaledDiff = diff / std::sqrt(variance);  // Normalize by standard deviation
            probability *= std::exp(-scaledDiff * scaledDiff);
        }
        
        particle.weight *= probability;
        sumWeights += particle.weight;
        cumulativeWeights.push_back(sumWeights);
        
        // Track best particle
        if (particle.weight > maxWeight) {
            maxWeight = particle.weight;
            bestParticle = particle;
        }
    }

    // Normalize weights and cumulative weights
    if (sumWeights > 0) {
        for (auto& p : particles) {
            p.weight /= sumWeights;
        }
        for (auto& c : cumulativeWeights) {
            c /= sumWeights;
        }
    }
}

void ParticleFilter::resample() {
    // Sort particles by weight in descending order
    std::sort(particles.begin(), particles.end(), 
              [](const Particle& a, const Particle& b) { return a.weight > b.weight; });
    
    // Create new vector with top 95% of particles
    size_t numParticlesToKeep = particles.size() * 0.95;
    std::vector<Particle> newParticles(particles.begin(), particles.begin() + numParticlesToKeep);
    
    // Generate random particles around best particle
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Add new random particles
    size_t numNewParticles = numParticles - numParticlesToKeep;
    for (size_t i = 0; i < numNewParticles; i++) {
        newParticles.emplace_back(
            generateNoisyPose(bestParticle.pose, 4.0, lemlib::degToRad(15), gen),
            1.0/numParticles
        );
    }
    
    // Swap the new particles with the old ones
    particles.swap(newParticles);
}

lemlib::Pose ParticleFilter::getEstimatedPose() const {
    if (particles.empty() || bestParticle.weight <= 0) {
        return lemlib::Pose(0, 0, 0);
    }

    double x_sum = 0;
    double y_sum = 0;
    double sin_sum = 0;
    double cos_sum = 0;
    double weight_sum = 0;

    for (const auto& p : particles) {
        x_sum += p.pose.x * p.weight;
        y_sum += p.pose.y * p.weight;
        sin_sum += std::sin(p.pose.theta) * p.weight;
        cos_sum += std::cos(p.pose.theta) * p.weight;
        weight_sum += p.weight;
    }

    if (weight_sum <= 0) {
        return bestParticle.pose;
    }

    // Calculate weighted average pose
    x_sum /= weight_sum;
    y_sum /= weight_sum;
    sin_sum /= weight_sum;
    cos_sum /= weight_sum;

    // Calculate average angle using atan2 with clockwise-positive convention
    double theta = normalizeAngle(-std::atan2(sin_sum, cos_sum));
    
    // Normalize to -180 to 180 degrees
    while (theta > M_PI) theta -= 2*M_PI;
    while (theta < -M_PI) theta += 2*M_PI;

    return lemlib::Pose(x_sum, y_sum, theta);
}

void ParticleFilter::start() {
    if (filterTask == nullptr && chassis != nullptr) {
        isRunning = true;
        filterTask = new pros::Task([this] {
            while (isRunning) {
                lemlib::update();
                this->update(chassis->getPose());
                this->measurementUpdate();
                this->resample();
                pros::delay(10);
            }
        });
    }
}

void ParticleFilter::stop() {
    isRunning = false;
    if (filterTask != nullptr) {
        delete filterTask;
        filterTask = nullptr;
    }
}