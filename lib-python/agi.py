__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

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

def agi_escape_string(s):
	return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', ' ')

def agi_verbose(txt):
	print "VERBOSE \"%s\"" % agi_escape_string(txt)

def agi_set_variable(var, val):
        print "SET VARIABLE %s \"%s\"" %(agi_escape_string(var), agi_escape_string(val))

def agi_answer():
        print "ANSWER"

def agi_exec(app, args):
        print "EXEC %s \"%s\"" %(agi_escape_string(app), agi_escape_string(args))
