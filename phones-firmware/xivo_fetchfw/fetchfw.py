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
import md5
import sha
import sys
import shutil
import urllib2
import subprocess
import ConfigParser
from stat import S_IRWXU, S_IRUSR, S_IWUSR, S_IRGRP, S_IXGRP, S_IROTH, S_IXOTH

CONFIG_FILE = "/etc/pf-xivo/fetchfw.conf"
CONFIG_SECTION_GENERAL = "general"

# Installation functions by brand and optionnally model. install_fns is
# a dictionnary of dictionnaries. The first level keys are brands and the
# second level keys are models. The key "__default__", if it exists,
# references the common brand installation function.
install_fns = {}

# Dictionnary used to browse models per brand, and versions per model.
# XXX Could be merged with install_fns.
brands = {}

# (name -> object) dictionnary.
firmwares = {}

class RemoteFile:
    """This class is used to gather some properties about a remote file
    accessible through a protocol supported by the urllib2 module and
    download it. The properties are given when a RemoteFile object is
    instantiated. The user can then call the fetch() method to actually
    download the remote file into the tftp_path directory. The file can
    be cached, in which case there is of course no download. The
    fetch() method always performs size/hash checking, even if the file
    was cached.
    """

    BUFFER_SIZE = 8192

    def __init__(self, filename, url, size, md5sum, sha1sum):
        global tmp_path

        self.filename = filename
        self.url = url
        self.size = size
        self.md5sum = md5sum
        self.sha1sum = sha1sum
        self.path = os.path.join(tmp_path, filename)

    def fetch(self):
        md_md5 = md5.new()
        md_sha = sha.new()
        size = 0

        try:
            src = open(self.path)
            dst = None
        except IOError:
            src = urllib2.urlopen(self.url)
            dst = open(self.path, "w")

        while True:
            buf = src.read(RemoteFile.BUFFER_SIZE)

            if not buf:
                break

            md_md5.update(buf)
            md_sha.update(buf)
            size += len(buf)

            if dst:
                dst.write(buf)

        src.close()

        if dst:
            dst.close()

        def die_dont_match(what, filename):
            print "error: %s doesn't match for file %s" % (what, filename)
            sys.exit()

        if md_md5.hexdigest() != self.md5sum:
            die_dont_match("MD5 sum", self.filename)

        if md_sha.hexdigest() != self.sha1sum:
            die_dont_match("SHA-1 sum", self.filename)

        if size != self.size:
            die_dont_match("size", self.filename)

class firmware:
    """This class is used to store firmware properties and automatically
    select the appropriate installation function. Properties are given
    when a firmware object is created, and the user can later call the
    install() method to get the required files and install the firmware.
    """
    def __init__(self, name, brand, model, version, remote_files, description = None):
        global install_fns

        self.name = name
        self.brand = brand
        self.model = model
        self.version = version

        if description == None:
            self.description = "%s %s %s firmware" % (brand, model, version)
        else:
            self.description = description

        if model in install_fns[brand]:
            self.install_fn = install_fns[brand][model]
        else:
            self.install_fn = install_fns[brand]['__default__']

        self.remote_files = remote_files

    def install(self):
        for remote_file in self.remote_files:
            remote_file.fetch()

        return self.install_fn(self)

def warn(message):
    sys.stderr.write("warning: %s.\n" % message)

def die(message):
    sys.stderr.write("error: %s.\n" % message)
    sys.exit(1)

def register_install_fn(brand, model, install_fn):
    """Register install_fn for the given brand/model pair. If model is
    None, install_fn applies to all models of the given brand.
    """

    global install_fns

    if not model:
        model = "__default__"

    if brand not in install_fns:
        install_fns[brand] = {}

    if model in install_fns[brand]:
        warn("multiple registrations for %s %s" % (brand, model))

    install_fns[brand][model] = install_fn

def fix_permissions(path):
    """Set 755/644 permisions recursively on the given path to ensure
    extracted files are usable.
    """
    dir_mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
    file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
    os.chmod(path, dir_mode)

    for root, dirs, files, in os.walk(path):
        for name in dirs:
            os.chmod(os.path.join(root, name), dir_mode)

        for name in files:
            os.chmod(os.path.join(root, name), file_mode)

def zip_extract_all(label, zipfile_path):
    """Extract the content of zipfile_path into tmp_path/label and
    return this path. This function completely removes the destination
    directory before extracting files. label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """

    zip_path = os.path.join(tmp_path, label)
    shutil.rmtree(zip_path, True)
    os.mkdir(zip_path)

    try:
        result = subprocess.call(['unzip', '-q', zipfile_path], cwd = zip_path, close_fds = True)
        fix_permissions(zip_path)

        if result:
            die("zip extraction failed")
    except OSError, e:
        die("zip extraction failed: %s" % e)

    return zip_path

def tar_extract_all(label, tarfile_path):
    """Extract the content of tarfile_path into tmp_path/label and
    return this path. This function completely removes the destination
    directory before extracting files. label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """

    tar_path = os.path.join(tmp_path, label)
    shutil.rmtree(tar_path, True)
    os.mkdir(tar_path)

    try:
        result = subprocess.call(['tar', 'xf', tarfile_path], cwd = tar_path, close_fds = True)
        fix_permissions(tar_path)

        if result:
            die("tar extraction failed")
    except OSError, e:
        die("tar extraction failed: %s" % e)

    return tar_path

def tgz_extract_all(label, tgzfile_path):
    """Extract the content of tgzfile_path into tmp_path/label and
    return this path. This function completely removes the destination
    directory before extracting files. label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """

    tgz_path = os.path.join(tmp_path, label)
    shutil.rmtree(tgz_path, True)
    os.mkdir(tgz_path)

    try:
        result = subprocess.call(['tar', 'xzf', tgzfile_path], cwd = tgz_path, close_fds = True)
        fix_permissions(tgz_path)

        if result:
            die("tgz extraction failed")
    except OSError, e:
        die("tgz extraction failed: %s" % e)

    return tgz_path

def tbz2_extract_all(label, tbz2file_path):
    """Extract the content of tbz2file_path into tmp_path/label and
    return this path. This function completely removes the destination
    directory before extracting files. label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """

    tbz2_path = os.path.join(tmp_path, label)
    shutil.rmtree(tbz2_path, True)
    os.mkdir(tbz2_path)

    try:
        result = subprocess.call(['tar', 'xjf', tbz2file_path], cwd = tbz2_path, close_fds = True)
        fix_permissions(tbz2_path)

        if result:
            die("tbz2 extraction failed")
    except OSError, e:
        die("tbz2 extraction failed: %s" % e)

    return tbz2_path

config = ConfigParser.RawConfigParser()
fp = open(CONFIG_FILE)
config.readfp(fp)
tftp_path = config.get(CONFIG_SECTION_GENERAL, "tftp_path")
kfw_path = config.get(CONFIG_SECTION_GENERAL, "kfw_path")
tmp_path = config.get(CONFIG_SECTION_GENERAL, "tmp_path")
firmwares_db_path = config.get(CONFIG_SECTION_GENERAL, "firmwares_db_path")
fp.close()

fp = open(firmwares_db_path)
config.readfp(fp)

try:
    os.makedirs(tmp_path)
except OSError:
    pass # XXX: catching every OSError is not appropriate

import aastra         # pylint: disable-msg=W0611
import linksys        # pylint: disable-msg=W0611
import polycom        # pylint: disable-msg=W0611
import snom           # pylint: disable-msg=W0611
import thomson        # pylint: disable-msg=W0611
import digium         # pylint: disable-msg=W0611

fw_names = config.sections()

for fw_name in fw_names:
    if not config.has_option(fw_name, "brand"):
        continue

    brand = config.get(fw_name, "brand")
    model = config.get(fw_name, "model")
    version = config.get(fw_name, "version")
    files = config.get(fw_name, "files").split()
    remote_files = []

    for xfile in files:
        url = config.get(xfile, "url")
        size = config.getint(xfile, "size")
        md5sum = config.get(xfile, "md5sum")
        sha1sum = config.get(xfile, "sha1sum")
        remote_files.append(RemoteFile(xfile, url, size, md5sum, sha1sum))

    if config.has_option(fw_name, "description"):
        description = config.get(fw_name, "description")
    else:
        description = None

    fw = firmware(fw_name, brand, model, version, remote_files, description)

    if brand not in brands:
        brands[brand] = {}

    if model not in brands[brand]:
        brands[brand][model] = {}

    if version in brands[brand][model]:
        warn("multiple registrations for %s %s %s" % (brand, model, version))

    brands[brand][model][version] = fw

    if fw_name in firmwares:
        warn("multiple registrations for firmware %s" % fw_name)

    firmwares[fw_name] = fw
