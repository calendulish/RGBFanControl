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

inline void fan_lo(uint8_t fan)
{
    *FAN_PORT[fan] &= ~FAN_MASK[fan];
}

inline void fan_hi(uint8_t fan)
{
    *FAN_PORT[fan] |= FAN_MASK[fan];
}

inline void fan_speed_lo(uint8_t fan)
{
    *FAN_SPEED_PORT[fan] &= ~FAN_SPEED_MASK[fan];
}

inline void fan_speed_hi(uint8_t fan)
{
    *FAN_SPEED_PORT[fan] |= FAN_SPEED_MASK[fan];
}

inline byte fan_speed_read(uint8_t fan)
{
    return *FAN_SPEED_IN[fan] & FAN_SPEED_MASK[fan];
}

#pragma clang diagnostic pop


// Workaround for back led power
bool guess_fan_power()
{
    for (int fan : FAN_SPEED)
        if (fan != 0) return true;

    return false;
}