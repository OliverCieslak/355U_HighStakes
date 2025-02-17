#pragma once

#include "globals.h"

enum LadyBrownState {
    RESTING = 0,
    LOADING = 1,
    // VERTICAL = 2,
    SCORING = 3,
    HORIZONTAL = 4,
    DOWN_LOW = 5,
    NUM_STATES
};

void nextLadyBrownState();
void prevLadyBrownState();
void ladyBrownControl();
void resetLadyBrown();

extern LadyBrownState ladyBrownState;
extern int ladyBrownPidEnabled;