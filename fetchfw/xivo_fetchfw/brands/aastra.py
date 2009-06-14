__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2009  Proformatique <technique@proformatique.com>

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

AASTRA_MODEL_STANDARDIZE = {'6751i':    '51i',
                            '6753i':    '53i',
                            '6755i':    '55i',
                            '6757i':    '57i'}

def aastra_install_langs(firmware, xfile):
    label = "%s_%s" % ("aastra", "langs")
    zip_path = fetchfw.zip_extract_all(label, xfile.path)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Aastra")
    
    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate
    
    for fw_file in os.listdir(zip_path):
        fw_src_path = os.path.join(zip_path, fw_file)
        fw_dst_path = os.path.join(fw_dst_dir, fw_file)
        shutil.copy2(fw_src_path, fw_dst_path)


def aastra_install_fw(firmware, xfile):
    zip_path = fetchfw.zip_extract_all(firmware.name, xfile.path)
    fw_src_path = os.path.join(zip_path, "%s.st" % firmware.model)
    fw_dst_dir = os.path.join(fetchfw.TFTP_PATH, "Aastra")

    if not (os.access(fw_src_path, os.R_OK) and os.path.isfile(fw_src_path)) \
       and AASTRA_MODEL_STANDARDIZE.has_key(firmware.model):
        fw_src_path = os.path.join(zip_path, "%s.st" % AASTRA_MODEL_STANDARDIZE.get(firmware.model))

    try:
        os.makedirs(fw_dst_dir)
    except OSError:
        pass # XXX: catching every OSError is not appropriate
    
    shutil.copy2(fw_src_path, fw_dst_dir)


def aastra_install(firmware):
    for xfile in firmware.remote_files:
        if xfile.filename.startswith('Lang'):
            aastra_install_langs(firmware, xfile)
        else:
            aastra_install_fw(firmware, xfile)


fetchfw.register_install_fn("Aastra", None, aastra_install)
