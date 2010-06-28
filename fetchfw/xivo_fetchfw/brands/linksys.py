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

import os.path
import shutil
from xivo_fetchfw import fetchfw


def linksys_install_locale(xfile, src_file, dst_file):
    unzip_dir = fetchfw.zip_extract_all('linksys_langs', xfile.path)
    lang_dst_path = os.path.join(fetchfw.TFTP_PATH, 'Linksys', 'language', 'spa9X2')
    fetchfw.makedirs(lang_dst_path)
    shutil.copy(os.path.join(unzip_dir, src_file), os.path.join(lang_dst_path, dst_file))


def linksys_metainstall_locale(src_file, dst_file):
    def aux(firmware):
        linksys_install_locale(firmware.remote_files[0], src_file, dst_file)
    return aux


def linksys_dblzip_install_fw(firmware, zip_name, fw_name):
    zipfile1_path = firmware.remote_files[0].path
    unzip_dir = fetchfw.zip_extract_all(firmware.name, zipfile1_path)
    zipfile2_path = os.path.join(unzip_dir, zip_name)
    
    fw_dst_path = os.path.join(fetchfw.TFTP_PATH, 'Linksys', 'firmware')
    fetchfw.makedirs(fw_dst_path)
    fetchfw.zip_extract_files(zipfile2_path, (fw_name,), fw_dst_path)


def linksys_dblzip_metainstall_fw(zip_name, fw_name):
    def aux(firmware):
        linksys_dblzip_install_fw(firmware, zip_name, fw_name)
    return aux


def linksys_zip_install_fw(firmware, fw_name):
    zipfile_path = firmware.remote_files[0].path
    fw_dst_path = os.path.join(fetchfw.TFTP_PATH, 'Linksys', 'firmware')
    fetchfw.makedirs(fw_dst_path)
    fetchfw.zip_extract_files(zipfile_path, (fw_name,), fw_dst_path)


def linksys_zip_metainstall_fw(fw_name):
    def aux(firmware):
        linksys_zip_install_fw(firmware, fw_name)
    return aux
    

linksys_install_map = {
    'lsspa962_615a':
        linksys_dblzip_metainstall_fw('SPA962_6.1.5a.zip', 'spa962-6-1-5a.bin'),
    'lsspa942_615a':
        linksys_dblzip_metainstall_fw('SPA942_6.1.5a.zip', 'spa942-6-1-5a.bin'),
    'lsspa941_518':
        linksys_dblzip_metainstall_fw('spa941-5.1.8.zip', 'spa941-5-1-8.bin'),
    'lsspa922_615a':
        linksys_dblzip_metainstall_fw('SPA942_6.1.5a.zip', 'spa942-6-1-5a.bin'),
    'lsspa921_518':
        linksys_dblzip_metainstall_fw('spa941-5.1.8.zip', 'spa941-5-1-8.bin'),
    'lsspa901_515':
        linksys_dblzip_metainstall_fw('spa901--5.1.5.zip', 'spa901-5-1-5.bin'),
    'lspap2t_516':
        linksys_zip_metainstall_fw('pap2t-5-1-6.bin'),
    'lsspa8000_613':
        linksys_zip_metainstall_fw('spa8000-6-1-3.bin'),
    'lsspa3102_5110':
        linksys_zip_metainstall_fw('spa3102-5-1-10-GW.bin'),
    'lsspa2102_5210':
        linksys_zip_metainstall_fw('spa2102-5-2-10.bin'),
    'lsspa400_01010202':
        linksys_zip_metainstall_fw('spa400-01-01-02-02.bin'),
    'lsspa9xx_locale_fr':
        linksys_metainstall_locale('frS_FR_v615.xml', 'frS_FR.xml'),
    'lsspa9xx_locale_en':
        linksys_metainstall_locale('enS_US_v615.xml', 'enS_US.xml'),
}


def linksys_install_entry_point(firmware):
    if firmware.name in linksys_install_map:
        linksys_install_map[firmware.name](firmware)
    else:
        raise fetchfw.FirmwareInstallationError()


fetchfw.register_install_fn('Linksys', None, linksys_install_entry_point)