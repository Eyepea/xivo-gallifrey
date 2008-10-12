#!/usr/bin/python
"""
GCC warning messages filter
"""

# TODO:
#   Implement reverse check; generate our own warning when we should
#   have received one from gcc but we have not.  This feature must be
#   optional with a command line flag.

__version__ = "$Revision$ $Date$"

import os
import re
import sys
from subprocess import Popen, PIPE


src_search_ref = re.compile(r"WWW \[(\d+)\]").search
src_search_def = re.compile(r"\[(\d+)\]:(.*)$").search

match_gcc_warning = re.compile(r"[-A-Za-z0-9_.+/]*:(\d+): warning:(.*)$").match
match_gcc_notpre = re.compile(r"[-A-Za-z0-9_.+/]+:\d+:").match


class UsageError(Exception):
    "Raised by gcc_with_warning_removal() when called with invalid args"
    def __init__(self, errormsg):
        self.errormsg = errormsg


def usage_abort(e):
    """
    Print an usage help screen and abort program execution
    """
    if e and e.errormsg:
        print >> sys.stderr, e.errormsg
    print >> sys.stderr, "usage: %s source.c --- [compiler_command]"
    sys.exit(1)


def parse(src):
    """
    Parse the source code, looking for reference patterns (src_search_ref)
    and definition patterns (src_search_def)
    """
    refs = {}
    defs = {}
    for lineno, line in enumerate(src):
        line = line.strip()
        ref = src_search_ref(line)
        df = src_search_def(line)
        if ref:
            ref_num = int(ref.group(1))
            refs[lineno + 1] = [ref_num]
        elif df:
            ref_num = int(df.group(1))
            text = df.group(2)
            defs[ref_num] = text.strip()
    return refs, defs


class GccWarningLineFilter(object):
    """
    XXX
    """
    def __init__(self, refs, defs):
        self.refs = refs
        self.defs = defs
        self._plines = []
        self._plines_closed = False

    def _preline(self, line):
        """
        XXX
        """
        if    line.startswith("In file included from ") \
           or line.startswith("                 from ") \
           or "In function " in line \
           or "At top level" in line \
           or not match_gcc_notpre(line):
            if self._plines_closed:
                self._plines = []
                self._plines_closed = False
            self._plines.append(line)
            return True
        else:
            self._plines_closed = True
            return False

    def _print_prelines(self, line):
        """
        XXX
        """
        for l in self._plines:
            print >> sys.stderr, l
        print >> sys.stderr, line
        self._plines = []
        self._plines_closed = False

    def flush(self):
        """
        XXX
        """
        if not self._plines_closed:
            for l in self._plines:
                print >> sys.stderr, l

    def push(self, line):
        """
        XXX
        """
        if not self._preline(line):
            w = match_gcc_warning(line)
            if not w:
                self._print_prelines(line)
            else:
                try:
                    lineno = int(w.group(1))
                    for ref in self.refs[lineno]:
                        df = self.defs.get(ref)
                        if df in line:
                            break # remove gcc warning
                    else:
                        self._print_prelines(line)
                except (KeyError, TypeError, ValueError, IndexError):
                    self._print_prelines(line)


def run_gcc_filt(cmd, filt):
    """
    XXX
    """
    c = Popen(cmd, bufsize=1, stdin=None, stdout=None, stderr=PIPE)
    while True:
        line = c.stderr.readline()
        if not line:
            break
        filt.push(line.rstrip())
    rval = c.wait()
    filt.flush()
    return rval


def gcc_with_warning_removal(args):
    """
    args: [source_filepath, "---", <compiler_command>]
    
    Run the <compiler_command> filtering its stderr
    """
    if len(args) < 3 or args[1] != "---":
        raise UsageError(None)
    source_filepath = args[0]
    compiler_cmd = args[2:]
    
    try:
        src = open(source_filepath)
    except IOError, x:
        raise UsageError(str(x))
    
    refs, defs = parse(src)
    src.close()
    
    filt = GccWarningLineFilter(refs, defs)
    return run_gcc_filt(compiler_cmd, filt)


def main():
    "main program"
    try:
        rcode = gcc_with_warning_removal(sys.argv[1:])
    except UsageError, exc:
        usage_abort(exc)
    if rcode >= 0:
        sys.exit(rcode)
    else:
        os.kill(os.getpid(), -rcode)
    

if __name__ == '__main__':
    main()
