# XIVO Daemon
# Copyright (C) 2007, 2008  Proformatique
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Alternatively, XIVO Daemon is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XIVO Daemon
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, you will find one at
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

__version__ = '$Revision$ $Date$'

CMD_OTHER = 1 << 0
CMD_LOGIN = 1 << 1

class Command:
        def __init__(self, commandname, commandargs):
                self.name = commandname
                self.args = commandargs
                self.type = None

class BaseCommand:
        def __init__(self):
                pass

CommandClasses = {}
