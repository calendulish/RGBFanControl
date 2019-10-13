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


bool guess_back_fan_power()
{
    for(unsigned int i = 0; i < front.fan_count; i++)
    {
        if(front.fan_speed[i] != 0) {
            return 1;
        }
    }
    
    return 0;
}

void update_back_fan_power(unsigned int fan)
{
    if(!guess_back_fan_power())
    {
        fill_solid(back.leds, back.led_count, CRGB::Black);
        FastLED.show();
    }
}

void neopixel_update_colors(unsigned int led)
{
    if(!guess_back_fan_power())
    {
        return;
    }
    
    back.leds[led].r = back.rgb[led][0];
    back.leds[led].g = back.rgb[led][1];
    back.leds[led].b = back.rgb[led][2];
}

void neopixel_color_cycle(unsigned int led)
{
    if (back.hue[led] <  255 * 6)
    {
        back.hue[led] += 1;
    }
    else
    {
        back.hue[led] = 0;
    }

    memcpy(back.rgb[led], hue_to_rgb(back.hue[led]), 3);
    neopixel_update_colors(led);
}
