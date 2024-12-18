#include "main.h"
#include "robodash/api.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "autons.h"
#include "particle.hpp"  // Add this include

rd::Selector selector({
                        {SIMPLE_ALLIANCE, &simpleAllianceStake},
                        {SINGLE_MOGO, &simpleSingleMogo},
                        {QUAL_GOAL_RUSH, &qualsGoalRushAutonTweaked},
                        {GOAL_FILL, &goalFill},
                        {TWO_GOAL_SIDE_FILL, &twoGoalSideFill},
                        {AUTON_ONE, &simpleAuton},
                        {SAFE_SKILLS, &safeSkills},
                        {SCORING_SKILLS, &scoringSkills},
                        {LINEAR_PID_MOVEMENT, &linearPidMovementTest},
                        {TURN_PID_MOVEMENT, &turnPidMovementTest}, 
                       });
// linear_pid_movement
rd::Console console;

// controller
pros::Controller masterController(pros::E_CONTROLLER_MASTER);

// motor groups
pros::MotorGroup leftMotors({17, -3, 18},
                            pros::MotorGearset::blue);
pros::MotorGroup rightMotors({-1, 6, -9}, pros::MotorGearset::blue);
pros::Optical firstRingColorSensor(21);
pros::Imu imu(7);

int INTAKE_MOTOR_PORT = 4;
int DUMP_TRUCK_MOTOR_PORT = 10;
pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);
pros::adi::DigitalOut liftPnuematic('G');
pros::adi::DigitalOut backClampPnuematic('H');
pros::adi::DigitalOut doinker('A');

lemlib::Drivetrain drivetrain(&leftMotors,  // left motor group
                              &rightMotors, // right motor group
                              11.0,
                              lemlib::Omniwheel::NEW_275, // using new 2.75" omnis
                              600,                        // drivetrain rpm
                              8                           // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
/*
lemlib::ControllerSettings linearController(93,   // proportional gain (kP)
                                            0,   // integral gain (kI)
                                            750,   // derivative gain (kD)
                                            3,   // anti windup
                                            1,   // small error range, in inches
                                            60,  // small error range timeout, in milliseconds
                                            3,   // large error range, in inches
                                            250, // large error range timeout, in milliseconds
                                            70   // maximum acceleration (slew)
);
*/
lemlib::ControllerSettings linearController(20,   // proportional gain (kP)
                                            0,   // integral gain (kI)
                                            100,   // derivative gain (kD)
                                            3,   // anti windup
                                            1,   // small error range, in inches
                                            60,  // small error range timeout, in milliseconds
                                            3,   // large error range, in inches
                                            250, // large error range timeout, in milliseconds
                                            127   // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(1.8, // proportional gain (kP)
                                             0,   // integral gain (kI)
                                             10,  // derivative gain (kD)
                                             3,   // anti windup
                                             1,  // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             3,   // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0    // maximum acceleration (slew)
);

lemlib::TrackingWheel left_vertical_tracking_wheel(&leftMotors, lemlib::Omniwheel::NEW_275, -6, 600);
lemlib::TrackingWheel right_vertical_tracking_wheel(&rightMotors, lemlib::Omniwheel::NEW_275, 6, 600);

// odometry settings
lemlib::OdomSensors sensors(&left_vertical_tracking_wheel, // vertical tracking wheel 1,
                            &right_vertical_tracking_wheel, // vertical tracking wheel 2,
                            nullptr, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu     // inertial sensor
);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3,    // joystick deadband out of 127
                                     10,   // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3,    // joystick deadband out of 127
                                  10,   // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);
// Enumeration for the color states

// Enumeration for the color states
enum ColorState
{
   RED,
   BLUE,
   NO_COLOR
};

// Global variable to keep track of the current color state
ColorState currentColorState = NO_COLOR;

const int RED_SIDE_AUTON = 1;
const int BLUE_SIDE_AUTON = -1;
int autonSideDetected = RED_SIDE_AUTON;

pros::Distance backClampDistanceSensor(12);
pros::Distance frontDistanceSensor(13);
pros::Distance rightDistanceSensor(14);
pros::Distance leftDistanceSensor(15);
std::vector<ParticleFilter::SensorMount> sensorMounts = {
    {&frontDistanceSensor, lemlib::Pose(4.5, 4.5, lemlib::degToRad(0))},
    {&leftDistanceSensor, lemlib::Pose(4.5, 4.5, lemlib::degToRad(-90))},
    {&rightDistanceSensor, lemlib::Pose(-2.0, -4.5, lemlib::degToRad(90))},
    {&backClampDistanceSensor, lemlib::Pose(-7.5, 0.0, lemlib::degToRad(180))}
};
ParticleFilter particleFilter(100, sensorMounts);

// Function to cycle through the color states based on the current hue
void getAutonColorState()
{
   firstRingColorSensor.set_led_pwm(25);
   pros::delay(100); // Wait for the sensor to stabilize
   int colorDistance = (int)firstRingColorSensor.get_proximity();
   printf("Color Distance: %d\n", colorDistance);
   if (colorDistance < 100)
   {
      return;
   }

   int hue = (int)firstRingColorSensor.get_hue(); // Get the current hue from the sensor
   printf("Color Hue: %d\n", hue);

   // Check the hue range and update the state
   if (hue >= 0 && hue <= 20)
   {
      autonSideDetected = RED_SIDE_AUTON;
   }
   else if (hue >= 180 && hue <= 240)
   {
      autonSideDetected = BLUE_SIDE_AUTON;
   }
    else if (hue >= 300 && hue <= 340)
   {
      autonSideDetected = RED_SIDE_AUTON;
   }
}

// create the chassis
lemlib::Chassis chassis(drivetrain, linearController, angularController, sensors, &throttleCurve, &steerCurve);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
   printf("Initializing robot...\n");
   console.println("Initializing robot...");
   chassis.setPose(0, 0, 0);
   chassis.cancelAllMotions();
   chassis.calibrate();
   chassis.setPose(0, 0, 0);

   dumpTruckMotor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE); 
   dumpTruckMotor.tare_position();

   console.println("Initializing robot...Done!");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
   printf("Disabled robot...\n");
}

char auton_name_in_robodash_file[256];

void displayParticleFilterInfo()
{
   char buffer[100];
   if(particleFilter.isInitialized())
   {
      lemlib::Pose estimatedPose = particleFilter.getEstimatedPose();
      sprintf(buffer, "P: %d, %d, %d\n", (int)estimatedPose.x, (int)estimatedPose.y, (int)lemlib::radToDeg(estimatedPose.theta));
      printf("%s\n", buffer);
      masterController.print(0, 0, buffer);
      pros::delay(50);
   }
   char side[5];
   if(autonSideDetected == RED_SIDE_AUTON)
   {
      sprintf(side, "Red");
   }
   else
   {
      sprintf(side, "Blue");
   }
   sprintf(buffer, "%s %s\n", side, auton_name_in_robodash_file);
   masterController.print(1, 0, buffer);
   pros::delay(50);
   sprintf(buffer, "Battery: %.2f percent\n", pros::battery::get_capacity());
   masterController.print(2, 0, buffer);
}

extern pros::Task *trackingTask;

void stopLemLibTrackingTask()
{
   if (trackingTask != nullptr)
   {
      trackingTask->remove();
      delete trackingTask;
      trackingTask = nullptr;
   }
}
void startParticleFilter()
{
   stopLemLibTrackingTask();
   //if(!particleFilter.isInitialized())
   if(true)
   {
      getAutonColorState();
      // Figure out which auton was selected by reading it from the file since
      // RoboDash doesn't have a way to retrieve the selected auton
      const char *file_name = "/usd/rd_auton.txt"; // RoboDash file
      FILE *save_file;
      save_file = fopen(file_name, "r");
      if (!save_file) return;

      // Read contents
      char line[256];
      char saved_selector[256];

      // Should only really be 1 of these
      while (fgets(line, 256, save_file)) {
         sscanf(line, "%[^:]: %[^\n\0]", saved_selector, auton_name_in_robodash_file);
      }

      fclose(save_file);
      printf("Selected auton: %s\n", auton_name_in_robodash_file);

      // Initialize particle filter with starting pose based on auton
      lemlib::Pose initialPose(autonSideDetected * 48, -48, autonSideDetected * lemlib::degToRad(autonSideDetected * 90)); // Modify based on auton selection
      if (strcmp(auton_name_in_robodash_file, SIMPLE_ALLIANCE) == 0)
      {
         initialPose = lemlib::Pose(autonSideDetected * 48, 24, autonSideDetected *lemlib::degToRad(autonSideDetected * 90)); // Modify based on auton selection
      }
      chassis.setPose(initialPose);
      particleFilter.initialize(initialPose, &chassis, 2.0, lemlib::degToRad(10));
      particleFilter.start();
   }
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
   printf("Initializing competition...\n");

   stopLemLibTrackingTask();
   char buffer[100];

   lemlib::Pose initialPose = lemlib::Pose(-48, -48, lemlib::degToRad(90));
   chassis.setPose(initialPose);
   particleFilter.initialize(initialPose, &chassis, 1.0, lemlib::degToRad(5));
   //printf("---------------------\n");
   //particleFilter.printParticles();
   //printf("---------------------\n");
   lemlib::Pose estimatedPose = particleFilter.getEstimatedPose();
   sprintf(buffer, "Initial EstimatedPose: %d, %d, %d", (int)estimatedPose.x, (int)estimatedPose.y, (int)lemlib::radToDeg(estimatedPose.theta));
   printf("%s\n", buffer);
   particleFilter.update(initialPose);
   particleFilter.measurementUpdate();
   particleFilter.resample();
   //printf("---------------------\n");
   //particleFilter.printParticles();
   //printf("---------------------\n");
   estimatedPose = particleFilter.getEstimatedPose();
   double rdist = 0.0393701 * rightDistanceSensor.get_distance();
   double bdist = 0.0393701 * backClampDistanceSensor.get_distance();
   double fdist = 0.0393701 * frontDistanceSensor.get_distance();
   double ldist = 0.0393701 * leftDistanceSensor.get_distance();
   sprintf(buffer, "P: %d, %d, %d", (int)estimatedPose.x, (int)estimatedPose.y, (int)lemlib::radToDeg(estimatedPose.theta));
   printf("%s\n", buffer);
   sprintf(buffer, "R: %.2f B: %.2f L: %.2f F: %.2f", rdist, bdist, ldist, fdist);
   printf("%s\n", buffer);
   Particle bestParticle = particleFilter.getBestParticle();
   sprintf(buffer, "Best Particle: %d (%d, %d, %d) %e",bestParticle.id, (int)bestParticle.pose.x, (int)bestParticle.pose.y, (int)lemlib::radToDeg(bestParticle.pose.theta), (bestParticle.weight));
   printf("%s\n", buffer);

   particleFilter.start();
   pros::delay(1000); // Wait for the particle filter to stabilize
   //particleFilter.stop();

   printf("1 second later....\n");
   estimatedPose = particleFilter.getEstimatedPose();
   rdist = 0.0393701 * rightDistanceSensor.get_distance();
   bdist = 0.0393701 * backClampDistanceSensor.get_distance();
   fdist = 0.0393701 * frontDistanceSensor.get_distance();
   ldist = 0.0393701 * leftDistanceSensor.get_distance();
   sprintf(buffer, "P: %d, %d, %d", (int)estimatedPose.x, (int)estimatedPose.y, (int)lemlib::radToDeg(estimatedPose.theta));
   printf("%s\n", buffer);
   sprintf(buffer, "R: %.2f B: %.2f L: %.2f F: %.2f", rdist, bdist, ldist, fdist);
   printf("%s\n", buffer);
   bestParticle = particleFilter.getBestParticle();
   sprintf(buffer, "Best Particle: %d (%d, %d, %d) %e",bestParticle.id, (int)bestParticle.pose.x, (int)bestParticle.pose.y, (int)lemlib::radToDeg(bestParticle.pose.theta), (bestParticle.weight));
   printf("%s\n", buffer);

   //particleFilter.start();
   pros::delay(5000); // Wait for the particle filter to stabilize
   //particleFilter.stop();

   printf("5 more seconds later....\n");
   estimatedPose = particleFilter.getEstimatedPose();
   rdist = 0.0393701 * rightDistanceSensor.get_distance();
   bdist = 0.0393701 * backClampDistanceSensor.get_distance();
   fdist = 0.0393701 * frontDistanceSensor.get_distance();
   ldist = 0.0393701 * leftDistanceSensor.get_distance();
   sprintf(buffer, "P: %d, %d, %d", (int)estimatedPose.x, (int)estimatedPose.y, (int)lemlib::radToDeg(estimatedPose.theta));
   printf("%s\n", buffer);
   sprintf(buffer, "R: %.2f B: %.2f L: %.2f F: %.2f", rdist, bdist, ldist, fdist);
   printf("%s\n", buffer);
   bestParticle = particleFilter.getBestParticle();
   sprintf(buffer, "Best Particle: %d (%d, %d, %d) %e",bestParticle.id, (int)bestParticle.pose.x, (int)bestParticle.pose.y, (int)lemlib::radToDeg(bestParticle.pose.theta), (bestParticle.weight));
   printf("%s\n", buffer);

   //particleFilter.start();
   pros::delay(10000); // Wait for the particle filter to stabilize
   particleFilter.stop();

   printf("10 more seconds later....\n");
   estimatedPose = particleFilter.getEstimatedPose();
   rdist = 0.0393701 * rightDistanceSensor.get_distance();
   bdist = 0.0393701 * backClampDistanceSensor.get_distance();
   fdist = 0.0393701 * frontDistanceSensor.get_distance();
   ldist = 0.0393701 * leftDistanceSensor.get_distance();
   sprintf(buffer, "P: %d, %d, %d", (int)estimatedPose.x, (int)estimatedPose.y, (int)lemlib::radToDeg(estimatedPose.theta));
   printf("%s\n", buffer);
   sprintf(buffer, "R: %.2f B: %.2f L: %.2f F: %.2f", rdist, bdist, ldist, fdist);
   printf("%s\n", buffer);
   bestParticle = particleFilter.getBestParticle();
   sprintf(buffer, "Best Particle: %d (%d, %d, %d) %e",bestParticle.id, (int)bestParticle.pose.x, (int)bestParticle.pose.y, (int)lemlib::radToDeg(bestParticle.pose.theta), (bestParticle.weight));
   printf("%s\n", buffer);

   printf("Initializing competition...Done!\n");
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous()
{
   char buffer[100];
   chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
   u_int64_t start = pros::millis();
   if(!particleFilter.isInitialized()) {
      console.println("Particle filter not initialized!");
      printf("Particle filter not initialized!\n");
      startParticleFilter();
   }

   console.println("Running auton...");
   printf("Running auton...\n");
   sprintf(buffer, "Auton side detected: %d\n", autonSideDetected);
   console.println(buffer);
   printf(buffer);
   selector.run_auton();
   
   u_int64_t end = pros::millis();
   console.println("Auton complete! Time: " + std::to_string((end - start) / 1000.0) + " seconds");
   sprintf(buffer, "Time: %.2f seconds\n", (end - start) / 1000.0);
   masterController.print(0, 0, buffer);
   printf("Auton complete! Time: %f seconds\n", (end - start) / 1000.0);
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol()
{
   static int backClampState = 0;
   chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

   while (true)
   {
      static bool liftState = false;
      // get left y and right x positions
      int leftY = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
      int rightX = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

      // move the robot
      chassis.curvature(leftY, rightX);

      if (masterController.get_digital(DIGITAL_R1))
      {
         intakeMotor.move_velocity(-200);
      }
      else if (masterController.get_digital(DIGITAL_R2))
      {
         intakeMotor.move_velocity(200);
      }
      else
      {
         intakeMotor.move_velocity(0);
      }
      if (masterController.get_digital(DIGITAL_L1) && (int)dumpTruckMotor.get_position() > -790)
      {
         dumpTruckMotor.move_velocity(-127);
         dumpTruckMotor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
      }
      else if (masterController.get_digital(DIGITAL_L2) && (
                  (!liftState && dumpTruckMotor.get_position() < -10) ||
                  (liftState && dumpTruckMotor.get_position() < -30)))
      {
         dumpTruckMotor.move_velocity(127);
         dumpTruckMotor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
      }
      else
      {
         dumpTruckMotor.move_velocity(0);
      }

      if (masterController.get_digital_new_press(DIGITAL_A))
      {
         if (backClampState == 0)
         {
            backClampState = 1;
         }
         else
         {
            backClampState = 0;
         }
         backClampPnuematic.set_value(backClampState);
      }
      if(backClampState == 0 && masterController.get_digital(DIGITAL_B))
      {
         if(backClampDistanceSensor.get() < 17)
         {
            backClampState = 1;
            backClampPnuematic.set_value(backClampState);
         }
      }
      if (masterController.get_digital_new_press(DIGITAL_DOWN))
      {
         printf("Dump Truck Position: %d\n", (int)dumpTruckMotor.get_position());
         if((int)dumpTruckMotor.get_position() < -30)
         {
            dumpTruckMotor.move_absolute(-30, 200);
         }
         if(liftState) 
         {
            dumpTruckMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
         }
         liftState = !liftState;
         liftPnuematic.set_value(liftState);
      }
      static bool doinkerState = false;
      if (masterController.get_digital_new_press(DIGITAL_LEFT))
      {
         doinkerState = !doinkerState;
         
         doinker.set_value(doinkerState);
      }
      // delay to save resources
      pros::delay(20);
   } // Run for 20 ms then update
}