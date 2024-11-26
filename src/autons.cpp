#include "autons.h"
#include "globals.h"

void qualsGoalRushAuton()
{
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);

    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -30, 4000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(6 * autonSideDetected, -43, 3000 ,{.forwards = false, .maxSpeed = 50}, false); //drive to pick up the middle stake

    backClampPnuematic.set_value(1); //pick up middle stake 
    dumpTruckScore(&dumpTruckMotor); 

    intakeMotor.move_velocity(-127); //intake ring 
    chassis.moveToPoint(12 * autonSideDetected, -31, 2000 ,{.forwards = true, .maxSpeed = 50}, false); //drive to close ring 
    chassis.moveToPoint(12 * autonSideDetected, -28, 2000 ,{.forwards = true, .maxSpeed = 30}, false); 
    pros::delay(750); 
    intakeMotor.move_velocity(0); 
    dumpTruckScore(&dumpTruckMotor); 

    chassis.turnToHeading(180 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::AUTO}); 
    chassis.moveToPoint(12 * autonSideDetected, -5, 2000 ,{.forwards = false, .maxSpeed = 70}, false); //drive to close ring 
    backClampPnuematic.set_value(0);  // Drop middle stake 
    chassis.moveToPoint(12 * autonSideDetected, -28, 2000 ,{.forwards = true, .maxSpeed = 70}, false); 

    //chassis.turnToHeading(90 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE}); 
    chassis.moveToPoint(33 * autonSideDetected, -26, 3000 ,{.forwards = false, .maxSpeed = 70}, false); 
    backClampPnuematic.set_value(1); 
    pros::delay(100); 
    dumpTruckScore(&dumpTruckMotor); 

    // Touch ladder
    chassis.moveToPoint(50 * autonSideDetected, -31, 4000 ,{.forwards = true, .maxSpeed = 50}, false); //drive to pick up other stake

}

void simpleAuton()
{ 
    chassis.setPose(0, 0, 0);
    chassis.cancelAllMotions();
    chassis.moveToPoint(0 * autonSideDetected, 50, 4000 ,{.forwards = true, .maxSpeed = 80}, true);
}

void skills()
{
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);

    console.println("Skills");
    printf("Skills\n");
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    dumpTruckMotor.tare_position();

    dumpTruckScore(&dumpTruckMotor); //load ring on alliance stake
    chassis.moveToPoint(0 * autonSideDetected, 15, 2000 ,{.forwards = true, .maxSpeed = 50}, false); 
    chassis.moveToPoint(20 * autonSideDetected, 9, 2000 ,{.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(1); 
    chassis.moveToPoint(65 * autonSideDetected, -5, 2000 ,{.forwards = false, .maxSpeed = 50}, false); 
    chassis.moveToPoint(55 * autonSideDetected, 0, 2000 ,{.forwards = true, .maxSpeed = 50}, false); 
    backClampPnuematic.set_value(0);
    chassis.moveToPoint(-20 * autonSideDetected, 25, 4000 ,{.forwards = true, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(1); 
    chassis.moveToPoint(6 * autonSideDetected, -46, 8000 ,{.forwards = true, .maxSpeed = 50}, false); 
    backClampPnuematic.set_value(0); 
}

void linearPidMovementTest()
{
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    double start_heading = imu.get_heading();
    // set position to x:0, y:0, heading:0
    chassis.cancelAllMotions();
    lemlib::Pose start_pose = chassis.getPose();
    printf("Starting pose: x:%.1f, y:%.1f, h:%.1f\n", start_pose.x, start_pose.y, start_pose.theta);
    // move 24" forwards
    chassis.moveToPoint(0, 48, 10000, {.forwards = true, .maxSpeed = 80});
    chassis.waitUntilDone();
    chassis.cancelAllMotions();

    lemlib::Pose pose = chassis.getPose();
    double end_heading = imu.get_heading();
    float distance_travelled = pose.distance(start_pose);
    float heading_change = pose.theta - start_pose.theta;

    char buffer[100];
    sprintf(buffer, "d:%.1f,x:%.1f,y:%.1f,h:%.1f,eh:%.1f,dh:%.1f\n", distance_travelled, pose.x, pose.y, pose.theta, end_heading, heading_change);
    console.println(buffer);
    printf(buffer);

    pros::delay(500); // Need to wait for odom to sync
    pose = chassis.getPose();
    end_heading = imu.get_heading();
    distance_travelled = pose.distance(start_pose);
    heading_change = pose.theta - start_pose.theta;
    sprintf(buffer, "d:%.1f,x:%.1f,y:%.1f,h:%.1f,eh:%.1f,dh:%.1f\n", distance_travelled, pose.x, pose.y, pose.theta, end_heading, heading_change);
    console.println(buffer);
    printf(buffer);

}

void turnPidMovementTest()
{
    double start_heading = imu.get_heading();
    // set position to x:0, y:0, heading:0
    chassis.setPose(0, 0, 0);
    pros::delay(200);
    // turn to face heading 90 with a very long timeout
    chassis.turnToHeading(180, 10000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE});
    pros::delay(750); // Need to wait for odom to sync

    lemlib::Pose pose = chassis.getPose();
    double end_heading = imu.get_heading();
    char buffer[100];
    sprintf(buffer, "x: %.1f, y: %.1f, h: %.1f, eh: %.1f", pose.x, pose.y, pose.theta, end_heading - start_heading);
    console.println(buffer);
    printf(buffer);
    printf("\n");
}

void goalFill() //negative side for quals
{
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT); 
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT); 

    chassis.cancelAllMotions(); 
    chassis.setPose(0, 0, 0); 
    lemlib::Pose start_pose = chassis.getPose(); 
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -15, 4000 ,{.forwards = false, .maxSpeed = 50}, false);
    pros::delay(50);
    chassis.moveToPoint(0 * autonSideDetected, -5, 2000 ,{.forwards = true, .maxSpeed = 50}, false); 
    pros::delay(50);
    chassis.swingToHeading(-90, DriveSide::LEFT, 2000, {.direction = AngularDirection::CW_CLOCKWISE});
    pros::delay(50);
    //dumpTruckScore(&dumpTruckMotor); //not working temporarily; replacemnt below
    dumpTruckMotor.move_velocity(-127); //load ring on alliance stake
    pros::delay(400);
    dumpTruckMotor.move_velocity(127);
    pros::delay(400);
    dumpTruckMotor.move_velocity(0);

    chassis.moveToPoint(5 * autonSideDetected, 0, 2000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(25 * autonSideDetected, 15, 4000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);

}


void twoGoalSideFill() //elims goal rush
{
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);

    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -36, 4000 ,{.forwards = false, .maxSpeed = 70}, false); 
    chassis.moveToPoint(8.5 * autonSideDetected, -42, 2000 ,{.forwards = false, .maxSpeed = 50}, false); //drive to pick up the middle stake
    backClampPnuematic.set_value(1);//pick up middle stake
    dumpTruckScore(&dumpTruckMotor);

    // backClampPnuematic.set_value(0); //drop stake
    intakeMotor.move_velocity(-127); //intake ring 
    chassis.moveToPoint(-9 * autonSideDetected, -31, 2000 ,{.forwards = true, .maxSpeed = 50}, false); //drive to close ring 
    chassis.moveToPoint(-9 * autonSideDetected, -22, 2000 ,{.forwards = true, .maxSpeed = 30}, false); 
    pros::delay(700);
    intakeMotor.move_velocity(0);

    dumpTruckMotor.move_velocity(-127); //load ring
    pros::delay(800);
    dumpTruckMotor.move_velocity(127);
    pros::delay(600);
    dumpTruckMotor.move_velocity(0);

    backClampPnuematic.set_value(0); 
    intakeMotor.move_velocity(127);  

    chassis.moveToPoint(-48 * autonSideDetected, -8, 2000 ,{.forwards = true, .maxSpeed = 50}, false); //drive to pick up other stake
    // backClampPnuematic.set_value(0); //load ring on stake
    chassis.moveToPoint(-52 * autonSideDetected, -4, 2000 ,{.forwards = true, .maxSpeed = 50}, false); 

    pros::delay(400); 
    dumpTruckMotor.move_velocity(-127); //dump rings
    pros::delay(800);
    dumpTruckMotor.move_velocity(127);
    pros::delay(600);
    dumpTruckMotor.move_velocity(0);

    chassis.moveToPoint(-54 * autonSideDetected, 0, 2000 ,{.forwards = false, .maxSpeed = 50}, false); //drive to pick up other stake

}

void simpleAllianceStake() {
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);
    dumpTruckMotor.tare_position();

    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -12, 2000 ,{.forwards = false, .maxSpeed = 50}, false); 
    chassis.waitUntil(6);
    dumpTruckScore(&dumpTruckMotor);

    chassis.moveToPoint(4 * autonSideDetected, 20, 2000 ,{.forwards = true, .maxSpeed = 50}, false); 
}

void simpleSingleMogo() {
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);
    dumpTruckMotor.tare_position();

    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -24, 2000 ,{.forwards = false, .maxSpeed = 50}, false); 
    chassis.waitUntil(22);
    backClampPnuematic.set_value(1);
    dumpTruckScore(&dumpTruckMotor);
}

void dumpTruckScore(pros::Motor *dumpTruckMotor) {
    double startingDumpMotorPosition = dumpTruckMotor->get_position();
    int startTime = pros::millis();
    dumpTruckMotor->move_velocity(-127);
    pros::delay(500);
    double currentDumpMotorPosition = dumpTruckMotor->get_position();
    dumpTruckMotor->move_absolute(startingDumpMotorPosition, 127);
    while (!((dumpTruckMotor->get_position() < startingDumpMotorPosition + 5) && (dumpTruckMotor->get_position() > startingDumpMotorPosition - 5))) {
        // Continue running this loop as long as the motor is not within +-5 units of its goal
        pros::delay(2);
        // Exit the loop if the motor has been running for more than 500ms
        if(pros::millis() - startTime > 500) {
            break;
        }
    }    
}