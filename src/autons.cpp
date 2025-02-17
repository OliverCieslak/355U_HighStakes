#include "autons.h"
#include "globals.h"
#include "ladybrown.hpp"

void goalFill() //negative side for quals
{
    chassis.cancelAllMotions(); 
    chassis.setPose(0, 0, 0); 
    lemlib::Pose start_pose = chassis.getPose(); 
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -7, 4000 ,{.forwards = false, .maxSpeed = 50}, false);
    chassis.turnToHeading(90 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE}); 
    pros::delay(300); 
    chassis.moveToPoint(-5 * autonSideDetected, -7, 2000 ,{.forwards = false, .maxSpeed = 50}, false);
     
    pros::delay(300); 

    chassis.moveToPoint(30 * autonSideDetected, 15, 2000 ,{.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(1);
    //chassis.moveToPoint(20 * autonSideDetected, 15, 4000 ,{.forwards = true, .maxSpeed = 70}, false);
}

void twoGoalSideFill() //elims goal rush
{
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -36, 4000 ,{.forwards = false, .maxSpeed = 70}, false); 
    chassis.moveToPoint(8.5 * autonSideDetected, -42, 2000 ,{.forwards = false, .maxSpeed = 50}, false); //drive to pick up the middle stake
    backClampPnuematic.set_value(1);//pick up middle stake
    

    // backClampPnuematic.set_value(0); //drop stake
    IntakeStageOne.move_velocity(-127); //intake ring 
    chassis.moveToPoint(-9 * autonSideDetected, -31, 2000 ,{.forwards = true, .maxSpeed = 50}, false); //drive to close ring 
    chassis.moveToPoint(-9 * autonSideDetected, -22, 2000 ,{.forwards = true, .maxSpeed = 30}, false); 
    pros::delay(700);
    IntakeStageOne.move_velocity(0);
    hookState = HOOK_UP;
    pros::delay(800);
    hookState = HOOK_STOPPED;

    backClampPnuematic.set_value(0); 
    IntakeStageOne.move_velocity(127);  

    chassis.moveToPoint(-48 * autonSideDetected, -8, 2000 ,{.forwards = true, .maxSpeed = 50}, false); //drive to pick up other stake
    // backClampPnuematic.set_value(0); //load ring on stake
    chassis.moveToPoint(-52 * autonSideDetected, -4, 2000 ,{.forwards = true, .maxSpeed = 50}, false); 

    pros::delay(400); 
    IntakeStageTwo.move_velocity(-127); //dump rings
    pros::delay(800);
    IntakeStageTwo.move_velocity(127);
    pros::delay(600);
    IntakeStageTwo.move_velocity(0);

    chassis.moveToPoint(-54 * autonSideDetected, 0, 2000 ,{.forwards = false, .maxSpeed = 50}, false); //drive to pick up other stake
}
