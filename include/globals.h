#pragma once

#include "lemlib/api.hpp" // IWYU pragma: keep
#include "robodash/api.h"

extern int INTAKE_MOTOR_PORT;
extern int DUMP_TRUCK_MOTOR_PORT;

extern int autonSideDetected;
extern const int RED_SIDE_AUTON;
extern const int BLUE_SIDE_AUTON;

extern pros::Controller masterController;
extern pros::Motor intakeMotor;
extern pros::Motor dumpTruckMotor;
extern lemlib::Chassis chassis;

extern rd::Console console;
extern pros::Imu imu;

extern pros::adi::DigitalOut backClampPnuematic;
extern pros::Optical firstRingColorSensor;
extern pros::Distance backClampDistanceSensor;
extern pros::Distance frontDistanceSensor;
extern pros::Distance lefttDistanceSensor;
extern pros::Distance rightDistanceSensor;

extern rd::Console console;