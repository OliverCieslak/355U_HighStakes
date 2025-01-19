#include "autons.h"
#include "globals.h"

void safeSkills()
{
    console.println("Safe Skills");
    printf("Safe Skills\n");
    masterController.print(0, 0, "Safe Skills");
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
 
     //load ring on alliance stake
    pros::delay(100);
    chassis.moveToPoint(0, 8, 1000 ,{.forwards = true, .maxSpeed = 70}, false); 
    chassis.turnToPoint(26, 8, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO});
    chassis.moveToPoint(26, 8, 3000 ,{.forwards = false, .maxSpeed = 40}, false);
    backClampPnuematic.set_value(1);  // Get first Mogo
    pros::delay(100);
    chassis.moveToPoint(64, 0, 4000 ,{.forwards = false, .maxSpeed = 50}, false); 
    backClampPnuematic.set_value(0);  // Drop first Mogo in corner
    chassis.moveToPoint(20, 12, 4000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24, 12, 4000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);  // Get second Mogo
    chassis.moveToPoint(-55, -5, 5000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(0);  // Drop second Mogo in corner
    chassis.moveToPoint(-48, 12, 5000 ,{.forwards = true, .maxSpeed = 70}, false);

    // Long drive to 3rd Mogo
    chassis.moveToPoint(-48, 72, 7000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 112, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);  // Get 3rd Mogo (by blue alliance stake)
    chassis.moveToPoint(-48, 112, 3000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-65, 120, 2000 ,{.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(0);  // Drop 3rd Mogo in corner
    // Long drive to 4th Mogo
    chassis.moveToPoint(-40, 116, 4000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 112, 7000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(55, 112, 7000 ,{.forwards = false, .maxSpeed = 50}, false);

    chassis.moveToPoint(0, 112, 7000 ,{.forwards = true, .maxSpeed = 50}, false);
}

void scoringSkills()
{
    console.println("Scoring Skills");
    printf("Scoring Skills\n");
    masterController.print(0, 0, "Scoring Skills");
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);


     //load ring on alliance stake
    pros::delay(100);
    chassis.moveToPoint(0, 8, 1000 ,{.forwards = true, .maxSpeed = 70}, false); 
    chassis.turnToPoint(26, 8, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO});
    chassis.moveToPoint(26, 8, 3000 ,{.forwards = false, .maxSpeed = 40}, false);
    backClampPnuematic.set_value(1);  // Get first Mogo
    pros::delay(100);
    // call the intake with stall detection as a task
    pros::Task *intakeTask = new pros::Task(intakeStallDetection);
    chassis.turnToPoint(44, 8, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70});
    chassis.moveToPoint(44, 8, 3000 ,{.forwards = true, .maxSpeed = 50}, false);
    chassis.moveToPoint(60, 8, 3000 ,{.forwards = true, .maxSpeed = 40}, false);
    chassis.turnToPoint(60, 0, 1000, {.forwards = false, .direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE});
    IntakeStageOne.move_voltage(12000); // eject partial ring
    pros::delay(500);
    IntakeStageOne.move_voltage(0);
     //load ring on mogo
    pros::delay(100);
    chassis.moveToPoint(60, 0, 1000 ,{.forwards = false, .maxSpeed = 50}, false);

    backClampPnuematic.set_value(0);  // Drop first Mogo
    pros::delay(250);
    IntakeStageOne.move_voltage(-12000); //intake ring
    chassis.moveToPoint(56, 56, 5000 ,{.forwards = true, .maxSpeed = 50}, false);

    chassis.turnToPoint(14, 112, 1000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(14, 112, 5000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.turnToPoint(60, 118, 1000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(60, 118, 5000 ,{.forwards = false, .maxSpeed = 70}, false);

    chassis.moveToPoint(50, 110, 2000 ,{.forwards = true, .maxSpeed = 70}, false);
    IntakeStageOne.move_voltage(0);

    chassis.turnToPoint(0, 104, 1000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 104, 5000 ,{.forwards = false, .maxSpeed = 50}, false); // Empty Mogo by blue alliance stake
    backClampPnuematic.set_value(1);  // Get Far Mogo
    pros::delay(100);
     //load ring on mogo
    pros::delay(500);
    chassis.moveToPoint(-40, 104, 5000 ,{.forwards = false, .maxSpeed = 70}, false); // Corner Mogo
    chassis.moveToPoint(-60, 116, 3000 ,{.forwards = false, .maxSpeed = 50}, false); // Corner Mogo
    backClampPnuematic.set_value(0);  // 3rd corner Mogo

    IntakeStageOne.move_voltage(-12000); //intake ring
    chassis.moveToPoint(-36, 80, 5000 ,{.forwards = true, .maxSpeed = 50}, false); // Ring for Blue Alliance Stake
    IntakeStageOne.move_voltage(0);
    chassis.moveToPoint(0, 104, 5000 ,{.forwards = false, .maxSpeed = 70}, false); // Near Blue Alliance Stake
    chassis.moveToPoint(0, 124, 2000 ,{.forwards = false, .maxSpeed = 50}, false); // Lineup Blue Alliance Stake
     //load ring on mogo
    pros::delay(100);
    chassis.moveToPoint(0, 116, 1000 ,{.forwards = true, .maxSpeed = 70}, false); // Move from Blue Alliance Stake

    chassis.moveToPoint(-60, 116, 3000 ,{.forwards = false, .maxSpeed = 50}, false); // Resmash corner Mogo
    chassis.moveToPoint(-48, 104, 3000 ,{.forwards = true, .maxSpeed = 70}, false); // Prepare for last Mogo

    IntakeStageOne.move_voltage(-12000); //intake ring
    chassis.moveToPoint(-48, 40, 3000 ,{.forwards = true, .maxSpeed = 50}, false); // Get Rings for last Mogo
    IntakeStageOne.move_voltage(0); // stop intake
}

void wallStakeScoringSkills()
{
    console.println("Scoring Wallstakes Skills");
    printf("Scoring Wallstakes Skills\n");
    masterController.print(0, 0, "WS Skills");
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);


     //load ring on alliance stake
    chassis.moveToPoint(0, 8, 1000 ,{.forwards = true, .maxSpeed = 70}, false); 
    chassis.turnToPoint(26, 8, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO});
    chassis.moveToPoint(26, 8, 3000 ,{.forwards = false, .maxSpeed = 40}, false);
    backClampPnuematic.set_value(1);  // Get first Mogo
    pros::delay(100);
    IntakeStageOne.move_velocity(-127); // intake 2 rings
    chassis.turnToPoint(44, 8, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70});
    chassis.moveToPoint(44, 8, 3000 ,{.forwards = true, .maxSpeed = 50}, false);
    chassis.moveToPoint(60, 8, 3000 ,{.forwards = true, .maxSpeed = 40}, false);
    chassis.turnToPoint(60, 0, 1000, {.forwards = false, .direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE});
    chassis.moveToPoint(60, 0, 1000 ,{.forwards = false, .maxSpeed = 50}, false);
    pros::delay(1000);
    IntakeStageOne.move_velocity(0);
    pros::delay(100);
     //load ring on mogo
    pros::delay(100);
    backClampPnuematic.set_value(0);  // Drop first Mogo
    pros::delay(250);
    IntakeStageOne.move_velocity(-127); //intake ring
    chassis.moveToPoint(56, 56, 8000 ,{.forwards = true, .maxSpeed = 50}, false);
    chassis.turnToHeading(-90, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE});
    chassis.moveToPoint(60, 56, 2000 ,{.forwards = false, .maxSpeed = 50}, false);
    IntakeStageOne.move_velocity(0);
    // liftPnuematic.set_value(1);  // Wall Stake
    pros::delay(500);
     // Score Wall Stake
    // liftPnuematic.set_value(0);
    pros::delay(100);
    return;

    IntakeStageOne.move_velocity(0);
     //load ring on mogo
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
