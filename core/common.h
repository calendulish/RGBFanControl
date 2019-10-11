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

byte fast_write(char register_, byte pin, byte value)
{
    if(value == 1)
    {
        switch(register_)
        {
            case 'B': PORTB |= (1 << pin); break;
            case 'C': PORTC |= (1 << pin); break;
            case 'D': PORTD |= (1 << pin); break;
        }
    }
    else
    {
        switch(register_)
        {
            case 'B': PORTB &= ~(1 << pin); break;
            case 'C': PORTC &= ~(1 << pin); break;
            case 'D': PORTD &= ~(1 << pin); break;
        }
    }
}

byte* hue_to_rgb(unsigned int hue)
{
    static byte rgb[3];

    if(hue <=  255)
    {
        rgb[0] = 255;
        rgb[1] = hue;
        rgb[2] = 0;
    }
    if(hue >=  255 and hue <=  255 * 2)
    {
        rgb[0] = 255 * 2 - hue;
        rgb[1] = 255;
        rgb[2] = 0;
    }
    if(hue >=  255 * 2 and hue <=  255 * 3)
    {
        rgb[0] = 0;
        rgb[1] = 255;
        rgb[2] = hue -  255 * 2;
    }
    if(hue >=  255 * 3 and hue <=  255 * 4)
    {
        rgb[0] = 0;
        rgb[1] = 255 * 4 - hue;
        rgb[2] =  255;
    }
    if(hue >=  255 * 4 and hue <=  255 * 5)
    {
        rgb[0] = hue -  255 * 4;
        rgb[1] = 0;
        rgb[2] = 255;
    }
    if(hue >=  255 * 5 and hue <=  255 * 6)
    {
        rgb[0] = 255;
        rgb[1] = 0;
        rgb[2] = 255 * 6 - hue;
    }

    return rgb;
}
