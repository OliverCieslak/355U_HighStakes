#pragma once

#include "lemlib/api.hpp" // IWYU pragma: keep
#include "robodash/api.h"

extern pros::Controller controller;
extern pros::MotorGroup intakeMotors;
extern lemlib::Chassis chassis;

extern rd::Console console;
extern pros::Imu imu;