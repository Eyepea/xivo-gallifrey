__version__ = "$Revision: 4580 $ $Date: 2008-11-03 18:26:20 +0100 (Mon, 03 Nov 2008) $"
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

SANGOMA_FW_PATH = "/etc/wanpipe/firmware"
SANGOMA_ECHOCAN_PATH = "/etc/wanpipe/wan_ec"

def sangoma_install(firmware):
    xfile = firmware.remote_files[0]
    
    try:
        os.makedirs(SANGOMA_FW_PATH)
    except OSError:
        pass # XXX: catching every OSError is not appropriate
    
    shutil.copy2(xfile.path, SANGOMA_FW_PATH)

def sangoma_echocan_install(firmware):
    xfile = firmware.remote_files[0]
    tgz_path = fetchfw.tgz_extract_all("sangoma_echocan", xfile.path)
    
    try:
        os.makedirs(SANGOMA_ECHOCAN_PATH)
    except OSError:
        pass # XXX: catching every OSError is not appropriate
    
    for root, dirs, files, in os.walk(tgz_path):
        for fw_file in files:
            if fw_file[-4:] == ".ima":
                fw_src_path = os.path.join(tgz_path, root, fw_file)
                shutil.copy2(fw_src_path, SANGOMA_ECHOCAN_PATH)


fetchfw.register_install_fn("Sangoma", 'Echo Canceller for all cards', sangoma_echocan_install)
fetchfw.register_install_fn("Sangoma", None, sangoma_install)
