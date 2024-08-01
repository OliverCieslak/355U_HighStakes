#pragma once

#include "lemlib/api.hpp" // IWYU pragma: keep
#include "robodash/api.h"

extern int STAGE1_INTAKE_PORT;
extern int STAGE2_INTAKE_PORT;

extern int autonSideDetected;
extern const int RED_SIDE_AUTON;
extern const int BLUE_SIDE_AUTON;

extern pros::Controller masterController;
extern pros::MotorGroup lowIntakeMotor;
extern pros::MotorGroup highIntakeMotor;
extern lemlib::Chassis chassis;

extern rd::Console console;
extern pros::Imu imu;

extern pros::adi::DigitalOut backClampPnuematic;
extern pros::adi::DigitalOut wiperPneumatic;
extern pros::adi::DigitalOut climbArms;

extern int isWiperTaskRunning;

extern rd::Console console;