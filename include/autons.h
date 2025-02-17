#include "api.h"
#include "globals.h"
#include "lemlib/api.hpp"
#include "main.h"

#pragma once

#define SIMPLE_ALLIANCE "Simple Alliance"
#define SINGLE_MOGO "Single MoGo"
#define QUAL_GOAL_RUSH "Qual Goal Rush"
#define ELIM_GOAL_RUSH "Elim Goal Rush"
#define QUAL_R_NEG_SIDE "Qual R Neg Side"
#define ELIM_R_NEG_SIDE "Elim R Neg Side"
#define QUAL_B_NEG_SIDE "Qual B Neg Side"
#define ELIM_B_NEG_SIDE "Elim B Neg Side"
#define POS_SIDE_WALL_STAKE "Pos Wall Stake"
#define NEG_SIDE_RING_RUSH "Neg Ring Rush"
#define GOAL_FILL "Goal Fill"
#define TWO_GOAL_SIDE_FILL "Two Goal Side Fill"
#define AUTON_ONE "Auton 1"
#define SAFE_SKILLS "Skills Safe"
#define PF_SKILLS "PF Skills"
#define SCORING_SKILLS "Skills Rings"
#define LINEAR_PID_MOVEMENT "Linear PID Movement"
#define TURN_PID_MOVEMENT "Turn PID Movement"

void qualsGoalRushAutonTweaked();
void elimGoalRushAuton();
void simpleAuton();
void baseNegSide();
void qualNegSide();
void elimNegSide();
void qualRedNegSide();
void elimRedNegSide();
void qualBlueNegSide();
void elimBlueNegSide();

void positiveSideWallStakeAuton();
void negRingRushAuton();

void linearPidMovementTest();
void turnPidMovementTest();

void goalFill();
void twoGoalSideFill();

void simpleAllianceStake();
void simpleSingleMogo();

void safeSkills();
void scoringSkills();
void pfSkills();

void intakeStallDetection();
void stopLemLibTrackingTask();
void startParticleFilter();
void getAutonColorState();
