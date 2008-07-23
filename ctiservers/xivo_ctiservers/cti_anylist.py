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

from xivo_log import *
import cti_urllist

def log_debug(level, text):
        log_debug_file(level, text, 'anylist')
        return

class AnyList:
        def __init__(self, newurls = []):
                self.commandclass = None
                self.requested_list = {}
                self.__clean_urls__()
                self.__add_urls__(newurls)
                return
        
        def setcommandclass(self, commandclass):
                self.commandclass = commandclass
                return
        
        def __clean_urls__(self):
                self.requested_list = {}
                return
        
        def __add_urls__(self, newurls):
                for url in newurls:
                        if url not in self.requested_list:
                                self.requested_list[url] = cti_urllist.UrlList(url)
                return
        
        def setandupdate(self, newurls = []):
                self.__add_urls__(newurls)
                if len(self.requested_list) == 0:
                        return
                self.update()
                return
