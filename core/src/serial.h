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

#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])

char next_char(String *string_)
{
    String substring_value = string_->substring(0, 1);
    string_->remove(0, 1);

    return substring_value[0];
}

int next_int(String *string_, uint8_t count)
{
    if (string_->length() < count)
    {
        Serial.println("[-] Wrong length");
        return -1;
    }

    String substring_value = string_->substring(0, count);
    string_->remove(0, count);

    for (uint8_t c = 0; c < count; c++)
    {
        if (isalpha(substring_value[c]))
        {
            Serial.println("[-] Wrong type");
            return -1;
        }
    }
    return substring_value.toInt();
}

int is_smaller(const void *left, const void *right)
{
    return (*(int *) left - *(int *) right);
}

bool update_setting(
        String *string_,
        int setting[],
        size_t setting_size,
        uint8_t count = 1,
        __compar_fn_t sort = is_smaller
)
{
    int new_setting[setting_size];
    for (auto &current_setting : new_setting)
    {
        current_setting = next_int(string_, count);
        if (current_setting == -1) return false;
    }

    if (sort != nullptr)
    {
        qsort(new_setting, setting_size, sizeof(new_setting[0]), sort);
    }

    memcpy(setting, new_setting, sizeof(new_setting));
    return true;
}

void remove_endlines(String *string_)
{
    int new_line_index;

    new_line_index = string_->lastIndexOf('\n');
    if (new_line_index != -1) string_->remove(new_line_index, 1);

    new_line_index = string_->lastIndexOf('\r');
    if (new_line_index != -1) string_->remove(new_line_index, 1);
}

void led_options(String *serial_string)
{
    char option = next_char(serial_string);

    switch (option)
    {
        case 'b':
            int brightness;
            brightness = next_int(serial_string, 3);
            if (brightness == -1) break;

            if (brightness > 255)
            {
                Serial.println("[lb] Wrong brightness value");
                break;
            }

            config.brightness = brightness;
            Serial.println("[lb] Brightness changed!");
            break;
        case 'e':
            update_setting(serial_string, config.effect_id, ARRAY_LEN(config.effect_id), 3);
            Serial.println("[le] Effect changed!");
            break;
        case 'c':
            update_setting(serial_string, config.color, ARRAY_LEN(config.color), 3, nullptr);
            Serial.println("[lc] Color changed!");
        case 's':
            update_setting(serial_string, config.secondary_color, ARRAY_LEN(config.secondary_color), 3, nullptr);
            Serial.println("[lc] Color changed!");
        case 'a':
            int effect_id;
            effect_id = next_int(serial_string, 2);
            if (effect_id == -1) break;

            if (effect_id > 10)
            {
                Serial.println("[la] Wrong auto effect id");
                break;
            }

            config.auto_effect_id = effect_id;
            AUTO_EFFECT_CHANGED = true;
            Serial.println("[la] Auto effect changed!");
            break;
        default:
            Serial.println("[l] Wrong option");
            return;
    }
}

void memory_options(String *serial_string)
{
    char option = next_char(serial_string);

    switch (option)
    {
        case 's':
            memory_save();
            Serial.println("[ms] Saved!");
            break;
        case 'l':
            memory_load();
            Serial.println("[ml] Loaded!");
            break;
        case 'c':
            memory_clear();
            Serial.println("[mc] Memory cleared!");
            break;
        case 'e':
            set_memory_status(1);
            Serial.println("[me] Memory enabled!");
            break;
        case 'd':
            set_memory_status(0);
            Serial.println("[md] Memory disabled!");
            break;
        default:
            Serial.println("[m] Wrong option");
            break;
    }
}

void cooler_options(String *serial_string)
{
    char option = next_char(serial_string);

    switch (option)
    {
        case 's':
            uint8_t fan;
            for (fan = 0; fan < FAN_COUNT; fan++)
            {
                Serial.print("Fan ");
                Serial.print(fan + 1);
                Serial.print(": ");
                Serial.println(FAN_SPEED[fan]);
                Serial.println();
            }
            break;
        case 'p':
            update_setting(serial_string, config.fan_power, ARRAY_LEN(config.fan_power), 1, nullptr);
            Serial.println("[c] Cooler power changed!");
            break;
        default:
            Serial.println("[c] Wrong option");
            break;
    }
}

void fast_serial()
{
    if (Serial.available())
    {
        String serial_string = Serial.readString();
        remove_endlines(&serial_string);

        // sub option must be present
        if (serial_string.length() < 2)
        {
            Serial.println("[:] Wrong length");
            return;
        }

        switch (next_char(&serial_string))
        {
            case 'l':
                led_options(&serial_string);
                break;
            case 'm':
                memory_options(&serial_string);
                break;
            case 'c':
                cooler_options(&serial_string);
                break;
            default:
                Serial.println("[:] Wrong option");
                break;
        }
    }
}
