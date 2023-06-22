#include "net_config.h"
#include "Stepper.h"
#include "Artnet.h"

#ifndef SSID
#define SSID "ssid"
#define PASSWORD "password"
#endif

constexpr Stepper::Parameters paramsStepper = {
    .max_speed = 1000.0,
    .acceleration = 200.0,
    .rms_current = 800,
    .distance_mm = 21,
    .steps_per_mm = 215,
    .pause_in = {5000, 10000}, // (min/max)
    .pause_out = {0, 30000},   //  (min/max)
    .speed_in = 800,
    .speed_out = 2,
};

constexpr Artnet::Parameters paramsArtnet = {
    .ssid = SSID,
    .password = PASSWORD,
    .universe = 0,
    .first = 20,
};

enum LedId
{
    LED_WIFI = 0,
    LED_MOVING = 1,
    LED_MODE = 2,
};

constexpr Led::Parameters<3> paramsLed = {
    .pins = {
        // Wifi
        33,
        // Moving
        25,
        // Mode (auto=blink, artnet=on)
        32,
    },
};
