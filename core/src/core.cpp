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

#include "core.h"
#include "memory.h"
#include "serial.h"
#include "cooler.h"

void setup()
{
    Serial.begin(19200);
    Serial.setTimeout(10);
    Serial.println("");
    Serial.println("RGB Cooler Control V3.0.0 <dev@lara.click> 2020");
    Serial.println("Functions:");
    Serial.println(" - led[brightness, effect, color, secondary color, auto effect] <l[b, e, c, s, a]>");
    Serial.println(" - memory[save, load, clear, enable, disable] <m[s, l, c, e, d]>");
    Serial.println(" - cooler[speed, power] <c[s, p]>");
    Serial.println("");

    //DDRD |= 0b11111100;
    //DDRB |= 0b11111111;
    //DDRC |= 0b00000000;

    CFastLED::addLeds<NEOPIXEL, 8>(DIGITAL_LEDS, BACK_FAN_LED_COUNT);
    CFastLED::addLeds<NEOPIXEL, A3>(DIGITAL_LEDS+BACK_FAN_LED_COUNT, STRIP1_LED_COUNT);
    CFastLED::addLeds<NEOPIXEL, A4>(DIGITAL_LEDS+STRIP1_LED_COUNT+BACK_FAN_LED_COUNT, STRIP2_LED_COUNT);

#ifdef ANALOG_LEDS_ENABLED
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
#endif

    for (uint8_t pin = 0; pin < AUTOEFFECT_LED_COUNT; pin++)
    {
        pinMode(AUTOEFFECT_DATA_PIN[pin], OUTPUT);
        AUTOEFFECT_LED_MASK[pin] = digitalPinToBitMask(AUTOEFFECT_DATA_PIN[pin]);
        AUTOEFFECT_LED_PORT[pin] = portOutputRegister(digitalPinToPort(AUTOEFFECT_DATA_PIN[pin]));
    }

    for (uint8_t pin = 0; pin < FAN_COUNT; pin++)
    {
        pinMode(FAN_PIN[pin], OUTPUT);
        FAN_MASK[pin] = digitalPinToBitMask(FAN_PIN[pin]);
        FAN_PORT[pin] = portOutputRegister(digitalPinToPort(FAN_PIN[pin]));
        fan_lo(pin);

        pinMode(FAN_SPEED_PIN[pin], INPUT);
        FAN_SPEED_MASK[pin] = digitalPinToBitMask(FAN_SPEED_PIN[pin]);
        FAN_SPEED_PORT[pin] = portOutputRegister(digitalPinToPort(FAN_SPEED_PIN[pin]));
        FAN_SPEED_IN[pin] = portInputRegister(digitalPinToPort(FAN_SPEED_PIN[pin]));
        fan_speed_hi(pin);
    }

    AUTO_EFFECT_CHANGED = true;
    set_memory_status(1);
    memory_load();
}


void loop()
{
    fast_serial();

    uint8_t wave;
    CRGB wave_rgb;
    CRGB color = CRGB(config.color[0], config.color[1], config.color[2]);
    CRGB secondary_color = CRGB(config.secondary_color[0], config.secondary_color[1], config.secondary_color[2]);
    FastLED.setBrightness(config.brightness);

    EVERY_N_SECONDS(5)
    {
        //if (memory_status() == 1)
        //    memory_load();

        for (uint8_t fan = 0; fan < FAN_COUNT; fan++)
        {
            if (config.fan_power[fan] == 1)
                fan_hi(fan);
            else
                fan_lo(fan);

            FAN_SPEED[fan] = FAN_SPEED_FREQUENCY[fan] * 8;
            FAN_SPEED_FREQUENCY[fan] = 0;
        }
    }

    for (uint8_t fan = 0; fan < FAN_COUNT; fan++)
    {
        if (fan_speed_read(fan) == 0)
            FAN_SPEED_FREQUENCY[fan] += 1;
    }

    if (AUTO_EFFECT_CHANGED)
    {
        for (uint8_t led = 0; led < AUTOEFFECT_LED_COUNT; led++)
        {
            analog_pulse(led, 2000, 1);
            analog_pulse(led, 50, config.auto_effect_id);
        }

        AUTO_EFFECT_CHANGED = false;
    }

    if (!guess_fan_power())
    {
        fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT, CRGB::Black);
        FastLED.show();
        return;
    }

    for (auto effect_id : config.effect_id)
    {
        switch (effect_id)
        {
            case 0: // disable
                if(config.effect_id[0] == 0 &&
                   config.effect_id[1] == 0 &&
                   config.effect_id[2] == 0)
                    auto_fill_solid(CRGB::Black);
                continue;
            case 1: // fade
                wave = beatsin16(10, 0, 255);
                FastLED.setBrightness(wave);
                continue;
            case 2: // pulse
                EVERY_N_MILLISECONDS(400)
                {
                    auto_fill_solid(CRGB::Black);
                    auto_show();
                    FastLED.delay(200);
                }
                continue;
            case 50: // solid color
                auto_fill_solid(color);
                break;
            case 51: // solid color rotating
                auto_fadeToBlackBy(20);
                wave = beatsin16(30, 0, DIGITAL_LED_COUNT-1);
                DIGITAL_LEDS[wave] = color;

#ifdef ANALOG_LEDS_ENABLED
                wave = beatsin16(30, 0, ANALOG_LED_COUNT-1);
                ANALOG_LEDS[wave] = color;
#endif
                break;
            case 100: // two colors
                auto_fill_gradient_RGB(0, color, secondary_color);
                break;
            case 101: // two colors gradient
                wave_rgb = blend(color, secondary_color, beatsin8(10));
                auto_fill_solid(wave_rgb);
                break;
            case 102: // two colors up-down
                auto_fill_gradient_RGB(20, color, secondary_color);
                break;
            case 103: // two colors up-down oposite
                EVERY_N_MILLISECONDS(10)
                {
                    auto_fadeToBlackBy(60);
                    wave = beatsin16(15, 0, DIGITAL_LED_COUNT-1);
                    DIGITAL_LEDS[wave] = color;
                    wave = map(wave, 0, DIGITAL_LED_COUNT-1, DIGITAL_LED_COUNT-1, 0);
                    DIGITAL_LEDS[wave] = secondary_color;

#ifdef ANALOG_LEDS_ENABLED
                    wave = beatsin16(15, 0, ANALOG_LED_COUNT-1);
                    ANALOG_LEDS[wave] = color;
                    wave = map(wave, 0, ANALOG_LED_COUNT-1, ANALOG_LED_COUNT-1, 0);
                    ANALOG_LEDS[wave] = secondary_color;
#endif
                }
                break;
            case 104: // two colors random
                EVERY_N_MILLISECONDS(20)
                {
                    auto_fadeToBlackBy(15);

                    if(random8() < 20)
                    {
                        auto_random_led_rgb(color);
                        auto_random_led_rgb(secondary_color);
                    }
                }
                break;
            case 200: // rainbow
                auto_fill_rainbow(0, 35);
                break;
            case 201: // rotating rainbow
                wave = beat8(30);
                auto_fill_rainbow(wave, 35);
                break;
            case 500: // party
                int entropy;
                entropy = random16(100, 200);

                EVERY_N_MILLISECONDS(entropy * 2)
                {
                    auto_fill_solid(CRGB::White);
                }

                EVERY_N_MILLISECONDS(entropy / 3)
                {
                    auto_fill_solid(CRGB::Black);
                    auto_show();
                }

                if (random8() < 30)
                {
                    auto_random_led_hue(random8());
                    auto_show();
                }
                break;
            case 501: //police
                EVERY_N_MILLISECONDS(50)
                {
                    auto_fill_solid(CRGB::Black);
                    auto_show();
                }

                EVERY_N_MILLISECONDS(90)
                {
                    wave = beatsin8(50, 0, 50);
                    if(wave < 10)
                    {
                        fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT / 2, CRGB::Red);
#ifdef ANALOG_LEDS_ENABLED
                        //fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT / 2, CRGB::Red);
                        // custom 2 X 1
                        ANALOG_LEDS[0] = CRGB::Red;
                        ANALOG_LEDS[2] = CRGB::Red;
#endif
                    }

                    if(wave > 10 && wave < 30)
                    {
                        fill_solid(DIGITAL_LEDS+DIGITAL_LED_COUNT / 2, DIGITAL_LED_COUNT / 2, CRGB::Blue);
#ifdef ANALOG_LEDS_ENABLED
                        fill_solid(ANALOG_LEDS+ANALOG_LED_COUNT / 2, ANALOG_LED_COUNT / 2, CRGB::Blue);
#endif
                    }
                    if(wave > 30)
                    {
                        auto_fill_solid(CRGB::Black);
                    }
                }
                break;
            case 502: // 3 colors cycle
                EVERY_N_MILLISECONDS(40)
                {
                    fadeToBlackBy(DIGITAL_LEDS, DIGITAL_LED_COUNT, 60);

                    DIGITAL_CUSTOM_LED_COUNT++;

                    if(DIGITAL_CUSTOM_LED_COUNT == DIGITAL_LED_COUNT)
                    {
                        DIGITAL_CUSTOM_LED_COUNT = 0;

                        if(DIGITAL_CUSTOM_COLOR == CRGB(CRGB::Red))
                            DIGITAL_CUSTOM_COLOR = CRGB::Green;
                        else if(DIGITAL_CUSTOM_COLOR == CRGB(CRGB::Green))
                            DIGITAL_CUSTOM_COLOR = CRGB::Blue;
                        else
                            DIGITAL_CUSTOM_COLOR = CRGB::Red;
                    }

                    DIGITAL_LEDS[DIGITAL_CUSTOM_LED_COUNT] = DIGITAL_CUSTOM_COLOR;
                }

#ifdef ANALOG_LEDS_ENABLED
                EVERY_N_MILLISECONDS(250)
                {
                    fadeToBlackBy(ANALOG_LEDS, ANALOG_LED_COUNT, 60*2);

                    ANALOG_CUSTOM_LED_COUNT++;

                    if(ANALOG_CUSTOM_LED_COUNT == ANALOG_LED_COUNT)
                    {
                        ANALOG_CUSTOM_LED_COUNT = 0;

                        if(ANALOG_CUSTOM_COLOR == CRGB(CRGB::Red))
                            ANALOG_CUSTOM_COLOR = CRGB::Green;
                        else if(ANALOG_CUSTOM_COLOR == CRGB(CRGB::Green))
                            ANALOG_CUSTOM_COLOR = CRGB::Blue;
                        else
                            ANALOG_CUSTOM_COLOR = CRGB::Red;
                    }

                    ANALOG_LEDS[ANALOG_CUSTOM_LED_COUNT] = ANALOG_CUSTOM_COLOR;
                }
#endif
                break;
            default:
                continue;
        }

        auto_show();
    }
}
