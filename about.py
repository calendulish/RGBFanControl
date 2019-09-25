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

from gi.repository import Gtk
from typing import Union

# noinspection PyUnusedLocal
class AboutWindow(Gtk.AboutDialog):
    def __init__(self, *, parent_window: Union[Gtk.Window, Gtk.ApplicationWindow]) -> None:
        super().__init__(self, transient_for=parent_window, modal=True)
        self.set_program_name("RGBFanControl")

        self.set_authors([
            "Lara Maia",
        ])

        # self.set_translator_credits(__credits__)

        self.set_website("http://github.com/ShyPixie/RGBFanControl")
        self.set_website_label("Git Repository")
        # self.set_version(__version__)
        self.set_copyright("Lara Maia (C) <DATE> - dev@lara.click")
        self.set_comments("Made with Love <3")
        self.set_license_type(Gtk.License.GPL_3_0)

        # logo = GdkPixbuf.Pixbuf.new_from_file(<image>)
        # self.set_logo(logo)

        self.connect("response", self.on_dialog_response)

        self.present()

    def on_dialog_response(self, dialog: Gtk.Dialog, response_id: int) -> None:
        self.destroy()
