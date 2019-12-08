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

static const uint8_t DIGITAL_LED_COUNT = 30;
static const byte DIGITAL_DATA_PIN = 8;
CRGB DIGITAL_LEDS[DIGITAL_LED_COUNT] = {};

struct config_t
{
    int effect_id[3];
    uint8_t color[3];
} config = {1, 0, 0, 255, 0, 0};