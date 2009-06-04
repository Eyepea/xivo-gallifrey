__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>

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

def phonestatus(agi, cursor, args):
    userid = agi.get_variable('XIVO_USERID')

    feature_list = objects.FeatureList(agi, cursor)

    try:
        user = objects.User(agi, cursor, int(userid), feature_list)
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    enableunc   = ""
    destunc     = ""
    enablebusy  = ""
    destbusy    = ""
    enablerna   = ""
    destrna     = ""

    if feature_list.fwdunc:
        enableunc = user.enableunc

        if enableunc:
            destunc = user.destunc

    if feature_list.fwdbusy:
        enablebusy = user.enablebusy

        if enablebusy:
            destbusy = user.destbusy

    if feature_list.fwdrna:
        enablerna = user.enablerna

        if enablerna:
            destrna = user.destrna

    agi.set_variable('XIVO_ENABLEUNC', enableunc)
    agi.set_variable('XIVO_DESTUNC', destunc)
    agi.set_variable('XIVO_ENABLEBUSY', enablebusy)
    agi.set_variable('XIVO_DESTBUSY', destbusy)
    agi.set_variable('XIVO_ENABLERNA', enablerna)
    agi.set_variable('XIVO_DESTRNA', destrna)

    if user.vmbox:
        enablevoicemail = user.enablevoicemail
    else:
        enablevoicemail = ""

    if feature_list.incallfilter:
        incallfilter = user.callfilter
    else:
        incallfilter = ""

    if feature_list.incallrec:
        callrecord = user.callrecord
    else:
        callrecord = ""

    if feature_list.enablednd:
        enablednd = user.enablednd
    else:
        enablednd = ""

    agi.set_variable('XIVO_ENABLEVOICEMAIL', enablevoicemail)
    agi.set_variable('XIVO_INCALLFILTER', incallfilter)
    agi.set_variable('XIVO_CALLRECORD', callrecord)
    agi.set_variable('XIVO_ENABLEDND', enablednd)

agid.register(phonestatus)
