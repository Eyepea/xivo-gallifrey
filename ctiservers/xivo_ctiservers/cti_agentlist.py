# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

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

from xivo_ctiservers.cti_anylist import AnyList

class AgentList(AnyList):
        def __init__(self, newurls = []):
                AnyList.__init__(self, newurls)
                self.agentlist = {}
                return
        
        def update(self):
                lstadd = []
                lstdel = []
                oldagentlist = self.agentlist
                newagentlist = {}
                for url, urllist in self.requested_list.iteritems():
                        gl = urllist.getlist(1, 4, True)
                        if gl == 2:
                                newagentlist.update(self.commandclass.getagentslist_json(urllist.jsonreply))
                for a, b in newagentlist.iteritems():
                        if a not in oldagentlist:
                                self.agentlist[a] = b
                                lstadd.append(a)
                for a, b in oldagentlist.iteritems():
                        if a not in newagentlist:
                                lstdel.append(a)
                for a in lstdel:
                        del self.agentlist[a]
                return { 'add' : lstadd,
                         'del' : lstdel }
        
        def findagent(self, agentname):
                if agentname in self.agentlist:
                        return self.agentlist.get(agentname)
                else:
                        return None
