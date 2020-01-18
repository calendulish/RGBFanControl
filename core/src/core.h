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
    fill_solid(ANALOG_LEDS, ANALOG_LED_COUNT, color);
}

void auto_fill_rainbow(uint8_t initialhue, uint8_t deltahue = 5)
{
    fill_rainbow(DIGITAL_LEDS, DIGITAL_LED_COUNT, initialhue, deltahue);
    fill_rainbow(ANALOG_LEDS, ANALOG_LED_COUNT, initialhue, deltahue);
}

void auto_fill_gradient_RGB(const CRGB &c1, const CRGB &c2)
{
    fill_gradient_RGB(DIGITAL_LEDS, DIGITAL_LED_COUNT, c1, c2);
    fill_gradient_RGB(ANALOG_LEDS, ANALOG_LED_COUNT, c1, c2);
}

void auto_hue(uint8_t led, uint8_t hue)
{
    DIGITAL_LEDS[led].setHue(hue);
    ANALOG_LEDS[led].setHue(hue);
}

void auto_show()
{
    FastLED.show();
    analog_show();
}

#endif /* CORE_H */

