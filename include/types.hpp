#pragma once

enum RingDetectorState {
    BLUE_RING_DETECTED = -1,
    NO_RING_DETECTED = 0,
    RED_RING_DETECTED = 1
};

enum HookState {
    HOOK_DOWN = -100,
    HOOK_STOPPED = 0,
    HOOK_UP = 100,
    HOOK_UP_AUTON = 75  // 75% speed for autonomous (represented as an integer percentage)
};