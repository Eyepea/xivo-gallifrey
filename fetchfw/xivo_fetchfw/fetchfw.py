"""Main fetch firmwares module

Copyright (C) 2008-2010  Proformatique <technique@proformatique.com>

"""

from __future__ import with_statement

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
import hashlib
import sys
import shutil
import urllib
import urllib2
import cookielib
import subprocess
import ConfigParser
from stat import S_IRWXU, S_IRUSR, S_IWUSR, S_IRGRP, S_IXGRP, S_IROTH, S_IXOTH


from xivo_fetchfw import brands as brand_modules

CONFIG_FILE = '/etc/fetchfw.conf'
CONFIG_SECTION_GENERAL = 'general'
CONFIG_SECTION_CISCO = 'cisco'

TFTP_PATH = None            # modified by _init()
KFW_PATH = None             # modified by _init()
TMP_PATH = None             # modified by _init()
FIRMWARES_DB_PATH = None    # modified by _init()
PROXY_URL = None            # modified by _init()
CISCO_USER = None           # modified by _init()
CISCO_PASS = None           # modified by _init()


# Installation functions by brand and optionnally model.  INSTALL_FNS is
# a dictionary of dictionnaries.  The first level keys are brands and the
# second level keys are models.  The key '__default__', if it exists,
# references the common brand installation function.
INSTALL_FNS = {}

# Dictionary used to browse models per brand, and versions per model.
# XXX Could be merged with INSTALL_FNS.
BRANDS = {}

# (name -> object) dictionary.
FIRMWARES = {}


class RemoteFile(object):
    """
    This class is used to gather some properties about a remote file
    accessible through a protocol supported by the urllib2 module and
    download it.  The properties are given when a RemoteFile object is
    instantiated.  The user can then call the fetch() method to actually
    download the remote file into the TFTP_PATH directory.  The file can
    be cached, in which case there is of course no download.  The
    fetch() method always performs size/hash checking, even if the file
    was cached.
    """

    BUFFER_SIZE = 8192

    def __init__(self, filename, url, size, sha1sum):
        self.filename = filename
        self.url = url
        self.size = size
        self.sha1sum = sha1sum
        self.path = os.path.join(TMP_PATH, filename)

    def open_src(self):
        """ Returns a file-like object on the original source. """
        
        if PROXY_URL:
            thisproxy = urllib2.ProxyHandler({"http" : PROXY_URL})
            opener = urllib2.build_opener(urllib2.HTTPDefaultErrorHandler, thisproxy)
            return opener.open(self.url)
        return urllib2.urlopen(self.url)
    
    def fetch(self):
        """
        Download the firmware
        """
        md_sha = hashlib.sha1()
        size = 0

        try:
            src = open(self.path)
            dst = None
        except IOError:
            src = self.open_src()
            dst = open(self.path, "wb")

        while True:
            buf = src.read(self.BUFFER_SIZE)

            if not buf:
                break

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

        if md_sha.hexdigest() != self.sha1sum:
            die_dont_match("SHA-1 sum", self.filename)

        if size != self.size:
            die_dont_match("size", self.filename)

class NoCiscoCredentialsError(Exception):
    pass

class CiscoRemoteFile(RemoteFile):
    """ Encapsulate authenticated file access for downloads on Cisco website.
    
    """
    
    __login_url = 'https://www.cisco.com/authc/forms/CDClogin.fcc?TYPE=33619969&REALMOID=06-59fc1640-c46c-104a-a635-83846dc9304d&GUID=&SMAUTHREASON=0&METHOD=GET&SMAGENTNAME=-SM-zjGKGqr62shoVBG6cNUdYNajdKPzmOFLa%2fZkeebT0%2bNV%2bEcoXFhv%2fvB8k65Cw%2f%2bx&TARGET=-SM-http%3a%2f%2fcisco%2ecom%2fcgi--bin%2flogin'
    __opener = None
    
    def __init__(self, filename, url, size, sha1sum):
        if not CiscoRemoteFile.__has_credentials():
            raise NoCiscoCredentialsError()
        if not CiscoRemoteFile.__is_authenticated():
            CiscoRemoteFile.__authenticate()

        RemoteFile.__init__(self, filename, url, size, sha1sum)
    
    @staticmethod
    def __has_credentials():
        return True if CISCO_USER and CISCO_PASS else False
    
    @staticmethod
    def __is_authenticated():
        return CiscoRemoteFile.__opener is not None
    
    @staticmethod
    def __authenticate():
        handlers = []
        handlers.append(urllib2.HTTPCookieProcessor(cookielib.CookieJar()))
        if PROXY_URL:
            handlers.append(urllib2.ProxyHandler({"http" : PROXY_URL}))
        op = CiscoRemoteFile.__opener = urllib2.build_opener(*handlers)
        
        if __debug__:
            print >> sys.stderr, 'Authenticating on cisco website...'
        
        params = {'USER': CISCO_USER,
                  'PASSWORD': CISCO_PASS,
                  'target': 'http://cisco.com/cgi-bin/login?referer=http://cisco.com/',
                  'smauthreason': 0,
                  'smquerydata': '',
                  'smagentname': 'zjGKGqr62shoVBG6cNUdYNajdKPzmOFLa/ZkeebT0+NV+EcoXFhv/vB8k65Cw/+x',
                  'postpreservationdata': '',
                  'SMENC': 'ISO-8859-1',
                  'SMLOCALE': 'US-EN',
                  'login-button': 'Log In'}
        f = op.open(CiscoRemoteFile.__login_url, urllib.urlencode(params))
        f.close()
        
        if __debug__:
            print >> sys.stderr, 'Authentication done.'
        
    def open_src(self):
        return CiscoRemoteFile.__opener.open(self.url)


class Firmware(object):
    """
    This class is used to store firmware properties and automatically
    select the appropriate installation function.  Properties are given
    when a firmware object is created, and the user can later call the
    install() method to get the required files and install the firmware.
    """
    def __init__(self, name, brand, model, version, remote_files):
        self.name = name
        self.brand = brand
        self.model = model
        self.version = version

        self.description = "%s %s %s firmware" % (brand, model, version)

        if model in INSTALL_FNS[brand]:
            self.install_fn = INSTALL_FNS[brand][model]
        else:
            self.install_fn = INSTALL_FNS[brand]['__default__']

        self.remote_files = remote_files

    def install(self):
        """
        Install the firmware
        """
        for remote_file in self.remote_files:
            remote_file.fetch()

        return self.install_fn(self)


def warn(message):
    """
    XXX replace with logging module
    """
    sys.stderr.write("warning: %s.\n" % message)


def die(message):
    """
    XXX replace with logging module
    """
    sys.stderr.write("error: %s.\n" % message)
    sys.exit(1)


def register_install_fn(brand, model, install_fn):
    """
    Register install_fn for the given brand/model pair.  If model is
    None, install_fn applies to all models of the given brand.
    """
    if not model:
        model = '__default__'
    
    brand_dict = INSTALL_FNS.setdefault(brand, {})
    
    if model in brand_dict:
        warn("multiple registrations for %s %s" % (brand, model))
    
    brand_dict[model] = install_fn


def fix_permissions(path):
    """
    Set 755/644 permisions recursively on the given path to ensure
    extracted files are usable.
    """
    dir_mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
    file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
    os.chmod(path, dir_mode)
    
    for root, dirs, files, in os.walk(path):
        for name in dirs:
            filename = os.path.join(root, name)
            if os.path.exists(filename):
                os.chmod(filename, dir_mode)

        for name in files:
            filename = os.path.join(root, name)
            if os.path.exists(filename):
                os.chmod(filename, file_mode)


def _common_extract_all(label, file_path, start_of_cmd, kind):
    """
    internal
    """
    archive_path = os.path.join(TMP_PATH, label)
    shutil.rmtree(archive_path, True)
    os.mkdir(archive_path)
    
    try:
        result = subprocess.call(start_of_cmd + [file_path], cwd=archive_path, close_fds=True)

        fix_permissions(archive_path)

        if result:
            die(kind + " extraction failed")
    except OSError, e:
        die(kind + " extraction failed: %s" % e)
    
    return archive_path


def zip_extract_all(label, zipfile_path):
    """
    Extract the content of zipfile_path into TMP_PATH/label and
    return this path.  This function completely removes the destination
    directory before extracting files.  label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """
    return _common_extract_all(label, zipfile_path, ['unzip', '-q'], "zip")


def tar_extract_all(label, tarfile_path):
    """
    Extract the content of tarfile_path into TMP_PATH/label and
    return this path.  This function completely removes the destination
    directory before extracting files.  label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """
    return _common_extract_all(label, tarfile_path, ['tar', 'xf'], "tar")


def tgz_extract_all(label, tgzfile_path):
    """
    Extract the content of tgzfile_path into TMP_PATH/label and
    return this path.  This function completely removes the destination
    directory before extracting files.  label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """
    return _common_extract_all(label, tgzfile_path, ['tar', 'xzf'], "tgz")


def tbz2_extract_all(label, tbz2file_path):
    """
    Extract the content of tbz2file_path into TMP_PATH/label and
    return this path.  This function completely removes the destination
    directory before extracting files.  label is used as a unique
    identifier and usually contains the firmware name as a prefix.
    """
    return _common_extract_all(label, tbz2file_path, ['tar', 'xjf'], "tbz2")


def _init():
    """
    Initialize global variables and create the temporary directory
    Called at module loading
    """
    global TFTP_PATH
    global KFW_PATH
    global TMP_PATH
    global FIRMWARES_DB_PATH
    global PROXY_URL
    global CISCO_USER
    global CISCO_PASS
    
    config = ConfigParser.RawConfigParser()
    with open(CONFIG_FILE) as f:
        config.readfp(f)
    
    TFTP_PATH = config.get(CONFIG_SECTION_GENERAL, 'tftp_path')
    KFW_PATH = config.get(CONFIG_SECTION_GENERAL, 'kfw_path')
    TMP_PATH = config.get(CONFIG_SECTION_GENERAL, 'tmp_path')
    FIRMWARES_DB_PATH = config.get(CONFIG_SECTION_GENERAL, 'firmwares_db_path')
    if config.has_option(CONFIG_SECTION_GENERAL, 'proxy_url'):
        PROXY_URL = config.get(CONFIG_SECTION_GENERAL, 'proxy_url')

    if config.has_option(CONFIG_SECTION_CISCO, 'username'):
        CISCO_USER = config.get(CONFIG_SECTION_CISCO, 'username')
        CISCO_PASS = config.get(CONFIG_SECTION_CISCO, 'password')

    try:
        os.makedirs(TMP_PATH)
    except OSError:
        pass # XXX: catching every OSError is not appropriate


def load():
    """
    load all brand modules and do related initializations
    """
    config = ConfigParser.RawConfigParser()
    with open(FIRMWARES_DB_PATH) as f:
        config.readfp(f)
    
    # Load every brand module
    # Each register itself using register_install_fn()
    for brand_name in brand_modules.__all__:
        __import__('xivo_fetchfw.brands.' + brand_name, globals(), {}, [brand_name])
    
    for fw_name in config.sections():
        if not config.has_option(fw_name, 'brand'):
            continue
        
        brand = config.get(fw_name, 'brand')
        model = config.get(fw_name, 'model')
        version = config.get(fw_name, 'version')
        files = config.get(fw_name, 'files').split()
        remote_files = []
        
        if brand == 'Cisco':
            remote_file_class = CiscoRemoteFile
        else:
            remote_file_class = RemoteFile

        for xfile in files:
            url = config.get(xfile, 'url')
            size = config.getint(xfile, 'size')
            sha1sum = config.get(xfile, 'sha1sum')
            remote_files.append(remote_file_class(xfile, url, size, sha1sum))
        
        fw = Firmware(fw_name, brand, model, version, remote_files)
        
        brand_dict = BRANDS.setdefault(brand, {})
        model_dict = brand_dict.setdefault(model, {})
        
        if version in model_dict:
            warn("multiple registrations for %s %s %s" % (brand, model, version))
        
        model_dict[version] = fw
        
        if fw_name in FIRMWARES:
            warn("multiple registrations for firmware %s" % fw_name)
        
        FIRMWARES[fw_name] = fw


_init()
