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

from gi.repository import Gtk, Gdk

import config


def _led_color_to_rgba(led_color: str) -> Gdk.RGBA:
    return Gdk.RGBA(*[int(led_color[c * 3:3 + c * 3]) / 255.0 for c in range(int(len(led_color) / 3))])


class BFanSettings(Gtk.Frame):
    def __init__(self, application, fan_index: int) -> None:
        super().__init__()
        self.application = application
        self.fan_index = fan_index

        if fan_index == -1:
            self.set_label('Neo All')
        else:
            self.set_label(f'Fan Neo [{fan_index + 1}]')

        self.set_label_align(0.02, 0.5)

        self._grid = Gtk.Grid()
        self._grid.set_border_width(5)
        self._grid.set_row_spacing(5)
        self.add(self._grid)

        self.select_color = Gtk.Button("Set Color Manually")
        self.select_color.connect("clicked", self.on_select_color_clicked)

        self._grid.attach(self.select_color, 1, 0, 1, 1)

        self.effect = Gtk.RadioButton("Color Effect")
        self._grid.attach(self.effect, 0, 1, 1, 1)

        self.hue_color_cycle = Gtk.RadioButton("Hue Color Cycle", group=self.effect)
        self._grid.attach(self.hue_color_cycle, 0, 2, 1, 1)

        if config.parser.get("back", "effect") == '1':
            self.hue_color_cycle.props.active = True
            self.hue_color_cycle.set_active(True)
        else:
            self.effect.props.active = True
            self.effect.set_active(True)

        self.effect.connect('toggled', self.on_effect_changed, 0)
        self.hue_color_cycle.connect('toggled', self.on_effect_changed, 1)

    def on_select_color_clicked(self, button: Gtk.Button) -> None:
        color_dialog = Gtk.ColorSelectionDialog()
        color_dialog.props.cancel_button.hide()

        color_selection = color_dialog.get_color_selection()
        rgba_color = _led_color_to_rgba(config.get_from_index('back', 'led_colors', self.fan_index, 9))
        color_selection.set_current_rgba(rgba_color)
        color_selection.connect('color_changed', self.on_led_color_changed)

        color_dialog.connect("response", self.on_select_color_response)
        color_dialog.run()

    def on_select_color_response(self, dialog, response):
        if response == Gtk.ResponseType.OK:
            color_selection = dialog.get_color_selection()
            self.on_led_color_changed(color_selection)

        dialog.destroy()

    def on_led_color_changed(self, color_selection: Gtk.ColorSelection) -> None:
        serial_message = 'bc'
        color_raw = color_selection.get_current_rgba().to_string()
        color_string = ['{0:0=3d}'.format(int(color)) for color in color_raw[4:-1].split(',')]
        config.new("back", "led_colors", ''.join(color_string) * 29)

        serial_message += ''.join(config.parser.get("back", "led_colors"))
        self.application.send_serial(serial_message)

    def on_effect_changed(self, radio_button: Gtk.RadioButton, effect: int) -> None:
        if not radio_button.get_active():
            return

        serial_message = 'be'
        config.new("back", "effect", str(effect))
        serial_message += config.parser.get("back", "effect")
        self.application.send_serial(serial_message)


class FFanSettings(Gtk.Frame):
    def __init__(self, application, fan_index: int) -> None:
        super().__init__()
        self.application = application
        self.fan_index = fan_index

        if fan_index == -1:
            self.set_label('ALL')
        else:
            self.set_label(f'Fan [{fan_index + 1}]')

        self.set_label_align(0.02, 0.5)

        self._grid = Gtk.Grid()
        self._grid.set_border_width(5)
        self._grid.set_row_spacing(5)
        self.add(self._grid)

        self.power = Gtk.ToggleButton('Power')
        self._grid.attach(self.power, 0, 0, 1, 1)

        if config.get_from_index("front", "power_status", fan_index) == '1':
            self.power.set_active(True)

        self.power.connect('clicked', self.on_power_clicked)

        self.select_color = Gtk.Button("Set Color Manually")
        self.select_color.connect("clicked", self.on_select_color_clicked)
        self._grid.attach(self.select_color, 1, 0, 1, 1)

        self.effect = Gtk.RadioButton("Color Effect")
        self._grid.attach(self.effect, 0, 1, 1, 1)

        self.hue_color_cycle = Gtk.RadioButton("Hue Color Cycle", group=self.effect)
        self._grid.attach(self.hue_color_cycle, 0, 2, 1, 1)

        if config.get_from_index("front", "effect", self.fan_index) == '1':
            self.hue_color_cycle.props.active = True
            self.hue_color_cycle.set_active(True)
        else:
            self.effect.props.active = True
            self.effect.set_active(True)

        self.effect.connect('toggled', self.on_effect_changed, 0)
        self.hue_color_cycle.connect('toggled', self.on_effect_changed, 1)

        self.delay = Gtk.Scale.new_with_range(Gtk.Orientation.VERTICAL, 0, 200, 1)
        self.delay.set_value(int(config.get_from_index("front", "delay", fan_index, 3)))
        self.delay.connect('change-value', self.on_delay_change)
        self._grid.attach(self.delay, 2, 0, 1, 3)

        self.divisor = Gtk.Scale.new_with_range(Gtk.Orientation.VERTICAL, 1, 100, 1)
        self.divisor.set_value(int(config.get_from_index("front", "divisor", fan_index, 3)))
        self.divisor.connect('change-value', self.on_divisor_change)
        self._grid.attach(self.divisor, 3, 0, 1, 3)

        self.multiplier = Gtk.Scale.new_with_range(Gtk.Orientation.VERTICAL, 1, 100, 1)
        self.multiplier.set_value(int(config.get_from_index("front", "multiplier", fan_index, 3)))
        self.multiplier.connect('change-value', self.on_multiplier_change)
        self._grid.attach(self.multiplier, 4, 0, 1, 3)

    def on_power_clicked(self, button: Gtk.Button) -> None:
        serial_message = 'fl'
        status = str(int(button.get_active()))

        if self.fan_index == -1:
            config.new("front", "power_status", status * 3)
        else:
            config.set_to_index("front", "power_status", status, self.fan_index)

        serial_message += config.parser.get("front", "power_status")

        self.application.send_serial(serial_message)

    def on_select_color_clicked(self, button: Gtk.Button) -> None:
        color_dialog = Gtk.ColorSelectionDialog()
        color_dialog.props.cancel_button.hide()

        color_selection = color_dialog.get_color_selection()
        rgba_color = _led_color_to_rgba(config.get_from_index('back', 'led_colors', self.fan_index, 9))
        color_selection.set_current_rgba(rgba_color)
        color_selection.connect('color_changed', self.on_led_color_changed)

        color_dialog.connect("response", self.on_select_color_response)
        color_dialog.run()

    def on_select_color_response(self, dialog, response):
        if response == Gtk.ResponseType.OK:
            color_selection = dialog.get_color_selection()
            self.on_led_color_changed(color_selection)

        dialog.destroy()

    def on_led_color_changed(self, color_selection: Gtk.ColorSelection) -> None:
        serial_message = 'fc'
        raw_color = color_selection.get_current_rgba().to_string()
        color_string = ['{0:0=3d}'.format(int(color)) for color in raw_color[4:-1].split(',')]

        if self.fan_index == -1:
            config.new("front", "led_colors", ''.join(color_string) * 3)
        else:
            config.set_to_index("front", "led_colors", ''.join(color_string), self.fan_index)

        serial_message += config.parser.get("front", "led_colors")
        self.application.send_serial(serial_message)

    def on_effect_changed(self, radio_button: Gtk.RadioButton, effect: int) -> None:
        if not radio_button.get_active():
            return

        serial_message = 'fe'

        if self.fan_index == -1:
            config.new("front", "effect", str(effect) * 3)
        else:
            config.set_to_index("front", "effect", str(effect), self.fan_index)

        serial_message += config.parser.get("front", "effect")
        self.application.send_serial(serial_message)

    def on_delay_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fd'
        pulse = int(value)
        if pulse < 0: pulse = 0

        if self.fan_index == -1:
            config.new("front", "delay", '{0:0=3d}'.format(pulse) * 3)
        else:
            config.set_to_index("front", "delay", '{0:0=3d}'.format(pulse), self.fan_index)

        serial_message += config.parser.get("front", "delay")
        self.application.send_serial(serial_message)

    def on_divisor_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fi'
        pulse = int(value)
        if pulse < 1: pulse = 1

        if self.fan_index == -1:
            config.new("front", "divisor", '{0:0=3d}'.format(pulse) * 3)
        else:
            config.set_to_index("front", "divisor", '{0:0=3d}'.format(pulse), self.fan_index)

        serial_message += config.parser.get("front", "divisor")
        self.application.send_serial(serial_message)

    def on_multiplier_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fm'
        pulse = int(value)
        if pulse < 1: pulse = 1

        if self.fan_index == -1:
            config.new("front", "multiplier", '{0:0=3d}'.format(pulse) * 3)
        else:
            config.set_to_index("front", "multiplier", '{0:0=3d}'.format(pulse), self.fan_index)

        serial_message += config.parser.get("front", "multiplier")
        self.application.send_serial(serial_message)
