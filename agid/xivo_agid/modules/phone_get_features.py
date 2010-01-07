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

def phone_get_features(agi, cursor, args):
    userid = agi.get_variable('XIVO_USERID')

    feature_list = objects.ExtenFeatures(agi, cursor)

    try:
        user = objects.User(agi, cursor, int(userid))
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    for fwd in feature_list.FEATURES['forwards']:
        fwdupper    = fwd[1].upper()
        enable      = 0
        dest        = ""

        if getattr(feature_list, fwd[0], 0) \
            and getattr(user, "enable%s" % fwd[1], 0):
            enable  = 1
            dest    = getattr(user, "dest%s" % fwd[1], "")

        agi.set_variable("XIVO_ENABLE%s" % fwdupper, enable)
        agi.set_variable("XIVO_DEST%s" % fwdupper, dest)

    for service in feature_list.FEATURES['services']:
        enable = bool(getattr(feature_list, service[0], 0) and getattr(user, service[1], 0))
        agi.set_variable("XIVO_%s" % service[1].upper(), int(enable))

agid.register(phone_get_features)
