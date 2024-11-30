#pragma once

#include "lemlib/api.hpp" // IWYU pragma: keep
#include "robodash/api.h"

extern int INTAKE_MOTOR_PORT;
extern int DUMP_TRUCK_MOTOR_PORT;

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
extern pros::Distance backClampDistanceSensor;

extern rd::Console console;