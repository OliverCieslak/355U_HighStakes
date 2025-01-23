#include "api.h"
#include "globals.h"
#include "lemlib/api.hpp"
#include "main.h"

#pragma once

#define SIMPLE_ALLIANCE "Simple Alliance"
#define SINGLE_MOGO "Single MoGo"
#define QUAL_GOAL_RUSH "Qual Goal Rush"
#define ELIM_GOAL_RUSH "Elim Goal Rush"
#define GOAL_FILL "Goal Fill"
#define TWO_GOAL_SIDE_FILL "Two Goal Side Fill"
#define AUTON_ONE "Auton 1"
#define SAFE_SKILLS "Skills Safe"
#define SCORING_SKILLS "Skills Rings"
#define LINEAR_PID_MOVEMENT "Linear PID Movement"
#define TURN_PID_MOVEMENT "Turn PID Movement"

void qualsGoalRushAutonTweaked();
void elimGoalRushAuton();
void simpleAuton();

void linearPidMovementTest();
void turnPidMovementTest();

void goalFill();
void twoGoalSideFill();

void simpleAllianceStake();
void simpleSingleMogo();

void safeSkills();
void scoringSkills();

void intakeStallDetection();