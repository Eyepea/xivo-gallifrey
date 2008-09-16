"""Trace to multiple other trace backends

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

class MultipleTraces:
	def __init__(self, *others):
		self.others = others
	
	def err(self, message):
		for other in self.others:
			other.err(message)
	
	def warning(message):
		for other in self.others:
			other.warning(message)
	
	def notice(message):
		for other in self.others:
			other.notice(message)
	
	def info(message):
		for other in self.others:
			other.info(message)
	
	def debug(message):
		for other in self.others:
			other.debug(message)

__all__ = ('MultipleTraces',)
