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

#ifndef CORE_H
#define CORE_H

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"
#include "analog.h"

void auto_fill_solid(const struct CRGB& color)
{
    fill_solid(DIGITAL_LEDS, DIGITAL_LED_COUNT, color);

#ifdef ANALOG_LEDS_ENABLED
    fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT, color);
#endif
}

void auto_fill_rainbow(uint8_t initialhue, uint8_t deltahue = 5)
{
    fill_rainbow(DIGITAL_LEDS, DIGITAL_LED_COUNT, initialhue, deltahue);

#ifdef ANALOG_LEDS_ENABLED
    fill_rainbow(ANALOG_LEDS, ANALOG_LED_COUNT, initialhue, deltahue);
#endif
}

void auto_fill_gradient_RGB(uint8_t bpm, const CRGB &c1, const CRGB &c2)
{
    uint8_t digital_wave = 0;
    uint8_t analog_wave = 0;

    if(bpm != 0)
    {
        digital_wave = beatsin16(bpm, 0, DIGITAL_LED_COUNT -1);
        analog_wave = beatsin16(bpm, 0, ANALOG_LED_COUNT -1);
    }

    fill_gradient_RGB(DIGITAL_LEDS + digital_wave, DIGITAL_LED_COUNT - digital_wave, c1, c2);

#ifdef ANALOG_LEDS_ENABLED
    fill_gradient_RGB(ANALOG_LEDS + analog_wave, ANALOG_LED_COUNT - analog_wave, c1, c2);
#endif
}

void auto_random_led_hue(uint8_t hue)
{
    DIGITAL_LEDS[random16(DIGITAL_LED_COUNT)].setHue(hue);

#ifdef ANALOG_LEDS_ENABLED
    ANALOG_LEDS[random16(ANALOG_LED_COUNT)].setHue(hue);
#endif
}

void auto_random_led_rgb(const CRGB& rgb)
{
    DIGITAL_LEDS[random16(DIGITAL_LED_COUNT)] = rgb;

#ifdef ANALOG_LEDS_ENABLED
    ANALOG_LEDS[random16(ANALOG_LED_COUNT)] = rgb;
#endif
}

void auto_fadeToBlackBy(uint8_t fadeBy)
{
    fadeToBlackBy(DIGITAL_LEDS, DIGITAL_LED_COUNT, fadeBy);

#ifdef ANALOG_LEDS_ENABLED
    fadeToBlackBy(ANALOG_LEDS, ANALOG_LED_COUNT, fadeBy * 2);
#endif
}

void auto_show()
{
    FastLED.show();

#ifdef ANALOG_LEDS_ENABLED
    analog_show();
#endif
}

#endif /* CORE_H */

