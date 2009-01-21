__version__ = "$Revision$ $Date$"
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


FW_FILES = {
    'ST2022': {
        '3.65': os.path.join("Binary", "v2022SG.081010.3.65.1.zz")
    },

    'ST2030': {
        '1.66': os.path.join("ST2030_SG_v1[1].66_Release_package", "Binary", "v2030SG.081210.1.66.2.zz")
    }
}


def thomson_install(firmware):
    try:
        fw_src_path = FW_FILES[firmware.model][firmware.version]
    except KeyError:
        fetchfw.die("unsupported model/version (%s/%s)" % (firmware.model, firmware.version))

    assert len(firmware.remote_files) == 1
    zip_path = fetchfw.zip_extract_all(firmware.name, firmware.remote_files[0].path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Thomson", "binary")
    fw_dst_path = os.path.join(fw_dst_dir, os.path.basename(fw_src_path))

    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate

    shutil.copy2(os.path.join(zip_path, fw_src_path), fw_dst_path)


fetchfw.register_install_fn("Thomson", None, thomson_install)
