#include "globals.h"
#include "lemlib/api.hpp"
#include "api.h"
#pragma once

#define SIMPLE_ALLIANCE "Simple Alliance"
#define SINGLE_MOGO "Single MoGo"
#define QUAL_GOAL_RUSH "Qual Goal Rush"
#define GOAL_FILL "Goal Fill"
#define TWO_GOAL_SIDE_FILL "Two Goal Side Fill"
#define AUTON_ONE "Auton 1"
#define SKILLS_RUN "Skills Run"
#define LINEAR_PID_MOVEMENT "Linear PID Movement"
#define TURN_PID_MOVEMENT "Turn PID Movement"

void qualsGoalRushAuton();
void simpleAuton();
void skills();

void linearPidMovementTest();
void turnPidMovementTest();

void goalFill();
void twoGoalSideFill();

void simpleAllianceStake();
void simpleSingleMogo();

void dumpTruckScore(pros::Motor *dumpTruckMotor);