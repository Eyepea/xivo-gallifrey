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
import os.path
import shutil
from xivo_fetchfw import fetchfw


def is_dict_file(xfile):
    return xfile.filename.startswith('SPA9X2_Dictionaries')


def linksys_install_spa9x2_langs(xfile):
    zip_path = fetchfw.zip_extract_all('linksys_langs', xfile.path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, 'Linksys', 'language', 'spa9X2')

    fetchfw.makedirs(fw_dst_dir)

    for fw_file in os.listdir(zip_path):
        fw_src_path = os.path.join(zip_path, fw_file)
        fw_dst_file = "%s.xml" % fw_file.rsplit('.', 1)[0].rsplit('_', 1)[0]
        fw_dst_path = os.path.join(fw_dst_dir, fw_dst_file)
        shutil.copy2(fw_src_path, fw_dst_path)


def linksys_dblzip_install(firmware, zip_name, fw_name):
    for xfile in firmware.remote_files:
        if is_dict_file(xfile):
            linksys_install_spa9x2_langs(xfile)
        else:
            zipfile1_path = firmware.remote_files[0].path
            unzip_dir = fetchfw.zip_extract_all(firmware.name, zipfile1_path)
            zipfile2_path = os.path.join(unzip_dir, zip_name)
            
            fw_dst_path = os.path.join(fetchfw.TFTP_PATH, 'Linksys', 'firmware')
            fetchfw.makedirs(fw_dst_path)
            fetchfw.zip_extract_files(zipfile2_path, (fw_name,), fw_dst_path)


def linksys_zip_install(firmware, fw_name):
    for xfile in firmware.remote_files:
        if is_dict_file(xfile):
            linksys_install_spa9x2_langs(xfile)
        else:
            zipfile_path = xfile.path
            
            fw_dst_path = os.path.join(fetchfw.TFTP_PATH, 'Linksys', 'firmware')
            fetchfw.makedirs(fw_dst_path)
            fetchfw.zip_extract_files(zipfile_path, (fw_name,), fw_dst_path)


dblzipped_fw = {('SPA962', '6.1.5a'): ('SPA962_6.1.5a.zip', 'spa962-6-1-5a.bin'),
                ('SPA942', '6.1.5a'): ('SPA942_6.1.5a.zip', 'spa942-6-1-5a.bin'),
                ('SPA941', '5.1.8'):  ('spa941-5.1.8.zip', 'spa941-5-1-8.bin'),
                ('SPA922', '6.1.5a'): ('SPA942_6.1.5a.zip', 'spa942-6-1-5a.bin'),
                ('SPA921', '5.1.8'):  ('spa941-5.1.8.zip', 'spa941-5-1-8.bin'),
                ('SPA901', '5.1.5'):  ('spa901--5.1.5.zip', 'spa901-5-1-5.bin')
               }

zipped_fw = {('PAP2T', '5.1.6'): 'pap2t-5-1-6.bin',
             ('SPA8000', '6.1.3'): 'spa8000-6-1-3.bin',
             ('SPA3102', '5.1.10'): 'spa3102-5-1-10-GW.bin',
             ('SPA2102', '5.2.10'): 'spa2102-5-2-10.bin',
             ('SPA400', '01-01-02-02'): 'spa400-01-01-02-02.bin'
            }

def linksys_install(firmware):
    fw_key = (firmware.model, firmware.version)
    if fw_key in zipped_fw:
        linksys_zip_install(firmware, zipped_fw[fw_key])
    elif fw_key in dblzipped_fw:
        zip_name, fw_name = dblzipped_fw[fw_key]
        linksys_dblzip_install(firmware, zip_name, fw_name)
    else:
        raise fetchfw.FirmwareInstallationError()

fetchfw.register_install_fn('Linksys', None, linksys_install)