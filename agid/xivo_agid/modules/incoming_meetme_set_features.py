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

import time

from xivo_agid import agid
from xivo_agid import objects

from optparse import OptionParser

def conf_authentication(agi, meetme, adminflag):
    agi.appexec('Answer')
    calleridnum = agi.get_variable('CALLERID(num)')

    rs = meetme.authenticate(calleridnum=calleridnum,
                             adminflag=adminflag)
    if rs:
        return rs

    retry = 0

    while retry < 3:
        agi.appexec('Read', "PIN|conf-getpin|%s" % meetme.pin_len_max())
        rs = meetme.authenticate(agi.get_variable('PIN'),
                                 calleridnum,
                                 adminflag)
        if rs:
            return rs

        retry += 1
        agi.appexec('Playback', 'conf-invalidpin')

    agi.appexec('Playback', 'vm-goodbye')

    return False

def conf_exceed_max_number(agi, confno, maxuser):
    if not maxuser or int(maxuser) < 1:
        return False

    agi.appexec('MeetMeCount',"%s|MEETMECOUNT" % confno)
    meetmecount = agi.get_variable('MEETMECOUNT')

    if not meetmecount.isdigit():
        return None

    return (int(meetmecount) >= int(maxuser))

def conf_is_open(starttime, durationm):
    if not starttime:
        return None
    elif starttime > time.time():
        return False
    elif durationm:
        return ((starttime + (int(durationm) * 60)) > time.time())
    else:
        return True

def _incoming_meetme_set_features_argv(agi, args):
    parser = OptionParser(usage="usage: %prog [options]")

    parser.add_option("-i",
                      dest      = 'xid',
                      type      = 'int',
                      default   = agi.get_variable('XIVO_DSTID'),
                      help      = "MeetMe id")

    parser.add_option("-n",
                      dest      = 'name',
                      default   = None,
                      help      = "MeetMe name")

    parser.add_option("-u",
                      dest      = 'number',
                      default   = None,
                      help      = "MeetMe number")

    parser.add_option("-c",
                      dest      = 'context',
                      default   = None,
                      help      = "MeetMe context")

    parser.add_option("-a",
                      action    = 'store_true',
                      dest      = 'adminflag',
                      default   = False,
                      help      = "User has admin access")

    parser.add_option("-t",
                      action    = 'store_false',
                      dest      = 'authent',
                      default   = True,
                      help      = "Don't need authentication")

    parser.add_option("-m",
                      action    = 'store_false',
                      dest      = 'maxuser',
                      default   = True,
                      help      = "Don't verify if max number of users exceeded")

    parser.add_option("-d",
                      action    = 'store_false',
                      dest      = 'openingdate',
                      default   = True,
                      help      = "Don't take into account the opening date")

    parser.parse_args(args=args)

    options, args = parser.parse_args()

    if args:
        parser.error("no argument is allowed - use option --help to get an help screen")

    return options

def incoming_meetme_set_features(agi, cursor, args):
    argv = _incoming_meetme_set_features_argv(agi, args)

    try:
        meetme = objects.MeetMe(agi,
                                cursor,
                                int(argv.xid),
                                argv.name,
                                argv.number,
                                argv.context)
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    if argv.openingdate and conf_is_open(meetme.starttime, meetme.durationm) is False:
        # TODO: Change sound by conf-closed
        agi.appexec('Playback', "conf-locked&vm-goodbye")
        agi.dp_break("Unable to join the conference room, it's not open "
                     "(start date: %r, current date: %s, duration minutes: %r, id: %s, name: %s, confno: %s)"
                     % (meetme.startdate,
                        time.strftime('%Y-%m-%d %H:%M:%S'),
                        meetme.durationm,
                        meetme.id,
                        meetme.name,
                        meetme.confno))

    if not argv.authent:
        flag = meetme.FLAG_USER
    else:
        flag = conf_authentication(agi, meetme, argv.adminflag)
        if not flag:
            agi.dp_break("Conference room authentication failed (id: %s, name: %s, confno: %s)"
                         % (meetme.id, meetme.name, meetme.confno))

    pin     = ''
    options = ''.join(meetme.get_global_options())

    if flag & meetme.FLAG_USER:
        pin     = meetme.pin
        options += ''.join(meetme.get_user_options())

        if argv.maxuser and conf_exceed_max_number(agi, meetme.confno, meetme.maxuser):
            # TODO: Change sound by conf-maxuserexceeded
            agi.appexec('Playback', "conf-locked&vm-goodbye")
            agi.dp_break("Unable to join the conference room, max number of users exceeded "
                         "(max number: %s, id: %s, name: %s, confno: %s)"
                         % (meetme.maxuser, meetme.id, meetme.name, meetme.confno))
    elif flag & meetme.FLAG_ADMIN:
        pin     = meetme.pinadmin
        options += ''.join(meetme.get_admin_options())
    else:
        agi.dp_break("Unknown MeetMe FLAG (flag: %r, id: %s, name: %s, confno: %s)"
                     % (flag, meetme.id, meetme.name, meetme.confno))

    if meetme.OPTIONS_COMMON['musiconhold'] in options:
        agi.set_variable('CHANNEL(musicclass)',
                         meetme.get_option_by_flag('musiconhold', flag))

    if meetme.OPTIONS_COMMON['enableexitcontext'] in options:
        exitcontext = meetme.get_option_by_flag('exitcontext', flag)
    else:
        exitcontext = ""

    if meetme.preprocess_subroutine:
        preprocess_subroutine = meetme.preprocess_subroutine
    else:
        preprocess_subroutine = ""

    agi.set_variable('MEETME_EXIT_CONTEXT', exitcontext)
    agi.set_variable('MEETME_RECORDINGFILE', "meetme-%s-%s" % (meetme.confno, int(time.time())))

    agi.set_variable('XIVO_REAL_NUMBER', meetme.number)
    agi.set_variable('XIVO_REAL_CONTEXT', meetme.context)
    agi.set_variable('XIVO_MEETMECONFNO', meetme.confno)
    agi.set_variable('XIVO_MEETMENAME', meetme.name)
    agi.set_variable('XIVO_MEETMENUMBER', meetme.number)
    agi.set_variable('XIVO_MEETMEPIN', pin)
    agi.set_variable('XIVO_MEETMEOPTIONS', options)
    agi.set_variable('XIVO_MEETMEPREPROCESS_SUBROUTINE', preprocess_subroutine)

agid.register(incoming_meetme_set_features)
