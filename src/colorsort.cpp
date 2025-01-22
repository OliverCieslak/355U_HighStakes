#include "colorsort.hpp"

// These are the actual definitions
RingDetectorState prevColorState = NO_RING_DETECTED;
RingDetectorState currentColorState = NO_RING_DETECTED;

// Function to cycle through the color states based on the current hue
void getAutonColorState()
{
    firstRingColorSensor.set_led_pwm(25);
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
    int colorDistance = (int)firstRingColorSensor.get_proximity();
    if (colorDistance < 100)
    {
        return RingDetectorState::NO_RING_DETECTED;
    }

    int hue = (int)firstRingColorSensor.get_hue(); // Get the current hue from the sensor

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
    currentColorState = getRingDetectorState();
    if (hookState == HOOK_STOPPPED)
    {
        IntakeStageTwo.move_voltage(0);
    } else if (hookState == HOOK_DOWN)
    {
        IntakeStageTwo.move_voltage(-12000);
    } else if (hookState == HOOK_UP)
    {
        if(!colorSortEnabled) 
        {
            IntakeStageTwo.move_voltage(12000);
        } 
        else 
        {
            if(autonSideDetected == RED_SIDE_AUTON)
            {
                if(currentColorState == BLUE_RING_DETECTED)
                {
                    printf("Blue ring detected\n");
                    IntakeStageTwo.move_voltage(-12000);
                }
                else
                {
                    IntakeStageTwo.move_voltage(12000);
                }
            }
            else 
            {
                if(currentColorState == RED_RING_DETECTED)
                {
                    printf("Red ring detected\n");
                    IntakeStageTwo.move_voltage(-12000);
                }
                else
                {
                    IntakeStageTwo.move_voltage(12000);
                }
            }

        }
    }
}