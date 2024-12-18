#include <random>
#include <algorithm>
#include "particle.hpp"

// Add a static counter for unique IDs
static unsigned int next_particle_id = 0;

ParticleFilter::ParticleFilter(size_t num_particles, const std::vector<SensorMount>& sensorMounts) 
    : sensors(sensorMounts), 
      numParticles(num_particles),
      lastOdomPosition(0, 0, 0),
      bestParticle(next_particle_id++, lemlib::Pose(0, 0, 0), 0.0) {
    particles.reserve(numParticles);
    
    // Initialize sensor reading buffers
    sensorReadingsBuffer.resize(sensors.size(), std::vector<double>(READING_BUFFER_SIZE, MAX_RANGE_MM));
    bufferIndices.resize(sensors.size(), 0);
}

lemlib::Pose ParticleFilter::generateNoisyPose(const lemlib::Pose& reference, 
                                              double positionNoise, 
                                              double angleNoise,
                                              std::mt19937& gen) {
    std::normal_distribution<> poseDist(0, std::sqrt(positionNoise));
    std::normal_distribution<> angleDist(0, angleNoise);

    double randomAngleDelta = angleDist(gen);
    
    return lemlib::Pose(
        reference.x + poseDist(gen),
        reference.y + poseDist(gen),
        normalizeAngle(reference.theta + randomAngleDelta)
    );
}

void ParticleFilter::initialize(const lemlib::Pose& startPose, lemlib::Chassis* chassis, 
                              double positionNoise, double angleNoise) {
    this->chassis = chassis;
    lastOdomPosition = startPose;
    particles.clear();
    bestParticle = Particle(-1, startPose, -1);
    
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
            next_particle_id++,
            generateNoisyPose(startPose, positionNoise, angleNoise, gen),
            1.0 / numParticles  // Maintain uniform initial weights
        );
    }
    initialized = true;

    lastUpdateTime = pros::millis();
}

void ParticleFilter::update(const lemlib::Pose& newOdomPosition) {
    lemlib::Pose odomDelta = newOdomPosition - lastOdomPosition;
    
    // Calculate time delta
    uint32_t currentTime = pros::millis();
    double dt = (currentTime - lastUpdateTime);
    if (dt <= 0) dt = 0.01;
    
    // Only update if there's significant motion
    double linearDelta = std::hypot(odomDelta.x, odomDelta.y);
    double angularDelta = std::abs(odomDelta.theta);
    if (linearDelta < 0.1 && angularDelta < lemlib::degToRad(1)) {
        lastUpdateTime = currentTime;
        return;
    }
    
    // Calculate velocities but with much lower noise
    double posNoise = std::sqrt(0.0001 + 0.001 * linearDelta);
    double angleNoise = 0.001 * lemlib::degToRad(1);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> noise_xy(0, posNoise);
    std::normal_distribution<> noise_theta(0, angleNoise);

    // Constrain particles to field boundaries with margin
    const float MARGIN = 6.0f;  // Use float instead of double
    const float MIN_X = static_cast<float>(-FIELD_SIZE/2 + MARGIN);
    const float MAX_X = static_cast<float>(FIELD_SIZE/2 - MARGIN);
    const float MIN_Y = static_cast<float>(-FIELD_SIZE/2 + MARGIN);
    const float MAX_Y = static_cast<float>(FIELD_SIZE/2 - MARGIN);

    for (auto& particle : particles) {
        particle.pose = particle.pose + odomDelta;
        
        // Add minimal noise
        particle.pose.x += noise_xy(gen);
        particle.pose.y += noise_xy(gen);
        particle.pose.theta = normalizeAngle(particle.pose.theta + noise_theta(gen));
        
        // Constrain to field
        particle.pose.x = std::max(MIN_X, std::min(particle.pose.x, MAX_X));
        particle.pose.y = std::max(MIN_Y, std::min(particle.pose.y, MAX_Y));
    }
    
    lastOdomPosition = newOdomPosition;
    lastUpdateTime = currentTime;
}

double ParticleFilter::getExpectedWallDistance(const lemlib::Pose& sensorPose) const {
    const double left = -FIELD_SIZE/2;   // Field boundaries in inches
    const double right = FIELD_SIZE/2;
    const double bottom = -FIELD_SIZE/2;
    const double top = FIELD_SIZE/2;
    
    const std::array<std::array<double,4>,4> walls = {{
        {left, bottom, left, top},    // Left wall
        {right, bottom, right, top},  // Right wall
        {left, bottom, right, bottom},// Bottom wall
        {left, top, right, top}      // Top wall
    }};
    
    double px = sensorPose.x;  // Position is already in inches
    double py = sensorPose.y;
    
    // Use sensor angle directly (VEX uses counter-clockwise positive)
    double dx = std::cos(sensorPose.theta);
    double dy = std::sin(sensorPose.theta);
    
    double minDist = MAX_RANGE_MM;  // Start with max range in mm
    
    for (const auto& wall : walls) {
        // Wall line segment points (all in inches)
        double x1 = wall[0], y1 = wall[1];
        double x2 = wall[2], y2 = wall[3];
        
        // Calculate intersection using parametric equations
        double denom = (dx * (y2 - y1)) - (dy * (x2 - x1));
        if (std::abs(denom) < 1e-6) continue;  // Lines are parallel
        
        double ua = ((x2 - x1) * (py - y1) - (y2 - y1) * (px - x1)) / denom;
        double ub = (dx * (py - y1) - dy * (px - x1)) / denom;
        
        // Check if intersection is valid (on wall segment and in front of sensor)
        if (ua >= 0 && ub >= 0 && ub <= 1) {
            double ix = px + ua * dx;  // Intersection x (in inches)
            double iy = py + ua * dy;  // Intersection y (in inches)
            
            // Calculate actual distance in inches then convert to mm
            double dist = std::hypot(ix - px, iy - py) * 25.4;  // Convert inches to mm
            if (dist < minDist) {
                minDist = dist;
            }
        }
    }
    
    return minDist;  // Return distance in mm directly
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
    std::vector<double> smoothedReadings;
    smoothedReadings.reserve(sensors.size());
    
    for (size_t i = 0; i < sensors.size(); i++) {
        int currentReading = sensors[i].sensor->get();
        // Ignore invalid readings
        if (currentReading < MIN_RANGE_MM || currentReading > MAX_RANGE_MM) {
            continue;
        }
        smoothedReadings.push_back(currentReading);
    }

    // Skip update if no valid readings
    if (smoothedReadings.empty()) return;

    double maxWeight = -1;
    double sumWeights = 0;
    cumulativeWeights.clear();
    cumulativeWeights.reserve(particles.size());
    
    // Increase minimum probability threshold significantly
    const double MIN_PROBABILITY = 0.1;  // Changed from 0.01
    
    for (auto& particle : particles) {
        particle.weight = 1.0;
        int validMeasurements = 0;
        
        // Reduce odometry penalty
        double odomDist = std::hypot(particle.pose.x - lastOdomPosition.x,
                                   particle.pose.y - lastOdomPosition.y);
        double odomAngleDiff = std::abs(normalizeAngle(particle.pose.theta - lastOdomPosition.theta));
        
        // Make odometry weight more lenient
        double odomWeight = std::exp(-odomDist * 0.05) * std::exp(-odomAngleDiff);
        particle.weight *= odomWeight;

        for (size_t i = 0; i < sensors.size(); i++) {
            double actualDistance_mm = smoothedReadings[i];
            if (actualDistance_mm <= MIN_RANGE_MM) continue;
            if (actualDistance_mm > MAX_RANGE_MM) continue;

            // Calculate sensor's world position
            double sensorTheta = sensors[i].relativePose.theta;
            double rotatedX = sensors[i].relativePose.x * std::cos(sensorTheta) - 
                            sensors[i].relativePose.y * std::sin(sensorTheta);
            double rotatedY = sensors[i].relativePose.x * std::sin(sensorTheta) + 
                            sensors[i].relativePose.y * std::cos(sensorTheta);
            
            lemlib::Pose sensorWorldPose(
                particle.pose.x + rotatedX,
                particle.pose.y + rotatedY,
                normalizeAngle(particle.pose.theta + sensorTheta)
            );

            double expectedDistance_mm = getExpectedWallDistance(sensorWorldPose);
            double diff = std::abs(actualDistance_mm - expectedDistance_mm);
            
            // Debugging: Log expected wall distance and particle angle
            /*
            std::cout << "Particle Pose: " << particle.id << " (" << particle.pose.x << ", " << particle.pose.y << ", " << (int)(lemlib::radToDeg(particle.pose.theta)) << ")\n";
            std::cout << "Sensor World Pose: (" << sensorWorldPose.x << ", " << sensorWorldPose.y << ", " << (int)(lemlib::radToDeg(sensorWorldPose.theta)) << ")\n";            std::cout << "Expected Distance: " << expectedDistance_mm << " mm\n";
            std::cout << "Actual Distance: " << actualDistance_mm << " mm\n";
            std::cout << "Difference: " << diff << " mm\n";
            */
            // Make variance extremely strict for stationary operation
            double variance = std::min(50.0, getSensorVariance(actualDistance_mm) * 0.25);
            
            // Less aggressive probability calculation
            double probability = std::max(
                MIN_PROBABILITY,
                std::exp(-diff * diff / (2 * variance))  // Remove extra exponential penalty
            );
            
            particle.weight *= probability;
            validMeasurements++;
        }
        
        // Less aggressive geometric mean
        if (validMeasurements > 0) {
            particle.weight = std::pow(particle.weight, 1.0 / validMeasurements);  // Changed from 2.0
        }
        
        sumWeights += particle.weight;
        cumulativeWeights.push_back(sumWeights);
    }

    // Normalize weights before selecting best particle
    if (sumWeights > 0) {
        maxWeight = -1;
        for (auto& p : particles) {
            p.weight /= sumWeights;
            if (p.weight > maxWeight) {
                maxWeight = p.weight;
                bestParticle = p;
            }
        }
        for (auto& c : cumulativeWeights) {
            c /= sumWeights;
        }
    }
}

void ParticleFilter::resample() {
    if (particles.empty()) return;

    // Sort particles by weight in descending order
    std::sort(particles.begin(), particles.end(),
             [](const Particle& a, const Particle& b) { return a.weight > b.weight; });
    
    // Much higher proportion of kept particles when stationary
    size_t particlesToKeep = std::min(size_t(numParticles * 0.9), particles.size());
    double weightThreshold = particles[0].weight * 0.6;
    
    // Create new vector with kept particles
    std::vector<Particle> newParticles;
    newParticles.reserve(numParticles);
    
    // Copy high-weight particles
    for (size_t i = 0; i < particlesToKeep; i++) {
        newParticles.push_back(particles[i]);
    }
    
    try {
        std::random_device rd;
        std::mt19937 gen(rd());
        
        size_t remainingSlots = numParticles - newParticles.size();
        size_t numNewFarParticles = std::max(size_t(1), size_t(remainingSlots * 0.01)); // Greatly reduce far particles
        size_t numNewCloseParticles = remainingSlots - numNewFarParticles;
        
        // Add close particles with minimal noise
        for (size_t i = 0; i < numNewCloseParticles; i++) {
            newParticles.emplace_back(
                next_particle_id++,
                generateNoisyPose(bestParticle.pose, 0.5, lemlib::degToRad(1), gen),
                0.0
            );
        }

        // Add far particles with more limited spread
        for (size_t i = 0; i < numNewFarParticles; i++) {
            newParticles.emplace_back(
                next_particle_id++,
                generateNoisyPose(bestParticle.pose, 3.0, lemlib::degToRad(90), gen),
                0.0
            );
        }
        
        particles = std::move(newParticles);
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed during resampling, keeping existing particles" << std::endl;
    }
}

lemlib::Pose ParticleFilter::getEstimatedPose() {
    bpMutex.take(100);  // Take mutex to access best particle
    if (particles.empty() || bestParticle.weight <= 0) {
        bpMutex.give();
        return lastOdomPosition;
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
    double theta = normalizeAngle(std::atan2(sin_sum, cos_sum));
    bpMutex.give();
    return lemlib::Pose(x_sum, y_sum, theta);
}

void ParticleFilter::start() {
    if (filterTask == nullptr && chassis != nullptr) {
        isRunning = true;
        filterTask = new pros::Task([this] {
            while (isRunning) {
                if(bpMutex.take(5)) {
                    lemlib::update();
                    this->update(chassis->getPose());
                    this->measurementUpdate();
                    this->resample();
                    bpMutex.give();
                }
                pros::delay(10);
            }
        });
    }
}

void ParticleFilter::stop() {
    isRunning = false;
    if (filterTask != nullptr) {
        // Wait for task to complete before deleting
        filterTask->remove();
        delete filterTask;
        filterTask = nullptr;
    }
}

// Add destructor
ParticleFilter::~ParticleFilter() {
    stop();  // Ensure task is stopped and cleaned up
}

void ParticleFilter::printParticles() const {
    std::cout << "Best Particle Pose: (" 
              << bestParticle.pose.x << ", " 
              << bestParticle.pose.y << ", " 
              << lemlib::radToDeg(bestParticle.pose.theta) << " degrees), "
              << "Weight: " << bestParticle.weight << std::endl;

    for (const auto& particle : particles) {
        std::cout << "Particle ID: " << particle.id 
                  << " Pose: (" 
                  << particle.pose.x << ", " 
                  << particle.pose.y << ", " 
                  << lemlib::radToDeg(particle.pose.theta) << " degrees), "
                  << "Weight: " << particle.weight << std::endl;
    }
}