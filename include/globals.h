#pragma once

#include "lemlib/api.hpp" // IWYU pragma: keep
#include "robodash/api.h"

extern int Stage_One_Intake;
extern int Stage_Two_Intake;

extern int autonSideDetected;
extern const int RED_SIDE_AUTON;
extern const int BLUE_SIDE_AUTON;

extern pros::Controller masterController;
extern pros::Motor IntakeStageOne;
extern pros::Motor IntakeStageTwo;
extern lemlib::Chassis chassis;

extern rd::Console console;
extern pros::Imu imu;

extern pros::adi::DigitalOut backClampPnuematic;
extern pros::adi::DigitalOut liftPnuematic;
extern pros::Optical firstRingColorSensor;
extern pros::Distance backClampDistanceSensor;
extern pros::Distance frontDistanceSensor;
extern pros::Distance lefttDistanceSensor;
extern pros::Distance rightDistanceSensor;

extern rd::Console console;
extern int MIN_INTAKE_TIME;
extern int POST_INTAKE_DELAY;
extern int MAX_INTAKE_TIME;