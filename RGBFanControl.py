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

import gi
import serial

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gio

import config
import window
import about


class RGBFanControl(Gtk.Application):
    def __init__(self, id: str, flags: Gio.ApplicationFlags) -> None:
        super().__init__(application_id=id, flags=flags)
        self._main_window_id = 0

        self._serial = serial.Serial()
        self._serial.setDTR(False)
        self._serial.baudrate = 9600
        self._serial.port = 'COM9'

    @property
    def main_window(self) -> Gtk.ApplicationWindow:
        return self.get_window_by_id(self._main_window_id)

    def do_activate(self) -> None:
        main_window = window.Main(application=self, title="RGBFanControl")
        self._main_window_id = main_window.get_id()
        main_window.show()

    def do_startup(self) -> None:
        Gtk.Application.do_startup(self)

        about_action = Gio.SimpleAction.new("about")
        about_action.connect("activate", self.on_about_activate)
        self.add_action(about_action)

        exit_action = Gio.SimpleAction.new("exit")
        exit_action.connect("activate", self.on_exit_activate)
        self.add_action(exit_action)

    def do_reset(self) -> None:
        self._serial.setDTR(True)
        self._serial.open()
        self._serial.close()
        self._serial.setDTR(False)
        self._serial.open()
        self._serial.close()

    def send_serial(self, message: str) -> None:
        print(f"Sending {message}")
        self._serial.open()
        self._serial.write(message.encode())
        self._serial.close()

    def on_about_activate(self, action: Gio.Action, data: str) -> None:
        about_dialog = about.AboutWindow(parent_window=self.main_window)
        about_dialog.show()

    def on_exit_activate(self, action: Gio.Action, data: str) -> None:
        self.quit()


if __name__ == '__main__':
    config.init()

    application = RGBFanControl("click.lara.RGBFanControl", Gio.ApplicationFlags.FLAGS_NONE)

    # noinspection PyUnresolvedReferences
    application.run()
