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

struct front_fans_settings
{
    static const byte led_count = 3;
    byte fan_power[led_count] = {1, 1, 1};
    byte led_power[led_count] = {0, 0, 0};
    byte rgb[led_count][3] = {{255, 0, 0}, {255, 0, 0}, {255, 0, 0}};
    byte pulse_sync = 1;

    unsigned int effect_id[led_count] = {0, 0, 0};
    unsigned int p_delay[led_count] = {6, 6, 6};
    unsigned int p_divisor[led_count] = {4, 4, 4};
    unsigned int p_multiplier[led_count] = {2, 2, 2};
};
struct front_fans_settings front_settings;

struct back_fans_settings
{
    static const byte led_count = 28;
    byte rgb[led_count][3];
    byte pulse_sync = 1;

    unsigned int effect_id[led_count];
    unsigned int p_delay = 1;
    unsigned int p_divisor = 1;
    unsigned int p_multiplier = 1;
};
struct back_fans_settings back_settings;
