#pragma once

#include "globals.h"

enum LadyBrownState {
    RESTING = 0,
    LOADING,
    //VERTICAL,
    SCORING,
    HORIZONTAL,
    // DOWN_LOW,
    NUM_STATES
};

void nextLadyBrownState();
void prevLadyBrownState();
void ladyBrownControl();

extern LadyBrownState ladyBrownState;
