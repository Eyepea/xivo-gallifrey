__version__ = "$Revision: 5445 $ $Date: 2009-02-26 11:42:21 +0100 (jeu 26 fév 2009) $"
__license__ = """
    Copyright (C) 2008-2009  Proformatique <technique@proformatique.com>

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


def yealink_install_fw(firmware, xfile):
    zip_path = fetchfw.zip_extract_all(firmware.name, xfile.path)
    fw_file = xfile.filename
    fw_src_path = os.path.join(zip_path, fw_file)
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
