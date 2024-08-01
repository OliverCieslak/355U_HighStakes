#include "autons.h"
#include "globals.h"

void goalRushAuton()
{
    // Had to put these here for some reason
    pros::Motor intakeStage1(STAGE1_INTAKE_PORT);
    pros::Motor intakeStage2(STAGE2_INTAKE_PORT);

    console.println("goalRushAuton");
    printf("goalRushAuton\n");
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);    
    chassis.moveToPoint(0 * autonSideDetected, -46, 3000 ,{.forwards = false, .minSpeed = 42, .earlyExitRange = 11}, true);
    chassis.waitUntil(22);      // Move past 2 stack
    chassis.cancelAllMotions();
    chassis.moveToPoint(6 * autonSideDetected, -39, 2000 ,{.forwards = false, .maxSpeed = 50}, false);
    chassis.waitUntil(2.5);     // Position for Neutral Goal 
    chassis.moveToPoint(9 * autonSideDetected, -45, 4000 ,{.forwards = false, .maxSpeed = 30}, false);
    backClampPnuematic.set_value(1);  // Grab Neutral Goal
    chassis.cancelAllMotions();
    pros::delay(250);


    chassis.moveToPoint(0 * autonSideDetected, -40, 3000 ,{.forwards = true}, true);
    chassis.waitUntil(8);  // Steal Goal
    chassis.cancelAllMotions();
    chassis.moveToPoint(39 * autonSideDetected, -42, 3000 ,{.forwards = false}, true);
    chassis.waitUntil(20);  // Bump goal on ladder to position it for scoring
    chassis.cancelAllMotions();
    intakeStage2.move_velocity(127);
    pros::delay(1000);      // Put a ring on it
    intakeStage2.move_velocity(0);
    chassis.moveToPoint(20 * autonSideDetected, -20, 3000 ,{.forwards = true}, true);
    chassis.waitUntil(16);  // Bump 2 stack over
    backClampPnuematic.set_value(0);  // Release Neutral Goal
    chassis.cancelAllMotions();
    intakeStage1.move_velocity(127);  // Intake Ring from bottom
    pros::delay(500);
    chassis.moveToPoint(16 * autonSideDetected, -12, 3000 ,{.forwards = true, .maxSpeed=30}, true);
    chassis.waitUntil(12);   // Drive to get Ring
    chassis.cancelAllMotions();
    chassis.moveToPoint(39 * autonSideDetected, -32, 3000 ,{.forwards = false, .maxSpeed=80}, true);
    chassis.waitUntil(8);  // Position towards goal



    chassis.moveToPoint(44 * autonSideDetected, -36, 3000 ,{.forwards = false, .maxSpeed=30}, true);
    chassis.waitUntil(4);  // Line up on Goal
    backClampPnuematic.set_value(1);
    chassis.cancelAllMotions();
    pros::delay(250);

    chassis.moveToPoint(48 * autonSideDetected, -36, 3000 ,{.forwards = false, .maxSpeed=80}, true);
    chassis.waitUntil(22);  // Bump goal on ladder to position it for scoring
    chassis.cancelAllMotions();
    chassis.moveToPoint(48 * autonSideDetected, -20, 3000 ,{.forwards = false, .maxSpeed=80}, true);
    chassis.waitUntil(12);  // Position to drop goal
    chassis.cancelAllMotions();


    intakeStage2.move_velocity(127);
    pros::delay(3250);  // Score Ring
    intakeStage1.move_velocity(0);
    intakeStage2.move_velocity(0);
    backClampPnuematic.set_value(0);
    pros::delay(250);  // Release Goal
    chassis.moveToPoint(50 * autonSideDetected, -34, 3000 ,{.forwards = true, .maxSpeed=80}, true);
    chassis.waitUntil(18);  // Touch ladder
    chassis.cancelAllMotions();
}

void simple_auton()
{ 
    chassis.setPose(0, 0, 0);
    chassis.cancelAllMotions();
    chassis.moveToPoint(0 * autonSideDetected, 50, 4000 ,{.forwards = true, .maxSpeed = 80}, true);
}

void skills()
{
    // Skills code here
}

void linear_pid_movement()
{
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
    double start_heading = imu.get_heading();
    // set position to x:0, y:0, heading:0
    chassis.cancelAllMotions();
    lemlib::Pose start_pose = chassis.getPose();
    printf("Starting pose: x:%.1f, y:%.1f, h:%.1f\n", start_pose.x, start_pose.y, start_pose.theta);
    // move 24" forwards
    chassis.moveToPoint(0, 12, 10000);
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


    pros::delay(500); // Need to wait for odom to sync
    pose = chassis.getPose();
    end_heading = imu.get_heading();
    distance_travelled = pose.distance(start_pose);
    heading_change = pose.theta - start_pose.theta;
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

    pros::delay(500); // Need to wait for odom to sync
    pose = chassis.getPose();
    end_heading = imu.get_heading();
    distance_travelled = pose.distance(start_pose);
    heading_change = pose.theta - start_pose.theta;
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

void turn_pid_movement()
{
    double start_heading = imu.get_heading();
    // set position to x:0, y:0, heading:0
    chassis.setPose(0, 0, 0);
    pros::delay(200);
    // turn to face heading 90 with a very long timeout
    chassis.turnToHeading(90, 100000);
    pros::delay(500); // Need to wait for odom to sync

    lemlib::Pose pose = chassis.getPose();
    double end_heading = imu.get_heading();
    char buffer[100];
    sprintf(buffer, "x: %.1f, y: %.1f, h: %.1f, eh: %.1f", pose.x, pose.y, pose.theta, end_heading);
    console.println(buffer);
}

void GoalFill()
{
    pros::Motor intakeStage1(STAGE1_INTAKE_PORT);
    pros::Motor intakeStage2(STAGE2_INTAKE_PORT);

    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -15, 3000 ,{.forwards = false, .maxSpeed=80}, false);
    //chassis.waitUntil(4);  

    chassis.moveToPoint(9 * autonSideDetected, -27, 3000 ,{.forwards = false, .maxSpeed=80}, false);
    //chassis.waitUntil(4); 

    backClampPnuematic.set_value(1); 
    pros::delay(500);

    intakeStage2.move_velocity(127);
    pros::delay(1500);

    intakeStage2.move_velocity(0);
    pros::delay(500);

    chassis.moveToPoint(12 * autonSideDetected, -30, 3000 ,{.forwards = false, .maxSpeed=80}, false);
    //chassis.waitUntil(1);  

    chassis.moveToPoint(20* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=80}, true);
    chassis.waitUntil(8);  

    pros::delay(500);

    intakeStage1.move_velocity(127); 

    chassis.moveToPoint(30* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=20}, true);
    chassis.waitUntil(10);
    pros::delay(100);

    backClampPnuematic.set_value(0); 
    pros::delay(300);

    backClampPnuematic.set_value(1); 
    pros::delay(1100);

    intakeStage2.move_velocity(127);
    pros::delay(2000);

    intakeStage1.move_velocity(0);
    pros::delay(2500);

    intakeStage2.move_velocity(0);

    chassis.moveToPoint(20* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=80}, true);
    chassis.waitUntil(10);

    backClampPnuematic.set_value(0); 
    
    chassis.moveToPoint(-5 * autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=80}, true);
    chassis.waitUntil(25); 

    chassis.moveToPoint(-6* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=10}, true);
    chassis.waitUntil(1);
}


void TwoGoalSideFill()
{
    pros::Motor intakeStage1(STAGE1_INTAKE_PORT);
    pros::Motor intakeStage2(STAGE2_INTAKE_PORT);

    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    lemlib::Pose start_pose = chassis.getPose();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE); 

    chassis.moveToPoint(0 * autonSideDetected, -15, 3000 ,{.forwards = false, .maxSpeed=80}, false);
    //chassis.waitUntil(4);  

    chassis.moveToPoint(-9 * autonSideDetected, -27, 3000 ,{.forwards = false, .maxSpeed=80}, false);
    //chassis.waitUntil(4); 

    backClampPnuematic.set_value(1); 
    pros::delay(500);

    intakeStage2.move_velocity(127);
    pros::delay(1500);

    intakeStage2.move_velocity(0);
    pros::delay(500);

    chassis.moveToPoint(-12 * autonSideDetected, -30, 3000 ,{.forwards = false, .maxSpeed=80}, false);
    //chassis.waitUntil(1);  

    chassis.moveToPoint(-22* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=80}, true);
    chassis.waitUntil(10);  

    intakeStage1.move_velocity(127); 

    chassis.moveToPoint(-33* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=20}, true);
    chassis.waitUntil(11);
    pros::delay(100);

    backClampPnuematic.set_value(0); 
    pros::delay(300);

    backClampPnuematic.set_value(1); 
    pros::delay(1100);

    intakeStage2.move_velocity(127);
    pros::delay(1000);

    //chassis.moveToPoint(-23* autonSideDetected, -30, 3000 ,{.forwards = false, .maxSpeed=80}, true);
    //chassis.waitUntil(10);

    intakeStage1.move_velocity(0);
    pros::delay(500);

    intakeStage2.move_velocity(0);

    chassis.moveToPoint(-20* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=80}, true);
    chassis.waitUntil(10);

    intakeStage2.move_velocity(127);
    pros::delay(1000);
    intakeStage2.move_velocity(0);

    backClampPnuematic.set_value(0); 
    
    chassis.moveToPoint(5 * autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=40}, true);
    chassis.waitUntil(15); 

    chassis.moveToPoint(10* autonSideDetected, -30, 3000 ,{.forwards = true, .maxSpeed=10}, true);
    chassis.waitUntil(5);

}


void do_wiper()
{
    while(true) {
        if(isWiperTaskRunning == 1) {
            wiperPneumatic.set_value(1);
            pros::delay(500);
            wiperPneumatic.set_value(0);
            isWiperTaskRunning = 0;
        }
        pros::delay(10);
    }
}