#include "autons.h"
#include "globals.h"

void auton0()
{
    // Auton 0 code here
}

void simple_auton()
{
    // Simple auton code here
}

void skills()
{
    // Skills code here
}

void linear_pid_movement()
{
    double start_heading = imu.get_heading();
    // set position to x:0, y:0, heading:0
    chassis.cancelAllMotions();
    chassis.setPose(0, 0, 0);
    pros::delay(200);
    lemlib::Pose start_pose = chassis.getPose();
    // move 48" forwards
    chassis.moveToPoint(0, 48, 10000);
    pros::delay(500); // Need to wait for odom to sync

    lemlib::Pose pose = chassis.getPose();
    double end_heading = imu.get_heading();
    float distance_travelled = pose.distance(start_pose);
    float heading_change = pose.theta - start_pose.theta;

    char buffer[100];
    sprintf(buffer, "d:%.1f,x:%.1f,y:%.1f,h:%.1f,eh:%.1f,dh:%.1f", distance_travelled, pose.x, pose.y, pose.theta, end_heading, heading_change);
    console.println(buffer);
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