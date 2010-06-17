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
import distutils.dir_util
from xivo_fetchfw import fetchfw

# Destination path for Cisco SMB firmwares.
smb_fw_dst_path = os.path.join(fetchfw.TFTP_PATH, 'CiscoSMB', 'firmware')
smb_dict_dst_path = os.path.join(fetchfw.TFTP_PATH, 'CiscoSMB', 'language')

def ciscosmb_install(firmware, fw_name):
    zipfile_path = firmware.remote_files[0].path
    fetchfw.makedirs(smb_fw_dst_path)
    fetchfw.zip_extract_files(zipfile_path, (fw_name,), smb_fw_dst_path)
    
    try:
        dict_zipfile_path = firmware.remote_files[1].path
        unzip_dir = fetchfw.zip_extract_all('ciscosmb_langs', dict_zipfile_path)
        fetchfw.makedirs(smb_dict_dst_path)
        distutils.dir_util.copy_tree(unzip_dir, smb_dict_dst_path)
    except IndexError:
        # No dictionary file attached to the firmware
        pass

    
def cisco7900_install(firmware):
    zipfile_path = firmware.remote_files[0]
    unzip_dir = fetchfw.zip_extract_all(firmware.name, zipfile_path.path)
    distutils.dir_util.copy_tree(unzip_dir, fetchfw.TFTP_PATH)


def cisco_install(firmware):
    if firmware.model == 'SPA525' and firmware.version == '7.4.4':
        ciscosmb_install(firmware, 'spa525g-7-4-4.bin')
    elif firmware.model in ('SPA509', 'SPA508', 'SPA504', 'SPA502', 'SPA501') \
         and firmware.version == '7.4.4':
        ciscosmb_install(firmware, 'spa5x5-7-4-4.bin')
    elif firmware.model in ('7975', '7971', '7970', '7965', '7962', '7961',
                            '7960', '7945', '7942', '7941', '7940', '7931',
                            '7916', '7915', '7914', '7912', '7911', '7910',
                            '7906', '7905', '7902'):
        cisco7900_install(firmware)
    else:
        raise fetchfw.FirmwareInstallationError()


fetchfw.register_install_fn('Cisco', None, cisco_install)
fetchfw.register_install_fn('CiscoSMB', None, cisco_install)