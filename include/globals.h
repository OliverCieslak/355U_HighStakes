#pragma once

#include "types.hpp"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "robodash/api.h"
#include "api.h"
#include "pros/optical.hpp"
#include "pros/distance.hpp"
#include "particle.hpp"

extern int Stage_One_Intake;
extern int Stage_Two_Intake;

extern int autonSideDetected;
extern const int RED_SIDE_AUTON;
extern const int BLUE_SIDE_AUTON;

extern pros::Controller masterController;
extern pros::Motor IntakeStageOne;
extern pros::Motor IntakeStageTwo;
extern pros::Motor LadyBrownMotor;
extern lemlib::Chassis chassis;

extern rd::Console console;
extern pros::Imu imu;

extern pros::adi::DigitalOut backClampPnuematic;
extern pros::adi::DigitalOut leftDoinker;
extern pros::adi::DigitalOut rightDoinker;
extern pros::Optical sortingColorSensor;
extern pros::Optical autonColorSensor;
extern pros::Distance backDistanceSensor;
extern pros::Distance frontDistanceSensor;
extern pros::Distance leftDistanceSensor;
extern pros::Distance rightDistanceSensor;
extern pros::adi::DigitalIn goalDetector;
extern pros::Rotation lbRotationSensor;

extern rd::Console console;
extern int MIN_INTAKE_TIME;
extern int POST_INTAKE_DELAY;
extern int MAX_INTAKE_TIME;
 
extern int ladyBrownStateTargets[];
extern int ladyBrownRotationStateTargets[];

extern RingDetectorState prevColorState;
extern RingDetectorState currentColorState;
extern HookState hookState;
extern bool colorSortEnabled;

extern pros::Task *trackingTask;
extern ParticleFilter particleFilter;