#include "ladybrown.hpp"

void nextLadyBrownState()
{
    int nextState = static_cast<int>(ladyBrownState) + 1;
    if (nextState < LadyBrownState::NUM_STATES) {
        ladyBrownState = static_cast<LadyBrownState>(nextState);
    }
}

void prevLadyBrownState()
{
    int prevState = static_cast<int>(ladyBrownState) - 1;
    if (prevState >= 0) {
        ladyBrownState = static_cast<LadyBrownState>(prevState);
    }
}

void ladyBrownControl() {
    double kp = 20; // TODO - Tune this with the rotation sensor when ready
    // TODO - Switch this to rotation sensor
    double error = ladyBrownStateTargets[static_cast<int>(ladyBrownState)] - LadyBrownMotor.get_position();
    double v = kp * error;
    LadyBrownMotor.move_voltage(v);
}