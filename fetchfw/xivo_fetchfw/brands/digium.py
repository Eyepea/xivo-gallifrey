__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2010  Proformatique <technique@proformatique.com>

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


def digium_install(firmware):
    xfile = firmware.remote_files[0]
    tgz_path = fetchfw.tgz_extract_all("digium_fw", xfile.path)
    fw_dst_dir = fetchfw.KFW_PATH
    
    for fw_file in os.listdir(tgz_path):
        fw_src_path = os.path.join(tgz_path, fw_file)
        fw_dst_path = os.path.join(fw_dst_dir, fw_file)
        shutil.copy2(fw_src_path, fw_dst_path)


fetchfw.register_install_fn("Digium", None, digium_install)
