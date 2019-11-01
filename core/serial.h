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

char next_char(String *string_)
{
    String substring_value = string_->substring(0, 1);
    string_->remove(0, 1);

    return substring_value[0];
}

unsigned int next_int(String *string_, unsigned int count = 1)
{
    if(string_->length() < count)
    {
        Serial.println("[-] Wrong length");
        return -1;
    }

    String substring_value = string_->substring(0, count);
    string_->remove(0, count);

    for(unsigned int c = 0; c < count; c++)
    {
        if(isalpha(substring_value[c]))
        {
            Serial.println("[-] Wrong type");
            return -1;
        }
    }
    return substring_value.toInt();
}

void front_functions(String *serial_string)
{
    if(serial_string->length() < 1)
    {
        Serial.println("[f] Wrong length");
        return;
    }

    char function = next_char(serial_string);

    for(unsigned int i = 0; i < front.fan_count; i++)
    {
        switch(function)
        {
            case 'p':
                unsigned int fan_power;
                fan_power = next_int(serial_string);

                if(fan_power == -1)
                {
                    return;
                }

                front.fan_power[i] = fan_power;
                break;
            case 'l':
                unsigned int led_power;
                led_power = next_int(serial_string);

                if(led_power == -1)
                {
                    return;
                }

                front.led_power[i] = led_power;
                break;
            case 'e':
                unsigned int effect_id;
                effect_id = next_int(serial_string);

                if(effect_id == -1)
                {
                    return;
                }

                front.effect_id[i] = effect_id;
                break;
            case 'c':
                unsigned int c;
                for(c = 0; c <= 2; c++)
                {
                    unsigned int color;
                    color = next_int(serial_string, 3);;

                    if(color == -1)
                    {
                        return;
                    }

                    front.rgb[i][c] = color;
                }
                break;
            case 'd':
                unsigned int delay_;
                delay_ = next_int(serial_string, 3);

                if(delay_ == -1)
                {
                    return;
                }

                front.p_delay[i] = delay_;
                break;
            case 'i':
                unsigned int divisor;
                divisor = next_int(serial_string, 3);

                if(divisor == -1)
                {
                    return;
                }

                front.p_divisor[i] = divisor;
                break;
            case 'm':
                unsigned int multiplier;
                multiplier = next_int(serial_string, 3);

                if(multiplier == -1)
                {
                    return;
                }

                front.p_multiplier[i] = multiplier;
                break;
            case 's':
                Serial.println(front.fan_speed[i]);
                break;
            default:
                Serial.println("[f] Wrong option");
                return;
        }
    }

    Serial.println("[f] Done!");
}

void back_functions(String *serial_string)
{
    if(serial_string->length() < 1)
    {
        Serial.println("[b] Wrong length");
        return;
    }

    char function = next_char(serial_string);

    for(unsigned int i = 0; i < back.fan_count; i++)
    {
        switch(function)
        {
            case 'e':
                unsigned int effect_id;
                effect_id = next_int(serial_string);

                if(effect_id == -1)
                {
                    return;
                }

                back.effect_id[i] = effect_id;
                break;
            case 'c':
                unsigned int l;
                
                for(l = 0; l < back.led_count; l++)
                {
                    unsigned int c;
                    for(c = 0; c <= 2; c++)
                    {
                        unsigned int color;
                        color = next_int(serial_string, 3);;
    
                        if(color == -1)
                        {
                            return;
                        }
    
                        back.rgb[l][c] = color;
                    }
                }
                break;
            default:
                Serial.println("[b] Wrong option");
                return;
        }
    }

    Serial.println("[b] Done!");
}

void fast_serial()
{
    if(Serial.available())
    {
        String serial_string = Serial.readString();

        if(serial_string.length() < 2)
        {
            Serial.println("[:] Wrong length");
            return;
        }

        switch(next_char(&serial_string))
        {
            case 'f':
                front_functions(&serial_string);
                break;
            case 'b':
                back_functions(&serial_string);
                break;
            case 's':
                save();
                break;
            case 'l':
                load();
                break;
            case 'd':
                update_eeprom_status(0);
                break;
            case 'e':
                update_eeprom_status(1);
                break;
            default:
                Serial.println("[:] Wrong option");
                break;
        }
    }
}
