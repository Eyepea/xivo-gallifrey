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

import distutils.dir_util
import os
import shutil
from xivo_fetchfw import fetchfw


def snom3xx_install_lang(xfile):
    src_dir = fetchfw.zip_extract_all('snom-lang', xfile.path)
    dst_dir = os.path.join(fetchfw.TFTP_PATH, "Snom/i18n")
    distutils.dir_util.copy_tree(src_dir, dst_dir)


def snom_install_fw(xfile):
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Snom", "firmware")
    fw_dst_path = os.path.join(fw_dst_dir, xfile.filename)
    
    fetchfw.makedirs(fw_dst_dir)
    shutil.copy2(xfile.path, fw_dst_path)
    
def snom_install(firmware):
    snom_install_fw(firmware.remote_files[0])
    if len(firmware.remote_files) > 1:
        snom3xx_install_lang(firmware.remote_files[1])


def snom_m3_install(firmware):
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Snom", "firmware")
    
    fetchfw.makedirs(fw_dst_dir)
    
    for xfile in firmware.remote_files:
        fw_dst_path = os.path.join(fw_dst_dir, xfile.filename)
        shutil.copy2(xfile.path, fw_dst_path)


fetchfw.register_install_fn("Snom", None, snom_install)
fetchfw.register_install_fn("Snom", "m3", snom_m3_install)
