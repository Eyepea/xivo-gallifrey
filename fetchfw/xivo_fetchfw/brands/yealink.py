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

import os.path
import shutil
from xivo_fetchfw import fetchfw

FW_PATHS = {
    'T28': {
        '2.50.0.50': ('Yealink SIP-T28(P) V50 Firmware-2.50.0.50.rom', '2.50.0.50.rom')
    },

    'T26': {
        '6.50.0.50': ('Yealink SIP-T26(P) V50 Firmware-6.50.0.50.rom', '6.50.0.50.rom')
    },

    'T22': {
        '7.50.0.50': ('Yealink SIP-T22(P) V50 Firmware-7.50.0.50.rom', '7.50.0.50.rom')
    },

    'T20': {
        '9.50.0.50': ('Yealink SIP-T20(P) V50 Firmware-9.50.0.50.rom', '9.50.0.50.rom')
    }
}


def yealink_install(firmware):
    zip_path = fetchfw.zip_extract_all(firmware.name, firmware.remote_files[0].path)
    fw_src_name, fw_dst_name = FW_PATHS[firmware.model][firmware.version]
    fw_src_path = os.path.join(zip_path, fw_src_name)
    fw_dst_path = os.path.join(fetchfw.TFTP_PATH, 'Yealink', fw_dst_name)
    
    fetchfw.makedirs(os.path.dirname(fw_dst_path))
    shutil.copy2(fw_src_path, fw_dst_path)


fetchfw.register_install_fn('Yealink', None, yealink_install)
