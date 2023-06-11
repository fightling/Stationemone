#include "Stepper.h"

Stepper::Parameters params = {
    .max_speed = 500.0,
    .acceleration = 50.0,
    .rms_current = 900,
    .distance_mm = 21,
    .steps_per_mm = 215,
    .pause_in = {5000, 10000}, // (min/max)
    .pause_out = {0, 30000},   //  (min/max)
    .speed_in = 2,
    .speed_out = 500,
};