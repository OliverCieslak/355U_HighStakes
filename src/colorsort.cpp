#include "colorsort.hpp"

// These are the actual definitions
RingDetectorState prevColorState = NO_RING_DETECTED;
RingDetectorState currentColorState = NO_RING_DETECTED;

// Function to cycle through the color states based on the current hue
void getAutonColorState()
{
    firstRingColorSensor.set_led_pwm(25);
    secondRingColorSensor.set_led_pwm(25);
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
    int firstColorDistance = (int)firstRingColorSensor.get_proximity();
    int secondColorDistance = (int)secondRingColorSensor.get_proximity();
    if (firstColorDistance < 100 && secondColorDistance < 100)
    {
        return RingDetectorState::NO_RING_DETECTED;
    }

    int firstHue = (int)firstRingColorSensor.get_hue(); // Get the current hue from the sensor
    int secondHue = (int)secondRingColorSensor.get_hue(); // Get the current hue from the sensor

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

void hookColorSort()
{
    prevColorState = currentColorState;
    currentColorState = getRingDetectorState();
    static int ringsSorted = 0;
    int firstColorDistance = (int)firstRingColorSensor.get_proximity();
    int secondColorDistance = (int)secondRingColorSensor.get_proximity();
    static int time_to_flick = -1;
    if(colorSortEnabled){
        firstRingColorSensor.set_led_pwm(100);
        secondRingColorSensor.set_led_pwm(100);
    } else {
        firstRingColorSensor.set_led_pwm(0);
        secondRingColorSensor.set_led_pwm(0);
    }
    int hookUpVoltage = 12000;
    if (hookState == HOOK_UP_AUTON) {
        hookUpVoltage = .75 * 12000;
    }
    if (hookState == HOOK_STOPPED) {
        IntakeStageTwo.move_voltage(0);
    } else if (hookState == HOOK_DOWN) {
        IntakeStageTwo.move_voltage(-12000);
    } else if ((hookState == HOOK_UP) || (hookState == HOOK_UP_AUTON)) {
        if(!colorSortEnabled) {
            IntakeStageTwo.move_voltage(hookUpVoltage);
        } else {
            if(autonSideDetected == RED_SIDE_AUTON)
            {
                if(currentColorState == BLUE_RING_DETECTED)
                {
                    printf("Blue ring detected %d\n", ringsSorted++);
                    if(secondColorDistance < 100 || firstColorDistance < 100) {
                        IntakeStageTwo.move_voltage(-6000);
                    } else {
                        IntakeStageTwo.move_voltage(hookUpVoltage);
                    }
                } else {
                    IntakeStageTwo.move_voltage(hookUpVoltage);
                }
            } else {
                if(currentColorState == RED_RING_DETECTED)
                {
                    printf("Red ring detected %d\n", ringsSorted++);
                    if(secondColorDistance < 100 || firstColorDistance < 100) {
                        IntakeStageTwo.move_voltage(-6000);
                    } else {
                        IntakeStageTwo.move_voltage(hookUpVoltage);
                    }
                }
                else
                {
                    IntakeStageTwo.move_voltage(hookUpVoltage);
                }
            }
        }
    }
}