#!/usr/bin/env python2
# -*- coding: UTF-8 -*-

"""tzdataexport is a tool to export current UTC offsets and daylight-saving
   rules from the tz database's source files. Those are the same files used by
   the time zone compiler (zic) to create tz binary files. The format of these
   files are documented in the zic man pages.

   The default text output format is made of 3-fields lines. The fields are,
   in order, the timezone identifier, the offset from UTC in seconds, and the
   daylight-saving rules. Each fields are space delimited.
   
   The tool doesn't go great length in input file checking. If you pass it
   garbage, you might as well receive garbage (i.e. meaningless exception).
   
   More info about the tz database can be found here:
   - http://www.twinsun.com/tz/tz-link.htm
"""

__version__ = "$Revision: 8668 $ $Date: 2010-07-21 10:39:22 -0400 (Wed, 21 Jul 2010) $"
__license__ = """
    Copyright (c) 2010  Proformatique <technique@proformatique.com>

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
import logging
import re
import string

logger = logging.getLogger('tzdataexport')
logger.addHandler(logging.StreamHandler())
logger.setLevel(logging.ERROR)

_utcnow = datetime.datetime.utcnow


def is_abbrv_of(abbrv, word):
    """Return True if abbrv is an abbreviation of word.
    
    >>> is_abbrv_of('min', 'minimum')
    True
    >>> is_abbrv_of('minus', 'minimum')
    False
    """
    return word.startswith(abbrv)
        

def is_unambiguous_abbrv_of(abbrv, word, context):
    """Return True if abbrv is the only abbreviation of word in context context.
    
    >>> is_unambiguous_abbrv_of('mi', 'min', ['min', 'max'])
    True
    >>> is_unambiguous_abbrv_of('m', 'min', ['min', 'max'])
    False
    """
    if not is_abbrv_of(abbrv, word):
        return False
    for cword in context:
        if is_abbrv_of(abbrv, cword) and cword != word:
            return False
    return True


def find_word_from_abbrv(abbrv, context, ret_idx=False):
    """Return the word from context for who's abbrv is an unambiguous abbreviation in
    the current context. Return None if there is no such word.
    
    >>> find_word_from_abbrv('ma', ['min', 'max'])
    'max'
    >>> find_word_from_abbrv('ma', ['min', 'max'], True)
    1
    >>> find_word_from_abbrv('m', ['min', 'max'])
    >>>
    """
    for idx, word in enumerate(context):
        if is_unambiguous_abbrv_of(abbrv, word, context):
            if ret_idx:
                return idx
            else:
                return word


def _is_amount_of_time(str, accept_neg=False):
    m = re.match(r'^(-)?(?:\d|1\d|2[0-4])(?::[0-5]\d){0,2}$', str)
    if m:
        if not accept_neg and m.group(1):
            return False
        else:
            return True
    else:
        return False


def _amount_of_time_to_seconds(str):
    if str.startswith('-'):
        sign = -1
        str = str[1:]
    else:
        sign = 1
    res = 0
    for value, mult_factor in zip(str.split(':'), [3600, 60, 1]):
        res += int(value) * mult_factor
    return res * sign


class RuleLine(object):
    """Represent a rule line from a tz source file.
    
    Each instance of this class has three attributes:
    - name   : the name of the rule
    - from_  : the UTC offset of the zone, in seconds
    - rules  : the name of the rule that currently apply in the zone, or None
               if the standard time always applies
    """
    
    _FROMFIELD_CONTEXT = ('minimum', 'maximum', 'only')
    _TOFIELD_CONTEXT = ('minimum', 'maximum')
    _INFIELD_CONTEXT = ('january', 'february', 'march', 'april', 'may', 'june', 'july',
                        'august', 'september', 'october', 'november', 'december')
    _WEEKDAY_CONTEXT = ('sunday', 'monday', 'tuesday', 'wednesday', 'thursday',
                        'friday', 'saturday')
    
    def __init__(self, name, from_, to, type, in_, on, at, save):
        self.name = name
        self.from_ = self._fromfield_to_year(from_.lower())
        self.to = self._tofield_to_year(to.lower(), self.from_)
        self.type = type
        self.in_ = self._infield_to_month(in_.lower())
        self.on = self._onfield_to_day(on)
        self.at, self.at_time_type = self._atfield_to_seconds_and_type(at)
        self.save = self._savefield_to_seconds(save)
    
    @classmethod
    def _fromfield_to_year(cls, fromfield):
        if is_unambiguous_abbrv_of(fromfield, 'minimum', cls._FROMFIELD_CONTEXT):
            return 0   # Arbitrary 'small' year value
        elif is_unambiguous_abbrv_of(fromfield, 'maximum', cls._FROMFIELD_CONTEXT):
            return 20000   # Arbitrary 'large' year value
        else:
            return int(fromfield)
    
    @classmethod
    def _tofield_to_year(cls, tofield, fromval):
        if is_unambiguous_abbrv_of(tofield, 'minimum', cls._TOFIELD_CONTEXT):
            return 0   # Arbitrary 'small' year value
        elif is_unambiguous_abbrv_of(tofield, 'maximum', cls._TOFIELD_CONTEXT):
            return 20000   # Arbitrary 'large' year value
        elif is_unambiguous_abbrv_of(tofield, 'only', cls._TOFIELD_CONTEXT):
            return fromval
        else:
            return int(tofield)
    
    @classmethod
    def _infield_to_month(cls, infield):
        for i, month_name in enumerate(cls._INFIELD_CONTEXT):
            if is_unambiguous_abbrv_of(infield, month_name, cls._INFIELD_CONTEXT):
                return i + 1
        else:
            raise ValueError("'%s' is not an unambiguous month name abbreviation" % infield)
    
    @classmethod
    def _atfield_to_seconds_and_type(cls, atfield):
        last_char = atfield[-1]
        if last_char.isalpha():
            if last_char in 'ws':
                time_type = last_char
            elif last_char in 'ugz':
                time_type = 'z'
            else:
                raise ValueError("'%s' is not a valid 'AT' field value" % atfield)
            atfield = atfield[:-1]
        else:
            time_type = 'w'
            
        if not _is_amount_of_time(atfield):
            raise ValueError("'%s' is not a valid 'AT' field value" % atfield)
        return _amount_of_time_to_seconds(atfield), time_type
    
    @classmethod
    def _savefield_to_seconds(cls, savefield):
        if savefield == '-':
            return 0
        if not _is_amount_of_time(savefield):
            raise ValueError("'%s' is not a valid 'SAVE' field value" % savefield)
        return _amount_of_time_to_seconds(savefield)
        
    @classmethod
    def _fixed_day_to_str(cls, day):
        return 'D%s' % day
    
    @classmethod
    def _variable_day_to_str(cls, week, weekday):
        return 'W%s.%s' % (week, weekday)
    
    @classmethod
    def _onfield_to_day(cls, onfield):
        if onfield.isdigit():
            return cls._fixed_day_to_str(onfield)
        
        onfield = onfield.lower()
        if onfield.isalpha():
            if not onfield.startswith('last'):
                raise ValueError("'%s' is not a valid 'ON' field value" % onfield)
            weekday_idx = find_word_from_abbrv(onfield[4:], cls._WEEKDAY_CONTEXT, True)
            if weekday_idx is None:
                raise ValueError("'%s' is not a valid 'ON' field value (ambiguous)" % onfield)
            return cls._variable_day_to_str(5, weekday_idx + 1)
        
        mobj = re.match(r'^([a-z]+)([<>]=?)(\d{1,2})$', onfield, re.I) 
        if mobj is None:
            raise ValueError("'%s' is not a valid 'ON' field value" % onfield)
        weekday, op, num = mobj.groups()
        weekday_idx = find_word_from_abbrv(weekday, cls._WEEKDAY_CONTEXT, True)
        if weekday_idx is None:
            raise ValueError("'%s' is not a valid 'ON' field value (ambiguous)" % onfield)
        num = int(num)
        if op[0] == '>':
            if op[1:] == '':
                num += 1
            q, r = divmod(num - 1, 7)
            if r != 0:
                logger.info("'%s' is not a well supported 'ON' field value (%d %% 7 != 0)" % (onfield, num))
            weeknum = q + 1
            return cls._variable_day_to_str(weeknum, weekday_idx + 1)
        else:
            assert op[0] == '<'
            # XXX As of 2010, no Rule use this facility, so I didn't took the time
            # to implement it. Also, most phones doesn't have good support for this kind
            # of DST rule
            logger.info("'%s' is not a well supported 'ON' field value (use of <)" % onfield)
            return cls._variable_day_to_str(5, weekday_idx + 1)
        
    def will_occur_on_year(self, tyear=None):
        if tyear is None:
            tyear = _utcnow().year
        return self.from_ <= tyear <= self.to
    
    def is_dst_start(self):
        return self.save != 0
    
    def is_dst_end(self):
        return not self.is_dst_start()
    
    def as_rule(self):
        return {'month': self.in_, 'day': self.on, 'time': self.at}


class RuleSet(object):
    """Represent a set of rules with an identical name."""
    
    def __init__(self, name):
        self.name = name
        self._rules = []
    
    def add_rule(self, rule):
        if rule.name != self.name:
            raise ValueError("Rule name ('%s') is different from rule set name ('%s')" % (rule.name, self.name))
        self._rules.append(rule)
    
    def extract_dst_rules(self, utcoffset=0):
        """Return a 'wall-clock time' DST rules dictionary from this rule set.
        
        Return None if the rule set contains no currently applicable DST rules.
        """
        # TODO take utcoffset into account
        dst_start_rules = []
        dst_end_rules = []
        for rule in self._rules:
            if rule.will_occur_on_year():
                if rule.is_dst_start():
                    dst_start_rules.append(rule)
                elif rule.is_dst_end():
                    dst_end_rules.append(rule)
                else:
                    logger.info('We found a rule which is neither a start nor end DST rule')
        
        if not dst_start_rules or not dst_end_rules:
            logger.debug("The '%s' rule set doesn't have currently applicable DST rules" % self.name)
            return None
        if len(dst_start_rules) > 1 or len(dst_end_rules) > 1:
            raise NotImplementedError("The '%s' rule set has too much DST information (%d start, %d end)"
                                      % (self.name, len(dst_start_rules), len(dst_end_rules)))
        return {'start': dst_start_rules[0].as_rule(),
                'end': dst_end_rules[0].as_rule(),
                'save': dst_start_rules[0].save}


class ZoneLine(object):
    """Represent a zone line from a tz source file.
    
    Each instance of this class has three attributes:
    - name   : the name of the zone
    - gmtoff : the UTC offset of the zone, in seconds
    - rules  : the name of the rule that currently apply in the zone, or None
               if the standard time always applies
    """
    
    def __init__(self, name, gmtoff, rules_save):
        self.name = name
        self.gmtoff = self._gmtofffield_to_seconds(gmtoff)
        if rules_save == '-':
            self.rules = None
        elif _is_amount_of_time(rules_save, accept_neg=True):
            self.rules = None
            self.gmtoff += _amount_of_time_to_seconds(rules_save)
        else:
            self.rules = rules_save
    
    @classmethod
    def _gmtofffield_to_seconds(cls, gmtofffield):
        if not _is_amount_of_time(gmtofffield, accept_neg=True):
            raise ValueError("'%s' is not a valid 'GMTOFF' field value" % gmtofffield)
        return _amount_of_time_to_seconds(gmtofffield)
    

class LinkLine(object):
    """Represent a link line from a tz source file.
    
    Each instance of this class has two attributes:
    - link_from : the name of the existing timezone 
    - link_to   : the name of the new (alias) timezone
    """
    
    def __init__(self, link_from, link_to):
        self.link_from = link_from
        self.link_to = link_to
        

def _is_blank_line(line):
    """Returns True is the line is considered as a blank lines, else false.
    
    >>> _is_blank_line('')
    True
    >>> _is_blank_line('# This is a comment')
    True
    >>> _is_blank_line('Rule US ...')
    False
    """
    stripped_line = line.lstrip(string.whitespace)
    return not stripped_line or stripped_line[0] == '#'


def _strip_line(line):
    """Strip the line from terminal whitespace and comment.
    """
    idx = line.find('#')
    if idx != -1:
        line = line[:idx]
    return line.rstrip()


def _strip_iter(seq):
    return itertools.imap(lambda x: _strip_line(x), seq)


def _parse_tz_source_file(lines):
    """Parse a tz source file.
    
    Returns a tuple containing:
    - a dictionary of 'rule name' -> RuleSet object
    - a list of ZoneLine object
    - a list of LinkLine object
    """
    rulesets = {}
    zones = []
    links = []
    
    it = _strip_iter(lines)
    try:
        while True:
            cur_line = it.next()
            if _is_blank_line(cur_line):
                continue
            
            tokens = cur_line.split()
            if tokens[0] == 'Rule':
                logger.debug('Adding rule \'%s\' to rulesets', cur_line)
                rule_name = tokens[1]
                ruleset = rulesets.setdefault(rule_name, RuleSet(rule_name))
                ruleset.add_rule(RuleLine(*tokens[1:9]))
            elif tokens[0] == 'Zone':
                zone_name = tokens[1]
                tokens = ' '.join(tokens[2:]).split(None, 3)
                while len(tokens) > 3:
                    cur_line = it.next()
                    if not cur_line:
                        # Line, after striping, is empty - go to next line
                        continue
                    tokens = cur_line.split(None, 3)
                logger.debug('Adding zone \'%s\' to zones list', zone_name)
                zones.append(ZoneLine(zone_name, tokens[0], tokens[1]))
            elif tokens[0] == 'Link':
                logger.debug('Adding link \'%s\' to links list', cur_line)
                links.append(LinkLine(*tokens[1:]))
            else:
                err_msg = 'Invalid first token \'%s\' in line \'%s\'' % (tokens[0], cur_line)
                logger.error(err_msg)
                raise ValueError(err_msg)
    except StopIteration:
        pass
    return rulesets, zones, links


def _create_model_from_info(rulesets, zones, links):
    res = {}
    for zone in zones:
        if zone.rules is None:
            res[zone.name] = {'name': zone.name, 'utcoffset': zone.gmtoff, 'dst': None}
        else:
            ruleset = rulesets[zone.rules]
            try:
                res[zone.name] = {'name': zone.name, 'utcoffset': zone.gmtoff, 'dst': ruleset.extract_dst_rules(zone.gmtoff)}
            except ValueError:
                logger.exception('Probably an invalid zone - ignoring')
    for link in links:
        if link.link_from not in res:
            logger.warning("Can't create link from '%s' to '%s' - original zone doesn't exit", link.link_from, link.link_to)
        else:
            res[link.link_to] = res[link.link_from]
    return res


def create_model_from_files(filenames):
    """Read multiple tz source files and return an UTC offset/DST rules model.
    """
    rulesets = {}
    zones = []
    links = []
    for filename in filenames:
        fobj = open(filename)
        try:
            cruleset, czone, clinks = _parse_tz_source_file(fobj)
        finally:
            fobj.close()
        rulesets.update(cruleset)
        zones.extend(czone)
        links.extend(clinks)
    return _create_model_from_info(rulesets, zones, links)


def export_model_to_text_file(fobj, model):
    for zone_name in sorted(model.iterkeys()):
        zone_info = model[zone_name]
        if zone_info['dst'] is None:
            print >>fobj, '%-34s%10s    -' % (zone_name, zone_info['utcoffset'])
        else:
            print >>fobj, '%-34s%10s    %s' % (zone_name, zone_info['utcoffset'], _format_dst(zone_info['dst']))


def _format_dst(dst_dict):
    return '%s;%s;%d' % (_format_dst_change(dst_dict['start']), _format_dst_change(dst_dict['end']), dst_dict['save'])


def _format_dst_change(change):
    return '%(month)s/%(day)s/%(time)s' % change


if __name__ == '__main__':
    import optparse
    import os.path
    import sys
    import tarfile
    import tempfile
    import time
    
    STD_SOURCE_FILES = [
        "africa",
        "antarctica",
        "asia",
        "australasia",
        "backward",
        "etcetera",
        "europe",
        "northamerica",
        "pacificnew",
        "southamerica",
    ]
    
    p = optparse.OptionParser(usage='%prog [options] FILE...')
    p.add_option('-o', action='store', dest='outfile', metavar='OFILE', help='write the result to OFILE')
    p.add_option('-s', action='store_true', dest='std', default=False, help='use the standard tz source files name (africa, asia, ...) as FILEs')
    p.add_option('-t', action='store', dest='tarfile', metavar='TFILE', help='look for FILEs into the [gz|bz2] tarfile TFILE')
    p.add_option('-v', action='count', dest='verbose', help='Verbose mode.')
    opt, args = p.parse_args()
    
    if opt.verbose == 1:
        logger.setLevel(logging.INFO)
    elif opt.verbose > 1:
        logger.setLevel(logging.DEBUG)

    if opt.std:
        args.extend(STD_SOURCE_FILES)
    
    if opt.tarfile is not None:
        if not tarfile.is_tarfile(opt.tarfile):
            logger.error('error: file "%s" is not a valid tarfile' % opt.tarfile)
            sys.exit(1)
        tfile = tarfile.open(opt.tarfile, 'r')
        tempdir = tempfile.mkdtemp()
        tfile.extractall(tempdir)
        tfile.close()
        args = [os.path.join(tempdir, file) for file in args]
    
    if not args:
        logger.error('error: tool takes at least one argument')
        sys.exit(1)
    for file in args:
        if not os.path.isfile(file):
            logger.error('error: "%s" is not a file' % file)
            sys.exit(1)
    
    if opt.outfile is None:
        out = sys.stdout
    else:
        out = open(opt.outfile, 'w')
    
    try:
        model = create_model_from_files(args)
        out.write('# This file was automatically generated on %s by the tzdataexport tool\n' % time.strftime('%Y-%m-%d'))
        export_model_to_text_file(out, model)
    finally:
        if out is not sys.stdout:
            out.close()