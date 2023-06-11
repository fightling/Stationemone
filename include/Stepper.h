#ifndef STEPPER_H
#define STEPPER_H

#include <TMCStepper.h>
#include <AccelStepper.h>

#define STEP_PIN 14 // Step slope
#define DIR_PIN 12  // Direction
#define CS_PIN 5    // Chip select
#define SW_MOSI 23  // Software Master Out Slave In (MOSI)
#define SW_MISO 19  // Software Master In Slave Out (MISO)
#define SW_SCK 18   // Software Slave Clock (SCK)

#define R_SENSE 0.11f // Match to your driver

namespace Stepper
{
    struct Parameters
    {
        float max_speed;
        float acceleration;
        uint16_t rms_current;
        int distance_mm;
        uint32_t steps_per_mm;
        uint32_t pause_in[2];
        uint32_t pause_out[2];
        uint16_t speed_in;
        uint16_t speed_out;
    };

    enum Stage
    {
        MOVE_IN,
        MOVE_OUT,
        MOVING_IN,
        MOVING_OUT,
        PAUSE_IN,
        PAUSE_OUT,
        PAUSING_IN,
        PAUSING_OUT
    };

    struct Controller
    {
        Controller(const Parameters &params)
            : driver(CS_PIN, R_SENSE, SW_MOSI, SW_MISO, SW_SCK),
              stepper(stepper.DRIVER, STEP_PIN, DIR_PIN),
              params(params),
              stage(MOVE_IN)
        {
        }

        void setup()
        {
            pinMode(STEP_PIN, OUTPUT);
            pinMode(DIR_PIN, OUTPUT);
            pinMode(SW_MISO, INPUT_PULLUP);

            driver.begin();                         //  SPI: Init CS pins and possible SW SPI pins
                                                    //  driver.push();
            driver.rms_current(params.rms_current); // Set motor RMS current
            driver.en_pwm_mode(true);               // Toggle stealthChop on TMC2130/2160/5130/5160
            driver.pwm_autoscale(true);             // Needed for stealthChop
            driver.microsteps(16);                  // Set microsteps to 1/16th

            stepper.setMaxSpeed(params.max_speed * params.steps_per_mm);
            stepper.setAcceleration(params.acceleration * params.steps_per_mm);
            stepper.setPinsInverted(false, false, true);
            stepper.enableOutputs();
        }

        void loop()
        {
            switch (stage)
            {
            case MOVE_IN:
                stepper.disableOutputs();
                stepper.setMaxSpeed(params.speed_in * params.steps_per_mm);
                stepper.moveTo(params.distance_mm * params.steps_per_mm);
                stepper.enableOutputs();
                stage = MOVING_IN;
                break;
            case MOVE_OUT:
                stepper.disableOutputs();
                stepper.setMaxSpeed(params.speed_out * params.steps_per_mm);
                stepper.moveTo(0 * params.steps_per_mm);
                stepper.enableOutputs();
                stage = MOVING_OUT;
                break;
            case MOVING_IN:
                if (stepper.currentPosition() == params.distance_mm * params.steps_per_mm)
                    stage = PAUSE_IN;
                stepper.run();
                break;
            case MOVING_OUT:
                if (stepper.currentPosition() == 0)
                    stage = PAUSE_OUT;
                stepper.run();
                break;
            case PAUSE_IN:
                until = millis() + random(params.pause_in[0], params.pause_in[1]);
                stage = PAUSING_IN;
                break;
            case PAUSE_OUT:
                delay(random(params.pause_out[0], params.pause_out[1]));
                stage = PAUSING_OUT;
                break;
            case PAUSING_IN:
                if (millis() > until)
                    stage = MOVE_OUT;
                break;
            case PAUSING_OUT:
                if (millis() > until)
                    stage = MOVE_IN;
                break;
            }
        }

    private:
        const Parameters &params;
        TMC2130Stepper driver;
        AccelStepper stepper;
        Stage stage;
        unsigned long until;
    };
}

#endif