/**
 * Author Teemu Mäntykallio
 * Initializes the library and runs the stepper
 * motor in alternating directions.
 */

#include <Arduino.h>
#include <TMCStepper.h>

#define STEP_PIN 14
#define DIR_PIN 12
#define CS_PIN 5   // Chip select
#define SW_MOSI 23 // Software Master Out Slave In (MOSI)
#define SW_MISO 19 // Software Master In Slave Out (MISO)
#define SW_SCK 18  // Software Slave Clock (SCK)

#define R_SENSE 0.11f  // Match to your driver
                       // SilentStepStick series use 0.11
                       // UltiMachine Einsy and Archim2 boards use 0.2
                       // Panucatt BSD2660 uses 0.1
                       // Watterott TMC5160 uses 0.075
#define STALL_VALUE 15 // [-64..63]

uint16_t g_rms_current = 1200;
uint16_t g_microsteps = 32;
uint16_t g_delay = 1600;
bool g_dir = true;
bool g_run = true;

using namespace TMC2130_n;

// Select your stepper driver type
// TMC2130Stepper driver(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK); // Software SPI
TMC2130Stepper driver(CS_PIN, R_SENSE);
void setup()
{
  // pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(SW_MISO, INPUT_PULLUP);

  Serial.begin(9600);
  // Enable one according to your setup
  SPI.begin(); // SPI drivers

  driver.begin();          //  SPI: Init CS pins and possible SW SPI pins
  driver.toff(5);          // Enables driver in software
  driver.rms_current(800); // Set motor RMS current
  driver.microsteps(32);   // Set microsteps to 1/16th
  driver.sgt(STALL_VALUE);

  driver.en_pwm_mode(true); // Toggle stealthChop on TMC2130/2160/5130/5160
  // driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208/2209/2224
  driver.pwm_autoscale(true); // Needed for stealthChop
}

void loop()
{
  // for (int i = 0; i < 10000; i++)
  {
    if (g_run)
    {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(g_delay);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(g_delay);
    }

    if (driver.stallguard())
      Serial.println("STALL");

    if (Serial.available() > 0)
    {
      switch (Serial.read())
      {
      case '1':
        g_rms_current += 100;
        Serial.print("rms current = ");
        Serial.println(g_rms_current);
        break;
      case 'q':
        if (g_rms_current > 0)
          g_rms_current -= 100;
        Serial.print("rms current = ");
        Serial.println(g_rms_current);
        break;
      case '2':
        if (g_microsteps < 256)
          g_microsteps *= 2;
        Serial.print("microsteps  = ");
        Serial.println(g_microsteps);
        break;
      case 'w':
        g_microsteps /= 2;
        Serial.print("microsteps  = ");
        Serial.println(g_microsteps);
        break;
      case '3':
        g_delay += 100;
        Serial.print("delay       = ");
        Serial.println(g_delay);
        break;
      case 'e':
        if (g_delay > 0)
          g_delay -= 100;
        Serial.print("delay       = ");
        Serial.println(g_delay);
        break;
      case '^':
        g_dir = !g_dir;
        Serial.print("dir         = ");
        Serial.println(g_dir ? "OUT" : "IN");
        break;
      case ' ':
        g_run = !g_run;
        Serial.print("run         = ");
        Serial.println(g_dir ? "ON" : "OFF");
        break;
      }
      driver.rms_current(g_rms_current); // Set motor RMS current
      driver.microsteps(g_microsteps);   // Set microsteps to 1/16th
      digitalWrite(DIR_PIN, g_dir ? HIGH : LOW);
    }
  }
}
