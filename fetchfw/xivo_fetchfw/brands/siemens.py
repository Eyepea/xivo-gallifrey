__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2009  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import os
import shutil
from xivo_fetchfw import fetchfw


def siemens_install(firmware):
    xfile = firmware.remote_files[0]
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Siemens", firmware.model.lower())

    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate

    shutil.copy2(xfile.path, fw_dst_dir)


fetchfw.register_install_fn("Siemens", None, siemens_install)
