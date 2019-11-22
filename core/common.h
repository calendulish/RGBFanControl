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

class CUniLED
{
    private:
        static const uint8_t analog_led_count = 3;

        //static const byte analog_fan_power_pin[analog_led_count] = {7, 4, 6};

        const uint8_t analog_led_pin[analog_led_count] = {12, 3, 5};
        RwReg analog_led_pin_mask[analog_led_count];
        volatile RwReg *analog_led_pin_port[analog_led_count];

        const uint8_t analog_rgb_pin[3] = {10, 11, 9};
        RwReg analog_rgb_pin_mask[3];
        volatile RwReg *analog_rgb_pin_port[3];

        uint8_t analog_brightness = 255;
        CRGB analog_leds[analog_led_count];

        //

        static const uint8_t digital_led_count = 28;
        uint8_t digital_brightness = 125;
        CRGB digital_leds[digital_led_count];

        //unsigned int hue[led_count];

    public:
        CUniLED()
        {
            FastLED.addLeds<NEOPIXEL, 8>(this->digital_leds, this->digital_led_count);

            for(uint8_t pin; pin < analog_led_count; pin++)
            {
                pinMode(analog_led_pin[pin], OUTPUT);
                analog_led_pin_mask[pin] = digitalPinToBitMask(analog_led_pin[pin]);
                analog_led_pin_port[pin] = portOutputRegister(digitalPinToPort(analog_led_pin[pin]));
            }

            for(uint8_t pin; pin < 3; pin++)
            {
                pinMode(analog_rgb_pin[pin], OUTPUT);
                analog_rgb_pin_mask[pin] = digitalPinToBitMask(analog_rgb_pin[pin]);
                analog_rgb_pin_port[pin] = portOutputRegister(digitalPinToPort(analog_rgb_pin[pin]));
            }
        }

        inline static uint8_t get_analog_led_count() __attribute__ ((always_inline))
        {
            return analog_led_count;
        }

        inline static uint8_t get_digital_led_count() __attribute__ ((always_inline))
        {
            return digital_led_count;
        }

        inline static uint8_t get_total_led_count() __attribute__ ((always_inline))
        {
            return digital_led_count + analog_led_count;
        }

        inline void analog_lo(uint8_t led) __attribute__ ((always_inline))
        {
            *analog_led_pin_port[led] &= ~analog_led_pin_mask[led];
        }

        inline void analog_hi(uint8_t led) __attribute__ ((always_inline))
        {
            *analog_led_pin_port[led] |= analog_led_pin_mask[led];
        }

        inline void rgb_lo(uint8_t color) __attribute__ ((always_inline))
        {
            *analog_rgb_pin_port[color] &= ~analog_rgb_pin_mask[color];
        }

        inline void rgb_hi(uint8_t color) __attribute__ ((always_inline))
        {
            *analog_rgb_pin_port[color] |= analog_rgb_pin_mask[color];
        }

        void analog_show()
        {
            for(uint8_t l = 0; l < this->analog_led_count; l++)
            {
                if(front_settings.led_power[l] == HIGH)
                {
                    analog_hi(l);

                    for(uint8_t c = 0; c < 3; c++)
                    {
                        for(uint8_t h = 0; h < this->analog_leds[l][c]; h++)
                        {
                            rgb_hi(c);
                            delayMicroseconds(5);
                            rgb_lo(c);
                        }
                    }
                }
                analog_lo(l);
            }
        }

        void digital_show() {
            FastLED.show();
        }

        void show()
        {
            this->analog_show();
            this->digital_show();
        }

        inline void analog_set_colors()  __attribute__ ((always_inline))
        {
            for(uint8_t i = 0; i < this->get_analog_led_count(); i++)
            {
                this->analog_leds[i].r = front_settings.rgb[i][0];
                this->analog_leds[i].g = front_settings.rgb[i][1];
                this->analog_leds[i].b = front_settings.rgb[i][2];
            }
        }

        inline void digital_set_colors() __attribute__ ((always_inline))
        {
            for(uint8_t i = 0; i < this->get_digital_led_count(); i++)
            {
                this->digital_leds[i].r = back_settings.rgb[i][0];
                this->digital_leds[i].g = back_settings.rgb[i][1];
                this->digital_leds[i].b = back_settings.rgb[i][2];
            }
        }

        inline void set_colors() __attribute__ ((always_inline))
        {
            this->analog_set_colors();
            this->digital_set_colors();
        }

        void fill_rainbow(uint8_t initialhue, uint8_t deltahue = 5)
        {
            ::fill_rainbow(this->analog_leds, this->analog_led_count, initialhue, deltahue);
            ::fill_rainbow(this->digital_leds, this->digital_led_count, initialhue, deltahue);
        }
};
CUniLED *UniLED = new CUniLED();


void save() {
    EEPROM.put(0, front_settings);
    EEPROM.put(sizeof(front_settings), back_settings);
}

void load() {
    EEPROM.get(0, front_settings);
    EEPROM.get(sizeof(front_settings), back_settings);
}

void clear() {
    for(unsigned int i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0);
    }
}

byte get_eeprom_status() {
    byte EEPROM_status;

    EEPROM.get(sizeof(front_settings) + sizeof(back_settings), EEPROM_status);

    return EEPROM_status;
}

void update_eeprom_status(byte EEPROM_status) {
    EEPROM.put(sizeof(front_settings) + sizeof(back_settings), EEPROM_status);
}
