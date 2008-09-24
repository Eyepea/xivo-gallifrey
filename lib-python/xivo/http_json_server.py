"""HTTP JSON Server

Copyright (C) 2007, 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""


# TODO: a configuration option to send the backtraces
# or not to the client in error report

# TODO: catch disconnections in wfile.write, send_* and end_headers
# (and maybe others) and log them with a simple short line instead of 
# logging the complete Broken pipe exception (or in some cases nothing!)

# TODO: locks and implements SIGHUP (by reloading the configuration)

# TODO: add some teardown callbacks?
# maybe two stages:
#   - cb teardown stage 1
#   - wait for tread completion in this module
#   - cb teardown stage 2

from BaseHTTPServer import BaseHTTPRequestHandler
from xivo.ThreadingHTTPServer import ThreadingHTTPServer
from xivo import urisup
import signal
import logging
import re
import socket
import select
import errno
import cjson
import cgi
import traceback
import sys

CMD_R = 0
CMD_RW = 1

log = logging.getLogger('http_json_server') # pylint: disable-msg=C0103

_killed = False

_commands = {}
_cmd_r = {}
_cmd_rw = {}


class HttpReqError(Exception):
    """
    Catched in HttpReqHandler.common_req() which calls .report().
    This is useful to be sure only one response (in this case an
    error response) will be sent to the client; if an exception
    occurs while sending this response, we must not try to send an
    other response!
    """
    def __init__(self, code, text=None, exc=None):
        self.code = code
        self.text = text
        self.exc = exc
        msg = text or BaseHTTPRequestHandler.responses[code][1]
        Exception.__init__(self, msg)
    def report(self, req_handler):
        "Send a response corresponding to this error to the client"
        if self.exc:
            req_handler.send_exception(self.code, self.exc)
        elif self.text:
            req_handler.send_error_msgtxt(self.code, self.text)
        else:
            req_handler.send_error(self.code)


class HttpReqHandler(BaseHTTPRequestHandler):
    """
    Handle one HTTP request
    """

    def log_error(self, *args):
        """
        There is more information in log_request(), which is always called
        => do nothing
        """
        pass

    def log_request(self, code='-', size='-'):
        """
        Called by send_response()
        TODO: a configuration option to log or not
            (maybe using logging filters?)
        TODO: discriminate by code and dispatch to various log levels
        """
        log.info("%r %s %s", self.requestline, code, size)

    def send_response(self, code, message=None, size='-'):
        """
        Send the response header and log the response code.

        Also send two standard headers with the server software
        version and the current date.
        """
        # pylint: disable-msg=W0221
        self.log_request(code, size)
        if message is None:
            if code in self.responses:
                message = self.responses[code][0]
            else:
                message = ''
        if self.request_version != 'HTTP/0.9':
            self.wfile.write("%s %d %s\r\n" %
                             (self.protocol_version, code, message))
            # print (self.protocol_version, code, message)
        self.send_header('Server', self.version_string())
        self.send_header('Date', self.date_time_string())

    def send_error_explain(self, code, message):
        "do not use directly"
        self.send_response(code)
        self.send_header("Connection", "close")
        self.send_header("Content-Type", "text/html")
        self.end_headers()
        self.wfile.write(self.error_message_format % {
            'code': code,
            'message': message,
            'explain': self.responses[code][1]
        })

    def send_error_msgtxt(self, code, text):
        "text will be in a <pre> bloc"
        self.send_error_explain(
            code,
            ''.join(("<pre>\n", cgi.escape(text), "</pre>\n"))
        )

    def send_exception(self, code, exc_info=None):
        "send an error response including a backtrace to the client"
        if not exc_info:
            exc_info = sys.exc_info()
        x = ''.join(traceback.format_exception(*exc_info))
        self.send_error_msgtxt(code, x)

    def pathify(self):
        """
        rfc2616 says in 5.1.2: "all HTTP/1.1 servers MUST accept the
        absoluteURI form in requests" so if we get one, transform it
        to abs_path.
        Raises HttpReqError if the request is malformed, else returns
        (path, query, fragment)
        """
        try:
            path, query, fragment = urisup.uri_help_split(self.path)[2:]
            
            if not path:
                path = "/"
            
            if path[0] != "/":
                raise urisup.InvalidURIError, 'path %r does not start with "/"' % path
            
            path = re.sub("^/+", "/", path, 1)
            
            return path, query, fragment

        except urisup.InvalidURIError, e:
            log.error("invalid URI: %s", e)
            raise HttpReqError(400, str(e))
    
    @staticmethod
    def json_from_get(cmd):
        """
        Callback for .execute_command() for GET requests
        """
        if cmd not in _cmd_r:
            raise HttpReqError(404)

        func = _cmd_r[cmd][0]
        res = func()
        return cjson.encode(res)
    
    def json_from_post(self, cmd):
        """
        Callback for .execute_command() for POST requests
        """
        if cmd not in _cmd_rw:
            raise HttpReqError(404)

        tenc = self.headers.get('Transfer-Encoding')
        if tenc and tenc.lower() != 'identity':
            raise HttpReqError(501, "Not supported; Transfer-Encoding: %s" % tenc)

        ctype = self.headers.get('Content-Type')
        if ctype and ctype.lower() != 'application/json':
            raise HttpReqError(501, "Not supported; Content-Type: %s" % ctype)

        try:
            clen = int(self.headers.get('Content-Length'))
        except (ValueError, TypeError):
            raise HttpReqError(411)
        if clen < 0:
            raise HttpReqError(411)
        
        json_params = self.rfile.read(clen)
        try:
            params = cjson.decode(json_params)
        except cjson.DecodeError, e:
            raise HttpReqError(415, text=str(e))
        
        func = _cmd_rw[cmd][0]
        res = func(params)
        return cjson.encode(res)
    
    def common_req(self, execute, send_body=True):
        "Common code for GET and POST requests"
        try:
            try:
                path, query, fragment = self.pathify() # pylint: disable-msg=W0612
                cmd = path[1:]
                res_json = execute(cmd)

            except HttpReqError, e:
                e.report(self)
            
            except Exception:
                try:
                    self.send_exception(500) # XXX 500
                except Exception: # pylint: disable-msg=W0703
                    pass
                raise

            else:
                content_length = len(res_json) + 1
                self.send_response(200, size=content_length)
                self.send_header("Cache-Control", "no-cache")
                self.send_header("Pragma", "no-cache")
                self.send_header("Connection", "close")
                self.send_header("Content-Type", "application/json")
                self.send_header("Content-Length", str(content_length))
                self.end_headers()
                if send_body:
                    self.wfile.write(res_json)
                    self.wfile.write("\n")

        except Exception: # pylint: disable-msg=W0703
            log.exception("unexpected exception")

    def do_GET(self):
        "GET method"
        self.common_req(self.json_from_get)

    def do_HEAD(self):
        "HEAD method"
        self.common_req(self.json_from_get, send_body=False)

    def do_POST(self):
        "POST method"
        self.common_req(self.json_from_post)


class KillableThreadingHTTPServer(ThreadingHTTPServer):
    "Just introduces serve_until_killed(), which is specific to this module"

    def serve_until_killed(self):
        """Handle one request at a time until we are murdered."""
        while not _killed:
            self.handle_request()


def register(func, op, setup=None, fname=None):
    """
    Register a command
    @func: function to execute
    @op: CMD_R or CMD_RW
    """
    if not fname:
        fname = func.__name__
    if fname in _commands:
        raise ValueError, "%s is already registred" % fname
    _commands[fname] = (func, setup, op)


def sigterm_handler(signum, stack_frame):
    """
    Just tell the server to exit.
    
    WARNING: There are race conditions, for example with TimeoutSocket.accept.
    We don't care: the user can just rekill the process after like 1 sec. if
    the first kill did not work.
    """
    # pylint: disable-msg=W0613
    global _killed
    _killed = True


def run(options):
    "Start and execute the server"
    http_server = KillableThreadingHTTPServer(
        (options.listen_addr, options.listen_port),
        HttpReqHandler
    )
    while not _killed:
        try:
            http_server.serve_until_killed()
        except (socket.error, select.error), why:
            if errno.EINTR == why[0]:
                log.debug("interrupted system call")
            else:
                raise
    log.info("exiting")


def init(options):
    "Must be called before anything else"
    if hasattr(options, 'testmethods') and options.testmethods:
        def fortytwo():
            "test GET method"
            return 42
        def ping(args):
            "test POST method"
            return args
        register(fortytwo, CMD_R)
        register(ping, CMD_RW)
    for fname, (func, setup, op) in _commands.iteritems():
        if op == CMD_R:
            _cmd_r[fname] = (func, setup)
        else: # op == CMD_RW
            _cmd_rw[fname] = (func, setup)
        if setup:
            setup(options)
    # signal.signal(signal.SIGHUP, lambda *x: None) # XXX
    signal.signal(signal.SIGTERM, sigterm_handler)
