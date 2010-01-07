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

from xivo_agid import agid
from xivo_agid import objects

def phone_set_feature(agi, cursor, args):
    userid  = agi.get_variable('XIVO_USERID')
    xlen    = len(args)

    if xlen == 0:
        agi.dp_break("Invalid number of arguments")

    feature = args[0]

    if feature in ("vm", "dnd", "callrecord", "incallfilter"):
        if xlen > 1 and args[1] != '':
            try:
                if xlen == 2:
                    context = objects.User(agi, cursor, xid=int(userid)).context
                else:
                    context = args[2]

                user = objects.User(agi, cursor, exten=args[1], context=context)
            except (ValueError, LookupError), e:
                agi.dp_break(str(e))
        else:
            try:
                userid = int(userid)
                user = objects.User(agi, cursor, userid)
            except (ValueError, LookupError), e:
                agi.dp_break(str(e))

        if feature == "vm" and user.id != userid:
            if user.vmbox:
                passwd = user.vmbox.password
            else:
                try:
                    passwd = objects.VMBox(agi, cursor, int(user.voicemailid), commentcond=False).password
                except (ValueError, LookupError), e:
                    agi.dp_break(str(e))

            if passwd != '':
                agi.appexec('Authenticate', passwd)

        try:
            user.toggle_feature(feature)
        except objects.DBUpdateException, e:
            agi.verbose(str(e))

        if feature == "vm":
            agi.set_variable('XIVO_VMENABLED', user.enablevoicemail)
        elif feature == "dnd":
            agi.set_variable('XIVO_DNDENABLED', user.enablednd)
        elif feature == "callrecord":
            agi.set_variable('XIVO_CALLRECORDENABLED', user.callrecord)
        elif feature == "incallfilter":
            agi.set_variable('XIVO_INCALLFILTERENABLED', user.incallfilter)
        agi.set_variable('XIVO_USERID_OWNER', user.id)
    elif feature in ("unc", "rna", "busy"):
        if xlen < 2:
            agi.dp_break("Invalid number of arguments for %s" % feature)

        enabled = int(args[1])

        if xlen > 2:
            arg = args[2]
        else:
            arg = None

        if xlen > 3 and args[3] != '':
            try:
                if xlen == 4:
                    context = objects.User(agi, cursor, xid=int(userid)).context
                else:
                    context = args[4]

                user = objects.User(agi, cursor, exten=args[3], context=context)
            except (ValueError, LookupError), e:
                agi.dp_break(str(e))
        else:
            try:
                user = objects.User(agi, cursor, int(userid))
            except (ValueError, LookupError), e:
                agi.dp_break(str(e))

        try:
            user.set_feature(feature, enabled, arg)
        except objects.DBUpdateException, e:
            agi.verbose(str(e))

        agi.set_variable("XIVO_%sENABLED" % feature.upper(), getattr(user, "enable%s" % feature))
        agi.set_variable('XIVO_USERID_OWNER', user.id)
    elif feature == "bsfilter":
        try:
            user = objects.User(agi, cursor, int(userid))
        except (ValueError, LookupError), e:
            agi.dp_break(str(e))

        if xlen < 2:
            agi.dp_break("Invalid number of arguments for bsfilter")

        try:
            num1, num2 = args[1].split('*')
            if user.number not in (num1, num2):
                raise ValueError("Invalid number")
        except ValueError:
            agi.dp_break("Invalid number")

        bsf = None
        secretary = None

        # Both the boss and secretary numbers are passed, so select the one
        if user.number == num1:
            number = num2
        else:
            number = num1

        if user.bsfilter == "secretary":
            try:
                bsf = objects.BossSecretaryFilter(agi, cursor, number, user.context)
                secretary_number = user.number
            except LookupError:
                pass
        elif user.bsfilter == "boss":
            bsf = user.filter
            secretary_number = number

        if bsf:
            bsf.set_dial_actions()
            secretary = bsf.get_secretary_by_number(secretary_number)

        if not secretary:
            agi.dp_break("Unable to find boss-secretary filter")

        agi.verbose("Filter exists! (Caller: %r, secretary number: %r)" % (user.bsfilter, secretary_number))
        cursor.query("SELECT ${columns} FROM callfiltermember "
                     "WHERE callfilterid = %s "
                     "AND type = %s "
                     "AND typeval = %s "
                     "AND bstype = %s",
                     ('active',),
                     (bsf.id, "user", secretary.id, "secretary"))
        res = cursor.fetchone()

        if not res:
            agi.dp_break("Unable to find secretary (id = %d)" % secretary.id)

        new_state = int(not res['active'])
        cursor.query("UPDATE callfiltermember "
                     "SET active = %s "
                     "WHERE callfilterid = %s "
                     "AND type = %s "
                     "AND typeval = %s "
                     "AND bstype = %s",
                     parameters = (new_state, bsf.id, "user", secretary.id, "secretary"))

        if cursor.rowcount != 1:
            agi.dp_break("Unable to perform the requested update")

        agi.set_variable('XIVO_BSFILTERENABLED', new_state)
    else:
        agi.dp_break("Unknown feature %r" % feature)

agid.register(phone_set_feature)
