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
        self.stack          = None
        self.root           = None
        self.current        = None
        self.cdata_parts    = None

    def _set_item(self, k, v):
        if k != '__cdata__' and k in self.current:
            value = self.current[k]
            if not isinstance(value, list):
                value = [value]
            value.append(v)
        else:
            value = v
        self.current[k] = value

    def startElement(self, name, attrs):
        self.stack.append((self.current, self.cdata_parts))
        self.current        = {}
        self.cdata_parts    = []
        for k, v in attrs.items():
            self._set_item(k, v)

    def endElement(self, name):
        cdata = ''.join(self.cdata_parts).strip()

        if self.current:
            if cdata:
                self._set_item('__cdata__', cdata)

            obj = self.current
        else:
            obj = cdata

        self.current, self.cdata_parts = self.stack.pop()
        self._set_item(name, obj)

    def characters(self, content):
        self.cdata_parts.append(content)

    def Parse(self, xml):
        self.stack          = []
        self.root           = {}
        self.current        = self.root
        self.cdata_parts    = []

        self.parser = expat.ParserCreate()
        self.parser.StartElementHandler  = self.startElement
        self.parser.EndElementHandler    = self.endElement
        self.parser.CharacterDataHandler = self.characters

        self.parser.Parse(xml)

        return self.root

    def ParseFile(self, filename):
        f = open(filename)
        self.Parse(f.read())
        f.close()
        return self.root
