#ifndef LED_H
#define LED_H
#include "Arduino.h"

namespace Led
{
    enum Mode
    {
        OFF,
        ON,
        BLINK
    };

    const unsigned long BLINK_MILLIS = 500;

#define LED_ON HIGH
#define LED_OFF LOW

    const uint8_t EOL = 0;

    template <uint8_t NUM>
    struct Parameters
    {
        static const uint8_t num = NUM;
        uint8_t pins[NUM];
    };

    template <class PARAMS>
    struct Controller
    {
        Controller(const PARAMS &params)
            : params(params)
        {
        }

        void setup()
        {
            Serial.println("Led::setup()");
            for (uint8_t i = 0; i < params.num; i++)
                setPinMode(params.pins[i], OUTPUT);
        }

        void all(uint8_t mode)
        {
            for (uint8_t i = 0; i < params.num; i++)
                digitalWrite(params.pins[i], mode);
        }

        void loop()
        {
            static unsigned long start = 0;
            if (0 == start)
                start = millis();
            for (uint8_t i = 0; i < params.num; ++i)
            {
                switch (modes[i])
                {
                case OFF:
                    digitalWrite(params.pins[i], LED_OFF);
                    break;
                case ON:
                    digitalWrite(params.pins[i], LED_ON);
                    break;
                case BLINK:
                {
                    unsigned long time = millis() - start;

                    if (time < BLINK_MILLIS / 2)
                        digitalWrite(params.pins[i], LED_ON);
                    else if (time >= BLINK_MILLIS)
                        start = 0;
                    else
                        digitalWrite(params.pins[i], LED_OFF);
                    break;
                }
                }
            }
        }

        void off(uint8_t n)
        {
            assert(n < params.num);
            modes[n] = OFF;
            loop();
        }

        void on(uint8_t n)
        {
            assert(n < params.num);
            modes[n] = ON;
            loop();
        }

        void blink(uint8_t n)
        {
            assert(n < params.num);
            modes[n] = BLINK;
        }

    private:
        const PARAMS &params;
        Mode modes[PARAMS::num];
    };
}

#endif