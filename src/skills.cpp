#include "autons.h"
#include "globals.h"

void safeSkills()
{
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);

    console.println("Safe Skills");
    printf("Safe Skills\n");
    masterController.print(0, 0, "Safe Skills");
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    dumpTruckMotor.tare_position();
    dumpTruckMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

    dumpTruckScore(&dumpTruckMotor); //load ring on alliance stake
    chassis.moveToPoint(0, 15, 2000 ,{.forwards = true, .maxSpeed = 70}, false); 
    chassis.moveToPoint(20, 12, 2000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);  // Get first Mogo
    chassis.moveToPoint(64, -5, 2000 ,{.forwards = false, .maxSpeed = 50}, false); 
    backClampPnuematic.set_value(0);  // Drop first Mogo in corner
    chassis.moveToPoint(20, 12, 3000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24, 12, 3000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);  // Get second Mogo
    chassis.moveToPoint(-55, -5, 4000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(0);  // Drop second Mogo in corner
    chassis.moveToPoint(-48, 12, 4000 ,{.forwards = true, .maxSpeed = 70}, false);


    // Long drive to 3rd Mogo
    chassis.moveToPoint(-48, 72, 7000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 112, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);  // Get 3rd Mogo (by blue alliance stake)
    chassis.moveToPoint(-48, 108, 2000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-65, 112, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(0);  // Drop 3rd Mogo in corner
    // Long drive to 4th Mogo
    chassis.moveToPoint(-40, 108, 4000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 108, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(55, 118, 7000 ,{.forwards = true, .maxSpeed = 70}, false);
    // Make sure 3rd goal is in corner...
    chassis.moveToPoint(-65, 116, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
}

void scoringSkills()
{
    pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
    pros::Motor dumpTruckMotor(DUMP_TRUCK_MOTOR_PORT);

    console.println("Scoring Skills");
    printf("Scoring Skills\n");
    masterController.print(0, 0, "Scoring Skills");
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    dumpTruckMotor.tare_position();
    dumpTruckMotor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);

    dumpTruckScore(&dumpTruckMotor); //load ring on alliance stake
    chassis.moveToPoint(0, 8, 2000 ,{.forwards = true, .maxSpeed = 70}, false); 
    chassis.turnToPoint(26, 8, 2000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO});
    chassis.moveToPoint(26, 8, 4000 ,{.forwards = false, .maxSpeed = 40}, false);
    backClampPnuematic.set_value(1);  // Get first Mogo
    pros::delay(250);
    intakeMotor.move_velocity(-127); //intake ring
    chassis.moveToPoint(22, 38, 5000 ,{.forwards = true, .maxSpeed = 40}, false);
    chassis.moveToPoint(48, 38, 5000 ,{.forwards = true, .maxSpeed = 40}, false);
    chassis.turnToHeading(180, 2000, {.direction = lemlib::AngularDirection::AUTO}, false);
    pros::delay(500);
    intakeMotor.move_velocity(0);
    dumpTruckScore(&dumpTruckMotor); //load ring on mogo
    intakeMotor.move_velocity(-127); //intake ring
    chassis.moveToPoint(44, 8, 8000 ,{.forwards = true, .maxSpeed = 40}, false);
    intakeMotor.move_velocity(0);
    dumpTruckScore(&dumpTruckMotor); //load ring on mogo
    chassis.moveToPoint(55, 0, 4000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(0);  // Drop first Mogo in corner
    chassis.moveToPoint(0, 8, 3000 ,{.forwards = true, .maxSpeed = 70}, false); 
    return;


    chassis.moveToPoint(20, 12, 3000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24, 12, 3000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);  // Get second Mogo
    chassis.moveToPoint(-55, -5, 4000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(0);  // Drop second Mogo in corner
    chassis.moveToPoint(-48, 12, 4000 ,{.forwards = true, .maxSpeed = 70}, false);


    // Long drive to 3rd Mogo
    chassis.moveToPoint(-48, 72, 7000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 112, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);  // Get 3rd Mogo (by blue alliance stake)
    chassis.moveToPoint(-48, 108, 2000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-65, 112, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(0);  // Drop 3rd Mogo in corner
    // Long drive to 4th Mogo
    chassis.moveToPoint(-40, 108, 4000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 108, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(55, 118, 7000 ,{.forwards = true, .maxSpeed = 70}, false);
    // Make sure 3rd goal is in corner...
    chassis.moveToPoint(-65, 116, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
}
