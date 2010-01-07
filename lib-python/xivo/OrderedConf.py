"""Ordered Configuration Parser useful when the order of options counts.

Copyright (c) 2001, 2002, 2003, 2004 Python Software Foundation;
Copyright (C) 2007-2010  Proformatique

This module derives from the ConfigParser module distributed in Python 2.4.4

A setup file consists of sections, lead by a "[section]" header,
and followed by "name: value" entries, with continuations and such in
the style of RFC 822.

Unlike the ConfigParser module, this one can't contain format strings refering
to other values.  Also you can't load multiple files, modify the parsed parsed
configuration, or write it back in a config file.  Multiple sections with the
same name are handled differently, and so are multiple options in a given
section.

Keep in mind the following API and behavioral level differences and precisions
when using OrderedRawConf of this module instead of RawConfigParser of the
ConfigParser module, and remember this LEGACY API is provided only for LEGACY
CODE COMPATIBILITY and WONT BE OF ANY USE if you must RETRIEVE OPTIONS in an
ORDERED WAY (in which case you'll have to use the newly introduced methods and
iterators described in the classes PyDoc):

* __init__ takes following parameters:
    (fp=None, filename=None, sect_trans=lambda x:x, opt_trans=lambda x:x)
  They are not compatible at all with the arguments of the RawConfigParser
  __init__.
  See class PyDoc for details about theses parameters.

* defaults() is not available.

* sections() doesn't consider that DEFAULT has a special meaning.  Every
  elements of the returned list are in the output set of 'sect_trans'.  The
  same element can't be seen two times in the returned list.

* add_section(section) is not available.

* has_section(section): the 'sect_trans' function used at instantiation is used
  on the section parameter before any lookup.

* options(section): the 'sect_trans' function used at instantiation is used
  on the section parameter before any lookup.  Every elements of the returned
  list are in the output set of 'opt_trans'.  The same element can't be seen
  multiple times in the returned list.  If the parsed configuration file
  contained multiple sections for which the result of the 'sect_trans' function
  is the same, the first one in the file order is selected.

* has_option(section, option): 'sect_trans' is applied on 'section' and
  'opt_trans' on 'option' before lookup.

* read(filenames) is not available.

* readfp(fp[, filename]) can only be called once and can automatically be
  called by the constructor.

* get(section, option)
* getint(section, option)
* getfloat(section, option)
* getboolean(section, option):
  'sect_trans' is applied on 'section' and 'opt_trans' on 'option' before lookup

* items(section): 'sect_trans' is applied on 'section' before lookup.  Names of
  the returned list of '(name, value)' pairs are in the output set of
  'opt_trans' and the same name can't be seen multiple times.  If the parsed
  configuration file contained multiple sections for which the result of the
  'sect_trans' function is the same, the first one seen in the file order is
  selected.  If the selected section contains multiple option names for which
  the result of 'opt_trans' is the same, only the first one seen in the file
  order is returned.

* set(section, option, value) is not available.

* write(fileobject) is not available.

* remove_option(section, option) is not available.

* remove_section(section) is not available.

* optionxform(option) is not available but you can achieve similar functions
  by setting 'opt_trans' at instantiation time.

For improved SAFETY, be sure to USE some of the FOLLOWING TESTS METHODS in an
appropriate way BEFORE USING the OLD API:

* has_conflicting_section_names, has_conflicting_option_names,
  has_any_conflicting_option_name, is_probably_safe_with_old_api

The following methods are recommended for newly written code that intends to
use OrderedRawConf natively, and should be preferred:

  - Methods on OrderedRawConf instances:
      ordered_sections, has_section, has_conflicting_section_names,
      has_any_conflicting_option_name, get_conflicting_section_names,
      __iter__ (via standard python iterations) or iter_sections
  - Methods on SectionDesc instances (during iteration over sections):
      get_name, has_conflicting_option_names, get_conflicting_option_names,
      ordered_items, has_option, __iter__ or iter_options
  - Methods on OptionDesc instances (during iteration over options):
      get_name, get_value, getint, getfloat, getboolean

Under some conditions, the following new methods are also useful:

  - Methods on OrderedRawConf instances:
      ordered_items, iter_options, has_conflicting_option_names,
      get_conflicting_option_names, get_all_conflicting_option_names
  - Methods on SectionDesc instances (during iteration over sections):
      items, get, getint, getfloat, getboolean

See the the classes PyDoc for full description.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (c) 2001, 2002, 2003, 2004 Python Software Foundation;
    Copyright (C) 2007-2010  Proformatique
                                        All Rights Reserved

    Under PSF LICENSE AGREEMENT FOR PYTHON
    See the following URI for the full license:
        http://www.python.org/download/releases/2.4.4/license/
"""

from ConfigParser import Error, NoSectionError, DuplicateSectionError, \
        NoOptionError, InterpolationError, InterpolationMissingOptionError, \
        InterpolationSyntaxError, InterpolationDepthError, ParsingError, \
        MissingSectionHeaderError

import re


SECTCRE = re.compile(
    r'\['                                 # [
    r'(?P<header>[^]]+)'                  # very permissive!
    r'\]'                                 # ]
    )
OPTCRE = re.compile(
    r'(?P<option>[^:=\s][^:=]*)'          # very permissive!
    r'\s*(?P<vi>[:=])\s*'                 # any number of space/tab,
                                          # followed by separator
                                          # (either : or =), followed
                                          # by any # space/tab
    r'(?P<value>.*)$'                     # everything up to eol
    )


def _id1(x):
    """
    Identity
    """
    return x


class AlreadyLoadedError(Error):
    """
    Raised when trying to load multiple files in an instance that
    doesn't support that.
    """
    def __init__(self, done_filename, second_filename):
        Error.__init__(
	    self,
	    "Trying to load multiple files in an instance that can't do that.\n"
	    "\tdone_filename   : %s\n"
	    "\tsecond_filename : %s\n"
	    % (done_filename, second_filename))
        self.done_filename = done_filename
        self.second_filename = second_filename


class SectionDesc:
    """
    NEW API

    Instances of this class are what is iterated over when one wants to
    scan through sections and are made available thanks to the __iter__
    method of an OrderedRawConf instance.  That means that you can
    simply do, for example:

	conf = OrderedRawConf(None, "/etc/somecoolconfig.conf")
	for section in c:
            print section.get_name()
            print section.ordered_items()
            ...

    Methods callable on this object are automatically derived from
    methods of the OrderedRawConf having a conventional name of
    _<method_name>_by_section_token() in the latter, where <method_name>
    is to be replaced by what you can use on the former.  This happens
    thanks to the __getattr__() special method of this object.

    In the example given above, section.get_name() will eventually
    result in conf._get_name_by_section_token() being called, while
    section.ordered_items() will reach
    conf._ordered_items_by_section_token().
    The first parameter of _<method_name>_by_section_token() methods is
    automaticaly handled by this object, which passes it the correct
    position dependant token, so you don't have to worry much about that.
    Remaining parameters comes from yours.

    The following methods usable via this object are presently available
    thanks to OrderedRawConf and the mecanism described above:

    get_name(self)
        NEW API

        Return the name of the section.

    has_conflicting_option_names(self)
        TEST THAT BEFORE USING OLD/TRANSITIONARY API

        Test if multiple options exist with the same canonical name in
        this section

    get_conflicting_option_names(self)
        NEW API

        Return the list of non canonicalized option names sets for which
        corresponding canonicalized names (as per 'opt_trans' of the
        corresponding OrderedRawConf object) conflicts.

    items(self)
        TRANSITIONARY API

        Return the unordered list of (name, value) option pairs of this
        section.  If multiple options with the same canonical name (as
	per 'opt_trans' of the corresponding OrderedRawConf object)
	exist, only the first one of each different set will be
        returned.

    ordered_items(self)
        NEW API

        Return the ordered list of (name, value) option pairs of this
        section.

    options(self)
        TRANSITIONARY API

        Return the unordered list of option names of this section.  If
        multiple options with the same canonical name (as per
        'opt_trans' of the corresponding OrderedRawConf object) exists,
        only the first one of each different set will be returned.

    ordered_options(self)
        NEW API

        Return the ordered list of option names of this section.

    get(self, option)
        TRANSITIONARY API

        Get the value of the first option of this section with canonical
        name opt_trans(option).  If such an option is not found,
        NoOptionError is raised.

    getint(self, option)
    getfloat(self, option)
    getboolean(self, option)
        TRANSITIONARY API

        Same as get() but cast the results.  See OrderedRawConf PyDoc
        for details.

    has_option(self, option)
        TRANSITIONARY API (rather safe in any case...)

        Test if the section contains at least an option with canonical
        name opt_trans(option).

    iter_options(self)
        NEW API

        Return an option iterator for this section.  If you have an
        instance of this class called 'section' explicitely calling
        section.iter_options() has exactly the same effect.

        That means you can either do:
                for opt in section.iter_options():
                        ...
        or
                for opt in section:
                        ...

        The object returned at each iteration is an instance of
        OptionDesc and can be used to transparently access methods
        with a name of the form _(.*)_by_option_token() of class
        OrderedRawConf with the correct iterable option token abstract
        descriptor.
    """
    def __init__(self, conf_instance, sect_id_token):
        self.conf_instance = conf_instance
        self.sect_id_token = sect_id_token
    def get_section_token(self):
        return self.sect_id_token
    def __getattr__(self, name):
        try:
            func = getattr(self.conf_instance, '_' + name + '_by_section_token')
            return lambda *x: func(self.sect_id_token, *x)
        except AttributeError:
            raise AttributeError, name
    def __iter__(self):
        """
        NEW API

        Return an option iterator for this section.  If you have an
        instance of this class called 'section' explicitely calling
        section.iter_options() has exactly the same effect.

        That means you can either do:
                for opt in section:
                        ...
        or
                for opt in section.iter_options():
                        ...

        The object returned at each iteration is an instance of
        OptionDesc and can be used to transparently access methods
        with a name of the form _(.*)_by_option_token() of class
        OrderedRawConf with the correct iterable option token abstract
        descriptor.
        """
        return self.conf_instance._iter_options_by_section_token(self.sect_id_token)


class OptionDesc:
    """
    NEW API

    Instances of this class are what is iterated over when one wants to
    scan through options and are made available thanks to the __iter__
    or iter_options() methods of a SectionDesc instance or thanks to the
    iter_options() method of an OrderedRawConf instance.  That means
    that you can do for example:

        conf = OrderedRawConf(None, "/etc/somecoolconfig.conf")
        for opt in conf.iter_options("great-section"):
            print opt.get_name() + ': ' + opt.get_value()
        for sec in conf:
            for opt in sec:
                if "great" in opt.get_name():
                    print '['+sec.get_name()+'] ' \\
                          + opt.get_name() + ': ' \\
                          + opt.get_value()

    Methods callable on this object come from methods of the
    OrderedRawConf having a conventional name of
    _<method_name>_by_option_token() in the latter, where <method_name>
    is to be replaced by what you can use on the former.  This happens
    thanks to the __getattr__() special method of this object.

    In the example given above, opt.get_value() will eventually result
    in conf._get_value_by_option_token() in being called.  The mecanism
    is similar to the one used by SectionDesc objects.

    The following methods usable via this object are presently available
    thanks to the automatic mapping described above:

    get_name(self)
        Return the name of this option.

    get_value(self)
        Return the value of this option.

    getint(self)
    getfloat(self)
    getboolean(self)
        Same as get_value() but cast the results.  See OrderedRawConf
        PyDoc for details.
    """
    def __init__(self, conf_instance, opt_id_token):
        self.conf_instance = conf_instance
        self.opt_id_token = opt_id_token
    def get_option_token(self):
        return self.opt_id_token
    def __getattr__(self, name):
        try:
            func = getattr(self.conf_instance, '_' + name + '_by_option_token')
            return lambda *x: func(self.opt_id_token, *x)
        except AttributeError:
            raise AttributeError, name


def _str_to_boolean(v):
    _boolean_states = {'1': True, 'yes': True, 'true': True, 'on': True,
                       '0': False, 'no': False, 'false': False, 'off': False}
    if v.lower() not in _boolean_states:
        raise ValueError, 'Not a boolean: %s' % v
    return _boolean_states[v.lower()]


class OrderedRawConf:
    """
    OrderedRawConf instances work a little like RawConfigParser with the
    main purpose to preserve order of both sections in configuration
    file and options in sections.  While exposing a backward compability
    API consisting of a subset of the one of RawConfigParser (see module
    documentation for main variations and other details), it also
    provides a new API specialy designed to retrieve ordered entities in
    various pythonic ways :)
    """
    def __init__(self, fp = None, filename = None, sect_trans=_id1, opt_trans=_id1):
        """
        The constructor can optionally parse an open file or open and
        parse a file using its filename.  You can also provide it
        canonicalization functions for section names and/or option
        names.

        - If 'fp' does not evaluate to False, the configuration will be
          loaded from this opened file at instantiation time.  If 'fp'
          does evaluate to False but filename does not, the file linked
          by 'filename' will be opened while the constructor is running
          and the configuration will also be loaded from the opened
          file.

        - Whether or not 'fp' evaluate to False, 'filename' has
          precedence over 'fp.name' if not None, the latter being used
          if it exists.  The file name is used in exceptions.

        - 'sect_trans' is a function that must take a string as its
          arguments and must return a string.  It should behave as a
          mathematical function, with no side effect.  Section names are
          stored as dictionary keys for latter use by a subset of the
          RawConfigParser  API after having been transformed by
          'sect_trans', so if for section names 'x' and 'y',
          'sect_trans(x) == sect_trans(y)', only the first one in
          configuration file order will be retrievable with this
          compatibility API.  Also for a given section name 't' any
          lookup is of course done using 'sect_trans(t)' and not
          directly 't'.  Besides the default identity function, useful
          ones could be 'tolower', or maybe even one stripping some
          special character, performing multiple charset detection and
          translation or performing other forms of canonicalization.

        - 'opt_trans' does the same thing as 'sect_trans' is for similar
          use, but is the function to be applied to option names instead
          of section names.
        """
        self._sections = ([], {})
        self.loaded_filename = None
        self.sect_trans = sect_trans
        self.opt_trans = opt_trans
        auto_open = False
        if (not fp) and filename:
            auto_open = True
            fp = open(filename)
        if fp:
            self.readfp(fp, filename)
        if auto_open:
            fp.close()

    def sections(self):
        """
        OLD API FOR RawConfigParser COMPATIBILITY

        Return a list of section canonical names (as per 'sect_trans')
        with at max one occurence of a given canonical name in the
        returned list in case multiple sections with the same canonical
        name exists.
        """
        return self._sections[1].keys()

    def ordered_sections(self):
        """
        NEW API

        Return an ordered list of section names.
        """
        return [n for n, v in self._sections[0]]

    def has_section(self, section):
        """
        OLD API FOR RawConfigParser COMPATIBILITY -  but rather quite
        safe.

        Test if a section with canonical name sect_trans(section) exists
        """
        sec = self.sect_trans(section)
        return sec in self._sections[1]

    def readfp(self, fp, filename=None):
        """
        Load and parse opened file described by fp.
        fp.readline() will be used, and if filename is None fp.name will
        also be tried but is not mandatory.  The file name is used in
        exceptions.

        Note that this function can only be called once and can be
        automatically called at instantiation time by __init__.
        """
        if filename is None:
            try:
                filename = fp.name
            except AttributeError:
                filename = '<???>'
        if self.loaded_filename is not None:
            raise AlreadyLoadedError(self.loaded_filename, filename)
        self._read(fp, filename)
        self.loaded_filename = filename

    def _sectup_by_name(self, section):
        sec = self.sect_trans(section)
        if sec not in self._sections[1]:
            raise NoSectionError(section)
        return self._sections[1][sec]

    def get(self, section, option):
        """
        OLD API FOR RawConfigParser COMPATIBILITY

        Get the value of the first option with canonical name
        opt_trans(option) from the first section with canonical name
        sect_trans(section).  If such a section is not found,
        NoOptionError is raised.  Else if such an option is not found
        in the selected section, NoOptionError is raised.
        """
        s = self._sectup_by_name(section)
        opt = self.opt_trans(option)
        if opt not in s[1]:
            raise NoOptionError(option, section)
        return s[1][opt]

    def items(self, section):
        """
        OLD API FOR RawConfigParser COMPATIBILITY

        Return the unordered list of (name, value) option pairs of the
        first section with canonical name sect_trans(section).  If such
        a section is not found, NoOptionError is raised.  If multiple
        options with the same canonical name (as per 'opt_trans') exists
        in the selected section, only the first one of each different
        set will be returned.
        """
        return self._sectup_by_name(section)[1].items()

    def ordered_items(self, section):
        """
        NEW API - but could still be unsafe if
        has_conflicting_section_names() returns True.

        Return the ordered list of (name, value) option pairs of the
        first section with canonical name sect_trans(section).  If such
        a section is not found, NoOptionError is raised.  It is possible
        to retrieve multiple options with the same canonical name, an
        their respective order are also preserved.
        """
        return self._sectup_by_name(section)[0][:]

    def getint(self, section, option):
        """
        OLD API FOR RawConfigParser COMPATIBILITY

        Same as get() but also cast the to be returned value as an int
        """
        return int(self.get(section, option))

    def getfloat(self, section, option):
        """
        OLD API FOR RawConfigParser COMPATIBILITY

        Same as get() but also cast the to be returned value as a float
        """
        return float(self.get(section, option))

    def getboolean(self, section, option):
        """
        OLD API FOR RawConfigParser COMPATIBILITY

        Same as get() but also cast the to be returned value as a
        boolean.  Case insensitive 'yes', 'true', 'on' and '1' evaluates
        to True.  Case insensitive 'no', 'false', 'off' and '0'
        evaluates to False.  Other values are forbidden and result in a
        ValueError exception to be raised.
        """
        return _str_to_boolean(self.get(section, option))

    def options(self, section):
        """
        OLD API FOR RawConfigParser COMPATIBILITY

        Return the unordered list of canonical option names of the first
        section with canonical name sect_trans(section).  If such a
        section is not found, NoSectionError is raised.  If multiple
        options with the same canonical name exists in the selected
        section, only the first one of each different set will be
        returned.
        """
        return self._sectup_by_name(section)[1].keys()

    def ordered_options(self, section):
        """
        NEW API - but could still be unsafe if
        has_conflicting_section_names() returns True.

        Return the ordered list of option names of the first section
        with canonical name sect_trans(section).  If such a section is
        not found, NoSectionError is raised.
        """
        return [k for (k, v) in self._sectup_by_name(section)[0]]

    def has_option(self, section, option):
        """
        OLD API - unsafe unless has_conflicting_section_names()
        returns False.

        Test if at least a section with canonical name
        sect_trans(section) exists and if the first matching one
        contains at least an option with canonical name
        opt_trans(option).
        """
        sec = self.sect_trans(section)
        if sec not in self._sections[1]:
            return False
        return self.opt_trans(option) in self._sections[1][sec][1]

    def has_conflicting_section_names(self):
        """
        TEST THAT BEFORE USING OLD API

        Test if multiple sections exists with the same canonical name.
        If this is the case it is probably unsafe to use the old API,
        even if the order is not important, because a RawConfigParser
        object would have merged the content of such sections in that
        situation and we do not.
        The user can decide by itself if such a situation is an error
        and if it is not he can still safely use the new ordered API.
        """
        return len(self._sections[0]) != len(self._sections[1])

    def get_conflicting_section_names(self):
        """
        NEW API - probably useful for diagnostic purposes in both
        NEW and OLD API oriented code.

        Return the list of non canonicalized section names sets for
        which corresponding canonicalized names conflicts.
        """
        cf = []
        cd = {}
        for sec, opts in self._sections[0]:
            tsec = self.sect_trans(sec)
            if tsec not in cd:
                cf.append([sec])
                cd[tsec] = len(cf) - 1
            else:
                cf[cd[tsec]].append(sec)
        return [tuple(c) for c in cf if len(c) > 1]

    @staticmethod
    def _has_conflicting_option_names_int(s):
        return (len(s[0]) != len(s[1]))

    def _get_conflicting_option_names_int(self, s):
        cf = []
        cd = {}
        for opt, val in s[0]:
            topt = self.opt_trans(opt)
            if topt not in cd:
                cf.append([opt])
                cd[topt] = len(cf) - 1
            else:
                cf[cd[topt]].append(opt)
        return [tuple(c) for c in cf if len(c) > 1]

    def has_conflicting_option_names(self, section):
        """
        TEST THAT BEFORE USING OLD API
        This function makes most sense when
        has_conflicting_section_names() returns False.

        Test if multiple options exists with the same canonical name
        in the first section with canonical name sect_trans(section).
        If such a section is not found, NoOptionError is raised.
        If multiple options exists it is probably unsafe to use the
        old API, even if the order is not important, because a
        RawConfigParser object might have behaved differently in that
        situation.
        The user can decide by itself if such a situation is an error
        and if it is not he can still safely use the new ordered API.
        """
        return self._has_conflicting_option_names_int(
                self._sectup_by_name(section))

    def get_conflicting_option_names(self, section):
        """
        NEW API - probably useful for diagnostic purposes in both
        NEW and OLD API oriented code.
        This function makes most sense when
        has_conflicting_section_names() returns False.

        Return the list of non canonicalized option names sets for
        which corresponding canonicalized names conflicts, in the
        first found section with a canonical name of sect_trans(section)
        If such a section is not found, NoOptionError is raised.
        """
        return self._get_conflicting_option_names_int(
                self._sectup_by_name(section))

    def has_any_conflicting_option_name(self):
        """
        TEST THAT BEFORE USING OLD API

        Test if any section contains options with conflicting canonical
        names.
        """
        for sec, opts in self._sections[0]:
            if self._has_conflicting_option_names_int(opts):
                return True
        return False

    def get_all_conflicting_option_names(self):
        """
        NEW API - probably useful for diagnostic purposes in both
        NEW and OLD API oriented code.

        Return a list of tuples containing section name in the first
        element, and the list of conflicting canonical option name in
        the second.
        """
        lst = []
        for sec, opts in self._sections[0]:
            lst.append((sec, self._get_conflicting_option_names_int(opts)))
        return lst

    def has_no_duplication(self):
        """
        TEST THAT BEFORE USING OLD API

        Check that there is neither any conflicting section names, nor
        any conflicting option name in each sections.

        It is believed that you can safely use any public method if
        this one returns True, and that you should retain to do so for
        most OLD API ones and probably even some of the NEW API if it
        returns False.

        You might even want to abort report errors when this function
        returns False if you plan to use the NEW API, in some
        situations where it does not make sense to have duplicated
        section or option names.
        """
        return (not self.has_conflicting_section_names()) \
                and (not self.has_any_conflicting_option_name())
    is_probably_safe_with_old_api = has_no_duplication

    def iter_sections(self):
        """
        NEW API

        Return a section iterator for this parsed configuration object.
        __iter__ is also defined and does exactly the same thing, so
        you can either do (on an instance called 'conf'):
                for sec in conf.iter_sections():
                        ...
        or:
                for sec in conf:
                        ...

        The object returned at each iteration is an instance of
        SectionDesc and can be used to transparently access methods
        with a name of the form _(.*)_by_section_token() of this class
        with the correct iterable section token abstract descriptor.
        """
        for i in xrange(len(self._sections[0])):
            yield SectionDesc(self, i)
    __iter__ = iter_sections

    def _sectup_by_tok(self, token):
        return self._sections[0][token][1]

    def _get_name_by_section_token(self, i):
        return self._sections[0][i][0]

    def _has_conflicting_option_names_by_section_token(self, i):
        return self._has_conflicting_option_names_int(self._sectup_by_tok(i))

    def _get_conflicting_option_names_by_section_token(self, i):
        return self._get_conflicting_option_names_int(self._sectup_by_tok(i))

    def _items_by_section_token(self, i):
        return self._sectup_by_tok(i)[1].items()

    def _ordered_items_by_section_token(self, i):
        return self._sectup_by_tok(i)[0][:]

    def _get_by_section_token(self, i, option):
        s = self._sectup_by_tok(i)
        section = self._get_name_by_section_token(i)
        opt = self.opt_trans(option)
        if opt not in s[1]:
            raise NoOptionError(option, section)
        return s[1][opt]

    def _getint_by_section_token(self, i, option):
        return int(self._get_by_section_token(i, option))

    def _getfloat_by_section_token(self, i, option):
        return float(self._get_by_section_token(i, option))

    def _getboolean_by_section_token(self, i, option):
        return _str_to_boolean(self._get_by_section_token(i, option))

    def _has_option_by_section_token(self, i, option):
        opt = self.opt_trans(option)
        return opt in self._sectup_by_tok(i)[1]

    def _options_by_section_token(self, i):
        return self._sectup_by_tok(i)[1].keys()

    def _ordered_options_by_section_token(self, i):
        return [k for k, v in self._sectup_by_tok(i)[0]]

    def _iter_options_by_sectup(self, sectup):
        for j in xrange(len(sectup[0])):
            yield OptionDesc(self, (sectup, j))

    def _iter_options_by_section_token(self, i):
        return self._iter_options_by_sectup(self._sectup_by_tok(i))

    def iter_options(self, section):
        """
        NEW API - but could still be unsafe if
        has_conflicting_section_names() returns True.

        Return an option iterator for the first section having a
        canonical name of sect_trans(section).

        The object returned at each iteration is an instance of
        OptionDesc and can be used to transparently access methods
        with a name of the form _(.*)_by_option_token() of this class
        with the correct iterable option token abstract descriptor.
        """
        return self._iter_options_by_sectup(self._sectup_by_name(section))

    @staticmethod
    def _get_name_by_option_token((sectup, ot)):
        return sectup[0][ot][0]

    @staticmethod
    def _get_value_by_option_token((sectup, ot)):
        return sectup[0][ot][1]

    def _getint_by_option_token(self, tok):
        return int(self._get_value_by_option_token(tok))

    def _getfloat_by_option_token(self, tok):
        return float(self._get_value_by_option_token(tok))

    def _getboolean_by_option_token(self, tok):
        return _str_to_boolean(self._get_value_by_option_token(tok))

    def _read(self, fp, filename):
        lineno = 0
        cur_sect = None
        cur_opt = None
        cur_dict_opt = None
        e = None                        # None, or an exception
        while True:
            line = fp.readline()
            if not line:
                break
            lineno = lineno + 1
            # comment or blank line?
            if line.strip() == '' or line[0] in '#;':
                continue
            if line.split(None, 1)[0].lower() == 'rem' and line[0] in "rR":
                # no leading whitespace
                continue
            # continuation line?
            if line[0].isspace() and (cur_sect is not None) and (cur_opt is not None):
                value = line.strip()
                if value:
                    key = cur_sect[0][cur_opt][0]
                    newval = cur_sect[0][cur_opt][1] + '\n' + value
                    cur_sect[0][cur_opt] = (key, newval)
                    if cur_dict_opt is not None:
                        cur_sect[1][cur_dict_opt] = newval
                continue
            # a section header or option header?
            # is it a section header?
            mo = SECTCRE.match(line)
            if mo:
                sectname = mo.group('header')
                tsec = self.sect_trans(sectname)
                newopt = ([], {})
                self._sections[0].append((sectname, newopt))
                cur_sect = newopt
                if tsec not in self._sections[1]:
                    self._sections[1][tsec] = newopt
                cur_opt = None
                cur_dict_opt = None
                continue
            if cur_sect is None:
                raise MissingSectionHeaderError(filename, lineno, line)
            # an option line?
            mo = OPTCRE.match(line)
            if mo:
                optname, vi, optval = mo.group('option', 'vi', 'value')
                if vi in ('=', ':') and ';' in optval:
                    # ';' is a comment delimiter only if it follows
                    # a spacing character
                    pos = optval.find(';')
                    if pos > 0 and optval[pos-1].isspace():
                        optval = optval[:pos]
                optname = optname.rstrip()
                topt = self.opt_trans(optname)
                optval = optval.strip()
                cur_sect[0].append((optname, optval))
                cur_opt = len(cur_sect[0]) - 1
                if topt not in cur_sect[1]:
                    cur_dict_opt = topt
                    cur_sect[1][topt] = optval
                else:
                    cur_dict_opt = None
                continue
            if not e:
                e = ParsingError(filename)
            e.append(lineno, repr(line))
        if e:
            raise e


if __name__ == '__main__':
    from cStringIO import StringIO
    import unittest
    import operator
    import string
    import sys

    def whoami(lvl=0):
        return sys._getframe(lvl+1).f_code.co_name

    EMPTY_CONF = "\n"

    VALID_CONF = """
[blablabla]
key1=blablabla_val_1
key2=blablabla_val_2

[corpremedaille]
okey1=corpremedaille_val_1
okey2=corpremedaille_val_2

[transtyping]
key_int=42
key_float=131.31337
key_boolean_0=fALSe
key_boolean_1=1
    """

    VALID_CONF_SKV = (
      ('blablabla', 'key1', 'blablabla_val_1'),
      ('blablabla', 'key2', 'blablabla_val_2'),
      ('corpremedaille', 'okey1', 'corpremedaille_val_1'),
      ('corpremedaille', 'okey2', 'corpremedaille_val_2'),
      ('transtyping', 'key_int', '42'),
      ('transtyping', 'key_float', '131.31337'),
      ('transtyping', 'key_boolean_0', 'fALSe'),
      ('transtyping', 'key_boolean_1', '1')
    )

    VALID_CONF_INT = (
      ('transtyping', 'key_int', 42),
      ('transtyping', 'key_boolean_1', 1)
    )

    VALID_CONF_FLOAT = (
      ('transtyping', 'key_int', 42.0),
      ('transtyping', 'key_float', 131.31337),
      ('transtyping', 'key_boolean_1', 1.0)
    )

    VALID_CONF_BOOLEAN = (
      ('transtyping', 'key_boolean_0', False),
      ('transtyping', 'key_boolean_1', True)
    )

    VALID_SECTIONS = ('blablabla', 'corpremedaille', 'transtyping')
    OTHER_SECTIONS = ('kikoolol', 'wazza')

    SECTION_CONFLICT_CONF = """
[blabla]
[evil]
[blabla]
    """

    DOUBLE_SECTION_CONFLICT_CONF = """
[blabla]
[evil]
[kikoo]
[blabla]
[kikoo]
    """

    MORE_CANONICAL_SECTION_CONFLICT_CONF = DOUBLE_SECTION_CONFLICT_CONF + "\n[BLABLA]\n"

    OPTION_CONFLICT_CONF = """
[a]
z=t
[blabla]
k=v1
x=y
y=123
k=v2
y=456
[b]
z=t
[other]
c=1
c=2
    """

    MORE_CANONICAL_OPTION_CONFLICT_CONF = OPTION_CONFLICT_CONF + "\nC=42\n"

    C14N_CHANGE_SECTION_BEHAVIOR = "[blabla]\n[BLABLA]\n"

    C14N_CHANGE_OPTION_BEHAVIOR = "[blabla]\nkey=1\nKEY=2\n"

    NO_SEC_CONF = "k=v\n"

    INVAL_CONF = """
[sec]
INVALID
k=v
    """

    def parser_origin():
        return '<' + whoami(1) + '>'

    class Parsing(unittest.TestCase):
        def commonTest(self, fo, tst_name):
            try:
                OrderedRawConf(fo, tst_name)
            except Error:
                self.fail()
        def testEmpty(self):
            self.commonTest(StringIO(EMPTY_CONF), parser_origin())
        def testSimple(self):
            self.commonTest(StringIO(VALID_CONF), parser_origin())
        def testSectionConflict(self):
            self.commonTest(StringIO(SECTION_CONFLICT_CONF), parser_origin())
        def testOptionConflict(self):
            self.commonTest(StringIO(OPTION_CONFLICT_CONF), parser_origin())
        def testNoSection(self):
            self.assertRaises(Error, OrderedRawConf, StringIO(NO_SEC_CONF), parser_origin())
        def testInval(self):
            self.assertRaises(Error, OrderedRawConf, StringIO(INVAL_CONF), parser_origin())

    class SimpleOldApi(unittest.TestCase):
        set_skv, set_int, set_float, set_boolean = VALID_CONF_SKV, VALID_CONF_INT, VALID_CONF_FLOAT, VALID_CONF_BOOLEAN
        section_set = frozenset(VALID_SECTIONS)
        other_set = frozenset(OTHER_SECTIONS)
        @staticmethod
        def cmpSection(sa, sb):
            return sa == sb
        @staticmethod
        def keyToCanon(k):
            return k
        def setUp(self):
            self.ord_cnf = OrderedRawConf(StringIO(VALID_CONF), parser_origin())
        def commonGetLike(self, method, xset):
            map(lambda (x, y, z): self.assertEqual(method(x, y), z), xset)
        def testGet(self):
            self.commonGetLike(self.ord_cnf.get, self.set_skv)
        def testGetInt(self):
            self.commonGetLike(self.ord_cnf.getint, self.set_int)
        def testGetFloat(self):
            self.commonGetLike(self.ord_cnf.getfloat, self.set_float)
        def testGetBoolean(self):
            self.commonGetLike(self.ord_cnf.getboolean, self.set_boolean)
        def testGetRaisesSections(self):
            for wrong_s in OTHER_SECTIONS:
                self.assertRaises(NoSectionError, self.ord_cnf.get, wrong_s, 'x')
                self.assertRaises(NoSectionError, self.ord_cnf.getint, wrong_s, 'x')
                self.assertRaises(NoSectionError, self.ord_cnf.getfloat, wrong_s, 'x')
                self.assertRaises(NoSectionError, self.ord_cnf.getboolean, wrong_s, 'x')
        def testGetRaisesOption(self):
            for wrong_s in OTHER_SECTIONS:
                self.assertRaises(NoOptionError, self.ord_cnf.get, VALID_SECTIONS[0], wrong_s)
                self.assertRaises(NoOptionError, self.ord_cnf.getint, VALID_SECTIONS[0], wrong_s)
                self.assertRaises(NoOptionError, self.ord_cnf.getfloat, VALID_SECTIONS[0], wrong_s)
                self.assertRaises(NoOptionError, self.ord_cnf.getboolean, VALID_SECTIONS[0], wrong_s)
        def testSections(self):
            self.assertEqual(frozenset(self.ord_cnf.sections()), self.section_set)
        def testHasSection(self):
            map(lambda x: self.assertEqual(self.ord_cnf.has_section(x), True), self.section_set)
            map(lambda x: self.assertEqual(self.ord_cnf.has_section(x), False), self.other_set)
        def testItems(self):
            for sec in self.section_set:
                self.assertEqual(frozenset(self.ord_cnf.items(sec)),
                                 frozenset([(self.keyToCanon(k), v) for s, k, v in self.set_skv if self.cmpSection(s, sec)]))
        def testItemsRaises(self):
            for wrong_s in OTHER_SECTIONS:
                self.assertRaises(NoSectionError, self.ord_cnf.items, wrong_s)
        def testOptions(self):
            for sec in self.section_set:
                self.assertEqual(frozenset(self.ord_cnf.options(sec)),
                                 frozenset([self.keyToCanon(k) for s, k, v in self.set_skv if self.cmpSection(s, sec)]))
        def testOptionsRaises(self):
            for wrong_s in OTHER_SECTIONS:
                self.assertRaises(NoSectionError, self.ord_cnf.options, wrong_s)
        def testHasOption(self):
            map(lambda (s, k, v): self.assertEqual(self.ord_cnf.has_option(s, k), True), self.set_skv)
            for sec in self.other_set:
                map(lambda (s, k, v): self.assertEqual(self.ord_cnf.has_option(sec, k), False), self.set_skv)
                map(lambda (s, k, v): self.assertEqual(self.ord_cnf.has_option(s, sec), False), self.set_skv)

    class NonIterConfictApi(unittest.TestCase):
        @staticmethod
        def sectionSynonym(s):
            return s
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin())
        def testConflictingSectionNames(self):
            ConfSecName = (
              (EMPTY_CONF, []),
              (VALID_CONF, []),
              (SECTION_CONFLICT_CONF, [("blabla", "blabla")]),
              (DOUBLE_SECTION_CONFLICT_CONF, [("blabla", "blabla"), ("kikoo", "kikoo")]),
              (OPTION_CONFLICT_CONF, [])
            )
            for conftxt, result in ConfSecName:
                ord_cnf = self.factory(conftxt)
                self.assertEqual(ord_cnf.has_conflicting_section_names(), result != [])
                self.assertEqual(ord_cnf.get_conflicting_section_names(), result)
        def testConflictingOptionNames(self):
            ConfOptName = (
              (VALID_CONF, self.sectionSynonym("blablabla"), []),
              (VALID_CONF, self.sectionSynonym("corpremedaille"), []),
              (VALID_CONF, self.sectionSynonym("transtyping"), []),
              (SECTION_CONFLICT_CONF, self.sectionSynonym("blabla"), []),
              (SECTION_CONFLICT_CONF, self.sectionSynonym("evil"), []),
              (DOUBLE_SECTION_CONFLICT_CONF, self.sectionSynonym("blabla"), []),
              (DOUBLE_SECTION_CONFLICT_CONF, self.sectionSynonym("evil"), []),
              (DOUBLE_SECTION_CONFLICT_CONF, self.sectionSynonym("kikoo"), []),
              (OPTION_CONFLICT_CONF, self.sectionSynonym("a"), []),
              (OPTION_CONFLICT_CONF, self.sectionSynonym("blabla"), [('k', 'k'), ('y', 'y')]),
              (OPTION_CONFLICT_CONF, self.sectionSynonym("other"), [('c', 'c')]),
              (OPTION_CONFLICT_CONF, self.sectionSynonym("b"), [])
            )
            for conftxt, section, result in ConfOptName:
                ord_cnf = self.factory(conftxt)
                self.assertEqual(ord_cnf.has_conflicting_option_names(section), result != [])
                self.assertEqual(ord_cnf.get_conflicting_option_names(section), result)
                self.assertRaises(NoSectionError, ord_cnf.has_conflicting_option_names, "nothing")
                self.assertRaises(NoSectionError, ord_cnf.get_conflicting_option_names, "nothing")
        def testAllConflictingOptionNames(self):
            AllConfOptName = (
              (EMPTY_CONF, []),
              (VALID_CONF, [("blablabla", []), ("corpremedaille", []), ("transtyping", []),]),
              (SECTION_CONFLICT_CONF, [('blabla', []), ('evil', []), ('blabla', [])]),
              (DOUBLE_SECTION_CONFLICT_CONF, [('blabla', []), ('evil', []), ('kikoo', []), ('blabla', []), ('kikoo', [])]),
              (OPTION_CONFLICT_CONF, [('a', []), ('blabla', [('k', 'k'), ('y', 'y')]), ('b', []), ('other', [('c', 'c')])])
            )
            for conftxt, result in AllConfOptName:
                ord_cnf = self.factory(conftxt)
                self.assertEqual(ord_cnf.has_any_conflicting_option_name(), reduce(operator.concat, map(lambda x:x[1], result), []) != [])
                self.assertEqual(ord_cnf.get_all_conflicting_option_names(), result)
        def testIsProbablySafeWithOldApi(self):
            IsProbablySafeWithOldApi = (
              (EMPTY_CONF, True),
              (VALID_CONF, True),
              (SECTION_CONFLICT_CONF, False),
              (DOUBLE_SECTION_CONFLICT_CONF, False),
              (OPTION_CONFLICT_CONF, False)
            )
            for conftxt, result in IsProbablySafeWithOldApi:
                ord_cnf = self.factory(conftxt)
                self.assertEqual(ord_cnf.is_probably_safe_with_old_api(), result)
        def commonFullApiForOneConf(self, ord_cnf, resdesc):
            self.assertEqual(ord_cnf.has_conflicting_section_names(), resdesc['hcsn'])
            self.assertEqual(ord_cnf.get_conflicting_section_names(), resdesc['gcsn'])
            for sec, result in resdesc['acon_gcon']:
                self.assertEqual(ord_cnf.has_conflicting_option_names(sec), result != [])
                self.assertEqual(ord_cnf.get_conflicting_option_names(sec), result)
            self.assertEqual(ord_cnf.has_any_conflicting_option_name(), resdesc['hacon'])
            self.assertEqual(ord_cnf.get_all_conflicting_option_names(), resdesc['gacon'])
            self.assertEqual(ord_cnf.is_probably_safe_with_old_api(), resdesc['ipswoa'])

    class C14nSectionsSimpleOldApiNoChange(SimpleOldApi):
        def setUp(self):
            self.ord_cnf = OrderedRawConf(StringIO(VALID_CONF), parser_origin(), string.lower)

    class C14nSectionsNonIterConfictApiNoChange(NonIterConfictApi):
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), string.lower)

    class C14nOptionsSimpleOldApiNoChange(SimpleOldApi):
        def setUp(self):
            self.ord_cnf = OrderedRawConf(StringIO(VALID_CONF), parser_origin(), _id1, string.lower)

    class C14nOptionsNonIterConfictApiNoChange(NonIterConfictApi):
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), _id1, string.lower)

    def map_skv_sec_upper(skv):
        return map(lambda (s, k, v): (s.upper(), k, v), skv)

    class C14nSectionsSimpleOldApiRequestUpper(SimpleOldApi):
        set_skv, set_int, set_float, set_boolean = \
                map(map_skv_sec_upper, (VALID_CONF_SKV, VALID_CONF_INT, VALID_CONF_FLOAT, VALID_CONF_BOOLEAN))
        @staticmethod
        def cmpSection(sa, sb):
            return sa.lower() == sb.lower()
        def setUp(self):
            self.ord_cnf = OrderedRawConf(StringIO(VALID_CONF), parser_origin(), string.lower)

    def map_skv_opt_upper(skv):
        return map(lambda (s, k, v): (s, k.upper(), v), skv)

    class C14nOptionssSimpleOldApiRequestUpper(SimpleOldApi):
        set_skv, set_int, set_float, set_boolean = \
                map(map_skv_opt_upper, (VALID_CONF_SKV, VALID_CONF_INT, VALID_CONF_FLOAT, VALID_CONF_BOOLEAN))
        @staticmethod
        def keyToCanon(k):
            return k.lower()
        def setUp(self):
            self.ord_cnf = OrderedRawConf(StringIO(VALID_CONF), parser_origin(), _id1, string.lower)

    class C14nSectionsNonIterConfictRequestUpper(NonIterConfictApi):
        @staticmethod
        def sectionSynonym(s):
            return s.upper()
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), string.lower)
        def testC14nMoreSectionConflict(self):
            ord_cnf = self.factory(MORE_CANONICAL_SECTION_CONFLICT_CONF)
            result_descriptor = {
              'hcsn': True,
              'gcsn': [("blabla", "blabla", "BLABLA"), ("kikoo", "kikoo")],
              'acon_gcon': [("BLABLA", []), ("EVIL", []), ("KIKOO", [])],
              'hacon': False,
              'gacon': [('blabla', []), ('evil', []), ('kikoo', []),
                        ('blabla', []), ('kikoo', []), ('BLABLA', [])],
              'ipswoa': False
            }
            self.commonFullApiForOneConf(ord_cnf, result_descriptor)
        def testC14nInsensitiveBehavior(self):
            ord_cnf = self.factory(C14N_CHANGE_SECTION_BEHAVIOR)
            result_descriptor = {
              'hcsn': True,
              'gcsn': [("blabla", "BLABLA")],
              'acon_gcon': [("BLABLA", [])],
              'hacon': False,
              'gacon': [('blabla', []), ('BLABLA', [])],
              'ipswoa': False
            }
            self.commonFullApiForOneConf(ord_cnf, result_descriptor)
        def testC14nBaseBehavior(self):
            ord_cnf = OrderedRawConf(StringIO(C14N_CHANGE_SECTION_BEHAVIOR), parser_origin())
            co = [("blabla", []), ("BLABLA", [])]
            result_descriptor = {
              'hcsn': False,
              'gcsn': [],
              'acon_gcon': co,
              'hacon': False,
              'gacon': co,
              'ipswoa': True
            }
            self.commonFullApiForOneConf(ord_cnf, result_descriptor)

    class C14nOptionsNonIterConfictApiRequestUpper(NonIterConfictApi):
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), _id1, string.lower)
        def testC14nMoreOptionConflict(self):
            ord_cnf = self.factory(MORE_CANONICAL_OPTION_CONFLICT_CONF)
            ConfOptName = [
              ('a', []),
              ('blabla', [('k', 'k'), ('y', 'y')]),
              ('b', []),
              ('other', [('c', 'c', 'C')])
            ]
            result_descriptor = {
              'hcsn': False,
              'gcsn': [],
              'acon_gcon': ConfOptName,
              'hacon': True,
              'gacon': ConfOptName,
              'ipswoa': False
            }
            self.commonFullApiForOneConf(ord_cnf, result_descriptor)
        def testC14nInsensitiveBehavior(self):
            ord_cnf = self.factory(C14N_CHANGE_OPTION_BEHAVIOR)
            ConfOptName = [
              ('blabla', [('key', 'KEY')])
            ]
            result_descriptor = {
              'hcsn': False,
              'gcsn': [],
              'acon_gcon': ConfOptName,
              'hacon': True,
              'gacon': ConfOptName,
              'ipswoa': False
            }
            self.commonFullApiForOneConf(ord_cnf, result_descriptor)
        def testC14nBaseBehavior(self):
            ord_cnf = OrderedRawConf(StringIO(C14N_CHANGE_OPTION_BEHAVIOR), parser_origin())
            ConfOptName = [
              ('blabla', [])
            ]
            result_descriptor = {
              'hcsn': False,
              'gcsn': [],
              'acon_gcon': ConfOptName,
              'hacon': False,
              'gacon': ConfOptName,
              'ipswoa': True
            }
            self.commonFullApiForOneConf(ord_cnf, result_descriptor)

    ITERSECTION_ONE = "[section]\nk=v1\nint=42\nfloat=131.31337\nboolean=on\n"

    ITERSECTION_TWO = ITERSECTION_ONE + "[section2]\nk=v2\n"

    ITERSECTION_CONFLICT = ITERSECTION_ONE + "[section]\nk=v2\n"

    ITER_CONF = """
[sec_empty]
[sec_1]
key=value_sec_1
[sec_2]
key1=value_sec_2_key_1
key2=value_sec_2_key_2
[sec_opt_conflict]
key=val1
key=val2
[sec_c14n_opt_conflict]
key=this_one
KEY=second_one
[name_conflict]
key=this_one
[name_conflict]
key=not_this_one
[name_c14n_conflict]
key=this_one
[NAME_C14N_CONFLICT]
key=second_one
            """

    ITER_CONF_SECTIONS = ["sec_empty", "sec_1", "sec_2", "sec_opt_conflict",
                          "sec_c14n_opt_conflict", "name_conflict", "name_conflict",
                          "name_c14n_conflict", "NAME_C14N_CONFLICT"]

    class NewApi(unittest.TestCase):
        ordItems = (
          [],
          [('key', 'value_sec_1')],
          [('key1', 'value_sec_2_key_1'), ('key2', 'value_sec_2_key_2')],
          [('key', 'val1'), ('key', 'val2')],
          [('key', 'this_one'), ('KEY', 'second_one')],
          [('key', 'this_one')],
          [('key', 'not_this_one')],
          [('key', 'this_one')],
          [('key', 'second_one')]
        )
        @staticmethod
        def secMapper(secname):
            return secname
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin())
        def secFilter(self, lst_tuple_sec_data):
            for p, (secname, data) in enumerate(lst_tuple_sec_data):
                if self.secMapper(secname) not in map(lambda (x, y):x, lst_tuple_sec_data[:p]):
                    yield (secname, data)
        def injectAndFilter(self, datas):
            return self.secFilter(zip(ITER_CONF_SECTIONS, datas))
        def testOrderedSections(self):
            ord_cnf = self.factory(ITER_CONF)
            self.assertEqual(ord_cnf.ordered_sections(), ITER_CONF_SECTIONS)
        def testSections(self):
            ord_cnf = self.factory(ITER_CONF)
            self.assertEqual(frozenset(ord_cnf.sections()), frozenset(map(self.secMapper, ITER_CONF_SECTIONS)))
        def testOrderedItems(self):
            ord_cnf = self.factory(ITER_CONF)
            for s, loi in self.injectAndFilter(self.ordItems):
                self.assertEqual(ord_cnf.ordered_items(s), loi)
        def testOrderedItemsRaises(self):
            ord_cnf = self.factory(ITER_CONF)
            self.assertRaises(NoSectionError, ord_cnf.ordered_items, "nothing")
        def testOrderedOptions(self):
            ord_cnf = self.factory(ITER_CONF)
            for s, loi in self.injectAndFilter(self.ordItems):
                self.assertEqual(ord_cnf.ordered_options(s), map(lambda (k, v):k, loi))
        def testOrderedOptionsRaises(self):
            ord_cnf = self.factory(ITER_CONF)
            self.assertRaises(NoSectionError, ord_cnf.ordered_options, "nothing")

    class C14nSectionsNewApi(NewApi):
        @staticmethod
        def secMapper(secname):
            return secname.lower()
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), string.lower)

    class C14nOptionsNewApi(NewApi):
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), _id1, string.lower)

    class IterSections(unittest.TestCase):
        hcon = (False, False, False, True, False, False, False, False, False)
        gcon = ([], [], [], [('key', 'key')], [], [], [], [], [])
        c14n_skv = ('sec_c14n_opt_conflict', 'KEY', 'second_one')
        @staticmethod
        def secMapper(secname):
            return secname
        @staticmethod
        def optMapper(optname):
            return optname
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin())
        def testIterSectionEmpty(self):
            ord_cnf = self.factory("")
            for sec in ord_cnf:
                self.fail()
            for sec in ord_cnf.iter_sections():
                self.fail()
            isec = ord_cnf.iter_sections()
            self.assertRaises(StopIteration, isec.next)
        def commonSecIterOne(self, si1):
            self.assertEqual(si1.get_name(), 'section')
            self.assertEqual(si1.get('k'), 'v1')
            self.assertEqual(si1.getint('int'), 42)
            self.assertEqual(si1.getfloat('float'), 131.31337)
            self.assertEqual(si1.getboolean('boolean'), True)
            self.assertRaises(NoOptionError, si1.get, "nothing")
            self.assertRaises(NoOptionError, si1.getint, "nothing")
            self.assertRaises(NoOptionError, si1.getfloat, "nothing")
            self.assertRaises(NoOptionError, si1.getboolean, "nothing")
            options_pst = [('k', True), ('int', True), ('float', True),
                           ('boolean', True), ('nothere', False)]
            items = [('k', 'v1'), ('int', '42'), ('float', '131.31337'),
                     ('boolean', 'on')]
            map(lambda (k, v): self.assertEqual(si1.has_option(k), v), options_pst)
            self.assertEqual(si1.ordered_items(), items)
            self.assertEqual(frozenset(si1.items()), frozenset(map(lambda (k, v): (self.optMapper(k), v), items)))
            self.assertEqual(si1.ordered_options(), map(lambda (k, v): k, items))
            self.assertEqual(frozenset(si1.options()),
                             frozenset(map(lambda (k, v): self.optMapper(k), items)))
        def testIterSectionOne(self):
            ord_cnf = self.factory(ITERSECTION_ONE)
            more = False
            for sec in ord_cnf:
                self.failIf(more)
                self.commonSecIterOne(sec)
                more = True
            more = False
            for sec in ord_cnf.iter_sections():
                self.failIf(more)
                self.commonSecIterOne(sec)
                more = True
            isec = ord_cnf.iter_sections()
            self.assert_(bool(isec.next()))
            self.assertRaises(StopIteration, isec.next)
        def commonTwoSections(self, conftxt):
            ord_cnf = self.factory(conftxt)
            isec = ord_cnf.iter_sections()
            self.assertEqual(isec.next().get('k'), 'v1')
            self.assertEqual(isec.next().get('k'), 'v2')
            self.assertRaises(StopIteration, isec.next)
        def testIterSectionTwo(self):
            self.commonTwoSections(ITERSECTION_TWO)
        def testIterSectionConflict(self):
            self.commonTwoSections(ITERSECTION_CONFLICT)
        def testIterConf(self):
            ord_cnf = self.factory(ITER_CONF)
            self.assertEqual([sec.get_name() for sec in ord_cnf], ITER_CONF_SECTIONS)
            for sec, hc, gc in zip(ord_cnf, self.hcon, self.gcon):
                self.assertEqual(sec.has_conflicting_option_names(), hc)
                self.assertEqual(sec.get_conflicting_option_names(), gc)
        def testC14n(self):
            ord_cnf = self.factory(ITER_CONF)
            for sec in ord_cnf:
                if sec.get_name() == self.c14n_skv[0]:
                    self.assertEqual(sec.get(self.c14n_skv[1]), self.c14n_skv[2])
                    break

    class IterOptions(unittest.TestCase):
        c14n_skv = ('NAME_C14N_CONFLICT', 'key', 'second_one')
        @staticmethod
        def secMapper(secname):
            return secname
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin())
        def commonOptIterOne(self, iopt):
            opt = iopt.next()
            self.assertEqual(opt.get_name(), "k")
            self.assertEqual(opt.get_value(), "v1")
            opt = iopt.next()
            self.assertEqual(opt.get_name(), "int")
            self.assertEqual(opt.get_value(), "42")
            self.assertEqual(opt.getint(), 42)
            opt = iopt.next()
            self.assertEqual(opt.get_name(), "float")
            self.assertEqual(opt.get_value(), "131.31337")
            self.assertEqual(opt.getfloat(), 131.31337)
            opt = iopt.next()
            self.assertEqual(opt.get_name(), "boolean")
            self.assertEqual(opt.get_value(), "on")
            self.assertEqual(opt.getboolean(), True)
            self.assertRaises(StopIteration, iopt.next)
        def testIterRaises(self):
            ord_cnf = self.factory(ITERSECTION_ONE)
            self.assertRaises(NoSectionError, ord_cnf.iter_options, "nothing")
        def testIterSectionOne(self):
            ord_cnf = self.factory(ITERSECTION_ONE)
            self.commonOptIterOne(ord_cnf.iter_options("section"))
            more = False
            for sec in ord_cnf:
                self.failIf(more)
                self.commonOptIterOne(sec.iter_options())
                more = True
            more = False
            for sec in ord_cnf.iter_sections():
                self.failIf(more)
                self.commonOptIterOne(sec.iter_options())
                more = True
        def testIterOptionsPrio(self):
            ord_cnf = self.factory(ITER_CONF)
            more = False
            for opt in ord_cnf.iter_options(self.c14n_skv[0]):
                self.failIf(more)
                self.assertEqual(opt.get_name(), self.c14n_skv[1])
                self.assertEqual(opt.get_value(), self.c14n_skv[2])
                more = True
        def testIterConf(self):
            ord_cnf = self.factory(ITER_CONF)
            all_skv = (
              ('sec_empty', ()),
              ('sec_1', (('key', 'value_sec_1'),)),
              ('sec_2', (('key1', 'value_sec_2_key_1'), ('key2', 'value_sec_2_key_2'))),
              ('sec_opt_conflict', (('key', 'val1'), ('key', 'val2'))),
              ('sec_c14n_opt_conflict', (('key', 'this_one'), ('KEY', 'second_one'))),
              ('name_conflict', (('key', 'this_one'),)),
              ('name_conflict', (('key', 'not_this_one'),)),
              ('name_c14n_conflict', (('key', 'this_one'),)),
              ('NAME_C14N_CONFLICT', (('key', 'second_one'),))
            )
            for sec, sec_tup in zip(ord_cnf, all_skv):
                self.assertEqual(sec.get_name(), sec_tup[0])
                for opt, opt_desc in zip(sec, sec_tup[1]):
                    self.assertEqual(opt.get_name(), opt_desc[0])
                    self.assertEqual(opt.get_value(), opt_desc[1])

    class C14nIterSections(IterSections):
        @staticmethod
        def secMapper(secname):
            return secname.lower()
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), string.lower)

    class C14nIterSections_c14nOptions(IterSections):
        hcon = (False, False, False, True, True, False, False, False, False)
        gcon = ([], [], [], [('key', 'key')], [('key', 'KEY')], [], [], [], [])
        c14n_skv = ('sec_c14n_opt_conflict', 'KEY', 'this_one')
        @staticmethod
        def optMapper(optname):
            return optname.lower()
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), _id1, string.lower)

    class C14nIterOptions_c14nSections(IterOptions):
        c14n_skv = ('NAME_C14N_CONFLICT', 'key', 'this_one')
        @staticmethod
        def secMapper(secname):
            return secname.lower()
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), string.lower)

    class C14nIterOptions(IterOptions):
        @staticmethod
        def factory(conftxt):
            return OrderedRawConf(StringIO(conftxt), parser_origin(), _id1, string.lower)

    unittest.main()
