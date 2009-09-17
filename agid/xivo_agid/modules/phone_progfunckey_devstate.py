__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2009  Proformatique <technique@proformatique.com>

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

from xivo.xivo_helpers import fkey_extension

def phone_progfunckey_devstate(agi, cursor, args):
    userid  = agi.get_variable('XIVO_USERID')
    xlen    = len(args)

    if xlen < 2:
        agi.dp_break("Invalid number of arguments (args: %r)" % args)

    devstate = args[1]

    if devstate not in ('BUSY',
                        'INUSE',
                        'INVALID',
                        'NOT_INUSE',
                        'ONHOLD',
                        'RINGING',
                        'RINGINUSE',
                        'UNAVAILABLE',
                        'UNKNOWN'):
        agi.dp_break("Invalid device state: %r" % devstate)

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

    feature = args[0]

    if xlen > 2:
        dest = args[2]
    else:
        dest = ""

    try:
        extenfeatures   = objects.ExtenFeatures(agi, cursor)
        ppfkexten       = extenfeatures.get_exten_by_name('phoneprogfunckey')
    except LookupError, e:
        agi.verbose(str(e))
        return

    if feature not in extenfeatures.featureslist:
        agi.verbose("Invalid feature: %r" % feature)
        return

    try:
        featureexten = extenfeatures.get_exten_by_name(feature)
    except LookupError, e:
        agi.verbose(str(e))
        return

    xset = set()
    forwards = dict(extenfeatures.FEATURES['forwards'])

    if forwards.has_key(feature):
        try:
            xset.add(fkey_extension(ppfkexten,
                                    (user.id,
                                     featureexten,
                                     getattr(user, "dest%s" % forwards[feature], ""))))

            xset.add(fkey_extension(ppfkexten,
                                    (user.id,
                                     featureexten)))
        except ValueError, e:
            agi.verbose(str(e))

    xset.add(fkey_extension(ppfkexten,
                            (user.id,
                             featureexten,
                             dest)))

    for x in xset:
        agi.set_variable("DEVSTATE(Custom:%s)" % x, devstate)

agid.register(phone_progfunckey_devstate)
