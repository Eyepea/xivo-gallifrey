#!/usr/bin/python

"""Tests for shvar

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

from xivo import shvar

class TestShVar(unittest.TestCase):
    
    def test_load_not_assign(self):
        self.assertRaises(shvar.NotAssignmentError, shvar.load, ["echo bla\n"])
        self.assertRaises(shvar.NotAssignmentError, shvar.load, ["=bla\n"])
        self.assertRaises(shvar.NotAssignmentError, shvar.load, ["1=bla\n"])
        self.assertRaises(shvar.NotAssignmentError, shvar.load, ["a = bla\n"])
    
    def test_load_complex_statement(self):
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1 echo lol\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1;echo lol\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1|echo lol\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1&echo lol\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1</dev/null\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1>/dev/null\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1;b=2\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1 b=2\n"])
        self.assertRaises(shvar.ComplexStatementError, shvar.load, ["a=1\tb=2\n"])
    
    def test_load(self):
        self.assertEqual(shvar.load([]), ([], {}))
        self.assertEqual(shvar.load([" \t\n", "# comment\n", "a=1"]), ([(None, None, ""), (None, None, "# comment"), ('a', "1", "")], {'a': "1"}))
        self.assertEqual(shvar.load(["a=1\n", "a=2\n"]), ([('a', "1", ""), ('a', "2", "")], {'a': "2"}))
    
    def load_empty_helper(self, right_part):
        l, d = shvar.load(["a=%s\n" % right_part])
        self.assertEqual(d['a'], "")
    
    def test_load_empty(self):
        self.load_empty_helper("")
        self.load_empty_helper("''")
        self.load_empty_helper('""')
        self.load_empty_helper("$''")
    
    def load_variable_name_helper(self, varname):
        l, d = shvar.load(["%s=\n" % varname])
        self.assert_(varname in d)
    
    def test_load_variable_name(self):
        self.load_variable_name_helper("a")
        self.load_variable_name_helper("_a")
        self.load_variable_name_helper("a_")
        self.load_variable_name_helper("a1")
        self.load_variable_name_helper("a_a")
        self.load_variable_name_helper("a_1")
        self.load_variable_name_helper("__")
    
    def test_load_forbidden_names(self):
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["IFS=42\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["_=42\n"])
    
    def test_load_unsup(self):
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=`echo lol`\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$(echo lol)\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=~homesweethome\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=abc:~homesweethome\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$abc\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=${abc}\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$[1+1]\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$((1+1))\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ['a=$"to be translated"\n'])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$-\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$$\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$_\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$1\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ['a="$@"\n'])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ['a="$*"\n'])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ['a="$?"\n'])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$!\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$#\n"])
    
    def test_load_unsup_continued_line(self):
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=abc\\\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ['a="abc\n'])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a='abc\n"])
        self.assertRaises(shvar.UnsupportedAssignmentError, shvar.load, ["a=$'abc\n"])
    
    def load_value_helper(self, right_part, result):
        l, d = shvar.load(["a=%s\n" % right_part])
        self.assertEqual(d['a'], result)
    
    def test_load_tilde_ok(self):
        self.load_value_helper("a~b", "a~b")
        self.load_value_helper("a\\:~b", "a:~b")
        self.load_value_helper("a:''~b", "a:~b")
        self.load_value_helper("a:\"\"~b", "a:~b")
        self.load_value_helper("a:$''~b", "a:~b")
    
    def test_load_not_special(self):
        self.load_value_helper('"$"', "$")
        self.load_value_helper('"\\a"', "\\a")
        self.load_value_helper("'\\'", "\\")
    
    def test_load_compositing(self):
        self.load_value_helper("abc\\ \"bla bla\"'\tkikoo'$'\\x26'", "abc bla bla\tkikoo&")

# TODO more tests

unittest.main()
