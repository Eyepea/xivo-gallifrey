__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2010  Proformatique <technique@proformatique.com>
    
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

import re
from xivo import OrderedConf
from xivo.OrderedConf import SECTCRE, ParsingError, MissingSectionHeaderError

DIRECRE = re.compile(
    r'(?P<directive>#(?:(?i)exec|include))'
    r'\s*(?P<path>.+)$')

OPTCRE = re.compile(
    r'(?P<option>[^=\s][^=]*)'          # very permissive!
    r'\s*(?P<vi>=>?)\s*'                # any number of space/tab,
                                        # followed by separator
                                        # (either = or =>), followed
                                        # by any # space/tab
    r'(?P<value>.*)$'                   # everything up to eol
    )

class AsteriskConfigParser(OrderedConf.OrderedRawConf):
    """
    Asterisk Configuration file can contain directives #include and #exec.
    """

    def __init__(self, fp=None, filename=None, sect_trans=OrderedConf._id1, opt_trans=OrderedConf._id1):
        OrderedConf.OrderedRawConf.__init__(self, fp, filename, sect_trans, opt_trans, allow_multiple=True)
        self._directives = []

    def _read(self, fp, filename):
        lineno = 0
        cur_sect = None
        e = None                        # None, or an exception
        while True:
            line = fp.readline()
            if not line:
                break
            lineno = lineno + 1
            # comment or blank line?
            if line.strip() == '' or line[0] in ';':
                continue
            # a section header or option header?
            # is it a section header?
            mo = SECTCRE.match(line)
            if mo:
                sectname = mo.group('header')
                tsec = self.sect_trans(sectname)
                newopt = ([], {})
                self._sections[0].append((sectname, newopt))
                cur_sect = newopt
                if tsec not in self._sections[1]:
                    self._sections[1][tsec] = newopt
                continue
            if cur_sect is None:
                mo = DIRECRE.match(line)
                if mo:
                    self._directives.append((mo.group('directive').lower(),
                                             mo.group('path')))
                    continue
                elif line[0] == '#':
                    continue
                raise MissingSectionHeaderError(filename, lineno, line)
            # an option line?
            mo = OPTCRE.match(line)
            if mo:
                optname, vi, optval = mo.group('option', 'vi', 'value')
                if vi in ('=', '=>') and ';' in optval:
                    # ';' is a comment delimiter only if it follows
                    # a spacing character
                    pos = optval.find(';')
                    if pos > 0 and optval[pos-1].isspace():
                        optval = optval[:pos]
                optname = optname.rstrip()
                topt = self.opt_trans(optname)
                optval = optval.strip()
                cur_sect[0].append((optname, optval))
                cur_sect[1][topt] = optval
                continue
            mo = DIRECRE.match(line)
            if mo:
                directive, path = mo.group('directive', 'path')
                if ';' in path:
                    pos = path.find(';')
                    if pos > 0 and optval[pos-1].isspace():
                        path = path[:pos]
                directive = directive.rstrip().lower()
                path = path.strip()
                cur_sect[0].append((directive, path))
                cur_sect[1][directive] = path
                continue
            if not e:
                e = ParsingError(filename)
            e.append(lineno, repr(line))
        if e:
            raise e

    def write(self, fp):
        """
        write the configuration state
        """
        for directive in self._directives:
            fp.write("%s %s" % directive)
        for s in self._sections[0]:
            fp.write("[%s]\n" % s[0])
            for k, v in s[1][0]:
                if k not in ('#exec', '#include'):
                    fp.write("%s = %s\n" % (k, v))
                else:
                    fp.write("%s %s\n" % (k, v))
            fp.write("\n")
