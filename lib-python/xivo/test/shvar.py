#!/usr/bin/python

"""Tests for shvar

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
        self.load_value_helper("a#b", "a#b")
        self.load_value_helper('"a"#b', "a#b")
    
    def test_load_compositing(self):
        self.load_value_helper("abc\\ \"bla bla\"'\tkikoo'$'\\x26'", "abc bla bla\tkikoo&")
    
    def test_strip_overridden_assignments(self):
        self.assertEqual(shvar.strip_overridden_assignments([]), [])
        
        self.assertEqual(shvar.strip_overridden_assignments([("abc", "def", "")]), [("abc", "def", "")])
        
        param = [
            ("abc", "def1", ""),
            ("ghi", "def2", ""),
        ]
        expect = param[:]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)
        
        param = [
            (None, None, "# lol"),
            ("abc", "def1", ""),
            ("ghi", "def2", ""),
        ]
        expect = param[:]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)
        
        param = [
            (None, None, "# lol"),
            ("abc", "def1", ""),
            ("ghi", "def2", ""),
        ]
        expect = param[:]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)
        
        param = [
            ("abc", "def_rm", ""),
            ("abc", "def", ""),
        ]
        expect = param[-1:]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)
        
        param = [
            (None, None, ""),
            ("abc", "def_rm", ""),
            ("abc", "def", ""),
        ]
        expect = param[::2]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)
        
        param = [
            ("abc", "def_rm", ""),
            ("abc", "def", ""),
            (None, None, ""),
        ]
        expect = param[-2:]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)

        param = [
            (None, None, ""),
            ("abc", "def_rm", ""),
            ("abc", "def", ""),
            (None, None, ""),
        ]
        expect = param[:1] + param[-2:]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)

        param = [
            (None, None, ""),
            ("abc", "def1_rm", ""),
            ("abc", "def1", ""),
            ("ghi", "def2_rm", ""),
            ("ghi", "def2", ""),
            (None, None, ""),
        ]
        expect = param[::2] + param[-1:]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)

        param = [
            (None, None, ""),
            ("abc", "def1_rm", ""),
            ("ghi", "def2_rm", ""),
            ("ghi", "def2", ""),
            ("klm", "def3_rm", ""),
            ("klm", "def3", ""),
            ("abc", "def1", ""),
            (None, None, ""),
        ]
        expect = [
            (None, None, ""),
            ("ghi", "def2", ""),
            ("klm", "def3", ""),
            ("abc", "def1", ""),
            (None, None, ""),
        ]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)

        param = [
            (None, None, "# c1"),
            ("abc", "def1_rm", ""),
            ("ghi", "def2_rm", ""),
            ("ghi", "def2", ""),
            (None, None, "# c2"),
            ("klm", "def3_rm", ""),
            ("klm", "def3", ""),
            ("abc", "def1", ""),
            (None, None, "# c3"),
        ]
        expect = [
            (None, None, "# c1"),
            ("ghi", "def2", ""),
            (None, None, "# c2"),
            ("klm", "def3", ""),
            ("abc", "def1", ""),
            (None, None, "# c3"),
        ]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)

        param = [
            (None, None, ""),
            ("abc", "def1_rm", ""),
            ("a1", "1", ""),
            ("ghi", "def2_rm", ""),
            ("a2", "2", ""),
            ("ghi", "def2", ""),
            ("a3", "3", ""),
            ("klm", "def3_rm", ""),
            ("a4", "4", ""),
            ("klm", "def3", ""),
            ("a5", "5", ""),
            ("abc", "def1", ""),
            ("a6", "6", ""),
            (None, None, ""),
        ]
        expect = [
            (None, None, ""),
            ("a1", "1", ""),
            ("a2", "2", ""),
            ("ghi", "def2", ""),
            ("a3", "3", ""),
            ("a4", "4", ""),
            ("klm", "def3", ""),
            ("a5", "5", ""),
            ("abc", "def1", ""),
            ("a6", "6", ""),
            (None, None, ""),
        ]
        self.assertEqual(shvar.strip_overridden_assignments(param), expect)
    
    def escape_helper(self, to_code):
        line = "A=" + shvar.escape(to_code) + "\n"
        reslst, resdct = shvar.load([line])
        self.assertEqual(resdct['A'], to_code)
    
    def test_escape(self):
        self.escape_helper("")
        self.escape_helper(" ")
        self.escape_helper("abc")
        self.escape_helper(" abc")
        self.escape_helper("abc ")
        self.escape_helper(" abc ")
        self.escape_helper("abc def")
        self.escape_helper(" abc def")
        self.escape_helper("abc def ")
        self.escape_helper(" abc def ")
        self.escape_helper("\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f")
        self.escape_helper('"')
        self.escape_helper("'")
        self.escape_helper("\xe9")
        self.escape_helper(''.join((chr(c) for c in xrange(32, 127))))
        self.escape_helper(''.join((chr(c) for c in xrange(1, 256))))
    
    def format_helper(self, initial_content):
        # 1) parse
        reslst, resdct = shvar.load(initial_content)
        # 2) format
        new_content = list(shvar.format(reslst))
        # 3) re parse
        new_reslst, new_resdct = shvar.load(new_content)
        # 4) check
        self.assertEqual([(a,b) for a,b,c in reslst if a], [(a,b) for a,b,c in new_reslst if a])
    
    def test_format(self):
        self.format_helper([
            "A=\n",
        ])
        self.format_helper([
            "A=1\n",
            "B=a\n",
        ])
        self.format_helper([
            "# comment 1\n",
            "A=1 # comment 2\n",
            "B=a#b\n",
        ])
        self.format_helper([
            'A="\xe9"\n',
        ])


unittest.main()
