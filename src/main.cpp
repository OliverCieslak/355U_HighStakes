#include "main.h"
#include "robodash/api.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "autons.h"

rd::Selector selector({{"Auton 0", &auton0},
                       {"Auton 1", &simple_auton},
                       {"Skills Run", &skills}});

rd::Console console;

// controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// motor groups
pros::MotorGroup leftMotors({-4, -6, -5},
                            pros::MotorGearset::blue);
pros::MotorGroup rightMotors({10, 19, 7}, pros::MotorGearset::blue);

pros::MotorGroup intakeMotors({8, 20}, pros::MotorGearset::green);

pros::Imu imu(9);

// drivetrain settings
lemlib::Drivetrain drivetrain(&leftMotors,  // left motor group
                              &rightMotors, // right motor group
                              11.0,
                              lemlib::Omniwheel::NEW_275, // using new 2.75" omnis
                              600,                        // drivetrain rpm
                              8                           // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
lemlib::ControllerSettings linearController(5,   // proportional gain (kP)
                                            0,   // integral gain (kI)
                                            3,   // derivative gain (kD)
                                            3,   // anti windup
                                            .75, // small error range, in inches
                                            100, // small error range timeout, in milliseconds
                                            1.5, // large error range, in inches
                                            500, // large error range timeout, in milliseconds
                                            10   // maximum acceleration (slew)
);

// angular motion controller
lemlib::ControllerSettings angularController(1.7, // proportional gain (kP)
                                             0,   // integral gain (kI)
                                             10,  // derivative gain (kD)
                                             3,   // anti windup
                                             .5,  // small error range, in degrees
                                             100, // small error range timeout, in milliseconds
                                             1,   // large error range, in degrees
                                             500, // large error range timeout, in milliseconds
                                             0    // maximum acceleration (slew)
);

lemlib::TrackingWheel left_vertical_tracking_wheel(&leftMotors, lemlib::Omniwheel::NEW_275, -6, 600);
lemlib::TrackingWheel right_vertical_tracking_wheel(&rightMotors, lemlib::Omniwheel::NEW_275, 6, 600);

// odometry settings
lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel 1,
                            nullptr, // vertical tracking wheel 2,
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
   console.println("Initializing robot...");
   chassis.calibrate();

   console.println("Initializing robot...Done!");
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

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
   u_int64_t start = pros::millis();
   console.println("Running auton...");
   selector.run_auton();

   u_int64_t end = pros::millis();
   console.println("Auton complete! Time: " + std::to_string((end - start) / 1000.0) + " seconds");
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
   while (true)
   {
      // get left y and right x positions
      int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
      int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

      // move the robot
      chassis.curvature(leftY, rightX);

      // delay to save resources
      pros::delay(25);
   } // Run for 20 ms then update
   /*
   chassis.calibrate(true);
   chassis.cancelAllMotions();
   chassis.setPose(0, 0, 0);
   pros::delay(2000);
   lemlib::Pose start_pose = chassis.getPose();
   double start_heading = imu.get_rotation();
   chassis.moveToPoint(0, 40, 10000);
   pros::delay(500);
   lemlib::Pose pose = chassis.getPose();
   float distance_travelled = pose.distance(start_pose);
   float heading_change = pose.theta - start_pose.theta;
   double end_heading = imu.get_rotation();
   printf("d: %.1f, x: %.1f, y: %.1f, h: %.1f, dh: %.1f, sh: %.1f, eh: %.1f\n", distance_travelled, pose.x, pose.y, pose.theta, heading_change, start_heading, end_heading);
   //chassis.turnToHeading(90, 100000);
   pose = chassis.getPose();
   distance_travelled = pose.distance(start_pose);
   heading_change = pose.theta - start_pose.theta;
   end_heading = imu.get_rotation();
   printf("d: %.1f, x: %.1f, y: %.1f, h: %.1f, dh: %.1f, sh: %.1f, eh: %.1f\n", distance_travelled, pose.x, pose.y, pose.theta, heading_change, start_heading, end_heading);
   pros::delay(500);
   pose = chassis.getPose();
   distance_travelled = pose.distance(start_pose);
   heading_change = pose.theta - start_pose.theta;
   end_heading = imu.get_rotation();
   printf("d: %.1f, x: %.1f, y: %.1f, h: %.1f, dh: %.1f, sh: %.1f, eh: %.1f\n", distance_travelled, pose.x, pose.y, pose.theta, heading_change, start_heading, end_heading);
   */
}
