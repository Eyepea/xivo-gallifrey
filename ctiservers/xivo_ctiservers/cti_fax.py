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

import commands
import os
import random
import SocketServer
import string
import threading

from xivo_log import *

PATH_SPOOL_ASTERISK     = '/var/spool/asterisk'
PATH_SPOOL_ASTERISK_FAX = PATH_SPOOL_ASTERISK + '/fax'
PATH_SPOOL_ASTERISK_TMP = PATH_SPOOL_ASTERISK + '/tmp'
PDF2FAX = '/usr/bin/xivo_pdf2fax'

__alphanums__ = string.uppercase + string.lowercase + string.digits

def log_debug(level, text):
        log_debug_file(level, text, 'fax')

class Fax:
        def __init__(self, uinfo, args):
                self.reference = ''.join(random.sample(__alphanums__, 10)) + "-" + hex(int(time.time()))
                self.params = args
                self.uinfo = uinfo
                return

        def send(self, buffer, callerid, ami):
                filename = 'astfaxsend-' + self.reference
                tmpfilepath = PATH_SPOOL_ASTERISK_TMP + '/' + filename
                faxfilepath = PATH_SPOOL_ASTERISK_FAX + '/' + filename + '.tif'
                z = open(tmpfilepath, 'w')
                z.write(buffer)
                z.close()
                log_debug(SYSLOG_INFO, 'args = %s' % self.params)
                for p in self.params:
                        [var, val] = p.split('=')
                        if var == 'number':
                                number = val
                        elif var == 'size':
                                size = val
                        elif var == 'hide':
                                hide = val

                if hide != '0':
                        callerid = 'anonymous'

                reply = 'ko;unknown'
                comm = commands.getoutput('file -b %s' % tmpfilepath)
                brieffile = ' '.join(comm.split()[0:2])
                if brieffile == 'PDF document,':
                        log_debug(SYSLOG_INFO, 'fax : the file received is a PDF one : converting to TIFF/F')
                        reply = 'ko;convert-pdftif'
                        ret = os.system("%s -o %s %s" % (PDF2FAX, faxfilepath, tmpfilepath))
                else:
                        reply = 'ko;filetype'
                        log_debug(SYSLOG_WARNING, 'fax : the file received is a <%s> one : format not supported' % brieffile)
                        ret = -1

                if ret == 0:
                        if os.path.exists(PATH_SPOOL_ASTERISK_FAX):
                                try:
                                        reply = 'ko;AMI'
                                        ret = ami.txfax(PATH_SPOOL_ASTERISK_FAX,
                                                        filename, callerid, number, self.uinfo.get('context'))
                                        if ret:
                                                reply = 'ok;'
                                except Exception, exc:
                                        log_debug(SYSLOG_ERR, '--- exception --- (fax handler - AMI) : %s' %(str(exc)))
                        else:
                                reply = 'ko;exists-pathspool'
                                log_debug(SYSLOG_INFO, 'directory %s does not exist - could not send fax'
                                          % PATH_SPOOL_ASTERISK_FAX)

                if reply == 'ok;':
			# BUGFIX: myconn is undefined
                        # filename is actually an identifier.
                        #faxclients[filename] = myconn
                        reply = 'queued;'

                self.uinfo.get('login').get('connection').sendall('faxsent=%s\n' % reply)

                os.unlink(tmpfilepath)
                log_debug(SYSLOG_INFO, "faxhandler : removed %s" % tmpfilepath)
