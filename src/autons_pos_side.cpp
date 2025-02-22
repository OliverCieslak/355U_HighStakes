#include "autons.h"
#include "globals.h"
#include "ladybrown.hpp"

void positiveSideWallStakeAuton()
{
    colorSortEnabled = true;
    //colorSortEnabled = false;
    // start with 2nd notch on seam farther from the wall
    chassis.cancelAllMotions();
    chassis.setPose(-52 * autonSideDetected, -36, -107 * autonSideDetected);
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);

    // Pick up pos-side mogo
    chassis.moveToPoint(-24 * autonSideDetected, -24, 3000 ,{.forwards = false, .maxSpeed = 60}, false);
    backClampPnuematic.set_value(1);
    pros::delay(250);
    // If we clamped the mogo, score the preload
    IntakeStageOne.move_voltage(-12000);
    hookState = HOOK_UP;
    pros::delay(500);

    if(goalDetector.get_value()) {
        // Get bottom alliance ring from corner
        chassis.moveToPose(-64 * autonSideDetected, -64, 225 * autonSideDetected, 
            2500, {.forwards = true, .maxSpeed = 60}, false);
        pros::delay(250);
        chassis.moveToPoint(-56 * autonSideDetected, -56, 
            2000 ,{.forwards = false, .maxSpeed = 50}, false);
        // Get 2nd (other color) ring from corner
        chassis.moveToPoint(-64 * autonSideDetected, -64, 
            2000 ,{.forwards = true, .maxSpeed = 60}, false);
        chassis.moveToPoint(-56 * autonSideDetected, -56, 
                2000 ,{.forwards = false, .maxSpeed = 50}, false);
        // Get 3rd (our color) ring from corner
        chassis.moveToPoint(-64 * autonSideDetected, -64, 
            2000 ,{.forwards = true, .maxSpeed = 60}, false);
        chassis.moveToPoint(-48 * autonSideDetected, -48,
                            2000, {.forwards = false, .maxSpeed = 50}, false);
        chassis.turnToPoint(-24 * autonSideDetected, -48, 
            1000 ,{.forwards = true, .maxSpeed = 70}, false);
        hookState = HOOK_STOPPED;
    }

    // Prime the lady brown to hold a ring
    hookState = HOOK_STOPPED;
    ladyBrownState = LadyBrownState::LOADING;
    // Turn to Ring stack
    chassis.turnToPoint(-24 * autonSideDetected, -48, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24 * autonSideDetected, -48, 2000 ,{.forwards = true, .maxSpeed = 50}, false);

    // Load ring to lady brown
    hookState = HOOK_UP;
    pros::delay(600);
    hookState = HOOK_STOPPED;
    // Turn to wall stake
    chassis.turnToPoint(-10 * autonSideDetected, -62, 1000 ,{.forwards = true, .maxSpeed = 70}, false);
    ladyBrownState = LadyBrownState::SCORING;
    chassis.moveToPoint(-9 * autonSideDetected, -62, 3500 ,{.forwards = true, .maxSpeed = 50}, false);
    IntakeStageOne.move_voltage(12000);  // Spit out other ring
    pros::delay(400);
}

void qualsGoalRushAutonTweaked()
{   
    // start with 2nd notch on seam farther from the wall
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 
    //backClampPnuematic.set_value(1);
    chassis.moveToPoint(0 * autonSideDetected, -28, 4000 ,{.forwards = false, .maxSpeed = 90}, false); 
    chassis.moveToPoint(8 * autonSideDetected, -44.5, 3000 ,{.forwards = false, .maxSpeed = 50}, false); //drive to pick up the middle stake
    
    pros::delay(50);
    backClampPnuematic.set_value(1);
    pros::delay(150);
    chassis.moveToPoint(12 * autonSideDetected, -31, 2000 ,{.forwards = true, .maxSpeed = 50}, false);

    hookState = HOOK_UP;
    pros::delay(400);
    hookState = HOOK_STOPPED;

    IntakeStageOne.move_velocity(-127);
     //drive to close ring 
    chassis.moveToPoint(12 * autonSideDetected, -28, 2000 ,{.forwards = true, .maxSpeed = 30}, false); 

    chassis.turnToHeading(180 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::AUTO}); 
    pros::delay(150);
    chassis.moveToPoint(0 * autonSideDetected, 0, 3000 ,{.forwards = false, .maxSpeed = 70}, false); //drive to corner 

    hookState = HOOK_UP;
    pros::delay(500);
    hookState = HOOK_STOPPED;
     
    backClampPnuematic.set_value(0);  // Drop middle stake 
    chassis.moveToPoint(12 * autonSideDetected, -20, 2000 ,{.forwards = true, .maxSpeed = 70}, false); 
    IntakeStageOne.move_velocity(-127);

    chassis.turnToPoint(38 * autonSideDetected, -26, 1000 ,{.forwards = false}, false); 
    chassis.moveToPoint(38 * autonSideDetected, -26, 2500 ,{.forwards = false, .maxSpeed = 30}, false); 
    
    backClampPnuematic.set_value(1);
    pros::delay(100);
    hookState = HOOK_UP;

    // Touch ladder
    chassis.moveToPoint(38 * autonSideDetected, -44, 4000 ,{.forwards = true, .maxSpeed = 50}, false); //drive to pick up other stake
}

void elimGoalRushAutonZeroZeroZero()
{   
    colorSortEnabled = true;
    // start with 2nd notch on seam farther from the wall
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    IntakeStageOne.move_voltage(-12000); 
    chassis.moveToPoint(0 * autonSideDetected, 38, 
                        2500, {.forwards = true, .maxSpeed = 127}, false);
    if(autonSideDetected == RED_SIDE_AUTON) {
        leftDoinker.set_value(1); }
    else {
        rightDoinker.set_value(1); 
    }
    pros::delay(200);
    chassis.moveToPoint(5 * autonSideDetected, 10, 
                        2000, {.forwards = false, .maxSpeed = 90}, false);
    if(autonSideDetected == RED_SIDE_AUTON) {
        leftDoinker.set_value(0); }
    else {
        rightDoinker.set_value(0); 
    }
    pros::delay(200);
    if(autonSideDetected == RED_SIDE_AUTON) {
        chassis.turnToHeading(180 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE, .maxSpeed = 70}, false);
    } else {
        chassis.turnToHeading(180 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE, .maxSpeed = 70}, false);
    }
    chassis.cancelAllMotions();
    chassis.moveToPoint(-5 * autonSideDetected, 25, 
                        4000, {.forwards = false, .maxSpeed = 40}, false);
    chassis.cancelAllMotions(); 

    pros::delay(50);
    backClampPnuematic.set_value(1); 
    pros::delay(100);

    // Did we win the Goal Rush and clamp the goal?
    if(goalDetector.get_value()) {
        masterController.print(3, 0, "Goal Rush Won");
        // We won the goal rush
        hookState = HOOK_UP;
        pros::delay(200);

        chassis.moveToPoint(16 * autonSideDetected, 10, 
                            2000, {.forwards = true, .maxSpeed = 90}, false);
        // Try to get the bottom alliance ring from corner
        chassis.moveToPoint(28 * autonSideDetected, -8, 
                            1000, {.forwards = true, .maxSpeed = 60}, false);
        pros::delay(100);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 60}, false);
        // TODO - If we have time, we could try to get the 2nd alliance ring from corner
        chassis.moveToPoint(28 * autonSideDetected, -8, 
                            1000, {.forwards = true, .maxSpeed = 60}, false);
        pros::delay(100);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 60}, false);
        chassis.moveToPoint(28 * autonSideDetected, -8, 
                            1000, {.forwards = true, .maxSpeed = 60}, false);
        pros::delay(100);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 60}, false);

        // Get ready to place goal in corner
        if(autonSideDetected == RED_SIDE_AUTON) {
            chassis.turnToHeading(0 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE, .maxSpeed = 70}, false);
        } else {
            chassis.turnToHeading(0 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE, .maxSpeed = 70}, false);
        }
        hookState = HOOK_STOPPED;
        backClampPnuematic.set_value(0); 
        pros::delay(100);

        chassis.moveToPoint(16 * autonSideDetected, 6, 
                            1000, {.forwards = false, .maxSpeed = 70}, false);
        // Line up to get the alliance MoGo
        chassis.turnToPoint(-13 * autonSideDetected, 18, 
                            4000, {.forwards = false, .maxSpeed = 70}, false);
        chassis.moveToPoint(-13 * autonSideDetected, 18, 
                            4000, {.forwards = false, .maxSpeed = 50}, false);
        backClampPnuematic.set_value(1);
        pros::delay(100);

        // If we miss the 2nd MoGo, stay near it.
        if(goalDetector.get_value()) {
            // We might have a ring to score on this Mogo
            hookState = HOOK_UP;
            // Get ready to rush the other side
            chassis.moveToPoint(0 * autonSideDetected, 32, 
                                2000, {.forwards = true, .maxSpeed = 90}, false);
            pros::delay(500);
        }
    } else {
        // We lost the goal rush
        masterController.print(3, 0, "Goal Rush Lost");
        backClampPnuematic.set_value(0); 
 
        // Line up to get the alliance MoGo
        chassis.moveToPoint(6 * autonSideDetected, 6, 
                            4000, {.forwards = true, .maxSpeed = 80}, false);
        // Line up to get the alliance MoGo
        chassis.moveToPoint(-13 * autonSideDetected, 23, 
                            4000, {.forwards = false, .maxSpeed = 50}, false);
        backClampPnuematic.set_value(1); 
        pros::delay(100);
        // Get 2 rings on
        hookState = HOOK_UP;
        // Try to get the bottom alliance ring from corner
        chassis.moveToPoint(28 * autonSideDetected, -5, 
                            2000, {.forwards = true, .maxSpeed = 60}, false);

        pros::delay(100);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 80}, false);
        /* There's not enough time to get 2 corner rings and get in position
        // Try to get the (2nd) other alliance ring from corner and hope color sort works
        chassis.moveToPoint(28 * autonSideDetected, -5, 
                            1000, {.forwards = true, .maxSpeed = 50}, false);
        pros::delay(100);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 80}, false);
        // Try to get the 3rd (alliance) ring from corner
        chassis.moveToPoint(28 * autonSideDetected, -5, 
                            1000, {.forwards = true, .maxSpeed = 80}, false);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 50}, false);
        */

        // Get ready to place goal in corner
        if(autonSideDetected == RED_SIDE_AUTON) {
            chassis.turnToHeading(0 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE, .maxSpeed = 70}, false);
        } else {
            chassis.turnToHeading(0 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE, .maxSpeed = 70}, false);
        }
        backClampPnuematic.set_value(0); 
        pros::delay(100);
        IntakeStageOne.move_voltage(0);
        hookState = HOOK_STOPPED;
        // For Elims, position right on the line to rush them or defend.  For qual version, change this to hit the ladder
        chassis.turnToPoint(10 * autonSideDetected, 37, 
                            2000, {.forwards = false, .maxSpeed = 70}, false);
        chassis.moveToPoint(10 * autonSideDetected, 37, 
                            2000, {.forwards = false, .maxSpeed = 90}, false);
    }
}

void elimGoalRushAuton()
{   
    colorSortEnabled = true;
    // start with 2nd notch on seam farther from the wall
    chassis.cancelAllMotions();
    chassis.setPose(-52 * autonSideDetected, -36, 110 * autonSideDetected);
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);

    IntakeStageOne.move_voltage(-12000);
    chassis.moveToPose(-14
         * autonSideDetected, -54, 90 * autonSideDetected, 2000, {.forwards = true, .lead = .2, .maxSpeed = 127}, true);
    chassis.waitUntil(34);
    if(autonSideDetected == RED_SIDE_AUTON) {
        leftDoinker.set_value(1); }
    else {
        rightDoinker.set_value(1); 
    }
    chassis.waitUntilDone();
    chassis.moveToPoint(-36 * autonSideDetected, -48, 
                        2000, {.forwards = false, .maxSpeed = 90}, false);
    if(autonSideDetected == RED_SIDE_AUTON) {
        leftDoinker.set_value(0); }
    else {
        rightDoinker.set_value(0); 
    }
    pros::delay(100);
    // Goal rush done, now try to grab the goal and score the rings
    chassis.turnToPoint(0 * autonSideDetected, -48, 1000 ,{.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(-20 * autonSideDetected, -48, 
        2000, {.forwards = false, .maxSpeed = 50}, false);

    backClampPnuematic.set_value(1); 
    pros::delay(100);

    // Did we win the Goal Rush and clamp the goal?
    if(goalDetector.get_value()) {
        masterController.print(3, 0, "Goal Rush Won");
        // We won the goal rush
        hookState = HOOK_UP;
        pros::delay(400);

        // Reverse the intake to push the other ring away
        IntakeStageOne.move_voltage(12000);

        chassis.moveToPoint(-48 * autonSideDetected, -48, 
                            2000, {.forwards = true, .maxSpeed = 90}, false);
        // Face the corner
        chassis.turnToPoint(-64 * autonSideDetected, -64, 
                            1000, {.forwards = true, .maxSpeed = 70}, false);

        // Get ready to intake
        IntakeStageOne.move_voltage(-12000);
        hookState = HOOK_STOPPED;
        chassis.moveToPoint(-64 * autonSideDetected, -64, 
                            1500, {.forwards = true, .maxSpeed = 50}, false);
        pros::delay(250);
        // Back away to place the MoGo
        chassis.moveToPoint(-48 * autonSideDetected, -48, 
        1500, {.forwards = false, .maxSpeed = 50}, false);
    
        // Get ready to drop and move on to alliance side mogo
        chassis.turnToPoint(-24 * autonSideDetected, -48, 
            1000, {.forwards = true, .maxSpeed = 70}, false);
        // Drop the goal
        backClampPnuematic.set_value(0);
        pros::delay(100);

        chassis.moveToPoint(-24 * autonSideDetected, -48, 
            2000, {.forwards = true, .maxSpeed = 60}, false);

        // Get ready to get the Alliance side mogo
        chassis.turnToPoint(-24 * autonSideDetected, -24, 
            1000, {.forwards = false, .maxSpeed = 70}, false);
        chassis.moveToPoint(-24 * autonSideDetected, -24, 
            2500, {.forwards = false, .maxSpeed = 50}, false);
        backClampPnuematic.set_value(1); 
        pros::delay(100);
        if(goalDetector.get_value()) {
            hookState = HOOK_UP;
            // Get ready to attack
            chassis.turnToPoint(-8 * autonSideDetected, -48, 
                1000, {.forwards = false, .maxSpeed = 70}, false);
            // Get ready to get the Alliance side mogo
            chassis.moveToPoint(-8 * autonSideDetected, -48, 
                1500, {.forwards = false, .maxSpeed = 70}, false);
            chassis.turnToPoint(48 * autonSideDetected, -48, 
                1000, {.forwards = false, .maxSpeed = 70}, false);    
        }
        // Stay here to pick up the missed mogo
    } else {
        // We lost the goal rush
        masterController.print(3, 0, "Goal Rush Lost");
        backClampPnuematic.set_value(0);
        pros::delay(100);
        // Get ready to get the Alliance side mogo
        chassis.turnToPoint(-24 * autonSideDetected, -24, 
            1000, {.forwards = false, .maxSpeed = 70}, false);
        chassis.moveToPoint(-24 * autonSideDetected, -24, 
            2500, {.forwards = false, .maxSpeed = 50}, false);
        backClampPnuematic.set_value(1); 
        pros::delay(100);
        if(goalDetector.get_value()) {
            hookState = HOOK_UP;

            chassis.moveToPose(-64 * autonSideDetected, -64, 225 * autonSideDetected, 
                2500, {.forwards = true, .maxSpeed = 60}, false);
            // Get other color ring and color sort
            chassis.moveToPoint(-56 * autonSideDetected, -56, 
                1000, {.forwards = false, .maxSpeed = 50}, false);
            chassis.moveToPoint(-64 * autonSideDetected, -64, 
                1000, {.forwards = false, .maxSpeed = 50}, false);
            // Get our ring (3rd on stack)
            chassis.moveToPoint(-56 * autonSideDetected, -56, 
                1000, {.forwards = false, .maxSpeed = 50}, false);
            chassis.moveToPoint(-64 * autonSideDetected, -64, 
                1000, {.forwards = false, .maxSpeed = 50}, false);
            
            // Get ready to attack
            chassis.turnToPoint(-8 * autonSideDetected, -48, 
                1000, {.forwards = false, .maxSpeed = 70}, false);
            // Get ready to get the Alliance side mogo
            chassis.moveToPoint(-8 * autonSideDetected, -48, 
                1500, {.forwards = false, .maxSpeed = 70}, false);
            chassis.turnToPoint(48 * autonSideDetected, -48, 
                1000, {.forwards = false, .maxSpeed = 70}, false);    
        }
        // Stay here to pick up the missed mogo
    }
}
