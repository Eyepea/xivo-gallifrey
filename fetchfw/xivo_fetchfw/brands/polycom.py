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


APP_FILES = [
    "2345-11402-001.sip.ld",
    "2345-12200-001.sip.ld",
    "2345-12200-002.sip.ld",
    "2345-12200-004.sip.ld",
    "2345-12200-005.sip.ld",
    "2345-12360-001.sip.ld",
    "2345-12365-001.sip.ld",
    "2345-12375-001.sip.ld",
    "2345-12450-001.sip.ld",
    "2345-12500-001.sip.ld",
    "2345-12560-001.sip.ld",
    "2345-12600-001.sip.ld",
    "2345-12670-001.sip.ld",
    "2345-17960-001.sip.ld",
    "3111-15600-001.sip.ld",
    "3111-30900-001.sip.ld",
    "3111-40000-001.sip.ld",
    "Beach256x116.jpg",
    "BeachEM.jpg",
    "Beach.jpg",
    "Jellyfish256x116.jpg",
    "JellyfishEM.jpg",
    "Jellyfish.jpg",
    "Leaf256x116.jpg",
    "LeafEM.jpg",
    "Leaf.jpg",
    "LoudRing.wav",
    "Mountain256x116.jpg",
    "MountainEM.jpg",
    "Mountain.jpg",
    "Palm256x116.jpg",
    "PalmEM.jpg",
    "Palm.jpg",
    "phone1.cfg",
    "Sailboat256x116.jpg",
    "SailboatEM.jpg",
    "Sailboat.jpg",
    "sip.cfg",
    "sip.ver",
    "SoundPointIPWelcome.wav",
]


def polycom_install_bootrom(xfile):
    zip_path = fetchfw.zip_extract_all("polycom_bootrom", xfile.path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Polycom")
    
    fetchfw.makedirs(fw_dst_dir)
    
    for fw_file in os.listdir(zip_path):
        fw_src_path = os.path.join(zip_path, fw_file)
        fw_dst_path = os.path.join(fw_dst_dir, fw_file)
        shutil.copy2(fw_src_path, fw_dst_path)


def polycom_install_app(xfile):
    zip_path = fetchfw.zip_extract_all("polycom_app", xfile.path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Polycom")
    
    fetchfw.makedirs(fw_dst_dir)
    
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
        if xfile.filename.lower().find("bootrom") > -1:
            polycom_install_bootrom(xfile)
        else:
            polycom_install_app(xfile)


fetchfw.register_install_fn("Polycom", None, polycom_install)
