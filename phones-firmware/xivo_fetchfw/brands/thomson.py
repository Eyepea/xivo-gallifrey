__version__ = "$Revision: 4548 $ $Date: 2008-10-30 19:03:20 +0100 (Thu, 30 Oct 2008) $"
__license__ = """
    Copyright (C) 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os
import shutil
from xivo_fetchfw import fetchfw


FW_FILES = {
    'ST2022': {
        '1.63': "v2022SG.080728.3.63.2.zz"
    },
    
    'ST2030': {
        '1.64': "v2030SG.080924.1.64.2.zz"
    }
}


def thomson_install(firmware):
    try:
        fw_file = FW_FILES[firmware.model][firmware.version]
    except KeyError:
        fetchfw.die("unsupported model/version (%s/%s)" % (firmware.model, firmware.version))
    
    assert len(firmware.remote_files) == 1
    zip_path = fetchfw.zip_extract_all(firmware.name, firmware.remote_files[0].path)
    fw_src_path = os.path.join(zip_path, "Binary", fw_file)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Thomson", "binary")
    fw_dst_path = os.path.join(fw_dst_dir, fw_file)
    
    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate
    
    shutil.copy2(fw_src_path, fw_dst_path)


fetchfw.register_install_fn("Thomson", None, thomson_install)
