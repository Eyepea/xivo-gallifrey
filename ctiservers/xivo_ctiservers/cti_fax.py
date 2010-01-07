# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
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
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import base64
import commands
import logging
import os
import random
import SocketServer
import string
import threading
import time

PATH_SPOOL_ASTERISK     = '/var/spool/asterisk'
PATH_SPOOL_ASTERISK_FAX = PATH_SPOOL_ASTERISK + '/fax'
PATH_SPOOL_ASTERISK_TMP = PATH_SPOOL_ASTERISK + '/tmp'
PDF2FAX = '/usr/bin/xivo_pdf2fax'

__alphanums__ = string.uppercase + string.lowercase + string.digits

log = logging.getLogger('fax')

class Fax:
        def __init__(self, uinfo, size, number, hide):
                self.reference = ''.join(random.sample(__alphanums__, 10)) + "-" + hex(int(time.time()))
                self.size = size
                self.number = number.replace('.', '').replace(' ', '')
                self.hide = hide
                self.uinfo = uinfo
                return
        
        def sendfax(self, b64buffer, callerid, ami):
                filename = 'astfaxsend-' + self.reference
                tmpfilepath = PATH_SPOOL_ASTERISK_TMP + '/' + filename
                faxfilepath = PATH_SPOOL_ASTERISK_FAX + '/' + filename + '.tif'
                buffer = base64.b64decode(b64buffer.strip())
                z = open(tmpfilepath, 'w')
                z.write(buffer)
                z.close()
                log.info('size = %s (%d), number = %s, hide = %s'
                         % (self.size, len(buffer), self.number, self.hide))
                if self.hide != '0':
                    callerid = 'anonymous'
                    
                reply = 'ko;unknown'
                comm = commands.getoutput('file -b %s' % tmpfilepath)
                brieffile = ' '.join(comm.split()[0:2])
                if brieffile == 'PDF document,':
                    pdf2fax_command = '%s -o %s %s' % (PDF2FAX, faxfilepath, tmpfilepath)
                    log.info('fax (ref %s) PDF to TIF(F) : %s' % (self.reference, pdf2fax_command))
                    reply = 'ko;convert-pdftif'
                    sysret = os.system(pdf2fax_command)
                    ret = os.WEXITSTATUS(sysret)
                    if ret != 0:
                        log.warning('fax (ref %s) PDF to TIF(F) returned : %s (exitstatus = %s, stopsignal = %s)'
                                    % (self.reference, sysret, ret, os.WSTOPSIG(sysret)))
                else:
                    reply = 'ko;filetype'
                    log.warning('fax (ref %s) the file received is a <%s> one : format not supported' % (self.reference, brieffile))
                    ret = -1
                    
                if ret == 0:
                        if os.path.exists(PATH_SPOOL_ASTERISK_FAX):
                                try:
                                        reply = 'ko;AMI'
                                        ret = ami.txfax(PATH_SPOOL_ASTERISK_FAX, filename,
                                                        self.uinfo.get('xivo_userid'),
                                                        callerid, self.number,
                                                        self.uinfo.get('context'),
                                                        self.reference)
                                        if ret:
                                                reply = 'queued;'
                                except Exception:
                                        log.exception('(fax handler - AMI)')
                        else:
                                reply = 'ko;exists-pathspool'
                                log.warning('directory %s does not exist - could not send fax ref %s'
                                            % (PATH_SPOOL_ASTERISK_FAX, self.reference))
                                
                # BUGFIX: myconn is undefined
                # filename is actually an identifier.
                # faxclients[filename] = myconn
                
                os.unlink(tmpfilepath)
                log.info('fax (ref %s) removed %s (reply will be %s)' % (self.reference, tmpfilepath, reply))
                return reply
