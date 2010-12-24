#!/usr/bin/env python
# -*- coding: UTF-8 -*-

"""xivo_bnfos is a tool for managing beronet bero*fos devices. It tries to be
   easy to use for simple setup yet flexible enough for exotic one.

   The (patched) bnfos tool must be present on the system for this tool to
   work. Although it's not strictly required, you also need the res_bnfos
   asterisk module installed if you want it to sent heartbeat to the
   devices.

   More info about the berofos and the bntools can be found here:
   - http://www.beronet.com/content/view/157/116/lang,en/
"""

from __future__ import with_statement

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2010  Proformatique <technique@proformatique.com>

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

import datetime
import itertools
import re
import subprocess



class InvalidDeviceNameError(Exception):
    pass


class NoDeviceSelectedError(Exception):
    pass


class DeviceStorageError(Exception):
    pass


class ConfigFileSyntaxError(DeviceStorageError):
    pass


class ConfigFileSemanticError(DeviceStorageError):
    pass


class OverlayDict(object):
    """Represent a dictionary which is made from an 'under' and 'over'
       dictionary. The 'under' dictionary is never modified by instances of
       this class and is only looked up when the key is not found in the over
       dictionary.

       Not every method of dict has been implemented since it wasn't useful
       for our needs.
    """
    def __init__(self, under, over={}):
        self.under = under
        self.over = over

    def __len__(self):
        s = set(key for key in itertools.chain(self.under, self.over))
        return len(s)

    def __getitem__(self, key):
        if key in self.over:
            return self.over[key]
        else:
            return self.under[key]

    def __setitem__(self, key, value):
        self.over[key] = value

    def __delitem__(self, key):
        del self.over[key]

    def __contains__(self, obj):
        return obj in self.over or obj in self.under

    def __bool__(self):
        return bool(self.over) or bool(self.under)

    def __iter__(self):
        return self.iterkeys()

    def __str__(self):
        res = dict(self.under)
        res.update(self.over)
        return str(res)

    def clear(self):
        self.over.clear()

    def get(self, key, default=None):
        if key in self:
            return self[key]
        else:
            return default

    def items(self):
        return list(self.iteritems())

    def iteritems(self):
        keys = set(key for key in itertools.chain(self.under, self.over))
        for key in keys:
            yield (key, self[key])

    def iterkeys(self):
        return itertools.imap(lambda (k, v): k, self.iteritems())

    def itervalues(self):
        return itertools.imap(lambda (k, v): v, self.iteritems())

    def keys(self):
        return list(self.iterkeys())

    def values(self):
        return list(self.itervalues())


class AbstractDevice(object):
    """Represent an 'abstract' bero*fos device. It does not represent a real
       device, but it can be used as a configuration template for others
       devices, be it abstract or not.

       Each instance has the following attributes:
       - name
       - template: the abstract device on which this device is based
       - enabled: should we configure this device ?
       - password: the password of this device
       - device_conf: the configuration parameters of this device
       - module_conf: the configuration parameters for the res_bnfos module
                      for this device
    """
    def __init__(self, name, enabled=None, password=None, device_conf={},
                 module_conf={}, template=None):
        self.name = name
        self._enabled = enabled
        self._password = password
        if template:
            self.device_conf = OverlayDict(template.device_conf, device_conf)
            self.module_conf = OverlayDict(template.module_conf, module_conf)
        else:
            self.device_conf = device_conf
            self.module_conf = module_conf
        self.template = template

    def _get_enabled(self):
        if self._enabled is not None:
            return self._enabled
        elif self.template is not None:
            return self.template.enabled
        else:
            return None

    def _set_enabled(self, value):
        self._enabled = value

    enabled = property(_get_enabled, _set_enabled)

    def _get_password(self):
        if self._password is not None:
            return self._password
        elif self.template:
            return self.template.password
        else:
            assert self._password is None and not self.template
            return None

    def _set_password(self, value):
        self._password = value

    password = property(_get_password, _set_password)


class Device(AbstractDevice):
    """Represent a bero*fos device.

       Each instances has the following attributes:
       - mac: the MAC address of this device
       - ip: the IP address of this device
       - all the attributes inherited from AbstractDevice
    """
    def __init__(self, name, mac, ip, enabled=None, password=None, device_conf={},
                 module_conf={}, template=None):
        AbstractDevice.__init__(self, name, enabled, password, device_conf,
                                module_conf, template)
        self.mac = mac
        self.ip = ip

    def configure(self):
        """Remotely configure this device.

           Return True if the device has been successfully configured, else False.
        """
        return self.configure_with_params(self.device_conf)

    def configure_with_params(self, params):
        """Remotely configure this device but only with parameters from the
           params dictionary.

           Return True if the device has been successfully configured, else False.
        """
        if not self._ip_is_valid():
            raise ValueError("'%s' is not a valid IP address." % self.ip)
        head = ['bnfos', '--set']
        tail = ['-h', self.ip]
        if self.password is not None:
            tail.extend(['-u', 'admin:%s' % self.password])
        success = True
        for key, value in params.iteritems():
            command = head + ['%s=%s' % (key, value)] + tail
            success = False if subprocess.call(command) else success
        return success

    def _ip_is_valid(self):
        return self.ip is not None

    def _mac_is_valid(self):
        return self.mac is not None

    def flash(self, filename):
        """Remotely flash this device. This suppose that the device is in flash
           mode. If you don't want to brick your device, filename should be a
           path to a valid bero*fos firmware.

           Return True if the device has been flashed successfully, else False.
        """
        if not self._mac_is_valid():
            raise ValueError("'%s' is not a valid MAC address." % self.mac)
        if not self._ip_is_valid():
            raise ValueError("'%s' is not a valid IP address." % self.ip)
        command = ['bnfos', '--flash', filename, '-m', self.mac, '-h', self.ip]
        return not subprocess.call(command)


def configure_module(devices, filename, reload_module=True):
    """Configure the res_bnfos Asterisk module.

       This is a two part process.
       - Firstly, the configuration file for the module is regenerated and the
         result is written to filename.
       - Secondly, the module is reloaded if reload_module is True.

       Return True if the module has been successfully configured, else False.
    """
    with open(filename, 'w') as fobj:
        fobj.write('; This file has been automatically generated by xivo_bnfos on %s\n' %
                   datetime.datetime.now().isoformat())
        for device in devices:
            module_conf = {'dev_mac': device.mac,
                           'dev_ip': device.ip,
                           'disabled': 0 if device.enabled else 1}
            if device.password is not None:
                module_conf['user_pwd'] = 'admin:%s' % device.password
            module_conf.update(device.module_conf)
            fobj.write('[%s]\n' % device.name)
            for key, value in sorted(module_conf.iteritems()):
                fobj.write('%s=%s\n' % (key, value))
            fobj.write('\n')
    if reload_module:
        return not subprocess.call(['asterisk', '-rx', 'bnfos reload'])
    else:
        return True


class Model(object):
    """Represent a model of the device/abstract device in a device storage.
    """
    def __init__(self, devices, abstract_devices):
        self.devices = devices
        self.abstract_devices = abstract_devices

    def is_valid(self):
        """Check if the model is valid.
        """
        # XXX we could do more validity check
        return self._names_are_unique() and self._is_acyclic() 

    def _names_are_unique(self):
        names = set()
        for device in itertools.chain(self.devices, self.abstract_devices):
            if device.name in names:
                return False
            names.add(device.name)
        return True

    def _is_acyclic(self):
        """Check if we have a cycle in the model, i.e. A inherits from B and
           B inherits from A, directly or indirectly.
        """
        visited = set()
        for node in itertools.chain(self.devices, self.abstract_devices):
            if id(node) not in visited:
                currently_visited = set((id(node),))
                while node.template is not None:
                    node = node.template
                    if id(node) in currently_visited:
                        return False
                    currently_visited.add(id(node))
                visited.update(currently_visited)
        return True

    def get_device_by_name(self, name):
        """Note: raise an InvalidDeviceNameError if a device is not found."""
        for device in self.devices:
            if device.name == name:
                return device
        raise InvalidDeviceNameError(name)

    def get_template_by_name(self, name):
        """Note: raise an InvalidDeviceNameError if a device is not found."""
        for device in itertools.chain(self.abstract_devices, self.devices):
            if device.name == name:
                return device
        raise InvalidDeviceNameError(name)

    def get_devices_by_names(self, names):
        """Note: raise an InvalidDeviceNameError if a device is not found."""
        name_dev_map = dict((dev.name, dev) for dev in self.devices)
        result = []
        for name in names:
            try:
                result.append(name_dev_map[name])
            except KeyError:
                raise InvalidDeviceNameError(name)
        return result


class HomemadeSyntax_DeviceStorage(object):
    """Load and store a set of devices from/to a configuration file with an
       homemade syntax. The syntax is described in the example file.
    """
    _DEV_DEF_BEG = re.compile(r"^(abstract\s+)?device\s+'([\w-]+)'(?:\s+inherits\s+'([\w-]+)')?\s*\{$")
    _DEV_DEF_END = re.compile(r"^\}$")
    _PARAM = re.compile(r"^(\w+):(.*)$")
    _DEV_CONF_BEG = re.compile(r"device_conf\s*\{$")
    _DEV_CONF_END = _DEV_DEF_END
    _MOD_CONF_BEG = re.compile(r"module_conf\s*\{$")
    _MOD_CONF_END = _DEV_DEF_END

    def __init__(self, config_filename):
        self._config_file = config_filename

    def load(self):
        # 'ad-hoc' method for parsing. Not really pretty, but since the syntax
        # is relatively simple...
        # XXX we could do more extensive checking/more meaningful error message
        with open(self._config_file) as f:
            devices = {}
            templates = {}
            links = {} # template links
            lines = self._HelperIter(f)
            state = 0
            try:
                while True:
                    m = self._DEV_DEF_BEG.match(lines.next())
                    if not m:
                        self._raise_syntax_error('Invalid opening device definition line', lines)
                    abstract, name, template_name = m.groups()
                    global_conf = {}
                    device_conf = {}
                    module_conf = {}
                    state = 1
                    while True:
                        line = lines.next()
                        if self._DEV_DEF_END.match(line):
                            break

                        m = self._PARAM.match(line)
                        if m:
                            key, value = self._clean_key_value(*m.groups())
                            global_conf[key] = value
                            continue

                        m = self._DEV_CONF_BEG.match(line)
                        if m:
                            while True:
                                line = lines.next()
                                if self._DEV_CONF_END.match(line):
                                    break
                                m = self._PARAM.match(line)
                                if m:
                                    key, value = self._clean_key_value(*m.groups())
                                    device_conf[key] = value
                                else:
                                    self._raise_syntax_error('Invalid line in device_conf', lines)
                            continue

                        m = self._MOD_CONF_BEG.match(line)
                        if m:
                            while True:
                                line = lines.next()
                                if self._MOD_CONF_END.match(line):
                                    break
                                m = self._PARAM.match(line)
                                if m:
                                    key, value = self._clean_key_value(*m.groups())
                                    module_conf[key] = value
                                else:
                                    self._raise_syntax_error('Invalid line in module_conf', lines)
                            continue

                        self._raise_syntax_error("Invalid line in device definition '%s'" % name, lines)

                    # We have successfully parsed a device definition
                    state = 0
                    if name in devices or name in templates:
                        raise ConfigFileSemanticError("Two devices have the same name '%s'" % name)
                    if template_name:
                        links[name] = template_name
                    if abstract:
                        if not set(global_conf).issubset(('enabled', 'password')):
                            raise ConfigFileSemanticError("The abstract device definition '%s' has invalid global parameters" % name)
                        enabled = global_conf.get('enabled')
                        if enabled is not None:
                            enabled = int(enabled)
                        password = global_conf.get('password')
                        if template_name:
                            device_conf = OverlayDict(None, device_conf)
                            module_conf = OverlayDict(None, module_conf)
                        templates[name] = AbstractDevice(name, enabled, password, device_conf, module_conf)
                    else:
                        if not set(global_conf).issubset(('enabled', 'password', 'mac', 'ip')):
                            raise ConfigFileSyntaxError("The abstract device definition '%s' has invalid global parameters" % name)
                        enabled = global_conf.get('enabled')
                        if enabled is not None:
                            enabled = int(enabled)
                        password = global_conf.get('password')
                        if 'mac' not in global_conf:
                            raise ConfigFileSyntaxError("Device '%s' is missing the 'mac' parameter." % name)
                        if 'ip' not in global_conf:
                            raise ConfigFileSyntaxError("Device '%s' is missing the 'ip' parameter." % name)
                        if template_name:
                            device_conf = OverlayDict(None, device_conf)
                            module_conf = OverlayDict(None, module_conf)
                        devices[name] = Device(name, global_conf['mac'].lower(), global_conf['ip'], enabled, password, device_conf, module_conf)
            except StopIteration:
                if state:
                    raise DeviceStorageError('Unexpected end of file.')
            # We now create the links between the devices
            all_devices = {}
            all_devices.update(devices)
            all_devices.update(templates)
            for device_name, template_name in links.iteritems():
                if template_name not in all_devices:
                    raise ConfigFileSemanticError("Device '%s' inherits from unknown device '%s'" % (device_name, template_name))
                all_devices[device_name].template = all_devices[template_name]
                all_devices[device_name].device_conf.under = all_devices[template_name].device_conf
                all_devices[device_name].module_conf.under = all_devices[template_name].module_conf
            model = Model(devices.values(), templates.values())
            if not model.is_valid():
                raise ConfigFileSemanticError("Semantic validity problem in configuration file (do you have any loops?)")
            for device in devices.itervalues():
                if device.enabled is None:
                    raise ConfigFileSemanticError("Device '%s' is missing the 'enabled' parameter." % device.name)
            return model

    def _raise_syntax_error(self, msg, helper_iter):
        raise ConfigFileSyntaxError('%s:%s %s' % (self._config_file, helper_iter.count, msg))

    @staticmethod
    def _clean_key_value(key, value):
        return key.rstrip(), value.lstrip()

    class _HelperIter(object):
        def __init__(self, iterable):
            self.iter = iter(iterable)
            self.count = 0
            self.last_orig = None
            self.last_stripped = None

        def __iter__(self):
            return self

        def next(self):
            count = 0
            while True:
                last_orig = self.iter.next()
                last_stripped = self._strip_line(last_orig)
                count += 1
                if last_stripped:
                    self.last_orig = last_orig
                    self.last_stripped = last_stripped
                    self.count += count
                    return last_stripped

        @staticmethod
        def _strip_line(line):
            """Return line stripped from comments and starting and trailing whitespace."""
            idx_comment = line.find('#')
            if idx_comment != -1:
                line = line[:idx_comment]
            return line.strip()

    def save(self, model):
        with open(self._config_file, 'w') as f:
            f.write('# This file has been regenerated by xivo_bnfos on %s\n' %
                    datetime.datetime.now().isoformat())
            f.write('# See xivo_bnfos_example.conf for details about the syntax.\n')
            def cmp_by_dev_name(lhs, rhs):
                return cmp(lhs.name, rhs.name)
            for template in sorted(model.abstract_devices, cmp=cmp_by_dev_name):
                self._write_device_def(f, True, template)
                self._writeparam(f, 'enabled', 1 if template._enabled else 0, 1)
                self._writeparam(f, 'password', template._password, 1)
                self._writedict(f, 'device_conf', template.device_conf, 1)
                self._writedict(f, 'module_conf', template.module_conf, 1)
                f.write('}\n\n')
            for device in sorted(model.devices, cmp=cmp_by_dev_name):
                self._write_device_def(f, False, device)
                self._writeparam(f, 'enabled', 1 if device._enabled else 0, 1)
                self._writeparam(f, 'password', device._password, 1)
                self._writeparam(f, 'mac', device.mac, 1)
                self._writeparam(f, 'ip', device.ip, 1)
                self._writedict(f, 'device_conf', device.device_conf, 1)
                self._writedict(f, 'module_conf', device.module_conf, 1)
                f.write('}\n\n')

    @staticmethod
    def _writeline(f, content, indent=0):
        f.write('   ' * indent)
        f.write(content)
        f.write('\n')

    @classmethod
    def _write_device_def(cls, f, is_abstract, device, indent=0):
        content = "abstract " if is_abstract else ""
        content += "device '%s' " % device.name
        if device.template:
            content += "inherits '%s' {" % device.template.name
        else:
            content += "{"
        cls._writeline(f, content, indent)

    @classmethod
    def _writeparam(cls, f, name, value, indent=0):
        if value is not None:
            cls._writeline(f, "%s: %s" % (name, value), indent)

    @classmethod
    def _writedict(cls, f, dict_name, dict, indent=0):
        dict = dict.over if hasattr(dict, 'over') else dict
        if dict:
            cls._writeline(f, '%s {' % dict_name, indent)
            for name in sorted(dict.iterkeys()):
                cls._writeparam(f, name, dict[name], indent + 1)
            cls._writeline(f, '}', indent)


# All the CLI-related stuff is defined below here.
# We might split this into separate module one day...
import getpass
import hashlib
import optparse
import os.path
import StringIO
import sys
import tempfile
import urllib2

GLOBAL_OPTIONS = [
    optparse.make_option('-c', '--conf', action='store', dest='conffile',
                         default='/etc/pf-xivo/xivo_bnfos.conf', metavar='FILE',
                         help="load/save configuration from/to FILE"),
    ]
COMMON_OPTIONS = {
    'modfile':
        optparse.make_option('-m', '--mod', action='store', dest='modfile',
                             default='/etc/asterisk/bnfos.conf', metavar='FILE',
                             help="save res_bnfos module configuration to FILE"),
    'template':
        optparse.make_option('-t', '--template', action='store', dest='template',
                             default='default', metavar='TPL',
                             help="make devices inherit from template TPL"),
    'no_reload':
        optparse.make_option('--no-reload', action='store_false', dest='reload',
                             default=True,
                             help="do not reload res_bnfos module"),
    'force':
        optparse.make_option('-f', '--force', action='store_true', dest='force',
                             default=False,
                             help="force the command to apply to disabled devices"),
    'dry_run':
        optparse.make_option('--dry-run', action='store_true', dest='dry_run', 
                             default=False,
                             help="don't perform the actual action"),
    }


class Command(object):
    """Abstract base class which represent a CLI subcommand."""
    def __init__(self, name, help, options):
        self.name = name
        self.help = help
        self.options = options

    def print_help(self):
        print self.help
        if self.options:
            print
            print "Valid options:"
            for option in self.options:
                assert option in COMMON_OPTIONS
                option = COMMON_OPTIONS[option]
                print ' ', option, '\t', option.help 
        print
        print "Global options:"
        for option in GLOBAL_OPTIONS:
            print ' ', option, '\t', option.help

    def execute(self, storage, opt, args):
        """Execute this command with options opt and arguments args."""
        try:
            return self._do_execute(storage, opt, args)
        except Exception:
            # TODO better error handling
            print "error: An exception occurred during program execution..."
            raise

    def _do_execute(self, storage, opt, args):
        """This method should be overriden by concrete subclasses."""
        raise NotImplementedError()

    @staticmethod
    def _get_devices_from_args(model, args):
        if not args:
            if model.devices:
                return model.devices
            else:
                raise NoDeviceSelectedError()
        return model.get_devices_by_names(args)


class ConfdevCommand(Command):
    __OPTIONS = ['force']
    __HELP = \
"""confdev: Configure the devices.
usage: confdev [DEVICE...]

  Configure each enabled DEVICEs (default: all) with the configuration
  information in the configuration file.\
"""
    def __init__(self):
        Command.__init__(self, 'confdev', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        devices = self._get_devices_from_args(model, args)
        configured = self._configure_devices(devices, opt.force)
        if not configured:
            print 'No devices have been configured.'
        else:
            print 'These devices have been successfully configured:'
            for dev in sorted(configured):
                print '-  ' + dev
        not_enabled = set(dev.name for dev in devices if not dev.enabled)
        if not_enabled:
            print 'These devices have NOT been configured because they were disabled in the conf. file:'
            for dev in sorted(not_enabled):
                print '-  ' + dev
        configure_error = set(dev.name for dev in devices) - configured - not_enabled
        if configure_error:
            print 'There was an error while configuring these devices:'
            for dev in sorted(configure_error):
                print '-  ' + dev
        return 1 if configure_error else 0

    @staticmethod
    def _configure_devices(devices, force):
        configured = set()
        for device in devices:
            if force or device.enabled:
                if device.configure():
                    configured.add(device.name)
        return configured


class ConfmodCommand(Command):
    __OPTIONS = ['force', 'modfile', 'no_reload']
    __HELP = \
"""confmod: Configure the res_bnfos Asterisk module.
usage: confmod

  Configure the res_bnfos Asterisk module with the configuration information
  in the configuration file.\
"""
    def __init__(self):
        Command.__init__(self, 'confmod', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        return 1 if configure_module(model.devices, opt.modfile, opt.reload) else 0


class DisableCommand(Command):
    __OPTIONS = []
    __HELP = \
"""disable: Disable the devices.
usage: disable [DEVICE...]

  Disable each DEVICEs (default: all).\
"""
    def __init__(self):
        Command.__init__(self, 'disable', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        devices = self._get_devices_from_args(model, args)
        for device in devices:
            if device.enabled:
                device.enabled = 0
                print "Device '%s' is now disabled." % device.name
        storage.save(model)


class EnableCommand(Command):
    __OPTIONS = []
    __HELP = \
"""enable: Enable the devices.
usage: enable [DEVICE...]

  Disable each DEVICEs (default: all).\
"""
    def __init__(self):
        Command.__init__(self, 'enable', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        devices = self._get_devices_from_args(model, args)
        for device in devices:
            if not device.enabled:
                device.enabled = 1
                print "Device '%s' is now enabled." % device.name
        storage.save(model)


class FlashCommand(Command):
    _FIRMWARE = {'url': 'http://www.beronet.com/downloads/berofos/bnfos_v153.bin',
                 'name': 'bnfos_v153.bin',
                 'sha1sum': '2432d603ddc9cfc18251f62625203d6d7c28e608',
                 'version': 'v1.5.3'}
    __OPTIONS = ['dry_run']
    __HELP = \
"""enable: Flash the devices with firmware %(version)s.
usage: flash [DEVICE...]

  Flash each DEVICEs (default: all) with firmware %(version)s. Every devices must be
  in flash mode for the flash to be succesful. The command gives explanation on
  how to put the device into flash mode.\
""" % _FIRMWARE
    def __init__(self):
        Command.__init__(self, 'flash', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        devices = self._get_devices_from_args(model, args)
        print """\
 --- READ ATTENTIVELY ---
This will flash your device with firmware %(version)s. Before proceeding, you
must bring your devices into flash mode. You can do this by pressing the
small black button (on the left of the power button) and keeping it pressed
while powering on the device. The flash mode is signalled by the blinking
LED's of Port D. Do you want to continue ? [y/n]""" % self._FIRMWARE
        proceed = True if raw_input().lower() == 'y' else False
        if not proceed:
            return 0
        print "Downloading the firmware (~20sec)... ",
        sys.stdout.flush()
        f = urllib2.urlopen(self._FIRMWARE['url'])
        fw_content = f.read()
        f.close()
        print "done."
        print "Checking the SHA1 sum... ",
        digest = hashlib.sha1()
        digest.update(fw_content)
        sum = digest.hexdigest()
        if sum != self._FIRMWARE['sha1sum']:
            print " failed!"
            return 1
        print " done."
        fw_dest = os.path.join(tempfile.gettempdir(), self._FIRMWARE['name'])
        with open(fw_dest, 'w') as f:
            f.write(fw_content)
        global_success = True
        for device in devices:
            print "Flashing '%s'... " % device.name
            if opt.dry_run:
                success = True
            else:
                success = device.flash(fw_dest)
            if success:
                print "You can now reboot the device '%s'." % device.name
            else:
                print "failed!"
            global_success = global_success and success 
        return 0 if global_success else 1


class PasswordCommand(Command):
    __OPTIONS = ['modfile', 'no_reload']
    __HELP = \
"""password: Set the password of the devices.
usage: password [DEVICE...]

  Set the password of each DEVICEs (default: all).\
"""
    def __init__(self):
        Command.__init__(self, 'password', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        devices = self._get_devices_from_args(model, args)
        old_pass = getpass.getpass('Enter old password: ')
        new_pass1 = getpass.getpass('Enter new password: ')
        new_pass2 = getpass.getpass('Retype new password: ')
        if new_pass1 != new_pass2:
            print "Passwords do not match"
            return 1
        global_success = 1
        for device in devices:
            orig_pass = device.password
            device.password = old_pass
            if device.configure_with_params({'apwd': new_pass1}):
                device.password = new_pass1
                device.configure_with_params({'pwd': 1})
            else:
                device.password = orig_pass
                global_success = 0
        storage.save(model)
        if not configure_module(model.devices, opt.modfile, opt.reload):
            global_success = 0
        return global_success


class RefreshCommand(Command):
    __OPTIONS = ['template']
    __HELP = \
"""refresh: Find new devices on the network.
usage: refresh

  Find new devices on the network and add them to the configuration file.\
"""
    def __init__(self):
        Command.__init__(self, 'refresh', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        template = model.get_template_by_name(opt.template)
        devices_mac = set(device.mac for device in model.devices)
        new_devices = set()
        output = subprocess.Popen(['bnfos', '--sscan'], stdout=subprocess.PIPE).communicate()[0]
        for line in StringIO.StringIO(output):
            mac, ip = line.split(';')[:2]
            mac = mac.lower()
            if mac not in devices_mac:
                name = 'fos_%s' % ''.join(mac.split(':')[-3:])
                device = Device(name, mac, ip, template=template)
                devices_mac.add(mac)
                new_devices.add(device)
                model.devices.append(device)
        if not new_devices:
            print "No new devices found."
        else:
            storage.save(model)
            for new_device in new_devices:
                print ("- Added device '%s'  (mac: %s, ip: %s, enabled: %s)" %
                       (new_device.name, new_device.mac, new_device.ip, new_device.enabled))
            print "You might want to review the configuration file '%s' before running '%s configure'" % (opt.conffile, sys.argv[0])
        return 0


class SetCommand(Command):
    __OPTIONS = ['force']
    __HELP = \
"""set: Do non-persistent configuration modification to devices.
usage: set param1=value1[,...] [DEVICE...]

  Set each specified parameters to specific value for each DEVICEs (default: all).

  Valid parameters and values are the same that the bnfos tool use. For example,
  if you want to enable the wdog of every device and change the relais mode, you
  would write "set wdog=1,mode=1".\
"""
    def __init__(self):
        Command.__init__(self, 'set', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        if not args:
            self.print_help()
            return 1
        raw_params = args[0]
        if not re.match('^\w+=\w+(?:,\w+=\w+)*$', raw_params):
            print "Invalid params format '%s'" % raw_params
            return 1
        model = storage.load()
        devices = self._get_devices_from_args(model, args[1:])
        params = dict(param.split('=') for param in raw_params.split(','))
        retcode = 0
        for device in devices:
            if opt.force or device.enabled:
                retcode = 1 if not device.configure_with_params(params) else retcode
        return retcode


class ShowCommand(Command):
    __OPTIONS = []
    __HELP = \
"""show: Show information about devices in the storage.
usage: show

  Show information about devices in the storage.\
"""
    def __init__(self):
        Command.__init__(self, 'show', self.__HELP, self.__OPTIONS)

    def _do_execute(self, storage, opt, args):
        model = storage.load()
        if not model.devices:
            print "No devices in configuration file."
        for i, device in enumerate(sorted(model.devices)):
            print "%s." % (i + 1), device.name, \
                  " (mac: %s, ip: %s, enabled: %s, template: %s)" % \
                  (device.mac, device.ip, device.enabled, device.template.name if device.template else None)
        return 0


class TemplateCommand(Command):
    __OPTIONS = []
    __HELP = \
"""template: Change the template that devices inherit.
usage: template TEMPLATE [DEVICE...]

  For each device (default: all), change the template it inherits to TEMPLATE.\
"""
    def __init__(self):
        Command.__init__(self, 'template', self.__HELP, self.__OPTIONS)
    
    def _do_execute(self, storage, opt, args):
        if not args:
            self.print_help()
            return 1
        model = storage.load()
        template_name = args[0]
        template = model.get_template_by_name(template_name)
        devices = self._get_devices_from_args(model, args[1:])
        for device in devices:
            device.template = template
        storage.save(model)
        

if __name__ == '__main__':
    COMMANDS = {}
    for command_class in Command.__subclasses__():
        command = command_class()
        assert command.name not in COMMANDS
        COMMANDS[command.name] = command

    def print_help():
        print \
"""usage: %(name)s <subcommand> [options] [args]
bero*fos management utility.
Type '%(name)s <subcommand>' for help on a specific subcommand.

Available subcommands:""" % {'name': os.path.basename(sys.argv[0])}
        for command_name in sorted(itertools.chain(COMMANDS, ['help'])):
            print '  ' + command_name

    p = optparse.OptionParser(add_help_option=False)
    for option in itertools.chain(GLOBAL_OPTIONS, COMMON_OPTIONS.itervalues()):
        p.add_option(option)
    p.add_option('-h', '--help', action='store_true', dest='help')

    opt, args = p.parse_args()
    if not args or opt.help or (args[0] not in COMMANDS and args[0] != 'help'):
        print_help()
        sys.exit(1)
    if args[0] == 'help':
        if len(args) > 1 and args[1] in COMMANDS:
            COMMANDS[args[1]].print_help()
        else:
            print_help()
        sys.exit(0)
    assert args[0] in COMMANDS

    try:
        storage = HomemadeSyntax_DeviceStorage(opt.conffile)
    except DeviceStorageError, e:
        print "error: DeviceStorageError:", e
    else:
        sys.exit(COMMANDS[args[0]].execute(storage, opt, args[1:]))
