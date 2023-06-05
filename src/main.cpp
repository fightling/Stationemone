/**
 * Author Teemu Mäntykallio
 * Initializes the library and runs the stepper
 * motor in alternating directions.
 */

#include <TMCStepper.h>
#include <AccelStepper.h>

#define STEP_PIN 14
#define DIR_PIN 12
#define CS_PIN 5   // Chip select
#define SW_MOSI 23 // Software Master Out Slave In (MOSI)
#define SW_MISO 19 // Software Master In Slave Out (MISO)
#define SW_SCK 18  // Software Slave Clock (SCK)

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075
#define STALL_VALUE 0 // [-64..63]

// Select your stepper driver type
TMC2130Stepper driver(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK); // Software SPI
// TMC2130Stepper driver(CS_PIN, R_SENSE);

AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

constexpr uint32_t steps_per_mm = 215;

void setup()
{
  // pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(SW_MISO, INPUT_PULLUP);

  Serial.begin(9600);
  // Enable one according to your setup
  // SPI.begin();    // SPI drivers
  driver.begin();             //  SPI: Init CS pins and possible SW SPI pins
                              //  driver.push();
  driver.rms_current(1900);   // Set motor RMS current
  driver.en_pwm_mode(true);   // Toggle stealthChop on TMC2130/2160/5130/5160
  driver.pwm_autoscale(true); // Needed for stealthChop
  driver.microsteps(16);      // Set microsteps to 1/16th

  stepper.setMaxSpeed(100 * steps_per_mm);
  stepper.setAcceleration(50 * steps_per_mm);
  // stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
}

int distance = 20;
int dir = 0;
int max_n = 10000;
int n = 0;

void loop()
{
  if (stepper.currentPosition() <= 0 && dir != 1)
  {
    dir = 1;
    stepper.disableOutputs();
    delay(1000);
    stepper.setMaxSpeed(10 * steps_per_mm);
    stepper.moveTo(distance * steps_per_mm);
    stepper.enableOutputs();
    n = max_n;
  }
  else if (stepper.currentPosition() >= distance * steps_per_mm && dir != -1)
  {
    dir = -1;
    stepper.disableOutputs();
    delay(1000);
    stepper.moveTo(0 * steps_per_mm);
    stepper.setMaxSpeed(100 * steps_per_mm);
    stepper.enableOutputs();
    n = max_n;
  }
  if (++n > max_n)
  {
    n = 0;
    Serial.println(stepper.currentPosition() / steps_per_mm);
  }
  stepper.run();
}
