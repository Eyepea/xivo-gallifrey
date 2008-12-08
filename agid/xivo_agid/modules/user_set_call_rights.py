# TODO: see the call_rights module.

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006, 2007, 2008  Proformatique

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

from xivo_agid import agid
from xivo_agid import objects
from xivo_agid import call_rights

def _user_set_call_rights(agi, cursor, args):
    userid = agi.get_variable('XIVO_USERID')
    dstnum = agi.get_variable('XIVO_DSTNUM')
    context = agi.get_variable('XIVO_CONTEXT')
    exten_pattern = agi.get_variable('XIVO_EXTENPATTERN')

    cursor.query("SELECT ${columns} FROM rightcallexten",
                 ('rightcallid', 'exten'))
    res = cursor.fetchall()

    if not res:
        call_rights.allow(agi)

    rightcallidset = set((row['rightcallid'] for row in res if call_rights.extension_matches(dstnum, row['exten'])))

    if not rightcallidset:
        call_rights.allow(agi)

    rightcallids = '(' + ','.join((str(el) for el in rightcallidset)) + ')'

    try:
        user = objects.User(agi, cursor, int(userid))
    except (ValueError, LookupError):
        call_rights.allow(agi)

    cursor.query("SELECT ${columns} FROM rightcall "
                 "INNER JOIN rightcallmember "
                 "ON rightcall.id = rightcallmember.rightcallid "
                 "WHERE rightcall.id IN " + rightcallids + " "
                 "AND rightcallmember.type = 'user' "
                 "AND rightcallmember.typeval = %s "
                 "AND rightcall.commented = 0",
                 (call_rights.RIGHTCALL_AUTHORIZATION_COLNAME, call_rights.RIGHTCALL_PASSWD_COLNAME),
                 (user.id,))
    res = cursor.fetchall()
    call_rights.apply_rules(agi, res)

    cursor.query("SELECT ${columns} FROM groupfeatures "
                 "INNER JOIN queuemember "
                 "ON groupfeatures.name = queuemember.queue_name "
                 "INNER JOIN queue "
                 "ON queue.name = queuemember.queue_name "
                 "WHERE groupfeatures.deleted = 0 "
                 "AND queuemember.userid = %s "
                 "AND queuemember.usertype = 'user' "
                 "AND queuemember.category = 'group' "
                 "AND queuemember.commented = 0 "
                 "AND queue.category = 'group' "
                 "AND queue.commented = 0",
                 ('groupfeatures.id',),
                 (user.id,))
    res = cursor.fetchall()

    if res:
        groupids = [row['groupfeatures.id'] for row in res]
        cursor.query("SELECT ${columns} FROM rightcall "
                     "INNER JOIN rightcallmember "
                     "ON rightcall.id = rightcallmember.rightcallid "
                     "WHERE rightcall.id IN " + rightcallids + " "
                     "AND rightcallmember.type = 'group' "
                     "AND rightcallmember.typeval IN (" + ", ".join(["%s"] * len(res)) + ") "
                     "AND rightcall.commented = 0",
                     (call_rights.RIGHTCALL_AUTHORIZATION_COLNAME, call_rights.RIGHTCALL_PASSWD_COLNAME),
                     groupids)
        res = cursor.fetchall()
        call_rights.apply_rules(agi, res)

    if exten_pattern:
        cursor.query("SELECT ${columns} FROM rightcall "
                     "INNER JOIN rightcallmember "
                     "ON rightcall.id = rightcallmember.rightcallid "
                     "INNER JOIN extenumbers "
                     "ON rightcallmember.typeval = extenumbers.typeval "
                     "WHERE rightcall.id IN " + rightcallids + " "
                     "AND rightcallmember.type = 'outcall' "
                     "AND extenumbers.exten = %s "
                     "AND extenumbers.context = %s "
                     "AND extenumbers.type = 'outcall' "
                     "AND rightcall.commented = 0",
                     (call_rights.RIGHTCALL_AUTHORIZATION_COLNAME, call_rights.RIGHTCALL_PASSWD_COLNAME),
                     (exten_pattern, context))
        res = cursor.fetchall()
        call_rights.apply_rules(agi, res)

    call_rights.allow(agi)

def user_set_call_rights(agi, cursor, args):
    try:
        _user_set_call_rights(agi, cursor, args)
    except call_rights.RuleAppliedException:
        return

agid.register(user_set_call_rights)
