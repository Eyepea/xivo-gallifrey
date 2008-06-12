#!/usr/bin/python

"""Tests for xivo.udev

Copyright (C) 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import unittest

from xivo import udev

LINES = [
    "# a comment\n",
    "  # a comment too, some blank lines follow\n",
    "\n",
    " \n",
    "\t\n",
    " \t \n",
    "a line\n",
    "\n",
    "a multiline\\\n",
    " and it continues here\n",
    "\n",
    "a multiline made \\\n",
    "of more than two \\\n",
    "lines\n",
    "\n",
    " \t a multiline that starts  \\\n",
    "with some spaces\n",
    "\n",
    "# a multiline comment\\\n",
    " that continues here \n",
    "\n",
    "This multiline is \\\n",
    "# not a comment\n",
    "\n",
    "can we parse this at end of file? \\\n"
]

MULTILINES = [
    "# a comment",
    "# a comment too, some blank lines follow",
    "",
    "",
    "",
    "",
    "a line",
    "",
    "a multiline and it continues here",
    "",
    "a multiline made of more than two lines",
    "",
    "a multiline that starts  with some spaces",
    "",
    "# a multiline comment that continues here ",
    "",
    "This multiline is # not a comment",
    "",
    "can we parse this at end of file? "
]

class TestIterMultilines(unittest.TestCase):
    def test_iter_multilines(self):
        calc_mlines = list(udev.iter_multilines(LINES))
        for p, mline in enumerate(calc_mlines):
            self.assertEqual((p, mline), (p, MULTILINES[p]))
        self.assertEqual(len(calc_mlines), len(MULTILINES))
        self.assertEqual(calc_mlines, MULTILINES)

unittest.main()
