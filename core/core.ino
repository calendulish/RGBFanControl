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
    Serial.println("");

    update_eeprom_status(1);
}

void lazy_loop()
{
    if(get_eeprom_status() == 1)
    {
        load();
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

    switch(1)
    {
        case 0:
            UniLED->set_colors();
            break;
        case 1:
            UniLED->fill_rainbow(beatsin8(25) + beatsin8(50), 35);
            break;
    }

    UniLED->show();
}
