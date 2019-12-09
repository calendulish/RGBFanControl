/*
 Lara Maia <dev@lara.click> 2020

 The RGBFanControl is free software: you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.

 The RGBFanControl is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "analog.h"
#include "memory.h"
#include "serial.h"

void setup()
{
    Serial.begin(19200);
    Serial.setTimeout(10);
    Serial.println("");
    Serial.println("RGB Cooler Control V3.0.0 <dev@lara.click> 2020");
    Serial.println("Functions:");
    Serial.println(" - led[effect] <l[e]>");
    Serial.println(" - memory[save, load, clear, enable, disable] <m[s, l, c, e, d]>");
    Serial.println("");

    DDRD |= 0b11111100;
    DDRB |= 0b11111111;
    DDRC |= 0b00000000;

    CFastLED::addLeds<NEOPIXEL, DIGITAL_DATA_PIN>(DIGITAL_LEDS, DIGITAL_LED_COUNT);

    /*for (unsigned int i = 0; i < front_params.fan_count; i++)
    {
        fast_write(front_params.fan_speed_register[i], front_params.fan_speed_pin[i], HIGH);
    }*/

    for (uint8_t pin = 0; pin < ANALOG_LED_COUNT; pin++)
    {
        pinMode(ANALOG_LED_PIN[pin], OUTPUT);
        ANALOG_LED_MASK[pin] = digitalPinToBitMask(ANALOG_LED_PIN[pin]);
        ANALOG_LED_PORT[pin] = portOutputRegister(digitalPinToPort(ANALOG_LED_PIN[pin]));
    }

    for (uint8_t pin = 0; pin < 3; pin++)
    {
        pinMode(ANALOG_RGB_PIN[pin], OUTPUT);
        ANALOG_RGB_MASK[pin] = digitalPinToBitMask(ANALOG_RGB_PIN[pin]);
        ANALOG_RGB_PORT[pin] = portOutputRegister(digitalPinToPort(ANALOG_RGB_PIN[pin]));
    }

    set_memory_status(1);
}


void loop()
{
    fast_serial();

    uint8_t wave;
    CRGB wave_rgb;
    FastLED.setBrightness(255);

    EVERY_N_SECONDS(5)
    {
        if (memory_status() == 1)
            memory_load();
    }

    for (auto effect_id : config.effect_id)
    {
        switch (effect_id)
        {
            case 1: // fade
                FastLED.setBrightness(beatsin8(10));
                break;
            case 2: // pulse
            EVERY_N_MILLISECONDS(400)
                {
                    fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT, CRGB::Black);
                    fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT, CRGB::Black);
                    FastLED.show();
                    analog_show();
                    FastLED.delay(200);
                }
                break;
            case 50: // solid color
                fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT, CRGB::Red);
                fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT, CRGB::Red);
                break;
            case 100: // two colors
                fill_gradient_RGB(DIGITAL_LEDS, DIGITAL_LED_COUNT, CRGB::Red, CRGB::Blue);
                fill_gradient_RGB(ANALOG_LEDS, ANALOG_LED_COUNT, CRGB::Red, CRGB::Blue);
                break;
            case 101: // two colors gradient
                wave_rgb = blend(CRGB::Red, CRGB::Blue, beatsin8(10));
                fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT, wave_rgb);
                fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT, wave_rgb);
                break;
            case 102: // two colors up-down
                wave = beatsin8(20, 0, DIGITAL_LED_COUNT - 2);
                fill_gradient_RGB(DIGITAL_LEDS + wave, DIGITAL_LED_COUNT - wave, CRGB::Red, CRGB::Blue);

                wave = beatsin8(20, 0, ANALOG_LED_COUNT - 2);
                fill_gradient_RGB(ANALOG_LEDS + wave, ANALOG_LED_COUNT - wave, CRGB::Red, CRGB::Blue);
                break;
            case 200: // rainbow
                fill_rainbow(DIGITAL_LEDS, DIGITAL_LED_COUNT, 0, 35);
                fill_rainbow(ANALOG_LEDS, ANALOG_LED_COUNT, 0, 35);
                break;
            case 201: // rotating rainbow
                wave = beat8(40);
                fill_rainbow(DIGITAL_LEDS, DIGITAL_LED_COUNT, wave, 35);
                fill_rainbow(ANALOG_LEDS, ANALOG_LED_COUNT, wave, 35);
                break;
            case 500: // party
                int entropy;
                entropy = random16(100, 200);

                EVERY_N_MILLISECONDS(entropy * 2)
                {
                    fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT, CRGB::White);
                    fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT, CRGB::White);
                }

                EVERY_N_MILLISECONDS(entropy / 3)
                {
                    fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT, CRGB::Black);
                    fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT, CRGB::Black);
                    FastLED.show();
                    analog_show();
                }

                if (random8() < 30)
                {
                    DIGITAL_LEDS[random16(DIGITAL_LED_COUNT)].setHue(random8());
                    ANALOG_LEDS[random16(ANALOG_LED_COUNT)].setHue(random8());
                    FastLED.show();
                    analog_show();
                }
                break;
            default:
                continue;
        }

        FastLED.show();
        analog_show();
    }
}
