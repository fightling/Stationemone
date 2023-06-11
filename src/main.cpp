#include "Stepper.h"
#include "Artnet.h"
#include "config.h"

Stepper::Controller stepper(paramsStepper);
Artnet::Client<1> artnet(paramsArtnet);

enum Move
{
  Auto = 0,
  In = 1,
  Out = 2,
  Position = 3,
};

void setup()
{
  Serial.begin(9600);
  delay(1000);
  artnet.setup();
  stepper.setup();
}

uint8_t last[2] = {0, 0};

void loop()
{
  stepper.loop();
  uint8_t *data = artnet.get_data();
  if (data)
  {
    if (last[0] != data[0])
    {
      Serial.println(data[0]);
      switch (data[0])
      {
      case Auto:
        Serial.println("auto move");
        stepper.auto_move();
        break;
      case In:
        Serial.println("move in");
        stepper.move_in();
        break;
      case Out:
        Serial.println("move out");
        stepper.move_out();
        break;
      case Position:
        Serial.println("position");
        stepper.position_move();
        break;
      }
    }

    if (last[1] != data[1])
    {
      stepper.set_position(data[1]);
    }

    memcpy(last, data, sizeof(last));
  }
}
