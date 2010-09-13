# -*- coding: UTF-8 -*-

__version__ = "$Revision Date$"
__license__ = """
    Copyright (C) 2010  Proformatique <technique@proformatique.com>

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

import os.path
import shutil

from xivo_fetchfw import fetchfw


def nortel_install_fw(xfile):
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, 'Nortel', 'firmware')
    fw_dst_path = os.path.join(fw_dst_dir, xfile.filename)
    fetchfw.makedirs(fw_dst_dir)
    shutil.copy2(xfile.path, fw_dst_path)
    

def nortel_install(firmware):
    nortel_install_fw(firmware.remote_files[0])


fetchfw.register_install_fn('Nortel', None, nortel_install)
