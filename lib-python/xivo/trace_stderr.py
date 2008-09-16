"""Trace to stderr backend

Copyright (C) 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

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

import sys

def err(message):
	print >> sys.stderr, "ERR:", message

def warning(message):
	print >> sys.stderr, "WARNING:", message

def notice(message):
	print >> sys.stderr, "NOTICE:", message

def info(message):
	print >> sys.stderr, "INFO:", message

def debug(message):
	print >> sys.stderr, "DEBUG:", message

__all__ = ('err', 'warning', 'notice', 'info', 'debug')
