#include <Arduino.h>

inline void setPinMode(uint8_t pin, uint8_t mode)
{
  Serial.print("setup pin #");
  Serial.println(pin);
  pinMode(pin, mode);
}

#include "Stepper.h"
#include "Artnet.h"
#include "Led.h"
#include "config.h"

Stepper::Controller stepper(paramsStepper);
Artnet::Client<1> artnet(paramsArtnet);
Led::Controller<Led::Parameters<3>> leds(paramsLed);

enum Move
{
  Auto = 0,
  Position = 1,
};

void blink_wifi()
{
  leds.on(LED_WIFI);
  delay(250);
  leds.off(LED_WIFI);
  delay(250);
}

void setup()
{
  Serial.begin(9600);
  delay(1000);
  leds.setup();

  // say first artnet address with blinking LEDs
  for (uint8_t i = 0; i < paramsArtnet.first; ++i)
  {
    delay(300);
    leds.all(LED_ON);
    delay(300);
    leds.all(LED_OFF);
  }

  stepper.setup();

  if (artnet.setup(blink_wifi))
    leds.on(LED_WIFI);
  else
    leds.off(LED_WIFI);

  // signal initial auto mode
  leds.blink(LED_MODE);
}

uint8_t last[4] = {
    0, // Move 0 = auto, 1 = Artnet
    0, // Position (if Move ==1 ) 0 = out .. 255 = in
    0, // Speed (if Move == 1) 0 = default .. 255 = max
    0, // Acceleration (if Move == 1) 0 = default .. 255 = max
};

void loop()
{
  // manage stepper

  stepper.loop();

  // receive artnet

  uint8_t *data = artnet.get_data();
  if (data)
  {
    if (last[0] != data[0])
    {
      switch (data[0])
      {
      case Auto:
        Serial.println("auto move");
        stepper.auto_move();
        leds.blink(LED_MODE);
        break;
      case Position:
        Serial.println("position");
        stepper.position_move();
        leds.on(LED_MODE);
        break;
      }
    }

    if (data[0] == Position && memcmp(last, data, sizeof(last)))
    {
      stepper.set_position(data[1], data[2], data[3]);
    }
    memcpy(last, data, sizeof(last));
  }
  // set LEDs

  switch (stepper.get_stage())
  {
  case Stepper::MOVING_IN:
    leds.on(LED_MOVING);
    break;
  case Stepper::MOVING_OUT:
    leds.blink(LED_MOVING);
    break;
  default:
    leds.off(LED_MOVING);
  }

  leds.loop();
}
