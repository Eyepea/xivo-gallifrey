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
import os.path
import shutil
import fetchfw

def snom_install(firmware):
	assert len(firmware.remote_files) == 1
	fw_dst_dir = os.path.join(fetchfw.tftp_path, "Snom", "Firmware")
	fw_dst_path = os.path.join(fw_dst_dir, firmware.remote_files[0].filename)

	try:
		os.makedirs(fw_dst_dir)
	except OSError:
		pass

	shutil.copy2(firmware.remote_files[0].path, fw_dst_path)

fetchfw.register_install_fn("Snom", None, snom_install)

def snom_m3_install(firmware):
	fw_dst_dir = os.path.join(fetchfw.tftp_path, "Snom", "Firmware")

	try:
		os.makedirs(fw_dst_dir)
	except OSError:
		pass

	for file in firmware.remote_files:
		fw_dst_path = os.path.join(fw_dst_dir, file.filename)
		shutil.copy2(file.path, fw_dst_path)

fetchfw.register_install_fn("Snom", "m3", snom_m3_install)
