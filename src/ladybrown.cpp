#include "ladybrown.hpp"

void nextLadyBrownState()
{
    int nextState = static_cast<int>(ladyBrownState) + 1;
    if (nextState < LadyBrownState::NUM_STATES)
    {
        ladyBrownState = static_cast<LadyBrownState>(nextState);
    }
}

void prevLadyBrownState()
{
    int prevState = static_cast<int>(ladyBrownState) - 1;
    if (prevState >= 0)
    {
        ladyBrownState = static_cast<LadyBrownState>(prevState);
    }
}

void ladyBrownControl()
{
    //double kp = -1.2;
    double kp = 20;
    double error = ladyBrownStateTargets[static_cast<int>(ladyBrownState)] - LadyBrownMotor.get_position();
    double v = kp * error;

    // State transition check
    static LadyBrownState previousState = ladyBrownState;
    if (previousState == LadyBrownState::LOADING && ladyBrownState == LadyBrownState::SCORING) {
        IntakeStageTwo.move_relative(-50, 200); // Move backward a little
    }
    previousState = ladyBrownState;

    LadyBrownMotor.move_voltage(v);
}