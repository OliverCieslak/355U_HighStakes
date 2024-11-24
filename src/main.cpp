#include "main.h"
#include "robodash/api.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "autons.h"

rd::Selector selector({
                        {"Simple Alliance", &simpleAllianceStake},
                        {"Single MoGo", &simpleSingleMogo},
                        {"Qual Goal Rush", &qualsGoalRushAuton},
                        {"Goal Fill", &goalFill},
                        {"Two Goal Side Fill", &twoGoalSideFill},
                        {"Auton 1", &simpleAuton},
                        {"Skills Run", &skills},
                        {"Linear PID Movement", &linearPidMovementTest},
                        {"Turn PID Movement", &turnPidMovementTest}, 
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
pros::adi::DigitalOut liftpnuematic('G');
pros::adi::DigitalOut backClampPnuematic('H');
pros::Distance dSensor(12);

lemlib::Drivetrain drivetrain(&leftMotors,  // left motor group
                              &rightMotors, // right motor group
                              11.0,
                              lemlib::Omniwheel::NEW_275, // using new 2.75" omnis
                              600,                        // drivetrain rpm
                              8                           // horizontal drift is 2. If we had traction wheels, it would have been 8
);

// lateral motion controller
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

// Function to cycle through the color states based on the current hue
void getAutonColorState()
{
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

   dumpTruckMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST); 

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
   getAutonColorState();
   // TODO - Start the particle filter here where we can get the initial position of the based on the auton
   // selector file.
   const char *file_name = "/usd/rd_auton.txt"; // RoboDash file
	FILE *save_file;
	save_file = fopen(file_name, "r");
	if (!save_file) return;

	// Read contents
	char line[256];
	char saved_selector[256];
	char saved_name[256];

   // Should only really be 1 of these
	while (fgets(line, 256, save_file)) {
		sscanf(line, "%[^:]: %[^\n\0]", saved_selector, saved_name);
	}

   fclose(save_file);
   printf("Selected auton: %s\n", saved_name);

   // TODO - Based on the saved_name and the color sensor, we can determine the auton side and starting position
   // to initialize the particle filter with.

   // We can get the distance from the sensors here to start the particle filter with
   /*
   int d = (int)dSensor.get_distance();
   printf("competition_initialize Distance: %d\n", d);
   */

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
   console.println("Running auton...");
   printf("Running auton...\n");
   getAutonColorState();
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
   int backClampState = 0;
   while (true)
   {
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
      if (masterController.get_digital(DIGITAL_L1))
      {
         dumpTruckMotor.move_velocity(-127);
      }
      else if (masterController.get_digital(DIGITAL_L2))
      {
         dumpTruckMotor.move_velocity(127);
      }
      else
      {
         dumpTruckMotor.move_velocity(0);
      }

      if (masterController.get_digital_new_press(DIGITAL_A))
      {
         backClampPnuematic.set_value(backClampState);
         if (backClampState == 0)
         {
            backClampState = 1;
         }
         else
         {
            backClampState = 0;
         }
      }
      static bool liftState = false;
      if (masterController.get_digital_new_press(DIGITAL_DOWN))
      {
         liftState = !liftState;
         liftpnuematic.set_value(liftState);
      }
      // delay to save resources
      pros::delay(20);
   } // Run for 20 ms then update
}
