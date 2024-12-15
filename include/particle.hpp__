#pragma once

#include <vector>
#include <random>
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include "lemlib/pose.hpp"
#include "lemlib/util.hpp"
#include "pros/distance.hpp"
class Particle {
    public:
        lemlib::Pose pose;
        double weight;
        unsigned int id; // Unique identifier

        // Modify constructor to include id
        Particle(unsigned int id, lemlib::Pose p, double w = 1.0) 
            : id(id), pose(p), weight(w) {}
};

class ParticleFilter {
    public:
        struct SensorMount {
            pros::Distance* sensor;
            lemlib::Pose relativePose; // position relative to robot center
        };

        ParticleFilter(size_t numParticles, const std::vector<SensorMount>& sensors);
        
        // Update particles based on new odometry position
        void update(const lemlib::Pose& newOdomPosition);
        
        // Update weights based on sensor readings
        void measurementUpdate();
        
        // Resample particles based on weights
        void resample();
        
        // Get estimated pose
        lemlib::Pose getEstimatedPose() const;

        // Initialize particles around an estimated starting pose
        void initialize(const lemlib::Pose& startPose, lemlib::Chassis* chassis, 
                       double positionNoise = 2.0, double angleNoise = 0.1);
        
        // Get the particle with highest weight from last measurement update
        const Particle& getBestParticle() const { return bestParticle; }
        
        // Start the particle filter task
        void start();
        // Stop the particle filter task
        void stop();
        
        // Check if particle filter has been initialized
        bool isInitialized() const { return initialized; }
        
        // Check if particle filter task is running
        bool isTaskRunning() const { return isRunning; }
        
        // Calculate expected distance to nearest wall given a pose
        double getExpectedWallDistance(const lemlib::Pose& sensorPose) const;

        // Add a function to print all particles
        void printParticles() const;
        
    private:
        size_t numParticles;
        std::vector<Particle> particles;
        std::vector<SensorMount> sensors;
        
        lemlib::Pose lastOdomPosition;
        
        static constexpr double FIELD_SIZE = 144.0; // 12 feet in inches
        static constexpr double MM_TO_INCH = 0.0393701; // conversion factor
        
        // Helper to convert distance sensor reading to inches
        static double mmToInches(double mm) {
            return mm * MM_TO_INCH;
        }
        
        // Distance sensor constants
        static constexpr double MIN_RANGE_MM = 20.0;
        static constexpr double MAX_RANGE_MM = 2000.0;
        static constexpr double ACCURACY_THRESHOLD_MM = 200.0;
        static constexpr double LOW_RANGE_ACCURACY_MM = 15.0;
        static constexpr double HIGH_RANGE_ACCURACY_PERCENT = 0.05;
        static constexpr double MAX_ANGLE_RAD = 0.26;
        static constexpr double SENSOR_FOV_RAD = lemlib::degToRad(5);
        
        // Use the Vex Distance Sensor product spec to determine sensor variance
        static double getSensorVariance(double distance_mm) {
            if (distance_mm < ACCURACY_THRESHOLD_MM) {
                return LOW_RANGE_ACCURACY_MM * LOW_RANGE_ACCURACY_MM;
            }
            double accuracy = distance_mm * HIGH_RANGE_ACCURACY_PERCENT;
            return accuracy * accuracy;
        }
        
        std::vector<double> cumulativeWeights; // Store cumulative weights

        Particle bestParticle;

        // Helper to generate a noisy pose around a reference pose
        static lemlib::Pose generateNoisyPose(const lemlib::Pose& reference, 
                                            double positionNoise, 
                                            double angleNoise,
                                            std::mt19937& gen);

        // Helper to normalize angle to [-π, π]
        static double normalizeAngle(double angle) {
            while (angle > M_PI) angle -= 2*M_PI;
            while (angle < -M_PI) angle += 2*M_PI;
            return angle;
        }

        pros::Task* filterTask = nullptr;
        bool isRunning = false;
        bool initialized = false;

        // Main update loop for the task
        void updateLoop();

        static constexpr size_t READING_BUFFER_SIZE = 5; // Keep last 5 readings
        std::vector<std::vector<double>> sensorReadingsBuffer; // Circular buffer for each sensor
        std::vector<size_t> bufferIndices; // Current index in each circular buffer

        // Helper to get moving average of readings
        double getSmoothedReading(size_t sensorIndex) const;
        // Add new reading to circular buffer
        void addReading(size_t sensorIndex, double reading);

        lemlib::Chassis* chassis = nullptr;  // Add chassis pointer

        // Add these members
        uint32_t lastUpdateTime = 0;
        
        // Noise parameters
        static constexpr double BASE_POS_NOISE = 0.001;  // Very small base noise when stationary
        static constexpr double BASE_ANGLE_NOISE = 0.0001; // Very small base angle noise
        static constexpr double VELOCITY_NOISE_FACTOR = 0.1; // Scale noise with velocity
};