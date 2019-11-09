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

void pulse(unsigned int fan, unsigned int color) {
    unsigned int hue = (front_settings.rgb[fan][color] * front_settings.p_multiplier[fan]) / front_settings.p_divisor[fan];

    for(unsigned int i = 0; i < hue; i++)
    {
        PORTB |= (1 << front_params.rgb_pin[color]);
        delayMicroseconds(front_settings.p_delay[fan]);
        PORTB &= ~(1 << front_params.rgb_pin[color]);
    }

    if(front_settings.pulse_sync == 1)
    {
        PORTB |= (1 << front_params.rgb_pin[color]);
        delayMicroseconds(front_settings.p_delay[fan] * 255 - front_settings.rgb[fan][color]);
        PORTB &= ~(1 << front_params.rgb_pin[color]);
    }
}


void update_front_fan_power(unsigned int fan, short force_value = -1)
{
    if(force_value == -1)
    {
        fast_write(front_params.fan_power_register[fan], front_params.fan_power_pin[fan], front_settings.fan_power[fan]);
    }
    else
    {
        fast_write(front_params.fan_power_register[fan], front_params.fan_power_pin[fan], 0);
    }
}

void update_led_power(unsigned int fan)
{
    for(unsigned int i = 0; i < front_params.fan_count; i++)
    {
        if(i == fan)
        {
            fast_write(front_params.led_power_register[fan], front_params.led_power_pin[fan], front_settings.led_power[fan]);
        }
        else
        {
            fast_write(front_params.led_power_register[i], front_params.led_power_pin[i], 0);
        }
    }
}

void update_colors(unsigned int fan)
{
    update_led_power(fan);
    pulse(fan, 0);
    pulse(fan, 1);
    pulse(fan, 2);
}

void pulse_color_cycle(unsigned int fan)
{
    if (front_params.hue[fan] <  255 * 6)
    {
        front_params.hue[fan] += 1;
    }
    else
    {
        front_params.hue[fan] = 0;
    }

    memcpy(front_settings.rgb[fan], hue_to_rgb(front_params.hue[fan]), 3);
    update_colors(fan);
}
