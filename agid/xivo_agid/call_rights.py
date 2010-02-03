# TODO: handle rules priority.
# TODO: merge into objects and adapt modules.

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>

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

import re

RIGHTCALL_AUTHORIZATION_COLNAME = "rightcall.authorization"
RIGHTCALL_PASSWD_COLNAME = "rightcall.passwd"

rep = (('_', ''),
       ('*', '\*'),
       ('X', '[0-9]'),
       ('Z', '[1-9]'),
       ('N', '[2-9]'),
       ('.', '[0-9#\*]+'),
       ('!', '[0-9#\*]*'))


class RuleAppliedException(Exception):
    pass


def allow(agi):
    agi.set_variable('XIVO_AUTHORIZATION', "ALLOW")
    raise RuleAppliedException()


def deny(agi, password):
    if password:
        agi.set_variable('XIVO_PASSWORD', password)

    agi.set_variable('XIVO_AUTHORIZATION', "DENY")
    raise RuleAppliedException()


def extension_matches(number, pattern):
    for (key, val) in rep:
        pattern = pattern.replace(key, val)

    return bool(re.match("^%s$" % pattern, number))


def apply_rules(agi, rules):
    if not rules:
        return

    for rule in rules:
        if rule[RIGHTCALL_AUTHORIZATION_COLNAME]:
            allow(agi)

    deny(agi, rule[RIGHTCALL_PASSWD_COLNAME])
