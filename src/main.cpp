#include "Stepper.h"
#include "config.h"

Stepper::Controller stepper(params);

void setup()
{
  stepper.init();
}

void loop()
{
  stepper.loop();
}
