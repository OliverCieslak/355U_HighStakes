#include "autons.h"
#include "globals.h"

int MIN_INTAKE_TIME = 500;
int POST_INTAKE_DELAY = 500;
int MAX_INTAKE_TIME = 3000;

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

void elimGoalRushAuton()
{   
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
        hookState = HOOK_UP_AUTON;
        pros::delay(2500);

        chassis.moveToPoint(16 * autonSideDetected, 2, 
                            4000, {.forwards = true, .maxSpeed = 90}, false);
    } else {
        // We lost the goal rush
        masterController.print(3, 0, "Goal Rush Lost");
 
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
                            4000, {.forwards = true, .maxSpeed = 80}, false);

        pros::delay(100);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 80}, false);
        /* There's not enough time to get 2 corner rings and get in position
        // Try to get the (2nd) other alliance ring from corner and hope color sort works
        chassis.moveToPoint(28 * autonSideDetected, -5, 
                            4000, {.forwards = true, .maxSpeed = 50}, false);
        pros::delay(100);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 80}, false);
        // Try to get the 3rd (alliance) ring from corner
        chassis.moveToPoint(28 * autonSideDetected, -5, 
                            4000, {.forwards = true, .maxSpeed = 80}, false);
        chassis.moveToPoint(23 * autonSideDetected, 0, 
                            1000, {.forwards = false, .maxSpeed = 50}, false);
        */

        // Get ready to place goal in corner
        if(autonSideDetected == RED_SIDE_AUTON) {
            chassis.turnToHeading(0 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE, .maxSpeed = 70}, false);
        } else {
            chassis.turnToHeading(0 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE, .maxSpeed = 70}, false);
        }
        backClampPnuematic.set_value(1); 
        pros::delay(100);
        IntakeStageOne.move_voltage(0);
        hookState = HOOK_STOPPED;
        // For Elims, position right on the line to rush them or defend
        chassis.moveToPoint(10 * autonSideDetected, 32, 
                            2000, {.forwards = true, .maxSpeed = 127}, false);
        if(autonSideDetected == RED_SIDE_AUTON) {
            chassis.turnToHeading(180 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE, .maxSpeed = 70}, false);
        } else {
            chassis.turnToHeading(180 * autonSideDetected, 2000, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE, .maxSpeed = 70}, false);
        }
    }
    return;

    chassis.cancelAllMotions(); 
    chassis.moveToPoint(36 * autonSideDetected, 4, 
                        2000, {.forwards = true, .maxSpeed = 40}, false);
    chassis.cancelAllMotions();
    chassis.moveToPoint(22 * autonSideDetected, 6, 
                        2000, {.forwards = false, .maxSpeed = 40}, false);
    chassis.cancelAllMotions();
    chassis.moveToPoint(36 * autonSideDetected, 4, 
                        2000, {.forwards = true, .maxSpeed = 40}, false);
    chassis.cancelAllMotions();
    chassis.moveToPoint(22 * autonSideDetected, 6, 
                        2000, {.forwards = false, .maxSpeed = 40}, false);
    chassis.cancelAllMotions();
    chassis.moveToPoint(36 * autonSideDetected, 4, 
                        2000, {.forwards = true, .maxSpeed = 40}, false);
    chassis.cancelAllMotions();

    chassis.moveToPoint(22 * autonSideDetected, 8, 
                        4000, {.forwards = false, .maxSpeed = 60}, false);
    chassis.cancelAllMotions();

    chassis.turnToHeading((chassis.getPose().theta) + 180, 1000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE}, false);

    backClampPnuematic.set_value(0); 

    /*hookState = HOOK_STOPPPED; 

    // Get ready to place goal rush in corner
    if(autonSideDetected == RED_SIDE_AUTON) {
        chassis.turnToHeading(0 * autonSideDetected, 1000, {.direction = lemlib::AngularDirection::CCW_COUNTERCLOCKWISE}, false);
    } else {
        chassis.turnToHeading(0 * autonSideDetected, 1000, {.direction = lemlib::AngularDirection::CW_CLOCKWISE}, false);
    }
    chassis.cancelAllMotions();
    pros::delay(200);
    backClampPnuematic.set_value(0);
    
    // Get ready to get ring
    IntakeStageOne.move_velocity(-127);

    chassis.moveToPoint(-12 * autonSideDetected, 24, 
                        4000, {.forwards = true, .maxSpeed = 40}, false);
    
    // Now we have the ring and need to get the MoGo
    chassis.turnToHeading(95 * autonSideDetected, 1000, {.direction = lemlib::AngularDirection::AUTO}, false);
    chassis.moveToPoint(-32 * autonSideDetected, 32, 
                        4000, {.forwards = false, .maxSpeed = 40}, false);

    backClampPnuematic.set_value(1);
    pros::delay(250);
    hookState = HOOK_UP;
    pros::delay(500);
    chassis.turnToHeading(45 * autonSideDetected, 1000, {.direction = lemlib::AngularDirection::AUTO}, false);
    IntakeStageOne.move_velocity(0);
    hookState = HOOK_STOPPPED; */
}

void simpleAuton()
{ 
    chassis.setPose(0, 0, 0);
    chassis.cancelAllMotions();
    chassis.moveToPoint(0 * autonSideDetected, 50, 4000 ,{.forwards = true, .maxSpeed = 80}, true);
}

void linearPidMovementTest()
{
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    double start_heading = imu.get_heading();
    // set position to x:0, y:0, heading:0
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    printf("Starting pose: x:%.1f, y:%.1f, h:%.1f\n", start_pose.x, start_pose.y, start_pose.theta);
    // move 24" forwards
    chassis.moveToPoint(0, -48, 10000, {.forwards = false, .maxSpeed = 127});
    chassis.waitUntilDone();
    chassis.cancelAllMotions();

    lemlib::Pose pose = chassis.getPose();
    double end_heading = imu.get_heading();
    float distance_travelled = pose.distance(start_pose);
    float heading_change = pose.theta - start_pose.theta;

    char buffer[100];
    sprintf(buffer, "d:%.0f,h:%.1f\n", distance_travelled, pose.theta);
    console.println(buffer);
    printf(buffer);
    masterController.print(0, 0, buffer);

    pros::delay(500); // Need to wait for odom to sync
    pose = chassis.getPose();
    end_heading = imu.get_heading();
    distance_travelled = pose.distance(start_pose);
    heading_change = pose.theta - start_pose.theta;
    sprintf(buffer, "d:%.0f,h:%.1f\n", distance_travelled, pose.theta);
    console.println(buffer);
    printf(buffer);
    masterController.print(1, 0, buffer);
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

void simpleAllianceStake() {
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -12, 2000 ,{.forwards = false, .maxSpeed = 50}, false); 
    chassis.waitUntil(6);
    chassis.moveToPoint(4 * autonSideDetected, 20, 2000 ,{.forwards = true, .maxSpeed = 50}, false); 
}

void simpleSingleMogo() {
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -24, 2000 ,{.forwards = false, .maxSpeed = 50}, false); 
    chassis.waitUntil(22);
    backClampPnuematic.set_value(1);
    pros::delay(500);
    hookState = HOOK_UP_AUTON;
    pros::delay(100);
    chassis.turnToHeading(180 * autonSideDetected, 1000, {.direction = lemlib::AngularDirection::AUTO});
    chassis.moveToPoint(0 * autonSideDetected, -45, 4000 ,{.forwards = true, .maxSpeed = 50}, false);
    hookState = HOOK_STOPPED;
}

void intakeStallDetection() {
    int startTime = pros::millis();
    IntakeStageOne.move_voltage(-12000);
    pros::delay(MIN_INTAKE_TIME);
    while((pros::millis() - startTime) < MAX_INTAKE_TIME) {
        if(abs(IntakeStageOne.get_actual_velocity()) == 0) {
            IntakeStageOne.move_voltage(12000);
            pros::delay(100);
            IntakeStageOne.move_voltage(-12000);
        } else {
            pros::delay(20);
        }
    }
    pros::delay(POST_INTAKE_DELAY);
    IntakeStageOne.move_voltage(0);
}