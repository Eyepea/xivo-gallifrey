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
import glob
import os.path
import shutil
import tempfile
from xivo_fetchfw import fetchfw

# Destination path for Cisco SMB firmwares.
smb_fw_dst_path = os.path.join(fetchfw.TFTP_PATH, 'CiscoSMB', 'firmware')
smb_dict_dst_path = os.path.join(fetchfw.TFTP_PATH, 'CiscoSMB', 'i18n')

GZIP_MAGIC_NUMBER_ = '\x1f\x8b'  # see http://www.gzip.org/zlib/rfc-gzip.html#file-format


class InvalidFileError(Exception):
    pass


def unsign_from_fileobj(f_in, f_out):
    """Unsign the content of file-object f_in and write the extracted gzip file
       to file-object f_out.
      
       Note that f_in and f_out should be open in binary mode. This function does
       not call close on the file-object. Also, if you pass it garbage input, you
       might as well receive garbage output.
    """
    bytes_in = f_in.read(4096)
    index = bytes_in.find(GZIP_MAGIC_NUMBER_)
    if index == -1:
        raise InvalidFileError(u'This .sgn file doesn\'t hold a gzip file')
    bytes_in = bytes_in[index:]
    while bytes_in:
        f_out.write(bytes_in)
        bytes_in = f_in.read(4096)


def ciscospa5xx_install_fw(firmware, fw_name):
    zipfile_path = firmware.remote_files[0].path
    fetchfw.makedirs(smb_fw_dst_path)
    fetchfw.zip_extract_files(zipfile_path, (fw_name,), smb_fw_dst_path)


def ciscospa5xx_metainstall_fw(fw_name):
    def aux(firmware):
        ciscospa5xx_install_fw(firmware, fw_name)
    return aux


def ciscospa5xx_install_locale(xfile, locale_src_file, locale_dst_file):
    zipfile_path = xfile.path
    unzip_dir = fetchfw.zip_extract_all('spa5xx_lang', zipfile_path)
    fetchfw.makedirs(smb_dict_dst_path)
    shutil.copy(os.path.join(unzip_dir, locale_src_file),
                os.path.join(smb_dict_dst_path, locale_dst_file))


def ciscospa5xx_metainstall_locale(src_525, dst_525, src_50x, dst_50x):
    def aux(firmware):
        ciscospa5xx_install_locale(firmware.remote_files[0], src_525, dst_525)
        ciscospa5xx_install_locale(firmware.remote_files[1], src_50x, dst_50x)
    return aux


def cisco79xx_install_fw(firmware):
    zipfile_path = firmware.remote_files[0]
    unzip_dir = fetchfw.zip_extract_all(firmware.name, zipfile_path.path)
    distutils.dir_util.copy_tree(unzip_dir, fetchfw.TFTP_PATH)


def cisco79xx_install_locale(xfile, user_locale, create_7905font_file):
    signed_path = xfile.path
    
    # 1. Unsign
    signed_f = open(signed_path, 'rb')
    (unsigned_fd, unsigned_path) = tempfile.mkstemp()
    unsigned_f = os.fdopen(unsigned_fd, 'wb')
    try:
        unsign_from_fileobj(signed_f, unsigned_f)
    finally:
        signed_f.close()
        unsigned_f.close()
    # 2. Extract the first tar
    untar_dir1 = fetchfw.tgz_extract_all('79xx_lang1', unsigned_path)
    # 3. Find the second tar and extract it
    tar_path2 = glob.glob(os.path.join(untar_dir1, '*.tar'))[0]
    untar_dir2 = fetchfw.tar_extract_all('79xx_lang2', tar_path2)
    # 4. Copy the file into tftpboot
    src_base_dir = os.path.join(untar_dir2, 'usr', 'local', 'cm', 'tftp')
    src_dir = user_locale
    dest_dir = user_locale
    locale_path = os.path.join(fetchfw.TFTP_PATH, 'Cisco/i18n', dest_dir)
    distutils.dir_util.copy_tree(os.path.join(src_base_dir, src_dir),
                                 locale_path)
    # 5. Create an empty 7905-font.xml
    if create_7905font_file:
        path_7905font = os.path.join(locale_path, '7905-font.xml')
        if not os.path.isfile(path_7905font):
            f = open(path_7905font, 'w')
            try:
                f.write('<Glyphs></Glyphs>\n')
            finally:
                f.close()


def cisco79xx_metainstall_locale(user_locale, network_locale, create_7905font_file=False):
    def aux(firmware):
        cisco79xx_install_locale(firmware.remote_files[0], user_locale, create_7905font_file)
        cisco79xx_install_locale(firmware.remote_files[1], network_locale, False)
    return aux


cisco_install_map = {
    'cisco7975_sccp_903':
        cisco79xx_install_fw,
    'cisco7971_sccp_903':
        cisco79xx_install_fw,
    'cisco7970_sccp_903':
        cisco79xx_install_fw,
    'cisco7965_sccp_903':
        cisco79xx_install_fw,
    'cisco7962_sccp_903':
        cisco79xx_install_fw,
    'cisco7961_sccp_903':
        cisco79xx_install_fw,
    'cisco7960_sccp_812':
        cisco79xx_install_fw,
    'cisco7945_sccp_903':
        cisco79xx_install_fw,
    'cisco7942_sccp_903':
        cisco79xx_install_fw,
    'cisco7941_sccp_903':
        cisco79xx_install_fw,
    'cisco7940_sccp_812':
        cisco79xx_install_fw,
    'cisco7931_sccp_903':
        cisco79xx_install_fw,
    'cisco7912_sccp_804':
        cisco79xx_install_fw,
    'cisco7911_sccp_903':
        cisco79xx_install_fw,
    'cisco7910_sccp_507':
        cisco79xx_install_fw,
    'cisco7906_sccp_903':
        cisco79xx_install_fw,
    'cisco7905_sccp_803':
        cisco79xx_install_fw,
    'cisco7902_sccp_802':
        cisco79xx_install_fw,
    'cisco7916_sccp_104':
        cisco79xx_install_fw,
    'cisco7915_sccp_104':
        cisco79xx_install_fw,
    'cisco7914_sccp_504':
        cisco79xx_install_fw,
    'cisco79xx_locale_de_DE':
        cisco79xx_metainstall_locale('german_germany', 'germany', True),
    'cisco79xx_locale_es_ES':
        cisco79xx_metainstall_locale('spanish_spain', 'spain', True),
    'cisco79xx_locale_fr_CA':
        cisco79xx_metainstall_locale('french_france', 'canada', True),
    'cisco79xx_locale_fr_FR':
        cisco79xx_metainstall_locale('french_france', 'france', True),
    'ciscospa525_748':
        ciscospa5xx_metainstall_fw('spa525g-7-4-8.bin'),
    'ciscospa509_748':
        ciscospa5xx_metainstall_fw('spa50x-30x-7-4-8a.bin'),
    'ciscospa508_748':
        ciscospa5xx_metainstall_fw('spa50x-30x-7-4-8a.bin'),
    'ciscospa504_748':
        ciscospa5xx_metainstall_fw('spa50x-30x-7-4-8a.bin'),
    'ciscospa502_748':
        ciscospa5xx_metainstall_fw('spa50x-30x-7-4-8a.bin'),
    'ciscospa501_748':
        ciscospa5xx_metainstall_fw('spa50x-30x-7-4-8a.bin'),
    'ciscospa5xx_locale_de':
        ciscospa5xx_metainstall_locale('spa525_v746/spa525_de_v746.xml',
                                       'spa525_de.xml',
                                       'spa50x_30x_v746/spa50x_30x_de_v746.xml',
                                       'spa50x_30x_de.xml'),
    'ciscospa5xx_locale_en':
        ciscospa5xx_metainstall_locale('spa525_v746/spa525_en_v746.xml',
                                       'spa525_en.xml',
                                       'spa50x_30x_v746/spa50x_30x_en_v746.xml',
                                       'spa50x_30x_en.xml'),
    'ciscospa5xx_locale_es':
        ciscospa5xx_metainstall_locale('spa525_v746/spa525_es_v746.xml',
                                       'spa525_es.xml',
                                       'spa50x_30x_v746/spa50x_30x_es_v746.xml',
                                       'spa50x_30x_es.xml'),
    'ciscospa5xx_locale_fr':
        ciscospa5xx_metainstall_locale('spa525_v746/spa525_fr_v746.xml',
                                       'spa525_fr.xml',
                                       'spa50x_30x_v746/spa50x_30x_fr_v746.xml',
                                       'spa50x_30x_fr.xml'),
}


def cisco_install_entry_point(firmware):
    if firmware.name in cisco_install_map:
        cisco_install_map[firmware.name](firmware)
    else:
        raise fetchfw.FirmwareInstallationError()


fetchfw.register_install_fn('Cisco', None, cisco_install_entry_point)
fetchfw.register_install_fn('CiscoSMB', None, cisco_install_entry_point)