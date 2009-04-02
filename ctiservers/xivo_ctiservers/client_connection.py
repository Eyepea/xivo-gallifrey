# (c) Proformatique 2009
# Thomas Bernard
# non blocking socket

import socket
import errno
from collections import deque

class ClientConnection:
    class CloseException(Exception):
        pass

    def __init__(self, socket, address=None):
        self.socket = socket
        self.address = address
        self.socket.setblocking(0)
        self.sendqueue = deque()
        self.readbuff = ''
        return

    # usefull for select
    def fileno(self):
        return self.socket.fileno()

    def getpeername(self):
        return self.address

    # close socket
    def close(self):
        return self.socket.close()

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
                elif _errno in [errno.EPIPE, errno.ECONNRESET, errno.ENOTCONN, errn, errno.ETIMEDOUT]:
                    self.socket.close()
                    raise self.CloseException
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
                self.socket.close()
                raise self.CloseException
        except socket.error, (_errno, string):
            if _errno != errno.EAGAIN: # really an error
                raise socket.error(_errno, string)
        return

    # return a line if available or None
    def readline(self):
        self.recv()
        try:
            k = self.readbuff.index('\n')
            ret = self.readbuff[0:k+1]
            self.readbuff = self.readbuff[k+1:]
            return ret
        except:            
            return None


