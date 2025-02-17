#include "main.h"
#include "robodash/api.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "autons.h"
#include "colorsort.hpp"
#include "ladybrown.hpp"
#include "particle.hpp"

rd::Selector selector({
                        {ELIM_GOAL_RUSH, &elimGoalRushAuton},
                        {POS_SIDE_WALL_STAKE, &positiveSideWallStakeAuton},
                        {QUAL_R_NEG_SIDE, &qualRedNegSide},
                        {QUAL_B_NEG_SIDE, &qualBlueNegSide},
                        {ELIM_R_NEG_SIDE, &elimRedNegSide},
                        {ELIM_B_NEG_SIDE, &elimBlueNegSide},
                        {NEG_SIDE_RING_RUSH, &negRingRushAuton},
                        {PF_SKILLS, &pfSkills},
                        {SINGLE_MOGO, &simpleSingleMogo},
                        {QUAL_GOAL_RUSH, &qualsGoalRushAutonTweaked},
                        {SAFE_SKILLS, &safeSkills},
                        {SIMPLE_ALLIANCE, &simpleAllianceStake},
                        {GOAL_FILL, &goalFill},
                        {TWO_GOAL_SIDE_FILL, &twoGoalSideFill},
                        {AUTON_ONE, &simpleAuton},
                        {SCORING_SKILLS, &scoringSkills},
                        {LINEAR_PID_MOVEMENT, &linearPidMovementTest},
                        {TURN_PID_MOVEMENT, &turnPidMovementTest}, 
                       });

rd::Console console;

// controller
pros::Controller masterController(pros::E_CONTROLLER_MASTER);

// motor groups
pros::MotorGroup leftMotors({10, -9, -8},
                            pros::MotorGearset::blue);
pros::MotorGroup rightMotors({14, -2, 4}, pros::MotorGearset::blue);
pros::Optical firstRingColorSensor(6);
pros::Optical secondRingColorSensor(7);
// Remove these lines since they're defined in colorsort.cpp
// RingDetectorState prevColorState = NO_RING_DETECTED;
// RingDetectorState currentColorState = NO_RING_DETECTED;
HookState hookState = HOOK_STOPPED;
pros::Imu imu(19);

pros::Motor LadyBrownMotor(3);

int ladyBrownStateTargets[LadyBrownState::NUM_STATES] = {0, -425, /*-1200,*/ -1650, -2500, -3000};     // Took out vertical position for now.
// int ladyBrownStateTargets[LadyBrownState::NUM_STATES] = {0, -425, /*-1200,*/ -1650, -1650, -1650,};  // Took out vertical position for now.
LadyBrownState ladyBrownState = RESTING;
int ladyBrownPidEnabled = 1;
bool colorSortEnabled = false;
int Stage_One_Intake = 11;
int Stage_Two_Intake = 1;
pros::Motor IntakeStageOne(Stage_One_Intake);
pros::Motor IntakeStageTwo(Stage_Two_Intake);
pros::adi::DigitalOut backClampPnuematic('H');
pros::adi::DigitalOut leftDoinker('A');
pros::adi::DigitalOut rightDoinker('G');
pros::adi::DigitalIn goalDetector('C');

int MIN_INTAKE_TIME = 500;
int POST_INTAKE_DELAY = 500;
int MAX_INTAKE_TIME = 3000;

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
lemlib::ControllerSettings linearController(20,  // proportional gain (kP)
                                            0,   // integral gain (kI)
                                            100, // derivative gain (kD)
                                            3,   // anti windup
                                            1,   // small error range, in inches
                                            50,  // small error range timeout, in milliseconds
                                            3,   // large error range, in inches
                                            150, // large error range timeout, in milliseconds
                                            0    // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(2.5,   // proportional gain (kP)
                                             0,   // integral gain (kI)
                                             18,  // derivative gain (kD)
                                             0,   // anti windup
                                             1,   // small error range, in degrees
                                             30, // small error range timeout, in milliseconds
                                             3,   // large error range, in degrees
                                             180, // large error range timeout, in milliseconds
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
enum ColorState
{
   RED,
   BLUE,
   NO_COLOR
};

// Global variable to keep track of the current color state
// Remove this conflicting declaration
// ColorState currentColorState = NO_COLOR;

const int RED_SIDE_AUTON = 1;
const int BLUE_SIDE_AUTON = -1;
int autonSideDetected = RED_SIDE_AUTON;

pros::Distance backDistanceSensor(18);
pros::Distance frontDistanceSensor(12);
pros::Distance rightDistanceSensor(5);
pros::Distance leftDistanceSensor(21);

// TODO - Tune these values
std::vector<ParticleFilter::SensorMount> sensorMounts = {
    {&frontDistanceSensor, lemlib::Pose(4.5, 4.5, lemlib::degToRad(0))},
    {&leftDistanceSensor, lemlib::Pose(4.5, 4.5, lemlib::degToRad(-90))},
    {&rightDistanceSensor, lemlib::Pose(-2.0, -4.5, lemlib::degToRad(90))},
    {&backDistanceSensor, lemlib::Pose(-7.5, 0.0, lemlib::degToRad(180))}
};
ParticleFilter particleFilter(150, sensorMounts);

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

   IntakeStageTwo.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
   IntakeStageTwo.tare_position();

   LadyBrownMotor.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
   LadyBrownMotor.tare_position();

    pros::Task myAsyncControlTask([]{
      uint32_t lastTimeRun = pros::millis();
      while (true)
      {
         ladyBrownControl();
         hookColorSort();
         pros::c::task_delay_until(&lastTimeRun, 10);
      }
    });

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
   //stopLemLibTrackingTask();
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

      /*
      // Initialize particle filter with starting pose based on auton
      lemlib::Pose initialPose(autonSideDetected * 48, -48, autonSideDetected * lemlib::degToRad(autonSideDetected * 90)); // Modify based on auton selection
      if (strcmp(auton_name_in_robodash_file, SIMPLE_ALLIANCE) == 0)
      {
         initialPose = lemlib::Pose(autonSideDetected * 48, 24, autonSideDetected *lemlib::degToRad(autonSideDetected * 90)); // Modify based on auton selection
      }
      chassis.setPose(initialPose);
      particleFilter.initialize(initialPose, &chassis, 2.0, lemlib::degToRad(10));
      particleFilter.start();
      */
   }
}

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

   char buffer[100];
   getAutonColorState();
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
   lemlib::Pose pose = chassis.getPose();
   sprintf(buffer, "Pose: %d, %d, %d", (int)pose.x, (int)pose.y, (int)lemlib::radToDeg(pose.theta));
   console.println(buffer);
   pros::delay(50);
   masterController.print(1, 0, buffer);
   printf("Auton complete! Time: %f seconds\n", (end - start) / 1000.0);
   pros::delay(50);
   if(goalDetector.get_value()) {
      masterController.print(3, 0, "Goal Rush Won");
   } else {  
      masterController.print(3, 0, "Goal Rush Lost");
   }
   hookState = HOOK_STOPPED;
   IntakeStageOne.move_voltage(0); 
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
   colorSortEnabled = false;
   if(colorSortEnabled) {
      firstRingColorSensor.set_led_pwm(100);
      secondRingColorSensor.set_led_pwm(100);
   } else {
      firstRingColorSensor.set_led_pwm(0);
      secondRingColorSensor.set_led_pwm(0); 
   }
   static int backClampState = 0;
   static bool leftDoinkerState = false;
   static bool rightDoinkerState = false;
   chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
   
   const double STAGE1_STALL_CURRENT_THRESHOLD = 2300; // milliamps
   const int STAGE1_STALL_TIME_THRESHOLD = 60; // milliseconds
   
   uint32_t stage1_stall_timer = 0;
   bool stage1_was_stalled = false;

   const double STAGE2_STALL_CURRENT_THRESHOLD = 2300; // milliamps
   const int STAGE2_STALL_TIME_THRESHOLD = 60; // milliseconds
   
   uint32_t stage2_stall_timer = 0;
   bool stage2_was_stalled = false;

   while (true)
   {
      // get left y and right x positions
      int leftY = masterController.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
      int rightX = masterController.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

      // move the robot
      chassis.curvature(leftY, rightX);

      if (masterController.get_digital(DIGITAL_R1))
      {
         double current = std::abs(IntakeStageOne.get_current_draw());
         if (current > STAGE1_STALL_CURRENT_THRESHOLD) {
            if (stage1_stall_timer == 0) {
               stage1_stall_timer = pros::millis();
            }
            else if (pros::millis() - stage1_stall_timer > STAGE1_STALL_TIME_THRESHOLD) {
               IntakeStageOne.move_voltage(12000);
               if (!stage1_was_stalled) {
                  stage1_was_stalled = true;
               }
            }
         } else {
            stage1_stall_timer = 0;
            stage1_was_stalled = false;
            IntakeStageOne.move_voltage(-12000);
         }
      }
      else if (masterController.get_digital(DIGITAL_R2))
      {
         stage1_stall_timer = 0;
         stage1_was_stalled = false;
         IntakeStageOne.move_voltage(12000);
      }
      else
      {
         stage1_stall_timer = 0;
         stage1_was_stalled = false;
         IntakeStageOne.move_voltage(0);
      }

      if (masterController.get_digital(DIGITAL_L1))
      {
         hookState = HOOK_UP;
      }
      else if (masterController.get_digital(DIGITAL_L2)){ 
         hookState = HOOK_DOWN;
      }
      else
      {
         hookState = HOOK_STOPPED;
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

      if (masterController.get_digital_new_press(DIGITAL_DOWN))
      {
         nextLadyBrownState();
      }
      if (masterController.get_digital_new_press(DIGITAL_B)){ 
         prevLadyBrownState();
      }

      if (masterController.get_digital_new_press(DIGITAL_LEFT))
      {
         leftDoinkerState = !leftDoinkerState;
         
         leftDoinker.set_value(leftDoinkerState);
      }

      if (masterController.get_digital_new_press(DIGITAL_UP))
      {
         rightDoinkerState = !rightDoinkerState;
         
         rightDoinker.set_value(rightDoinkerState);
      }

      if(masterController.get_digital_new_press(DIGITAL_X)) {
         colorSortEnabled = !colorSortEnabled;
         if(colorSortEnabled) {
            firstRingColorSensor.set_led_pwm(100);
            secondRingColorSensor.set_led_pwm(100);
         } else {
            firstRingColorSensor.set_led_pwm(0);
            secondRingColorSensor.set_led_pwm(0);
         }
      }
      // delay to save resources
      pros::delay(20);
   } // Run for 20 ms then update
}