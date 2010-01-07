# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2009-2010 Proformatique'
__author__    = 'Thomas Bernard'

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

import socket
import errno
from collections import deque

class ClientConnection:
    class CloseException(Exception):
        def __init__(self, errno = -1):
            self.args = (errno,)
    
    def __init__(self, socket, address=None, sep = '\n'):
        self.socket = socket
        self.address = address
        self.socket.setblocking(0)
        self.sendqueue = deque()
        self.readbuff = ''
        self.isClosed = False
        self.toClose = False
        self.separator = sep
        return

    # useful for select
    def fileno(self):
        return self.socket.fileno()

    def getpeername(self):
        return self.address

    # close socket
    def close(self):
        if not self.isClosed:
            self.isClosed = True
            self.socket.close()

    # send data
    def send(self, data):
        if(len(data) > 0):
            self.sendqueue.append(data)
            self.process_sending()
        return

    # synonym for send
    def sendall(self, data):
        self.send(data)

    # to be called when the socket is ready for writing
    def process_sending(self):
        while len(self.sendqueue) > 0:
            data = self.sendqueue.popleft()
            try:
                n = self.socket.send(data)
                if n < len(data):   # there is some data left to be sent
                    self.sendqueue.appendleft(data[n:])
            except socket.error, (_errno, string):
                if _errno == errno.EAGAIN:
                    self.sendqueue.appendleft(data) # try next time !
                    return
                elif _errno in [errno.EPIPE, errno.ECONNRESET, errno.ENOTCONN, errno.ETIMEDOUT, errno.EHOSTUNREACH]:
                    self.close()
                    raise self.CloseException(_errno)
                elif _errno in [errno.EBADF]:
                    raise self.CloseException(_errno)
                else:
                    raise socket.error(_errno, string)
        return

    # do we have some data to be sent ?
    def need_sending(self):
        return len(self.sendqueue) > 0

    # to be called when the socked is ready for reading
    def recv(self):
        try:
            s = self.socket.recv(4096)
            if len(s) > 0:
                self.readbuff += s
            else:
                # remote host closed the connection
                self.close()
                raise self.CloseException()
        except socket.error, (_errno, string):
            if _errno in [errno.EPIPE, errno.ECONNRESET, errno.ENOTCONN, errno.ETIMEDOUT, errno.EHOSTUNREACH]:
                self.close()
                raise self.CloseException(_errno)
            elif _errno in [errno.EBADF]:
                # already closed !
                raise self.CloseException(_errno)
            elif _errno != errno.EAGAIN: # really an error
                raise socket.error(_errno, string)
        return

    # return a line if available or None
    # use the separator to split "lines"
    def readline(self):
        self.recv()
        try:
            k = self.readbuff.index(self.separator)
            ret = self.readbuff[0:k+1]
            self.readbuff = self.readbuff[k+1:]
            return ret
        except:
            return None

