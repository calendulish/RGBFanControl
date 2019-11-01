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

#include <EEPROM.h>

byte fast_read(char register_, byte pin)
{
    switch(register_)
    {
        case 'B': return (PINB & (1 << pin)); break;
        case 'C': return (PINC & (1 << pin)); break;
        case 'D': return (PIND & (1 << pin)); break;
    }
}

void fast_write(char register_, byte pin, byte value)
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

void save() {
    EEPROM.put(0, front);
    EEPROM.put(sizeof(front), back);
}

void load() {
    EEPROM.get(0, front);
    EEPROM.get(sizeof(front), back);
}

byte get_eeprom_status() {
    byte EEPROM_status;
    
    EEPROM.get(sizeof(front)+sizeof(back), EEPROM_status);

    return EEPROM_status;
}

void update_eeprom_status(byte EEPROM_status) {
    EEPROM.put(sizeof(front)+sizeof(back), EEPROM_status);
}
