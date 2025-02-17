#include "autons.h"
#include "globals.h"
#include "ladybrown.hpp"

void intakeStallDetection() {
    int startTime = pros::millis();
    IntakeStageOne.move_voltage(-12000);
    pros::delay(MIN_INTAKE_TIME);
    while((pros::millis() - startTime) < MAX_INTAKE_TIME) {
        if(abs(IntakeStageOne.get_actual_velocity()) == 0) {
            IntakeStageOne.move_voltage(12000);
            pros::delay(100);
            IntakeStageOne.move_voltage(-12000);
        } else {
            pros::delay(20);
        }
    }
    pros::delay(POST_INTAKE_DELAY);
    IntakeStageOne.move_voltage(0);
}