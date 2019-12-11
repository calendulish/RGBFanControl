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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

inline void analog_lo(uint8_t led)
{
    *ANALOG_LED_PORT[led] &= ~ANALOG_LED_MASK[led];
}

inline void analog_hi(uint8_t led)
{
    *ANALOG_LED_PORT[led] |= ANALOG_LED_MASK[led];
}

inline void rgb_lo(uint8_t color)
{
    *ANALOG_RGB_PORT[color] &= ~ANALOG_RGB_MASK[color];
}

inline void rgb_hi(uint8_t color)
{
    *ANALOG_RGB_PORT[color] |= ANALOG_RGB_MASK[color];
}

inline void auto_lo(uint8_t led)
{
    *AUTOEFFECT_LED_PORT[led] &= ~AUTOEFFECT_LED_MASK[led];
}

inline void auto_hi(uint8_t led)
{
    *AUTOEFFECT_LED_PORT[led] |= AUTOEFFECT_LED_MASK[led];
}

#pragma clang diagnostic pop

void analog_pulse(uint8_t led, uint16_t delay_, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++)
    {
        auto_hi(led);
        delay(delay_);
        auto_lo(led);
        delay(50);
    }
}

void analog_show()
{
    for (uint8_t led = 0; led < ANALOG_LED_COUNT; led++)
    {
        analog_hi(led);

        for (uint8_t color = 0; color < 3; color++)
        {
            rgb_hi(color);

            for (uint8_t wait = 0; wait < ANALOG_LEDS[led][color]; wait++)
                delayMicroseconds(16);

            rgb_lo(color);
        }

        analog_lo(led);
    }
}