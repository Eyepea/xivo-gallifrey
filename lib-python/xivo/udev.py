"""Interworking with udev

Copyright (C) 2008  Proformatique

NOTE: based on udev-105 internals (Debian Etch)
WARNING: Linux specific module - and maybe even Debian specific module
"""

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
import re
import time
import os.path
from itertools import count

from xivo import trace_null


PERSISTENT_NET_RULES_FILE = "/etc/udev/rules.d/z25_persistent-net.rules"

LOCKPATH_PREFIX = "/dev/.udev/.lock-"


def lockpath(rules_file):
    """
    Return the path of the directory to create in order to take a lock on
    @rules_file so that udev won't bother us while we are modifying it.
    """
    return LOCKPATH_PREFIX + os.path.basename(rules_file)


def lock_rules_file(rules_file):
    """
    Take a lock on @rules_file as udev does.
    On errors, this function retries to grab the lock for as much as 30 times,
    with pauses of 1 second between successive attempts.
    If the lock could not be grabbed at all, the last exception is re-raised.
    """
    lpath = lockpath(rules_file)
    for x in count():
        try:
            os.mkdir(lpath)
        except OSError:
            if x == 29:
                raise
            time.sleep(1)
        else:
            return


def unlock_rules_file(rules_file):
    """
    Unlock a lock previously taken with lock_rules_file()
    The user should _not_ try to unlock a lock that has not been successfully
    taken with lock_rules_file(), or unlock it twice, etc.
    """
    lpath = lockpath(rules_file)
    os.rmdir(lpath)


def is_comment(mline):
    """
    Return True if @mline is a comment, else False.
    """
    return mline[:1] == "#"


def iter_multilines(lines):
    """
    Iterate over @lines and yield the multilines they form.  A multiline stops
    only on a "\\n" (newline) that is not preceded with a "\\\\" (backslash) or at
    end of file.  "\\\\\\n" of continued lines are stripped, as well as regular
    "\\n" at end of multilines and spaces at their beginning.  Comment lines and
    blank lines are not stripped.
    """
    current = []
    for line in lines:
        if line[-2:] == "\\\n":
            current.append(line[:-2])
        else:
            if line[-1:] == "\n":
                current.append(line[:-1])
            else:
                current.append(line)
            yield ''.join(current).lstrip()
            current = []
    if current:
        yield ''.join(current).lstrip()


RuleKeyMatcher = re.compile(r'[\s,]*(.+?)\s*(==|!=|\+=|=|:=)\s*"(.*?)"(.*)$').match


OP_ALL = ('==', '!=', '+=', '=', ':=')
OP_MATCH_NOMATCH = ('==', '!=')


KEY_ATTR = {
    # key in file    # key in dict  # allowed operations
    'ATTR' :        ('ATTR',        OP_ALL, ),
    'ATTRS' :       ('ATTRS',       OP_ALL, ),
    'SYSFS' :       ('ATTRS',       OP_ALL, ),
    'ENV':          ('ENV',         OP_ALL, ),
}


KEY_OPT_ATTR = ('IMPORT', 'NAME')


KEY = {
    # key in file    # key in dict  # allowed operations
    'ACTION':       ('ACTION',      OP_MATCH_NOMATCH, ),
    'DEVPATH':      ('DEVPATH',     OP_MATCH_NOMATCH, ),
    'KERNEL':       ('KERNEL',      OP_MATCH_NOMATCH, ),
    'SUBSYSTEM':    ('SUBSYSTEM',   OP_MATCH_NOMATCH, ),
    'DRIVER':       ('DRIVER',      OP_MATCH_NOMATCH, ),
    'KERNELS':      ('KERNELS',     OP_MATCH_NOMATCH, ),
    'ID':           ('KERNELS',     OP_MATCH_NOMATCH, ),
    'SUBSYSTEMS':   ('SUBSYSTEMS',  OP_MATCH_NOMATCH, ),
    'BUS':          ('SUBSYSTEMS',  OP_MATCH_NOMATCH, ),
    'DRIVERS':      ('DRIVERS',     OP_MATCH_NOMATCH, ),
    'PROGRAM':      ('PROGRAM',     OP_ALL, ),
    'RESULT':       ('RESULT',      OP_MATCH_NOMATCH, ),
    'IMPORT':       ('IMPORT',      OP_ALL, ),
    'RUN':          ('RUN',         OP_ALL, ),
    'WAIT_FOR_SYSFS': ('WAIT_FOR_SYSFS', OP_ALL, ),
    'LABEL':        ('LABEL',       OP_ALL, ),
    'GOTO':         ('GOTO',        OP_ALL, ),
    'NAME':         ('NAME',        OP_ALL, ),
    'SYMLINK':      ('SYMLINK',     OP_ALL, ),
    'OWNER':        ('OWNER',       OP_ALL, ),
    'GROUP':        ('GROUP',       OP_ALL, ),
    'MODE':         ('MODE',        OP_ALL, ),
    'OPTIONS':      ('OPTIONS',     OP_ALL, ),
}


def parse_rule(mline, trace=trace_null):
    """
    Parse @mline quite like add_to_rules() does in udev.
    
    If the rule is syntaxically invalid, this function returns None.
    If the rule is not too much syntaxically invalid, this function
    returns a dictionary with the following structure:
    
    { key_attr_1: { attr_1: [op_1, val_1],
                    attr_2: [op_2, val_2],
                    ... },
      ...,
      key_3: [op_3, val_3],
      ... }
    
    where key_attr_{n} is an element of the "key in dict" column of KEY_ATTR,
    attr_{p} can be any string, op_{q} is an udev rule operator in
    ('==', '!=', '+=', '=', ':='), val_{r} can be any string, and key_{s} is an
    element of the "key in dict" column of KEY.
    
    NOTE: the "key in dict" columns are a subset of the keys of KEY or KEY_ATTR
    and the sets are disjoint.
    """
    rule = {}
    
    # rotl: rest of the line
    rotl = mline
    
    while True:
        matchkey = RuleKeyMatcher(rotl)
        if not matchkey:
            break
        key, op, val, rotl = matchkey.group(1, 2, 3, 4)
        
        # key with attribute?
        open_pos = key.find("{")
        if open_pos > 0:
            open_pos += 1
            close_pos = key.find("}", open_pos)
            if close_pos <= 0:
                trace.err("parse_rule: unclosed attribute in %r" % key)
                return None
            
            attr = key[open_pos:close_pos]
            lkey = key[:open_pos - 1]
            if lkey in KEY_ATTR:
                rule_key, rule_allowed_ops = KEY_ATTR[lkey]
                if op not in rule_allowed_ops:
                    trace.err("parse_rule: invalid rule multiline %r (invalid operation %r for key %r)" % (mline, op, key))
                    return None
                rule.setdefault(rule_key, {})
                rule[rule_key][attr] = [op, val]
                continue
            elif lkey in KEY_OPT_ATTR:
                key = lkey
                # behave as if there was no attribute;
                # don't skip simple key handling
            else:
                trace.warning("parse_rule: unknown key %r" % key)
                continue
        
        # Simple key 
        if key in KEY:
            rule_key, rule_allowed_ops = KEY[key]
            if op not in rule_allowed_ops:
                trace.err("parse_rule: invalid rule multiline %r (invalid operation %r for key %r)" % (mline, op, key))
                return None
            rule[rule_key] = [op, val]
        else:
            trace.warning("parse_rule: unknown key %r" % key)
    
    valid = len(rule) >= 2 or (len(rule) == 1 and "NAME" not in rule)
    if not valid:
        trace.err("parse_rule: invalid rule multiline %r" % mline)
        return None
    
    return rule


def parse_lines(lines, trace=trace_null):
    """
    @lines is a sequence of lines that comes from a udev rules file.
    This function parses the rules, taking into account continued lines, blank
    lines and comment lines.  It returns a list a rules, in which each rule is
    a dictionary formatted as described in the documentation of parse_rule().
    """
    rules = []
    for mline in iter_multilines(lines):
        if (not mline) or is_comment(mline):
            continue
        rule = parse_rule(mline, trace)
        if not rule:
            continue
        rules.append(rule)
    return rules    


def parse_file(rules_file, trace=trace_null):
    """
    Lock @rules_file, parse it with parse_lines(), and unlock it.
    """
    lock_rules_file(rules_file) # RW lock, anybody? :)
    try:
        return parse_lines(file(rules_file), trace)
    finally:
        unlock_rules_file(rules_file)
