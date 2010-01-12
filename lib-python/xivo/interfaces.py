"""/etc/network/interfaces parser

Copyright (C) 2008-2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2010  Proformatique

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
import sys
from itertools import islice

def warn(msg):
    print >> sys.stderr, "WARNING:", msg

class EniBlock:
    """
    Base class for a block of lines from interfaces(5)

    let B be an instance of EniBlock:
    * B.raw_lines is a list that contains the lines exactly as they appear
      in the source file
    * B.cooked_lines is a list that contains useful preprocessed lines,
      that is it contains neither blank lines nor comments, and raw
      continuated lines (terminated by a backslash) are joined
    """
    def __init__(self):
        self.raw_lines = []
        self.cooked_lines = []

    def append_raw_line(self, rawline):
        self.raw_lines.append(rawline)

    def extend_raw_lines(self, rawlines):
        self.raw_lines.extend(rawlines)

    def append_cooked_line(self, line):
        self.cooked_lines.append(line)

class EniBlockSpace(EniBlock):
    """
    A block of insignificant lines, i.e. blank lines or comments.
    Obviously, let B be an instance of EniBlockSpace:
    * B.cooked_line is empty
    """
    pass

class EniBlockWithIfName(EniBlock):
    """
    A stanza of interfaces(5) which is mainly related to a single network
    interface.
    let B be an instance of EniBlockWithIfName:
    * B.ifname contains the name of this related network interface
    """
    def __init__(self, ifname):
        EniBlock.__init__(self)
        self.ifname = ifname

class EniBlockMapping(EniBlockWithIfName):
    """
    A mapping stanza of interfaces(5)
    """
    pass

class EniBlockIface(EniBlockWithIfName):
    """
    An logical interface stanza of interfaces(5)
    """
    pass

class EniBlockAllow(EniBlock):
    """
    An "auto" / "allow-" stanza of interfaces(5)

    let B be an instance of EniBlockAllow:
    * B.allowup contains the keyword that might be used in a command line
      parameter of the ifup tool to address the interfaces specified by
      this block
    * B.allow_kw contains the identifying word of this allow stanza, that
      is "auto" or a word which begins with "allow-" - note that B.allowup
      is function of B.allow_kw
    * B.allow_list contains the list of allowed interfaces
    """

    ALLOWUP_CLASSES = ('allow-hotplug',
                       'allow-ifplugd',
                       'allow-auto',
                       'auto')

    def __init__(self, allow_kw, allow_list):
        EniBlock.__init__(self)
        if allow_kw.startswith("allow-"):
            self.allowup = allow_kw[6:]
        elif allow_kw == "auto":
            self.allowup = "auto"
        else:
            raise ValueError, "allow_kw should start with \"allow-\" or be egal to \"auto\""
        self.allow_kw = allow_kw
        self.allow_list = allow_list

class EniBlockUnknown(EniBlock):
    """
    An unknown (invalid) stanza of interfaces(5)
    """
    pass

def raw_2_contd_coockedPart(rawl):
    has_newline = (rawl[-1:] == '\n')
    if has_newline:
        contd = rawl[-2:-1] == '\\'
    else:
        contd = rawl[-1:] == '\\'
    if (not has_newline) and (not contd):
        return contd, rawl
    else:
        return contd, rawl[:-has_newline-contd]

MatchLeadingSpaces = re.compile(r'(\s)*').match
def leading_spaces(s):
    return MatchLeadingSpaces(s).group()

SearchTrailingSpaces = re.compile(r'(\s)*$').search
def trailing_spaces(s):
    return SearchTrailingSpaces(s).group()

class EniCookLineRecipe(object):

    # NOTE: only on 'allow-' / 'auto' stanzas

    __slots__ = (
            'first_raw_line',
            'nb_raw_lines',
            'pre_spaces',
            'post_spaces',
            'list_cooked_splitter',
            'cooked_line',
    )

    def __init__(self, raw_lines, warnfunc=warn):
        self.first_raw_line = None
        self.nb_raw_lines = 0
        part_cooked_lines = []
        for pos, rawl in enumerate(raw_lines):
            rawl_lstrip = rawl.lstrip()
            first_char = rawl_lstrip[0:1]
            second_char = rawl_lstrip[1:2]
            if first_char == '#' or (first_char == '\\' and second_char == '\n'):
                continue
            if self.first_raw_line is None:
                self.first_raw_line = pos
            self.nb_raw_lines += 1
            contd, cooked_part = raw_2_contd_coockedPart(rawl)
            part_cooked_lines.append(cooked_part)
            if not contd:
                break
        else:
            warnfunc("last line is continued in block of raw lines: " + `part_cooked_lines`)
        while part_cooked_lines and not part_cooked_lines[-1].strip():
            part_cooked_lines.pop()
        list_splitter = [len(cooked_part) for cooked_part in part_cooked_lines]
        list_splitter.pop()
        joined = ''.join(part_cooked_lines)
        self.pre_spaces = leading_spaces(joined)
        self.post_spaces = trailing_spaces(joined)
        len_pre_spaces = len(self.pre_spaces)
        end_useful = len(joined) - len(self.post_spaces)
        self.list_cooked_splitter = [ p - len_pre_spaces for p in list_splitter if len_pre_spaces <= p <= end_useful ]
        self.cooked_line = joined[len_pre_spaces:end_useful]

    def remove_part(self, cmin, cmax):
        for pos, elt in enumerate(self.list_cooked_splitter):
            if elt <= cmin:
                new_elt = elt
            elif elt < cmax:
                new_elt = cmin
            else:
                new_elt = elt - (cmax - cmin)
            self.list_cooked_splitter[pos] = new_elt
        pos = 0
        previous = None
        while pos < len(self.list_cooked_splitter):
            if (previous is not None) and (self.list_cooked_splitter[pos] == previous):
                del self.list_cooked_splitter[pos]
            else:
                previous = self.list_cooked_splitter[pos]
                pos += 1
        self.cooked_line = self.cooked_line[:cmin] + self.cooked_line[cmax:]

    def get_updated_raw_lines(self):
        if self.list_cooked_splitter:
            raw_lines = [ self.cooked_line[pmin:pmax] for pmin, pmax in zip([0] + self.list_cooked_splitter, self.list_cooked_splitter) ]
            if self.post_spaces or (self.list_cooked_splitter[-1] < len(self.cooked_line)):
                raw_lines.append(self.cooked_line[self.list_cooked_splitter[-1]:] + self.post_spaces)
        else:
            raw_lines = [ self.cooked_line + self.post_spaces ]
        if self.pre_spaces:
            raw_lines[0] = self.pre_spaces + raw_lines[0]
        for pos in xrange(len(raw_lines)-1):
            raw_lines[pos] += '\\\n'
        raw_lines[-1] += '\n'
        return raw_lines

    def update_block(self, block):
        block.cooked_lines[0] = self.cooked_line
        block.raw_lines[self.first_raw_line:self.first_raw_line+self.nb_raw_lines] = self.get_updated_raw_lines()

def parse(lines, warnfunc=warn):
    """
    This function parses the sequence of lines of an interfaces(5) file.
    It returns a list of EniBlock instances, more precisely a list of
    instances of classes of the set: EniBlockSpace, EniBlockMapping,
    EniBlockIface, EniBlockAllow, EniBlockAllow.
    """
    block_list = []

    current_block = None
    current_semantic_block = None

    current_raw = []
    current_raw_iscomment = []
    start_of_last_comment = 0

    itl = iter(lines)

    while True:
        try:
            line = itl.next()
        except StopIteration:
            break

        if line.lstrip()[0:1] == '#':
            current_raw.append(line)
            current_raw_iscomment.append(True)
            continue

        cooked_line_split = []
        while True:
            contd, coocked_part = raw_2_contd_coockedPart(line)

            cooked_line_split.append(coocked_part)

            current_raw.append(line)
            current_raw_iscomment.append(False)

            if not contd:
                break

            try:
                line = itl.next()
            except StopIteration:
                warnfunc("continued line at end of file - %r" % line.strip())
                break

        cooked_line = ''.join(cooked_line_split).strip()

        if not cooked_line:
            if not current_block:
                current_block = EniBlockSpace()
                block_list.append(current_block)
            if isinstance(current_block, EniBlockSpace):
                current_block.extend_raw_lines(current_raw)
                del current_raw[:]
                del current_raw_iscomment[:]
                start_of_last_comment = 0
            else:
                while current_raw_iscomment[0]:
                    current_block.append_raw_line(current_raw[0])
                    del current_raw[0]
                    del current_raw_iscomment[0]
                start_of_last_comment = len(current_raw)
            continue

        words = cooked_line.split()
        firstword = words[0]

        new_block = None

        if firstword == "mapping":
            ifname = None
            if len(words) < 2:
                warnfunc("mapping stanza with no interface name - %r" % cooked_line)
            else:
                ifname = words[1]
            new_block = EniBlockMapping(ifname)
            current_semantic_block = new_block
        elif firstword == "iface":
            ifname = None
            if len(words) < 2:
                warnfunc("iface stanza with no interface name - %r" % cooked_line)
            else:
                ifname = words[1]
            new_block = EniBlockIface(ifname)
            current_semantic_block = new_block
        elif firstword.startswith("allow-") or firstword == "auto":
            if len(words) < 2:
                warnfunc("auto/allow stanza but no interface names - %r" % cooked_line)
            new_block = EniBlockAllow(firstword, words[1:])
            current_semantic_block = None
        elif not current_semantic_block:
            warnfunc("spurious option line - %r" % cooked_line)
            new_block = EniBlockUnknown()
            current_semantic_block = None

        if new_block:
            if start_of_last_comment:
                new_space_block = EniBlockSpace()
                new_space_block.extend_raw_lines(islice(current_raw, 0, start_of_last_comment))
                del current_raw[0:start_of_last_comment]
                del current_raw_iscomment[0:start_of_last_comment]
                start_of_last_comment = 0
                block_list.append(new_space_block)
            block_list.append(new_block)
            current_block = new_block

        current_block.extend_raw_lines(current_raw)
        del current_raw[:]
        del current_raw_iscomment[:]
        start_of_last_comment = 0
        current_block.append_cooked_line(cooked_line)

    if current_raw:
        if current_block and not isinstance(current_block, EniBlockSpace):
            while current_raw_iscomment and current_raw_iscomment[0]:
                current_block.append_raw_line(current_raw[0])
                del current_raw[0]
                del current_raw_iscomment[0]
            current_block = None
        if current_raw:
            if not current_block:
                current_block = EniBlockSpace()
                block_list.append(current_block)
            current_block.extend_raw_lines(current_raw)

    return block_list

def get_mapping_dests(block_list, base, full):
    """
    Walk in mappings, looking for logical interfaces.  Return the set of
    logical interfaces that are mapped to one of the interfaces of
    full_interfaces or one vlan (including the eluded vlan 0) based on an
    interface of base_interfaces.
    NOTE: elements of 'full' must be formatted as the output of
    normalize_vlan() is and there must be no "." character in elements of
    'base'
    """
    logicals = set()
    for block in block_list:
        if not isinstance(block, EniBlockMapping):
            continue
        if not ifname_in_base_full(block.ifname, base, full):
            continue
        for cooked_line in block.cooked_lines:
            splitted = cooked_line.split(None, 3)
            if len(splitted) < 3 or splitted[0] != 'map':
                continue
            logicals.add(splitted[2])
    return logicals

def normalize_vlan(ifname):
    """
    Ensure 'ifname' contains a VLan interface name in its right part,
    formatted as a canonical decimal
    """
    if '.' not in ifname:
        return ifname + ".0"
    else:
        left, right = ifname.split(".")
        return "%s.%d" % (left, int(right))

def ifname_in_base_full(ifname, base, full):
    """
    Test if the interface name in 'ifname' is in the list 'full' or is a
    vlan based on an interface of the list 'base'
    NOTE: elements of 'full' must be formatted as the output of
    normalize_vlan() is and there must be no "." character in elements of
    'base'
    """
    norm_vlan_ifname = normalize_vlan(ifname)
    left = norm_vlan_ifname.split(".", 1)[0]
    return (norm_vlan_ifname in full) or (left in base)

def ifname_in_base_full_mapsymbs(ifname, base, full, mapsymbs):
    """
    * 'ifname' is an interface name
    * 'base' is a set of unmapped physical interface names
    * 'full' is a set of unmapped vlan interface names
    * 'mapsymbs' is a set of mapped (purely symbolic) interface names
    This function returns True iff the interface name is in 'mapsymbs' or
    has a corresponding physical interface name in 'base' or has a
    corresponding vlan interface name (that includes vlan 0) in 'full'.
    NOTE: elements of 'full' must be formatted as the output of
    normalize_vlan() is and there must be no "." character in elements of
    'base'
    """
    return ifname in mapsymbs or ifname_in_base_full(ifname, base, full)

def allowed(block_list, warnfunc=warn):
    """
    This function calculates and returns a dictionary in which each key is
    the "allow" keyword (without the 'allow-' part that is stored in
    interfaces(5) ) and the corresponding value is a set of allowed
    interfaces for this keyword.
    As a side effect, this function calls warnfunc() (the default of which
    sends a warning on sys.stderr) if some inconsistencies are detected.
    """
    allow = {}
    multiple = set()
    for block in block_list:
        if not isinstance(block, EniBlockAllow):
            continue
        if block.allowup not in allow:
            allow[block.allowup] = set()
        for ifallowed in block.allow_list:
            if ifallowed in allow[block.allowup]:
                multiple.add((ifallowed, block.allowup))
            else:
                allow[block.allowup].add(ifallowed)
    for if_allowup in multiple:
        warnfunc("%r is allowed multiple times for %r" % if_allowup)
    for key, val in allow.items():
        if not val:
            warnfunc("%r authorization is empty" % key)
            del allow[key]
    return allow
