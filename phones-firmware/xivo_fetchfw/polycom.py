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
import fetchfw

app_files = [
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
	"2345-12500-001.sip.ld",
	"2345-12560-001.sip.ld",
	"2345-12600-001.sip.ld",
	"sip.ld",
	"sip.ver",
	"SoundPointIPWelcome.wav"
]

def polycom_install_400(firmware, file):
	zip_path = fetchfw.zip_extract_all("polycom_fw", file.path)
	fw_dst_dir = os.path.join(fetchfw.tftp_path, "Polycom")

	try:
		os.makedirs(fw_dst_dir)
	except OSError:
		pass

	for fw_file in os.listdir(zip_path):
		fw_src_path = os.path.join(zip_path, fw_file)
		fw_dst_path = os.path.join(fw_dst_dir, fw_file)
		shutil.copy2(fw_src_path, fw_dst_path)

def polycom_install_app_222(firmware, file):
	zip_path = fetchfw.zip_extract_all("polycom_app", file.path)
	fw_dst_dir = os.path.join(fetchfw.tftp_path, "Polycom")

	try:
		os.makedirs(fw_dst_dir)
	except OSError:
		pass

	for fw_file in app_files:
		fw_src_path = os.path.join(zip_path, fw_file)
		fw_dst_path = os.path.join(fw_dst_dir, fw_file)
		shutil.copy2(fw_src_path, fw_dst_path)

	fw_src_path = os.path.join(zip_path, "SoundPointIPLocalization")
	fw_dst_path = os.path.join(fw_dst_dir, "SoundPointIPLocalization")
	shutil.rmtree(fw_dst_path, True)
	shutil.copytree(fw_src_path, fw_dst_path)

def polycom_install(firmware):
	for file in firmware.remote_files:
		if file.filename == "BootRom_4_0_0_release_sig.zip":
			polycom_install_400(firmware, file)
		elif file.filename == "spip_ssip_2_2_2_release_sig.zip":
			polycom_install_app_222(firmware, file)
		else:
			fetchfw.die("unsupported file for Polycom firmware")

fetchfw.register_install_fn("Polycom", None, polycom_install)
