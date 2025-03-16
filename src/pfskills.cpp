#include "autons.h"
#include "globals.h"
#include "ladybrown.hpp"
#include "particle.hpp"

void pfSkills() {
    char buffer[256];
    // stopLemLibTrackingTask();
    lemlib::Pose initialPose = lemlib::Pose(-60, 0, 90);
    chassis.setPose(initialPose);

    /*
    std::vector<ParticleFilter::SensorMount> sideSensorMounts = {
        {&leftDistanceSensor, lemlib::Pose(4.5, 4.5, lemlib::degToRad(-90))},
        {&rightDistanceSensor, lemlib::Pose(-2.0, -4.5, lemlib::degToRad(90))},
    };
    ParticleFilter particleFilter(1000, sideSensorMounts);
    */
    console.println("PF Skills");
    printf("PF Skills\n");
    masterController.print(0, 0, "PF Skills");
    chassis.cancelAllMotions();
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    colorSortEnabled = false;

    // load ring on alliance stake

    hookState = HOOK_UP;
    pros::delay(400);
    hookState = HOOK_STOPPED;
    backClampPnuematic.set_value(0);

    chassis.moveToPoint(-48, 0, 1000, {.forwards = true, .maxSpeed = 70}, false);
    chassis.turnToPoint(-48, -24, 1000, {.forwards = false, .direction = lemlib::AngularDirection::AUTO});
    chassis.moveToPoint(-48, -24, 3000, {.forwards = false, .maxSpeed = 40}, false);
    backClampPnuematic.set_value(1); // Get first Mogo
    pros::delay(150);
    IntakeStageOne.move_voltage(-12000);

    // Get first ring by ladder
    pros::delay(150);
    hookState = HOOK_UP; 

    chassis.turnToPoint(-24, -24, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70});
    chassis.moveToPoint(-24, -24, 3000, {.forwards = true, .maxSpeed = 50}, false);

    // Next ring by Wall Stake
    chassis.turnToPoint(0, -58, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, -58, 3000, {.forwards = true, .maxSpeed = 60}, false);
    pros::delay(200);
    chassis.moveToPoint(0, -48, 2000, {.forwards = false, .maxSpeed = 70}, false);
    // Come back to corner
    chassis.turnToPoint(-24, -48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-56, -48, 4000, {.forwards = true, .maxSpeed = 40}, false);
    pros::delay(250);

    // Back up for last Ring
    chassis.moveToPoint(-48, -48, 2000, {.forwards = false, .maxSpeed = 70}, false);
    chassis.turnToPoint(-48, -60, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48, -60, 1000, {.forwards = true, .maxSpeed = 50}, false);
    pros::delay(250);
    chassis.moveToPoint(-48, -48, 1000, {.forwards = true, .maxSpeed = 50}, false);

    // Back into corner and drop Mogo
    chassis.turnToPoint(0, 0, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-58, -58, 2000, {.forwards = false, .maxSpeed = 70}, false);
    backClampPnuematic.set_value(0);
    pros::delay(150);
    
    // Move away from corner and start next Mogo
    chassis.moveToPoint(-48, -48, 2000, {.forwards = true, .maxSpeed = 70}, false);

    // Use Particle Filter to orient robot
    /*particleFilter.initialize(chassis.getPose(), &chassis, 1.0, lemlib::degToRad(5));
    particleFilter.update(chassis.getPose());
    particleFilter.measurementUpdate();
    particleFilter.resample();
    lemlib::Pose pfPose = particleFilter.getEstimatedPose();
    chassis.setPose(pfPose);*/

    chassis.moveToPoint(-48, 12, 4000, {.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48, 24, 2000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(1); // Get second Mogo

    // Get ring by ladder
    chassis.turnToPoint(-24, -24, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-24, 24, 2000, {.forwards = true, .maxSpeed = 70}, false);
    // Get ring by wall stake
    chassis.moveToPoint(0, 60, 4000, {.forwards = true, .maxSpeed = 50}, false);
    pros::delay(200);
    chassis.moveToPoint(0, 48, 2000, {.forwards = false, .maxSpeed = 70}, false);

    // Come back to corner
    chassis.turnToPoint(-60, 48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-60, 48, 6000, {.forwards = true, .maxSpeed = 50}, false);
    // Backup for last ring
    chassis.moveToPoint(-48, 48, 1000, {.forwards = false, .maxSpeed = 50}, false);

    chassis.turnToPoint(-48, 60, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-48, 60, 2000, {.forwards = true, .maxSpeed = 50}, false);
    pros::delay(200);
    // Prepare to drop Mogo (change timeouts back to shorter after debugging)
    chassis.moveToPoint(-48, 48, 2000, {.forwards = false, .maxSpeed = 50}, false);
    // Back into corner and drop Mogo
    chassis.turnToPoint(0, 0, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(-60, 60, 3000, {.forwards = false, .maxSpeed = 50}, false);
    backClampPnuematic.set_value(0); // Drop second Mogo

    // Leave corner and get ready for blue side
    chassis.turnToPoint(0, 48, 1000, {.forwards = true, .direction = lemlib::AngularDirection::AUTO, .maxSpeed = 70}, false);
    chassis.moveToPoint(0, 48, 6000, {.forwards = true, .maxSpeed = 70}, false);
    hookState = HOOK_STOPPED;
    chassis.moveToPoint(24, 48, 3000, {.forwards = true, .maxSpeed = 50}, false);

    // Get nearest mogo with blue ring
    chassis.turnToPoint(60, 22, 1000, {.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(60, 22, 3000, {.forwards = false, .maxSpeed = 50}, false);

    backClampPnuematic.set_value(1); // Clamp 3rd MoGo
    pros::delay(100);

    chassis.turnToPoint(60, 60, 1000, {.forwards = false, .direction = lemlib::AngularDirection::CW_CLOCKWISE, .maxSpeed = 70}, false);
    chassis.moveToPoint(60, 60, 4000, {.forwards = false, .maxSpeed = 60}, false);

    backClampPnuematic.set_value(0); // Clamp 3rd MoGo
    pros::delay(100);
    chassis.moveToPoint(48, 24, 4000, {.forwards = true, .maxSpeed = 60}, false);  // Get 2nd ring by ladder

    chassis.turnToPoint(48, -2, 1000, {.forwards = false, .maxSpeed = 70}, false);
    chassis.moveToPoint(48, -2, 2000, {.forwards = false, .maxSpeed = 50}, false);

    backClampPnuematic.set_value(1); // Clamp 4th MoGo
    pros::delay(100);
    hookState = HOOK_UP;

    chassis.turnToPoint(24, -24, 1000, {.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(24, -24, 2000, {.forwards = true, .maxSpeed = 50}, false);
    chassis.moveToPoint(48, -24, 2000, {.forwards = true, .maxSpeed = 50}, false);

    chassis.turnToPoint(48, -56, 1000, {.forwards = true, .maxSpeed = 70}, false);
    chassis.moveToPoint(48, -56, 2000, {.forwards = true, .maxSpeed = 50}, false);
    chassis.moveToPoint(48, -48, 2000, {.forwards = false, .maxSpeed = 50}, false);

    chassis.moveToPoint(58, -58, 2000, {.forwards = false, .maxSpeed = 50}, false);

    //chassis.moveToPoint(48, -48, 2000, {.forwards = true, .maxSpeed = 50}, false);

    // Cleanup
    IntakeStageOne.move_voltage(0);
    hookState = HOOK_STOPPED;
    backClampPnuematic.set_value(0);
}