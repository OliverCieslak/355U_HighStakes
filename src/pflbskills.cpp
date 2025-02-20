#include "autons.h"
#include "globals.h"
#include "ladybrown.hpp"
#include "particle.hpp"

void pfLadyBrownSkills()
{
    console.println("PF Skills");
    printf("PF Skills\n");
    masterController.print(0, 0, "PF Skills");
    char buffer[256];
    /*
    std::vector<ParticleFilter::SensorMount> sideSensorMounts = {
        {&leftDistanceSensor, lemlib::Pose(4.5, 4.5, lemlib::degToRad(-90))},
        {&rightDistanceSensor, lemlib::Pose(-2.0, -4.5, lemlib::degToRad(90))},
    };
    ParticleFilter particleFilter(1000, sideSensorMounts);
    */
    // stopLemLibTrackingTask();
    chassis.setPose(-60, 0, -90);
    chassis.cancelAllMotions();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    colorSortEnabled = false;

    ladyBrownState = LadyBrownState::HORIZONTAL; // Use lady brown to score alliance stake
    pros::delay(500);
    chassis.moveToPoint(-48, 0, 1000, {.forwards = false, .maxSpeed = 50}, false); // Back up slightly
    ladyBrownState = LadyBrownState::RESTING;

    // Turn to first mogo
    chassis.turnToPoint(-48, 24, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48, 24, 2000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(1); // Get first Mogo
    pros::delay(100);
    IntakeStageOne.move_voltage(-12000); // Get ready to intake the ring
    hookState = HOOK_UP;                 // Score rings

    // Get first ring by ladder
    chassis.turnToPoint(-24, 24, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24, 24, 2000, {.forwards = true, .maxSpeed = 50}, false);

    // Get first ring past wall stake to put on it
    chassis.turnToPoint(24, 48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    hookState = HOOK_STOPPED; // Just intake the ring, will use Lady Brown for it
    chassis.moveToPoint(24, 48, 3000, {.forwards = true, .maxSpeed = 50}, false);

    // Back up to prepare for wall stake
    chassis.moveToPoint(0, 40, 1500, {.forwards = false, .maxSpeed = 50}, false);
    // Face Wall Stake
    chassis.turnToPoint(0, 72, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    ladyBrownState = LadyBrownState::LOADING; // Prepare to load the Lady Brown
    hookState = HOOK_UP;
    pros::delay(400);
    hookState = HOOK_STOPPED;
    chassis.moveToPoint(0, 60, 2000, {.forwards = true, .maxSpeed = 50}, false); // Drive to wall stake
    ladyBrownState = LadyBrownState::SCORING;
    pros::delay(500); // Score the ring on the wall stake
    ladyBrownState = LadyBrownState::RESTING;

    // Back up from wall stake
    chassis.moveToPoint(0, 48, 1000, {.forwards = false, .maxSpeed = 50}, false);
    hookState = HOOK_UP; // Score rings on MoGo

    chassis.turnToPoint(-58, 48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-58, 48, 5000, {.forwards = true, .maxSpeed = 60}, false);
    chassis.moveToPoint(-48, 48, 1000, {.forwards = false, .maxSpeed = 50}, false);

    // Get 6th ring on MoGo
    chassis.turnToPoint(-48, 64, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48, 64, 1000, {.forwards = true, .maxSpeed = 60}, false);

    // Back up to prepare to drop MoGo
    chassis.moveToPoint(-48, 48, 1000, {.forwards = false, .maxSpeed = 50}, false);
    chassis.turnToPoint(-63, 63, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-63, 63, 1000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(0); // Drop first Mogo
    pros::delay(100);
    chassis.moveToPoint(-48, 48, 1000, {.forwards = true, .maxSpeed = 60}, false);

    // Prepare for 2nd MoGo
    chassis.turnToPoint(-48, -24, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48, -6, 4000, {.forwards = false, .maxSpeed = 90}, false);
    chassis.moveToPoint(-48, -24, 2000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(1); // Get 2nd Mogo
    pros::delay(100);

    hookState = HOOK_UP; // Score rings
    chassis.turnToPoint(-24, -24, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24, -24, 2000, {.forwards = true, .maxSpeed = 50}, false); // Get ring by ladder

    // Get ring past wall stake for it
    chassis.turnToPoint(24, -48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    hookState = HOOK_STOPPED; // Just intake the ring, will use Lady Brown for it
    chassis.moveToPoint(24, -48, 3000, {.forwards = true, .maxSpeed = 50}, false);

    chassis.moveToPoint(0, -40, 1500, {.forwards = false, .maxSpeed = 50}, false);
    // Face Wall Stake
    chassis.turnToPoint(0, -72, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    ladyBrownState = LadyBrownState::LOADING; // Prepare to load the Lady Brown
    hookState = HOOK_UP;
    pros::delay(400);
    hookState = HOOK_STOPPED;
    chassis.moveToPoint(0, -60, 2000, {.forwards = true, .maxSpeed = 50}, false); // Drive to wall stake
    ladyBrownState = LadyBrownState::SCORING;
    pros::delay(500); // Score the ring on the wall stake
    ladyBrownState = LadyBrownState::RESTING;

    // Back up from wall stake
    chassis.moveToPoint(0, -48, 1000, {.forwards = false, .maxSpeed = 50}, false);
    hookState = HOOK_UP; // Score rings on MoGo

    chassis.turnToPoint(-58, -48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-58, -48, 5000, {.forwards = true, .maxSpeed = 60}, false);
    chassis.moveToPoint(-48, -48, 1000, {.forwards = false, .maxSpeed = 50}, false);

    // Get 6th ring on MoGo
    chassis.turnToPoint(-48, -64, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48, -64, 1000, {.forwards = true, .maxSpeed = 60}, false);

    // Back up to prepare to drop MoGo
    chassis.moveToPoint(-48, -48, 1000, {.forwards = false, .maxSpeed = 50}, false);
    chassis.turnToPoint(-63, -63, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-63, -63, 1000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(0); // Drop second Mogo
    pros::delay(100);
    hookState = HOOK_STOPPED; // Don't have a MoGo yet.
    chassis.moveToPoint(-48, -48, 1000, {.forwards = true, .maxSpeed = 60}, false);

    // Get a ring all the way by the mogo with a blue ring
    chassis.turnToPoint(58, -48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(58, -48, 6000, {.forwards = true, .maxSpeed = 60}, false);

    // Get the MoGo with the blue ring
    chassis.turnToPoint(58, -24, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(58, -24, 2000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(0); // Get 3rd MoGo
    pros::delay(100);

    chassis.turnToPoint(64, -44, 1000, {.forwards = false, .direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE, .maxSpeed = 70}, false);
    chassis.moveToPoint(64, -44, 2000, {.forwards = false, .maxSpeed = 60}, false);
    ladyBrownState = LadyBrownState::LOADING;
    hookState = HOOK_UP; // Load Lady Brown
    pros::delay(400);
    hookState = HOOK_STOPPED;

    // Get Ring by ladder
    chassis.moveToPoint(24, -24, 2000, {.forwards = true, .maxSpeed = 50}, false);

    // Get empty MoGo
    chassis.turnToPoint(48, 0, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(48, 0, 2000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(1); // Get empty MoGo
    pros::delay(100);

    // Use Lady Brown to score the ring on the Alliance stake
    chassis.turnToPoint(72, 0, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(66, 0, 2000, {.forwards = true, .maxSpeed = 50}, false);
    ladyBrownState = LadyBrownState::HORIZONTAL; // Use lady brown to score alliance stake
    pros::delay(500);
    chassis.moveToPoint(48, 0, 2000, {.forwards = false, .maxSpeed = 50}, false);
    ladyBrownState = LadyBrownState::RESTING;
    
    // Get another ring
    chassis.turnToPoint(24, 24, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    hookState = HOOK_UP; // Score rings on MoGo
    chassis.moveToPoint(24, 24, 2000, {.forwards = true, .maxSpeed = 50}, false);

    // Home stretch for the 4th mogo
    chassis.turnToPoint(24, 48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(24, 48, 2000, {.forwards = true, .maxSpeed = 50}, false);
    chassis.turnToPoint(60, 48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(60, 48, 2000, {.forwards = true, .maxSpeed = 50}, false);
    chassis.moveToPoint(48, 48, 2000, {.forwards = false, .maxSpeed = 50}, false);
    chassis.turnToPoint(48, 60, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(48, 60, 2000, {.forwards = true, .maxSpeed = 50}, false);
    chassis.moveToPoint(48, 48, 2000, {.forwards = false, .maxSpeed = 50}, false);

    // Get ready to drop it
    chassis.turnToPoint(64, 64, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(64, 64, 1000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(0); // Get empty MoGo
    pros::delay(100);
    chassis.moveToPoint(48, 48, 1000, {.forwards = true, .maxSpeed = 50}, false);

}
