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

import SocketServer

class MyTCPServer(SocketServer.ThreadingTCPServer):
        allow_reuse_address = True

class FaxRequestHandler(SocketServer.StreamRequestHandler):
        def handle(self):
                threading.currentThread().setName('fax-%s:%d' %(self.client_address[0], self.client_address[1]))
                filename = 'astfaxsend-' + ''.join(random.sample(__alphanums__, 10)) + "-" + hex(int(time.time()))
                tmpfilepath = PATH_SPOOL_ASTERISK_TMP + '/' + filename
                faxfilepath = PATH_SPOOL_ASTERISK_FAX + '/' + filename + ".tif"
                tmpfiles = [tmpfilepath]

                try:
                        file_definition = self.rfile.readline().strip()
                        log_debug(SYSLOG_INFO, 'fax : received <%s>' % file_definition)
                        a = self.rfile.read()
                        z = open(tmpfilepath, 'w')
                        z.write(a)
                        z.close()
                        log_debug(SYSLOG_INFO, 'fax : received %d bytes stored into %s' % (len(a), tmpfilepath))
                        params = file_definition.split()
                        for p in params:
                                [var, val] = p.split('=')
                                if var == 'number':
                                        number = val
                                elif var == 'context':
                                        context = val
                                elif var == 'astid':
                                        astid = val
                                elif var == 'hide':
                                        hide = val

                        if astid in faxbuffer:
                                [dummyme, myconn] = faxbuffer[astid].pop()

                        if hide == "0":
                                callerid = configs[astid].faxcallerid
                        else:
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
                                                ret = AMI_array_user_commands[astid].txfax(PATH_SPOOL_ASTERISK_FAX, filename, callerid, number, context)

                                                if ret:
                                                        reply = 'ok;'
                                        except Exception, exc:
                                                log_debug(SYSLOG_ERR, '--- exception --- (fax handler - AMI) : %s' %(str(exc)))
                                else:
                                        reply = 'ko;exists-pathspool'
                                        log_debug(SYSLOG_INFO, 'directory %s does not exist - could not send fax' %(PATH_SPOOL_ASTERISK_FAX))

                        if reply == 'ok;':
                                # filename is actually an identifier.
                                faxclients[filename] = myconn
                                reply = 'queued;'

                        if myconn[0] == 'udp':
                                myconn[1].sendto('faxsent=%s\n' % reply, (myconn[2], myconn[3]))
                        else:
                                myconn[1].send('faxsent=%s\n' % reply)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- (fax handler - global) : %s" %(str(exc)))

                for tmpfile in tmpfiles:
                        os.unlink(tmpfile)
                        log_debug(SYSLOG_INFO, "faxhandler : removed %s" % tmpfile)

