#include "autons.h"
#include "globals.h"
#include "ladybrown.hpp"



void negRingRushAuton() {
    colorSortEnabled = false;
    hookState = HOOK_UP;

    // Start facing the center rings
    chassis.setPose(-48 * autonSideDetected, -48, 90 * autonSideDetected);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    chassis.cancelAllMotions(); 

    // Rush the rings, picking up a 2nd alliance ring on the way
    chassis.moveToPoint(-12 * autonSideDetected, -48, 2000 ,{.forwards = true, .maxSpeed = 127}, true);
    chassis.waitUntil(6);
    IntakeStageOne.move_velocity(-127);
    // Deploy the doinkers as we rush
    leftDoinker.set_value(1);
    rightDoinker.set_value(1);
    chassis.waitUntilDone();
    // Retract the doinkers
    leftDoinker.set_value(0);
    rightDoinker.set_value(0);
    pros::delay(100);

    // Get ready to get the MoGo
    chassis.moveToPoint(-24 * autonSideDetected, 48, 1500 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.turnToPoint(-24 * autonSideDetected, 24, 1000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24 * autonSideDetected, 24, 1500 ,{.forwards = false, .maxSpeed = 50}, false);
    // Clamp the Mogo
    backClampPnuematic.set_value(1);
    pros::delay(200);
    hookState = HOOK_UP;  // Score our rings

    // Pull a ring from the negative corner
    chassis.turnToPoint(-59 * autonSideDetected, 59, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-59 * autonSideDetected, 59, 1500 ,{.forwards = true, .maxSpeed = 90}, false);
    pros::delay(250);
    chassis.moveToPoint(-48 * autonSideDetected, 48, 1000 ,{.forwards = false, .maxSpeed = 50}, false);

    // Get ring between ladder and alliance stake
    chassis.turnToPoint(-48 * autonSideDetected, -24, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48 * autonSideDetected, -24, 3000 ,{.forwards = true, .maxSpeed = 70}, false);

    // Head to positive corner
    chassis.moveToPoint(-48 * autonSideDetected, -48, 3000 ,{.forwards = true, .maxSpeed = 90}, false);
}

void baseNegSide()
{
    
    colorSortEnabled = true;
    chassis.setPose(-60 * autonSideDetected, 15, 225 * autonSideDetected);
    chassis.cancelAllMotions();
    ladyBrownState = LadyBrownState::HORIZONTAL;  // Use lady brown to score alliance stake
    pros::delay(550);
    chassis.moveToPoint(-48 * autonSideDetected, 24, 1000 ,{.forwards = false, .maxSpeed = 50}, false); // Back up slightly
    ladyBrownState = LadyBrownState::RESTING;  // Use lady brown to score alliance stake
    
    // Get the MoGo
    chassis.moveToPoint(-21 * autonSideDetected, 26, 2000 ,{.forwards = false, .maxSpeed = 60}, false);
    backClampPnuematic.set_value(1);
    pros::delay(100);
    // Get ready to intake rings
    IntakeStageOne.move_velocity(-127);
    hookState = HOOK_UP;
    chassis.turnToPoint(-10 * autonSideDetected, 38, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-10 * autonSideDetected, 38, 3000 ,{.forwards = true, .maxSpeed = 60, .earlyExitRange=3}, false);
    // Smoothly move to the 2nd ring with the early exit
    chassis.moveToPoint(-10 * autonSideDetected, 57, 2000 ,{.forwards = true, .maxSpeed = 60}, false);
    // Smoothly move back to the 3rd ring in the stack with the early exit
    chassis.turnToPoint(-24 * autonSideDetected, 48, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24 * autonSideDetected, 48, 3000 ,{.forwards = true, .maxSpeed = 60}, false);

    chassis.moveToPoint(-30 * autonSideDetected, 48, 1000 ,{.forwards = true, .maxSpeed = 100}, false);
    //chassis.turnToPoint(-37 * autonSideDetected, 55, 800 ,{.forwards = true, .maxSpeed = 70}, false);
    //chassis.moveToPoint(-65 * autonSideDetected, 58, 800 ,{.forwards = true, .maxSpeed = 100}, false);
    //chassis.turnToPoint(-75 * autonSideDetected, 83, 800 ,{.forwards = true, .maxSpeed = 60}, false);
    //chassis.moveToPoint(-80 * autonSideDetected, 82, 1100 ,{.forwards = true, .maxSpeed = 60}, false);
    
    /*chassis.moveToPoint(-50 * autonSideDetected, 50, 1000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(-60 * autonSideDetected, 60, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-50 * autonSideDetected, 50, 1000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(-60 * autonSideDetected, 60, 1000 ,{.forwards = true, .maxSpeed = 70}, false); */

    // 2 Stack between ladder and alliance stake
    //chassis.moveToPoint(-51 * autonSideDetected, -12, 5000 ,{.forwards = true, .maxSpeed = 60}, false);

    // For Elim, head to positive corner.  For quals, head to the ladder
    //chassis.moveToPoint(-53 * autonSideDetected, -52, 5000 ,{.forwards = true, .maxSpeed = 90}, false);
}

void qualNegSide()
{ 
    baseNegSide();
    // For Elim, head to positive corner.  For quals, head to the ladder
    chassis.turnToPoint(-25 * autonSideDetected, 0, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-25 * autonSideDetected, 0, 2000 ,{.forwards = true, .maxSpeed = 100}, false);
    //ladyBrownState = LadyBrownState::HORIZONTAL; 
}

void elimNegSide()
{
    baseNegSide();
    // For Elim, head to positive corner.  For quals, head to the ladder
   // chassis.moveToPoint(-58 * autonSideDetected, -52, 5000 ,{.forwards = true, .maxSpeed = 90}, false);
}

void qualRedNegSide() {
    autonSideDetected = RED_SIDE_AUTON;
    qualNegSide();
}

void elimRedNegSide() {
    autonSideDetected = RED_SIDE_AUTON;
    elimNegSide();
}

void qualBlueNegSide() {
    autonSideDetected = BLUE_SIDE_AUTON;
    qualNegSide();
}

void elimBlueNegSide() {
    autonSideDetected = BLUE_SIDE_AUTON;
    elimNegSide();
}
