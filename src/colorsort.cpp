#include "colorsort.hpp"
#include "ladybrown.hpp"

// These are the actual definitions
RingDetectorState prevColorState = NO_RING_DETECTED;
RingDetectorState currentColorState = NO_RING_DETECTED;

// Function to cycle through the color states based on the current hue
void getAutonColorState()
{
    sortingColorSensor.set_led_pwm(100);
    autonColorSensor.set_led_pwm(100);
    pros::delay(100); // Wait for the sensor to stabilize
    RingDetectorState ringDetectorState = getRingDetectorState();
    if (ringDetectorState == RingDetectorState::NO_RING_DETECTED)
    {
        return;
    }
    else if (ringDetectorState == RingDetectorState::RED_RING_DETECTED)
    {
        autonSideDetected = RED_SIDE_AUTON;
    }
    else if (ringDetectorState == RingDetectorState::BLUE_RING_DETECTED)
    {
        autonSideDetected = BLUE_SIDE_AUTON;
    }
}

RingDetectorState getRingDetectorState()
{
    int firstColorDistance = (int)sortingColorSensor.get_proximity();
    int secondColorDistance = (int)autonColorSensor.get_proximity();
    if (firstColorDistance < 100 && secondColorDistance < 100)
    {
        return RingDetectorState::NO_RING_DETECTED;
    }

    int firstHue = (int)sortingColorSensor.get_hue(); // Get the current hue from the sensor
    int secondHue = (int)autonColorSensor.get_hue(); // Get the current hue from the sensor

    // Check the hue range and update the state
    if ((firstHue >= 0 && firstHue <= 20) || (secondHue >= 0 && secondHue <= 20))
    {
        return RingDetectorState::RED_RING_DETECTED;
    }
    else if ((firstHue >= 180 && firstHue <= 240) || (secondHue >= 180 && secondHue <= 240))
    {
        return RingDetectorState::BLUE_RING_DETECTED;
    }
    else if ((firstHue >= 300 && firstHue <= 340) || (secondHue >= 300 && secondHue <= 340))
    {
        return RingDetectorState::RED_RING_DETECTED;
    }
    return RingDetectorState::NO_RING_DETECTED;
}

RingDetectorState getColorSortDetectorState()
{
    int colorDistance = (int)sortingColorSensor.get_proximity();
    if (colorDistance < 100)
    {
        return RingDetectorState::NO_RING_DETECTED;
    }

    int hue = (int)sortingColorSensor.get_hue(); // Get the current hue from the sensor

    // Check the hue range and update the state
    if (hue >= 0 && hue <= 20)
    {
        return RingDetectorState::RED_RING_DETECTED;
    }
    else if (hue >= 180 && hue <= 240)
    {
        return RingDetectorState::BLUE_RING_DETECTED;
    }
    else if (hue >= 300 && hue <= 340)
    {
        return RingDetectorState::RED_RING_DETECTED;
    }
    return RingDetectorState::NO_RING_DETECTED;
}

void hookColorSort()
{
    prevColorState = currentColorState;
    static int ringsSorted = 0;
    int firstColorDistance = (int)sortingColorSensor.get_proximity();
    int secondColorDistance = (int)autonColorSensor.get_proximity();
    static int time_to_flick = -1;
    if(colorSortEnabled){
        sortingColorSensor.set_led_pwm(100);
        autonColorSensor.set_led_pwm(100);
    } else {
        sortingColorSensor.set_led_pwm(0);
        autonColorSensor.set_led_pwm(0);
    }
    currentColorState = getColorSortDetectorState();
 
    // Stall detection parameters for IntakeStageTwo
    const double STAGE2_STALL_CURRENT_THRESHOLD = 2300; // milliamps
    const int STAGE2_STALL_TIME_THRESHOLD = 60; // milliseconds
    static uint32_t stage2StallTimer = 0;
    static bool stage2WasStalled = false;
    const int flickVoltage = 5000;

    int hookUpVoltage = 12000;
    if (hookState == HOOK_UP_AUTON) {
        hookUpVoltage = 9000;
    }
    if (hookState == HOOK_STOPPED) {
        IntakeStageTwo.move_voltage(0);
        stage2StallTimer = 0;
        stage2WasStalled = false;
    } else if (hookState == HOOK_DOWN) {
        IntakeStageTwo.move_voltage(-12000);
        stage2StallTimer = 0;
        stage2WasStalled = false;
    } else if ((hookState == HOOK_UP) || (hookState == HOOK_UP_AUTON)) {
        if(!colorSortEnabled) {
            double current = std::abs(IntakeStageTwo.get_current_draw());
            if (current > STAGE2_STALL_CURRENT_THRESHOLD) {
                if (stage2StallTimer == 0) {
                    stage2StallTimer = pros::millis();
                }
                else if (pros::millis() - stage2StallTimer > STAGE2_STALL_TIME_THRESHOLD) {
                    // Pulse the motor by alternating between forward and backward voltage
                    if (!stage2WasStalled) {
                        stage2WasStalled = true;
                        IntakeStageTwo.move_voltage(12000); // Initial forward pulse
                    } else {
                        // Alternate direction every 200ms
                        if ((pros::millis() % 400) > 200) {
                            IntakeStageTwo.move_voltage(12000); // Forward pulse
                        } else {
                            IntakeStageTwo.move_voltage(-6000); // Backward pulse
                        }
                    }
                }
            } else {
                stage2StallTimer = 0;
                stage2WasStalled = false;
                IntakeStageTwo.move_voltage(12000);
            }
            
        } else {
            double current = std::abs(IntakeStageTwo.get_current_draw());
            if (current > STAGE2_STALL_CURRENT_THRESHOLD) {
                if (stage2StallTimer == 0) {
                    stage2StallTimer = pros::millis();
                }
                else if (pros::millis() - stage2StallTimer > STAGE2_STALL_TIME_THRESHOLD) {
                    // Pulse the motor by alternating between forward and backward voltage
                    if (!stage2WasStalled) {
                        stage2WasStalled = true;
                        IntakeStageTwo.move_voltage(12000); // Initial forward pulse
                    } else {
                        // Alternate direction every 200ms
                        if ((pros::millis() % 400) > 200) {
                            IntakeStageTwo.move_voltage(12000); // Forward pulse
                        } else {
                            IntakeStageTwo.move_voltage(-6000); // Backward pulse
                        }
                    }
                }
            } else {
                stage2StallTimer = 0;
                stage2WasStalled = false;
                   if ((autonSideDetected == RED_SIDE_AUTON) && (prevColorState == BLUE_RING_DETECTED) ||
                       (autonSideDetected == BLUE_SIDE_AUTON) && (prevColorState == RED_RING_DETECTED)) {
                        printf("Was detected\n");
                        IntakeStageTwo.move_voltage(0);
                        pros::delay(25);
                        IntakeStageTwo.move_voltage(hookUpVoltage);
                   } else {
                        IntakeStageTwo.move_voltage(hookUpVoltage);
                }
            }
        }
    }
}