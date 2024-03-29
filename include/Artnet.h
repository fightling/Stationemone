#ifndef ARTNET_H
#define ARTNET_H

#include <ArtnetWifi.h>

namespace Artnet
{
    struct Parameters
    {
        const char *ssid;
        const char *password;
        uint16_t universe;
        uint16_t first;
    };

    static ArtnetWifi artnet;
    static bool new_data = false;
    static const Parameters *s_params = NULL;
    static uint8_t data[512];
    static int16_t prev_sequence = -1;
    static void (*fnConnecting)() = NULL;
    template <uint8_t DATA_LENGTH>
    struct Client
    {
        Client(const Parameters &params)
        {
            s_params = &params;
        }

        bool setup(void (*fn)())
        {
            bool state = true;
            int i = 0;
            fnConnecting = fn;

            WiFi.begin(s_params->ssid, s_params->password);
            Serial.println("");
            Serial.println("Connecting to WiFi");

            // Wait for connection
            Serial.print("Connecting");
            while (WiFi.status() != WL_CONNECTED)
            {
                fnConnecting();
                Serial.print(".");
                if (i > 20)
                {
                    state = false;
                    break;
                }
                i++;
            }
            if (state)
            {
                Serial.println("");
                Serial.print("Connected to ");
                Serial.println(s_params->ssid);
                Serial.print("IP address: ");
                Serial.println(WiFi.localIP());
            }
            else
            {
                Serial.println("");
                Serial.println("Connection failed.");
            }

            artnet.begin();
            artnet.setArtDmxCallback(onDmxFrame);

            return state;
        }

        bool loop()
        {
            if (WiFi.status() != WL_CONNECTED)
                return setup(fnConnecting);
            return true;
        }

        uint8_t *get_data()
        {
            artnet.read();

            if (new_data)
                return data;
            else
                return NULL;
            new_data = false;
        }

    private:
        static void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *_data)
        {
            if (prev_sequence > 128 && sequence < 128)
                prev_sequence = -1;

            if (universe == s_params->universe && sequence > prev_sequence)
            {
                assert(length == sizeof(data));
                memcpy(data, _data, length);
                new_data = true;
            }
        }
    };
}

#endif