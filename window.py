#!/usr/bin/env python
#
# Lara Maia <dev@lara.click> <DATE>
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

gi.require_version('Gtk', '3.0')
from gi.repository import Gtk, Gio, Gdk
import utils

class Main(Gtk.ApplicationWindow):
    def __init__(self, *, application: Gtk.Application, title: str) -> None:
        super().__init__(application=application, title=title)
        self.application = application

        header_bar = Gtk.HeaderBar()
        header_bar.set_show_close_button(True)

        # icon_image = GdkPixbuf.Pixbuf.new_from_file_at_size(<image_path>, 28, 28)

        # icon = Gtk.Image()
        # icon.set_from_pixbuf(icon_image)

        # header_bar.pack_start(icon)

        menu = Gio.Menu()
        menu.append("About", "app.about")
        menu.append("Exit", "app.exit")

        menu_button = Gtk.MenuButton()
        # menu_button.set_relief(Gtk.ReliefStyle.NONE)
        menu_button.set_menu_model(menu)
        header_bar.pack_end(menu_button)

        self.set_titlebar(header_bar)
        self.set_default_size(650, 450)
        self.set_resizable(False)
        self.set_position(Gtk.WindowPosition.CENTER)
        self.set_title(title)

        main_grid = Gtk.Grid()
        main_grid.set_border_width(10)
        main_grid.set_row_spacing(10)
        main_grid.set_column_spacing(10)
        main_grid.set_column_homogeneous(True)
        self.add(main_grid)

        fan1 = utils.FanSettings(application, 0)
        fan2 = utils.FanSettings(application, 1)
        fan3 = utils.FanSettings(application, 2)
        fan_all = utils.FanSettings(application, -1)

        main_grid.attach(fan1, 0, 0, 1, 1)
        main_grid.attach(fan2, 1, 0, 1, 1)
        main_grid.attach(fan3, 0, 1, 1, 1)
        main_grid.attach(fan_all, 1, 1, 1, 1)

        reset = Gtk.Button("Reset")
        reset.connect('clicked', self.on_reset_clicked)
        main_grid.attach(reset, 0, 3, 2, 1)

        self.show_all()

    def on_reset_clicked(self, button: Gtk.Button) -> None:
        self.application.do_reset()