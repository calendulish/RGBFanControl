from gi.repository import Gtk, Gdk


class FanSettings(Gtk.Frame):
    def __init__(self, application, fan_index: int) -> None:
        super().__init__()
        self.application = application
        self.fan_index = fan_index

        if fan_index == -1:
            self.set_label('Cooler [ALL]')
        else:
            self.set_label(f'Cooler [{fan_index + 1}]')

        self.set_label_align(0.02, 0.5)

        self._grid = Gtk.Grid()
        self._grid.set_border_width(5)
        self._grid.set_row_spacing(5)
        self.add(self._grid)

        self.power = Gtk.ToggleButton('Power')
        self.power.connect('clicked', self.on_power_clicked)

        if self.application.power_status[fan_index] == '1':
            self.power.set_active(True)

        self._grid.attach(self.power, 0, 0, 1, 1)

        self.color = Gtk.ColorButton()
        self.color.connect('notify::color', self.on_led_color_activated)
        self.color.set_rgba(self._led_color_to_rgba(self.application.leds_colors[fan_index]))

        self._grid.attach(self.color, 1, 0, 1, 1)

        self.effect = Gtk.RadioButton("Color Effect")
        self.effect.connect('toggled', self.on_effect_changed, 0)
        self._grid.attach(self.effect, 0, 1, 1, 1)

        self.hue_color_cycle = Gtk.RadioButton("Hue Color Cycle", group=self.effect)
        self.hue_color_cycle.connect('toggled', self.on_effect_changed, 1)
        self._grid.attach(self.hue_color_cycle, 0, 2, 1, 1)

        self.delay = Gtk.Scale.new_with_range(Gtk.Orientation.VERTICAL, 0, 200, 1)
        self.delay.connect('change-value', self.on_delay_change)
        self.delay.set_value(int(self.application.delay[fan_index]))
        self._grid.attach(self.delay, 2, 0, 1, 3)

        self.divisor = Gtk.Scale.new_with_range(Gtk.Orientation.VERTICAL, 1, 100, 1)
        self.divisor.connect('change-value', self.on_divisor_change)
        self.divisor.set_value(int(self.application.divisor[fan_index]))
        self._grid.attach(self.divisor, 3, 0, 1, 3)

        self.multiplier = Gtk.Scale.new_with_range(Gtk.Orientation.VERTICAL, 1, 100, 1)
        self.multiplier.connect('change-value', self.on_multiplier_change)
        self.multiplier.set_value(int(self.application.multiplier[fan_index]))
        self._grid.attach(self.multiplier, 4, 0, 1, 3)

    @staticmethod
    def _led_color_to_rgba(led_color) -> Gdk.RGBA:
        colors_list = list(led_color)
        red = int(''.join(colors_list[0:3])) / 256
        green = int(''.join(colors_list[3:6])) / 256
        blue = int(''.join(colors_list[6:9])) / 256

        return Gdk.RGBA(red, green, blue)

    def on_power_clicked(self, button: Gtk.Button) -> None:
        serial_message = 'fl'
        status = str(int(button.get_active()))

        if self.fan_index == -1:
            self.application.power_status = [status] * 3
        else:
            self.application.power_status[self.fan_index] = status

        serial_message += ''.join(self.application.power_status)

        self.application.send_serial(serial_message)

    def on_led_color_activated(self, color_button: Gtk.ColorButton, param: str) -> None:
        serial_message = 'fc'
        color_string = ['{0:0=3d}'.format(int(color)) for color in color_button.get_rgba().to_string()[4:-1].split(',')]

        if self.fan_index == -1:
            self.application.leds_colors = [''.join(color_string)] * 3
        else:
            self.application.leds_colors[self.fan_index] = ''.join(color_string)

        serial_message += ''.join(self.application.leds_colors)
        self.application.send_serial(serial_message)

    def on_effect_changed(self, radio_button: Gtk.RadioButton, effect: int) -> None:
        if not radio_button.get_active():
            return

        serial_message = 'fe'

        if self.fan_index == -1:
            self.application.effect = [str(effect)] * 3
        else:
            self.application.effect[self.fan_index] = str(effect)

        serial_message += ''.join(self.application.effect)
        self.application.send_serial(serial_message)

    def on_delay_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fd'
        pulse = int(value)
        if pulse < 0: pulse = 0

        if self.fan_index == -1:
            self.application.delay = ['{0:0=3d}'.format(pulse)] * 3
        else:
            self.application.delay[self.fan_index] = '{0:0=3d}'.format(pulse)

        serial_message += ''.join(self.application.delay)
        self.application.send_serial(serial_message)

    def on_divisor_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fi'
        pulse = int(value)
        if pulse < 1: pulse = 1

        if self.fan_index == -1:
            self.application.divisor = ['{0:0=3d}'.format(pulse)] * 3
        else:
            self.application.divisor[self.fan_index] = '{0:0=3d}'.format(pulse)

        serial_message += ''.join(self.application.divisor)
        self.application.send_serial(serial_message)

    def on_multiplier_change(self, scale: Gtk.Scale, type_: str, value: str) -> None:
        serial_message = 'fm'
        pulse = int(value)
        if pulse < 1: pulse = 1

        if self.fan_index == -1:
            self.application.multiplier = ['{0:0=3d}'.format(pulse)] * 3
        else:
            self.application.multiplier[self.fan_index] = '{0:0=3d}'.format(pulse)

        serial_message += ''.join(self.application.multiplier)
        self.application.send_serial(serial_message)
