"""Support routines to load content of configuration sections to a dictionary

Copyright (C) 2007-2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2010  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

# pylint: disable-msg=W0611
from ConfigParser import ConfigParser, Error, NoSectionError, NoOptionError, \
                         DuplicateSectionError, InterpolationError, \
                         InterpolationDepthError, InterpolationSyntaxError, \
                         ParsingError, MissingSectionHeaderError

def FillDictFromConfigSection(dicttofill, confpars_obj, section):
    """
    Fills dicttofill with the content of the wanted section of the
    configuration file represented here by an instance of a ConfigParser
    having already parsed the content of the file.

    Names from the configuration file are used as dictionary keys, and
    the value associated with a given name in the config file is assigned
    at the corresponding key position of the dictionary.

    No particular care is taken to avoid overriding existing dictionary
    entries, so the caller can prefill it with default values. Indeed when
    an entry found in the configuration file already exists in the
    dictionary and its pre-existing content is not of type type(None), the
    string extracted from the config file is converted in the type the old
    content of the dictionary has, before assignment.

    Returns the updated dicttofill so you can do - if you want so:
            dico = FillDictFromConfigSection({}, conf, sec)
    """
    for name, val in confpars_obj.items(section):
        if name in dicttofill:
            t = type(dicttofill[name])
            if t is not type(None):
                oldval = val
                try:
                    val = t(val)
                except ValueError:
                    raise ValueError, "Content of \"%s\" in section [%s] of configuration file should be of type \"%s\", but the actual value found there \"%s\" is not" % (name, section, str(t), oldval)
        dicttofill[name] = val
    return dicttofill

def FillDictFromMultipleConfig(dicttofill, confpars_obj, sect_mapping):
    """
    Acts the same as FillDictFromConfigSection(), but instead of filling
    the dictionary with every entries of a given section of the
    configuration file, uses the other dictionary sect_mapping to get the
    list of keys to populate in dicttofill and their corresponding section
    and name in the configuration file.

    Also the wanted type is not deduced from an existing entry in
    dicttofill but is instead explicitely given in a sect_mapping entry.

    The structure of sect_mapping is the following:

            { 'key_for_dicttofill': ('section', 'name', type), ... }

    An entry missing in the configuration file will result in
    the corresponding ConfigParser exception being raised.
    """
    for key, (section, name, t) in sect_mapping.iteritems():
        val = confpars_obj.get(section, name)
        if t is not None and t is not type(None):
            oldval = val
            try:
                val = t(val)
            except ValueError:
                raise ValueError, "Content of \"%s\" in section [%s] of configuration file should be of type \"%s\", but the actual value found there \"%s\" is not" % (name, section, str(t), oldval)
        dicttofill[key] = val
    return dicttofill

def ReadSingleKey(config_file_path, section, option):
    """
    Read a single option from a configuration file.
    Do not use if you need to read multiple options.
    """
    conf_obj = ConfigParser()
    conf_obj.readfp(open(config_file_path))
    return conf_obj.get(section, option)

__all__ = ['NoSectionError', 'NoOptionError',
           'Error', 'DuplicateSectionError',
           'InterpolationError', 'InterpolationDepthError',
           'InterpolationSyntaxError', 'ParsingError',
           'MissingSectionHeaderError',
           'FillDictFromConfigSection',
           'FillDictFromMultipleConfig',
           'ReadSingleKey']
