# vim: set fileencoding=utf-8 :
# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XiVO CTI Server is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XiVO CTI Server
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import logging
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('incomingcalllist')

class IncomingCallList(AnyList):
    def __init__(self, newurls = [], useless = None):
        self.anylist_properties = {
            'keywords' : ['exten', 'context', 'destidentity', 'action',
                          'userfirstname', 'userlastname',
                          'usernumber', 'username',
                          'usercontext',
                          'groupcontext',
                          'queuecontext',
                          'meetmecontext',
                          'voicemenucontext',
                          'voicemailcontext'],
            'name' : 'incomingcall',
            'action' : 'getincomingcalllist',
            'urloptions' : (1, 5, True) }
        AnyList.__init__(self, newurls)
        return
    
    # [{"id":"2","preprocess_subroutine":null,"faxdetectenable":"0","faxdetecttimeout":"4","faxdetectemail":"","commented":false,"linked":true,"actionarg1":"3","actionarg2":"","groupname":null,"groupnumber":null,"groupcontext":null,"queuename":null,"queuenumber":null,"queuecontext":null,"meetmename":null,"meetmenumber":null,"meetmecontext":null,"voicemailfullname":null,"voicemailmailbox":null,"voicemailcontext":null,"schedulename":null,"schedulecontext":null,"voicemenuname":null,"voicemenunumber":null,"voicemenucontext":null,"destination":"user","identity":"9964 (from-extern)","destidentity":"Sylvain Boily (104@default)"}
    
    def update(self):
        ret = AnyList.update(self)
        # self.reverse_index = {}
        return ret
