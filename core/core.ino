/*
    Lara Maia <dev@lara.click> 2019

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

#include <FastLED.h>

#include "config.h"
#include "common.h"
#include "serial.h"
#include "front.h"
#include "back.h"

unsigned long start_time = millis();

void setup()
{
    Serial.begin(19200);
    Serial.setTimeout(10);
    Serial.println("");
    Serial.println("RGB Cooler Control V0.0.0 <dev@lara.click> 2019");
    Serial.println("Available functions:");
    Serial.println("- f: [p][l][e][c][d][i][m][s]");
    Serial.println("- b: [e,c]");
    Serial.println("- se: [-]");
    Serial.println("- le: [-]");
    Serial.println("- ee: [-]");
    Serial.println("- de: [-]");
    Serial.println("- ce: [-]");
    Serial.println("---> p: boolean <BB..>");
    Serial.println("---> l: boolean <BB..>");
    Serial.println("---> e: boolean <BB..>");
    Serial.println("---> c: 0..255,0..255,0..255 <RRRGGGBBB..>");
    Serial.println("---> d: integer <NNN..>");
    Serial.println("---> i: integer <NNN..>");
    Serial.println("---> m: integer <NNN..>");
    Serial.println("---> s: integer <NNN..>");

    DDRD |= 0b11111100;
    DDRB |= 0b11111111;
    DDRC |= 0b00000000;

    FastLED.addLeds<NEOPIXEL, back_params.data_pin>(back_params.leds, back_params.led_count);

    for(unsigned int i = 0; i < front_params.fan_count; i++)
    {
        fast_write(front_params.fan_speed_register[i], front_params.fan_speed_pin[i], HIGH);
    }

    for(unsigned int i = 0; i < back_params.led_count; i++)
    {
        back_settings.rgb[i][0] = 0;
        back_settings.rgb[i][1] = 0;
        back_settings.rgb[i][2] = 0;
        back_settings.effect_id[i] = 0;
    }

    update_eeprom_status(1);
}

void lazy_loop()
{
    if(get_eeprom_status() == 1)
    {
        load();
    }

    for(unsigned int i = 0; i < front_params.fan_count; i++)
    {
        update_front_fan_power(i);
        front_params.fan_speed[i] = front_params.fan_speed_frequency[i] * 8;
        front_params.fan_speed_frequency[i] = 0;
    }

    for(unsigned int i = 0; i < back_params.fan_count; i++)
    {
        update_back_fan_power(i);
    }
}

void front_loop(unsigned int current)
{
    if(fast_read(front_params.fan_speed_register[current], front_params.fan_speed_pin[current]) == 0)
    {
        front_params.fan_speed_frequency[current] += 1;
    }

    switch(front_settings.effect_id[current])
    {
        case 0:
            update_colors(current);
            break;
        case 1:
            pulse_color_cycle(current);
            break;
    }
}

void back_loop(unsigned int current, unsigned int led)
{
    switch(back_settings.effect_id[current])
    {
        case 0:
            neopixel_update_colors(led);
            break;
        case 1:
            neopixel_color_cycle(led);
            break;
    }
}

void loop()
{
    fast_serial();

    if(millis() - start_time >= 5000)
    {
        lazy_loop();
        start_time = millis();
    }

    for(unsigned int i = 0; i < front_params.fan_count; i++)
    {
        front_loop(i);
    }

    for(unsigned int i = 0; i < back_params.fan_count; i++)
    {
        for(unsigned int l = 0; l < back_params.led_count; l++)
        {
            back_loop(i, l);
        }
        FastLED.show();
    }
}
