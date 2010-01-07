"""Threaded HTTP Server

Copyright (C) 2007-2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (c) 2001, 2002, 2003, 2004 Python Software Foundation;
    Copyright (C) 2007-2010  Proformatique
                                        All Rights Reserved

    Under PSF LICENSE AGREEMENT FOR PYTHON
    See the following URI for the full license:
        http://www.python.org/download/releases/2.4.4/license/
"""

import SocketServer
from SocketServer import socket

class ThreadingHTTPServer(SocketServer.ThreadingTCPServer):
    """
    Same as HTTPServer, but derives from ThreadingTCPServer instead of
    TCPServer so that each http handler instance runs in its own thread.
    """
    
    allow_reuse_address = 1    # Seems to make sense in testing environment
    
    def server_bind(self):
        """Override server_bind to store the server name."""
        SocketServer.TCPServer.server_bind(self)
        host, port = self.socket.getsockname()[:2]
        self.server_name = socket.getfqdn(host)
        self.server_port = port

__all__ = ['ThreadingHTTPServer']
