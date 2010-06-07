# -*- coding: UTF-8 -*-

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>

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

FW_PATHS = {
    'T28': {
        '2.43.0.50': '2.43.0.50.rom'
    },

    'T26': {
        '6.43.0.50': '6.43.0.50.rom'
    },

    'T22': {
        '7.43.0.50': '7.43.0.50.rom'
    },

    'T20': {
        '9.43.0.50': '9.43.0.50.rom'
    }
}


def yealink_install_fw(firmware, xfile):
    zip_path = fetchfw.zip_extract_all(firmware.name, xfile.path)
    fw_file_tmp = FW_PATHS[firmware.model][firmware.version]
    fw_file = xfile.filename
    fw_src_path = os.path.join(zip_path, fw_file_tmp)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, 'Yealink')
    fw_dst_path = os.path.join(fw_dst_dir, fw_file)
    
    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate

    shutil.copy2(fw_src_path, fw_dst_path)


def yealink_install(firmware):
    for xfile in firmware.remote_files:
        yealink_install_fw(firmware, xfile)


fetchfw.register_install_fn('Yealink', None, yealink_install)
