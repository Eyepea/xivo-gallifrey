"""A simple class to convert XML data into Python dictionary.

Copyright (C) 2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2010  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from xml.parsers import expat

class XML2Dict:
    """
    A simple class to convert XML data into Python dictionary.
    """
    def __init__(self):
        self._cdata_parts   = None
        self._current       = None
        self._stack         = None
        self._parser        = None
        self.root           = None

    def _set_item(self, k, v):
        if k != '__cdata__' and k in self._current:
            value = self._current[k]
            if not isinstance(value, list):
                value = [value]
            value.append(v)
        else:
            value = v
        self._current[k] = value

    def startElement(self, name, attrs):
        self._stack.append((self._current, self._cdata_parts))
        self._current       = {}
        self._cdata_parts   = []
        for k, v in attrs.items():
            self._set_item(k, v)

    def endElement(self, name):
        cdata = ''.join(self._cdata_parts).strip()

        if self._current:
            if cdata:
                self._set_item('__cdata__', cdata)

            obj = self._current
        else:
            obj = cdata

        self._current, self._cdata_parts = self._stack.pop()
        self._set_item(name, obj)

    def characters(self, content):
        self._cdata_parts.append(content)

    def Parse(self, data):
        self.root           = {}
        self._stack         = []
        self._current       = self.root
        self._cdata_parts   = []

        self._parser = expat.ParserCreate()
        self._parser.StartElementHandler    = self.startElement
        self._parser.EndElementHandler      = self.endElement
        self._parser.CharacterDataHandler   = self.characters

        self._parser.Parse(data)
        return self.root


def Parse(data):
    return XML2Dict().Parse(data)
