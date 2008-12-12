__version__ = "$Revision$ $Date$"
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


APP_FILES = [
    "2201-06642-001.sip.ld",
    "2345-11300-010.sip.ld",
    "2345-11402-001.sip.ld",
    "2345-11500-030.sip.ld",
    "2345-11500-040.sip.ld",
    "2345-11600-001.sip.ld",
    "2345-11605-001.sip.ld",
    "2345-12200-001.sip.ld",
    "2345-12200-002.sip.ld",
    "2345-12200-004.sip.ld",
    "2345-12200-005.sip.ld",
    "2345-12450-001.sip.ld",
    "2345-12500-001.sip.ld",
    "2345-12560-001.sip.ld",
    "2345-12600-001.sip.ld",
    "2345-12670-001.sip.ld",
    "3111-15600-001.sip.ld",
    "3111-40000-001.sip.ld",
    "phone1.cfg",
    "sip.ld",
    "sip.ver",
    "sip.cfg",
    "SoundPointIPWelcome.wav"
]


def polycom_install_412(firmware, xfile):
    zip_path = fetchfw.zip_extract_all("polycom_fw", xfile.path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Polycom")
    
    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate
    
    for fw_file in os.listdir(zip_path):
        fw_src_path = os.path.join(zip_path, fw_file)
        fw_dst_path = os.path.join(fw_dst_dir, fw_file)
        shutil.copy2(fw_src_path, fw_dst_path)


def polycom_install_app_311(firmware, xfile):
    zip_path = fetchfw.zip_extract_all("polycom_app", xfile.path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Polycom")
    
    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate
    
    for fw_file in APP_FILES:
        fw_src_path = os.path.join(zip_path, fw_file)
        fw_dst_path = os.path.join(fw_dst_dir, fw_file)
        shutil.copy2(fw_src_path, fw_dst_path)
    
    fw_src_path = os.path.join(zip_path, "SoundPointIPLocalization")
    fw_dst_path = os.path.join(fw_dst_dir, "SoundPointIPLocalization")
    shutil.rmtree(fw_dst_path, True)
    shutil.copytree(fw_src_path, fw_dst_path)


def polycom_install(firmware):
    for xfile in firmware.remote_files:
        if xfile.filename == "spip_ssip_BootROM_4_1_2_release_sig.zip":
            polycom_install_412(firmware, xfile)
        elif xfile.filename == "spip_ssip_3_1_1_release_sig.zip":
            polycom_install_app_311(firmware, xfile)
        else:
            fetchfw.die("unsupported file for Polycom firmware")


fetchfw.register_install_fn("Polycom", None, polycom_install)
