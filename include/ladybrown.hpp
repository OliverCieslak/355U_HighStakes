#pragma once

#include "globals.h"

enum LadyBrownState {
    RESTING = 0,
    LOADING = 1,
    VERTICAL = 2,
    SCORE_DOWN = 3,
    HORIZONTAL = 4,
    DOWN_LOW = 5,
    NUM_STATES
};

void nextLadyBrownState();
void prevLadyBrownState();
void ladyBrownControl();

extern LadyBrownState ladyBrownState;