#!/usr/bin/env python
#
# Lara Maia <dev@lara.click> 2019
#
# The RGBFanControl is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# The RGBFanControl is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see http://www.gnu.org/licenses/.
#

import configparser
import os
from typing import Any

parser = configparser.RawConfigParser()
config_file = os.path.join(os.getcwd(), 'config.ini')


def init() -> None:
    parser.read_dict(
        {
            'front': {
                'power_status': '000',
                'led_colors': '255000000' * 3,
                'delay': '000' * 3,
                'divisor': '001' * 3,
                'multiplier': '001' * 3,
                'effect': '000',
            },
            'back': {
                'led_colors': '000000000' * 29,
                'effect': '0',
            },
            'effects': {
                'primary_effect': '050',
                'secondary_effect': '000',
                'extra_effect': '000',
            },
        }
    )

    if os.path.isfile(config_file):
        parser.read(config_file)


def new(section: str, option: str, value: Any) -> None:
    if parser.get(section, option) != str(value):
        parser.set(section, option, str(value))

        with open(config_file, 'w') as config_file_object:
            parser.write(config_file_object)


def get_from_index(section: str, option: str, index: int, size: int = 1) -> str:
    if index == -1:
        return '0'

    return parser.get(section, option)[size * index:size * index + size]


def set_to_index(section: str, option: str, value: str, index: int) -> None:
    old_value = parser.get(section, option)
    value_size = len(value)
    temp_value = [old_value[k * value_size:value_size + k * value_size] for k in
                  range(int(len(old_value) / value_size))]
    temp_value[index] = value

    new(section, option, ''.join(temp_value))
