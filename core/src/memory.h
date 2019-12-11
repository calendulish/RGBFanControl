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

#include <EEPROM.h>

void memory_save()
{
    EEPROM.put(0, config);
}

void memory_load()
{
    EEPROM.get(0, config);
}

void memory_clear()
{
    for (uint16_t i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.write(i, 0);
    }
}

byte memory_status()
{
    byte status;

    EEPROM.get(sizeof(config), status);

    return status;
}

void set_memory_status(byte status)
{
    EEPROM.put(sizeof(config), status);
}