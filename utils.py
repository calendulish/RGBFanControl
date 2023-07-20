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

import logging
from collections import OrderedDict
from typing import Callable, Any, Union, Optional

from gi.repository import Gtk, Gdk

import config

log = logging.getLogger(__name__)


def led_color_to_rgba(led_color: str) -> Gdk.RGBA:
    return Gdk.RGBA(
        *[
            int(led_color[c * 3 : 3 + c * 3]) / 255.0
            for c in range(len(led_color) // 3)
        ]
    )


def rgba_to_led_color(rgba: Gdk.RGBA) -> str:
    return ''.join(['{:0=3d}'.format(int(color)) for color in rgba.to_string()[4:-1].split(',')])


class Section(Gtk.Frame):
    def __init__(self, name: str, label: str) -> None:
        super().__init__(label=label)
        self.set_label_align(0.03, 0.5)
        self.set_name(name)

        self.grid = Gtk.Grid()
        self.grid.set_name(name)
        self.grid.set_row_spacing(10)
        self.grid.set_column_spacing(10)
        self.grid.set_border_width(10)

        self.add(self.grid)

    def __item_factory(self, children: Callable[..., Gtk.Widget]) -> Any:
        # FIXME: https://github.com/python/mypy/issues/2477
        children_ = children  # type: Any



        class Item(children_):
            def __init__(self, name: str, section_name: str, label: str, items: Optional[OrderedDict]) -> None:
                super().__init__()
                self._name = name
                self._items = items

                self.label = Gtk.Label(label)
                self.label.set_name(name)
                self.label.set_halign(Gtk.Align.START)

                self.set_hexpand(True)
                self.set_name(name)
                self._section_name = section_name

            def show_all(self) -> None:
                self.label.show()
                super().show()

            def hide(self) -> None:
                self.label.hide()
                super().hide()

            def get_section_name(self) -> str:
                assert isinstance(self._section_name, str)
                return self._section_name

            def load(self) -> None:
                # noinspection PyUnusedLocal
                value: Union[str, bool, int]

                if self._name.startswith('_'):
                    return

                if isinstance(self, Gtk.ComboBoxText):
                    value = config.parser.get(self._section_name, self._name)

                    try:
                        current_option = list(self._items).index(value)
                    except ValueError:
                        error_message = f"Please, fix your config file. Accepted values for {self._name} are:\n{', '.join(self._items.keys())}"
                        log.exception(error_message)
                        fatal_error_dialog(error_message)
                        # unset active self
                        current_option = -1

                    self.set_active(current_option)

                if isinstance(self, Gtk.CheckButton):
                    value = config.parser.getboolean(self._section_name, self._name)
                    self.set_active(value)

                if isinstance(self, Gtk.Scale):
                    value = config.parser.getint(self._section_name, self._name)
                    self.set_value(value)

                if isinstance(self, Gtk.ColorButton):
                    value = config.parser.get(self._section_name, self._name)
                    self.set_rgba(led_color_to_rgba(value))


        return Item

    def new(
            self,
            name: str,
            label: str,
            children: Callable[..., Gtk.Widget],
            *grid_position: int,
            items: 'OrderedDict[str, str]' = None,
    ) -> Gtk.Widget:
        item = self.__item_factory(children)(name, self.get_name(), label, items)

        self.grid.attach(item.label, *grid_position, 1, 1)
        self.grid.attach_next_to(item, item.label, Gtk.PositionType.RIGHT, 1, 1)

        section = self.get_name()
        option = item.get_name()
        # noinspection PyUnusedLocal
        value: Union[str, bool, int]

        if option.startswith('_'):
            return item

        if isinstance(item, Gtk.ComboBoxText):
            value = config.parser.get(section, option)

            for value_ in items.values():
                item.append_text(value_)

        if isinstance(item, Gtk.Entry):
            if value := config.parser.get(section, option):
                item.set_text(value)

        if isinstance(item, Gtk.Scale):
            item.set_range(0, 255)

        return item


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
        color_raw = color_selection.get_current_rgba().to_string()
        color_string = ['{0:0=3d}'.format(int(color)) for color in color_raw[4:-1].split(',')]
        config.new("back", "led_colors", ''.join(color_string) * 29)

        serial_message = 'bc' + ''.join(config.parser.get("back", "led_colors"))
        self.application.send_serial(serial_message)

    def on_effect_changed(self, radio_button: Gtk.RadioButton, effect: int) -> None:
        if not radio_button.get_active():
            return

        serial_message = 'be'
        config.new("back", "effect", str(effect))
        serial_message += config.parser.get("back", "effect")
        self.application.send_serial(serial_message)


def fatal_error_dialog(error_message: str, transient_for: Optional[Gtk.Window] = None) -> None:
    log.critical(error_message)
    error_dialog = Gtk.MessageDialog(transient_for=transient_for)
    error_dialog.set_title("Fatal Error")
    error_dialog.set_markup(error_message)
    error_dialog.set_position(Gtk.WindowPosition.CENTER)
    error_dialog.run()


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
        pulse = max(pulse, 0)

        if self.fan_index == -1:
            config.new("front", "delay", '{0:0=3d}'.format(pulse) * 3)
        else:
            config.set_to_index("front", "delay", '{0:0=3d}'.format(pulse), self.fan_index)

        serial_message += config.parser.get("front", "delay")
        self.application.send_serial(serial_message)

    def on_divisor_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fi'
        pulse = int(value)
        pulse = max(pulse, 1)

        if self.fan_index == -1:
            config.new("front", "divisor", '{0:0=3d}'.format(pulse) * 3)
        else:
            config.set_to_index("front", "divisor", '{0:0=3d}'.format(pulse), self.fan_index)

        serial_message += config.parser.get("front", "divisor")
        self.application.send_serial(serial_message)

    def on_multiplier_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fm'
        pulse = int(value)
        pulse = max(pulse, 1)

        if self.fan_index == -1:
            config.new("front", "multiplier", '{0:0=3d}'.format(pulse) * 3)
        else:
            config.set_to_index("front", "multiplier", '{0:0=3d}'.format(pulse), self.fan_index)

        serial_message += config.parser.get("front", "multiplier")
        self.application.send_serial(serial_message)
