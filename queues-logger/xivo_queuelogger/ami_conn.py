# vim: set expandtab ts=4 sw=4 sts=4 fileencoding=utf-8:

import sys
import socket
import re

from ami_logger import *
from log_event import *
from ami import *


class ami_conn:
# {
    def __init__(self, connectinfo):
    # {
        self.ip = connectinfo.ip
        self.port = connectinfo.port
        self.user = connectinfo.user
        self.password = connectinfo.password

        self.cache = {}

        self.sqltransaction = ""
        self.read = ""
        self.to_send = ""
        self.step = "wait_banner"
        self.error = ()

        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.ip, self.port))
        self.socket.setblocking(0)

        ami_logger.add_client(self)
    # }


    def fileno(self):
        return self.socket.fileno()

    def send_login(self):
        self.to_send = ami.forge_command('Login', {'Username': self.user,
                                                   'Secret'  : self.password})

    def parse_event(self):
    # {
        event_complete = self.read.find("\r\n\r\n")
        if event_complete == -1:
            return None

        event = self.read[:event_complete]
        self.read = self.read[event_complete + 4:]

        return dict(map(lambda ev: re.match(r"([^:]+): (.*)", ev).groups(),
                        event.splitlines()))
    # }

    def recv(self):
    # {
        buf = self.socket.recv(4096)
        if buf == "":
            self.error = ("%s:%d| Disconnected after read error\n" %\
                          (self.ip,self.port), -2)
            return -1

        self.read += buf
        getattr(self, self.step)() 
        return 0
    # }

    def send(self):
    # {
        sent = self.socket.send(self.to_send)
        if len <= 0:
            self.error = ("%s:%d| Disconnected while trying to send"\
                          "the following:\n'%s'\n" % \
                          (self.ip, self.port, self.to_send),\
                          -1)
            return -1
        else:
            self.to_send = self.to_send[sent:]
            return 0
    # }


    def wait_banner(self):
    # {
        line = self.read.find("\n")
        if line != -1:
            banner = self.read[:line]
            m = re.match(r"(Asterisk Call Manager)/([0-9.]+)", banner)
            if m and m.lastindex == 2:
                self.read = ""
                self.send_login()
                self.step = "wait_login_answer"
                return

            self.error = ("Wasn't i supposed to connect on an AMI server ?\n"\
                          "server banner is: '%s'\n" % self.read, 2)
    # }

    def wait_login_answer(self):
    # {
        ev = self.parse_event()

        if ev == None:
            return

        if ev['Message']  == 'Authentication accepted' and\
           ev['Response'] == 'Success':
               self.step = "watch_ami_event"
               sys.stderr.write("%s:%d| Authed, awaiting event\n" %\
                                (self.ip, self.port))
        else:
            self.error = ("password or login is wrong\n", 2)
    # }

    def watch_ami_event(self):
    # {
        while True:
            ev = self.parse_event()

            if ev == None:
                return

            if ev.has_key("Event"):
                sql = log_event(ev, self.cache).sql
                if sql != "":
                    self.sqltransaction.append(sql)
    # }
# }
