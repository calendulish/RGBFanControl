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
static const uint8_t DIGITAL_DATA_PIN = 8;
CRGB DIGITAL_LEDS[DIGITAL_LED_COUNT] = {};

static const uint8_t ANALOG_LED_COUNT = 3;
static const uint8_t ANALOG_LED_PIN[ANALOG_LED_COUNT] = {12, 3, 5};
RwReg ANALOG_LED_MASK[ANALOG_LED_COUNT];
volatile RwReg *ANALOG_LED_PORT[ANALOG_LED_COUNT];
static const uint8_t ANALOG_RGB_PIN[3] = {10, 11, 9};
RwReg ANALOG_RGB_MASK[3];
volatile RwReg *ANALOG_RGB_PORT[3];
CRGB ANALOG_LEDS[ANALOG_LED_COUNT] = {};

static const uint8_t AUTOEFFECT_LED_COUNT = 1;
static const uint8_t AUTOEFFECT_DATA_PIN[AUTOEFFECT_LED_COUNT] = {A5};
RwReg AUTOEFFECT_LED_MASK[AUTOEFFECT_LED_COUNT];
volatile RwReg *AUTOEFFECT_LED_PORT[AUTOEFFECT_LED_COUNT];
bool AUTO_EFFECT_CHANGED;

static const uint8_t FAN_COUNT = 3;
static const uint8_t FAN_PIN[FAN_COUNT] = {7, 4, 6};
RwReg FAN_MASK[FAN_COUNT];
volatile RwReg *FAN_PORT[FAN_COUNT];
static const uint8_t FAN_SPEED_PIN[FAN_COUNT] = {A0, A2, A1};
RwReg FAN_SPEED_MASK[FAN_COUNT];
volatile RwReg *FAN_SPEED_PORT[FAN_COUNT];
volatile RwReg *FAN_SPEED_IN[FAN_COUNT];
int FAN_SPEED_FREQUENCY[FAN_COUNT];
int FAN_SPEED[FAN_COUNT];

struct config_t
{
    int effect_id[3];
    int auto_effect_id;
    int color[3];
    int secondary_color[3];
    int fan_power[FAN_COUNT];
} config = {1, 0, 0, 0, 255, 0, 0, 0, 0, 255};