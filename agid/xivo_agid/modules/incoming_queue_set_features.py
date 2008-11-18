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

def incoming_queue_set_features(agi, cursor, args):
    queueid = agi.get_variable('XIVO_DSTID')
    referer = agi.get_variable('XIVO_FWD_REFERER')

    try:
        queue = objects.Queue(agi, cursor, xid=int(queueid))
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    options = ""

    if queue.data_quality:
        options += "d"

    if queue.hitting_callee:
        options += "h"

    if queue.hitting_caller:
        options += "H"

    if queue.retries:
        options += "n"

    if queue.ring:
        options += "r"

    if queue.transfer_user:
        options += "t"

    if queue.transfer_call:
        options += "T"

    if queue.write_caller:
        options += "w"

    if queue.write_calling:
        options += "W"

    agi.set_variable('XIVO_REAL_CONTEXT', queue.context)
    agi.set_variable('XIVO_QUEUENAME', queue.name)
    agi.set_variable('XIVO_QUEUEOPTIONS', options)
    agi.set_variable('XIVO_QUEUEURL', queue.url)
    agi.set_variable('XIVO_QUEUEANNOUNCEOVERRIDE', queue.announceoverride)

    if queue.preprocess_subroutine:
        agi.set_variable('XIVO_QUEUEPREPROCESS_SUBROUTINE', queue.preprocess_subroutine)

    if queue.timeout:
        agi.set_variable('XIVO_QUEUETIMEOUT', queue.timeout)

    queue.set_dial_actions()

    if referer == ("queue:%s" % queue.id):
        queue.set_caller_id()

agid.register(incoming_queue_set_features)
