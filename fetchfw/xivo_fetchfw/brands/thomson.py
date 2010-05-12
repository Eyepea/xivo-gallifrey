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


FW_PATHS = {
    'ST2022': {
        '4.68': os.path.join("ST2022_SG_v4[2].68_SED_Release_Package", "Binary")
    },

    'ST2030': {
        '2.69': os.path.join("ST2030_SG_v2[2].69_SED_Release_packag","Binary")
    },

    'TB30': {
        '1.70': os.path.join("TB30_SG_V1[1].70_Release_Package_20100421","Binary")
    }
}


def thomson_install(firmware):
    try:
        fw_src_path = FW_PATHS[firmware.model][firmware.version]
    except KeyError:
        fetchfw.die("unsupported model/version (%s/%s)" % (firmware.model, firmware.version))

    assert len(firmware.remote_files) == 1
    zip_path = fetchfw.zip_extract_all(firmware.name, firmware.remote_files[0].path)
    fw_zip_path = os.path.join(zip_path, fw_src_path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Thomson", "binary")

    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate

    modelnum = firmware.model[2:]
    if modelnum == TB30:
        pre_dsp_file = "%sS_V" % modelnum 
        pre_fw_file = "%sS." % modelnum
    else:
        pre_dsp_file = "v%s_dsp_" % modelnum
        pre_fw_file = "v%sSG." % modelnum

    for fw_file in os.listdir(fw_zip_path):
        if fw_file.startswith(pre_dsp_file) or fw_file.startswith(pre_fw_file):
            shutil.copy2(os.path.join(fw_zip_path, fw_file),
                         os.path.join(fw_dst_dir, fw_file))

fetchfw.register_install_fn("Thomson", None, thomson_install)
