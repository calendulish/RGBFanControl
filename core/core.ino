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
    Serial.begin(2000000);
    Serial.setTimeout(10);
    Serial.println("");
    Serial.println("RGB Cooler Control V0.0.0 <dev@lara.click> 2019");
    Serial.println("Available functions:");
    Serial.println("- f: [p][l][e][c][d][i][m][s]");
    Serial.println("- b: [e,c]");
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

    FastLED.addLeds<NEOPIXEL, back.data_pin>(back.leds, back.led_count);

    for(unsigned int i = 0; i < front.fan_count; i++)
    {
        fast_write(front.fan_speed_register[i], front.fan_speed_pin[i], HIGH);
    }

    for(unsigned int i = 0; i < back.led_count; i++)
    {
        back.rgb[i][0] = 200;
        back.rgb[i][1] = 0;
        back.rgb[i][2] = 0;

        back.effect_id[i] = 0;
    }
}

void loop()
{
    fast_serial();

    if(millis() - start_time >= 2000)
    {
        for(unsigned int i = 0; i < front.fan_count; i++)
        {
            update_front_fan_power(i);
            front.fan_speed[i] = front.fan_speed_frequency[i] * 8;
            front.fan_speed_frequency[i] = 0;
        }

        start_time = millis();
    }

    for(unsigned int i = 0; i < front.fan_count; i++)
    {
        update_back_fan_power(i);
        
        if(fast_read(front.fan_speed_register[i], front.fan_speed_pin[i]) == 0)
        {
            front.fan_speed_frequency[i] += 1;
        }

        switch(front.effect_id[i])
        {
            case 0:
                update_colors(i);
                break;
            case 1:
                pulse_color_cycle(i);
                break;
        }
    }

    for( unsigned int i = 0; i < back.fan_count; i++)
    {
        for(unsigned int l = 0; l < back.led_count; l++)
        {
            switch(back.effect_id[i])
            {
                case 0:
                    neopixel_update_colors(l);
                    break;
                case 1:
                    neopixel_color_cycle(l);
                    break;
            }
        }
        FastLED.show();
    }
}
