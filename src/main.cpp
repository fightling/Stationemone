#include "Stepper.h"
#include "config.h"

Stepper::Controller stepper(params);

void setup()
{
  stepper.setup();
}

void loop()
{
  stepper.loop();
}
