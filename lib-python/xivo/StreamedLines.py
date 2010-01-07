#!/usr/bin/python
"""Generator that buffers raw bytes from multiple pipes/sockets and yield lines

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

import os
import fcntl
import select

try:
    any
except NameError:
    def any(l):
        for el in l:
            if el:
                return True
        return False

def find(seq, f):
    """
    Return the first element el of seq for which f(el) is true, or
    None if f(el) is not true for each elements of seq.
    """
    for el in seq:
        if f(el):
            return el

def only_in(r, seq):
    """
    Return False if 'seq' contains at least one element that is not equal
    to 'r', else True.
    """
    for el in seq:
        if r != el:
            return False
    else:
        return True

class Error(Exception):
    "Base class of exceptions raised by this module."
    def __init__(self, msg, *opt):
        if not opt:
            self.__reprmsg = "<%s %r>" % (self.__class__.__name__, msg)
            self.__strmsg = str(msg)
        else:
            self.__reprmsg = "<%s %r: %s>" % (self.__class__.__name__, msg, ', '.join(map(repr, opt)))
            self.__strmsg = "%s: %r" % (msg, opt)
        Exception.__init__(self, msg)
    def __repr__(self):
        return self.__reprmsg
    def __str__(self):
        return self.__strmsg

class InvalidParam(Error):
    "Raised by rxStreamedLines when called with invalid parameters."
    pass

class RestartableException(Error):
    """
    Base class of exceptions raised by rxStreamedLines when it is possible
    to restart it later (see also the doc of rxStreamedLines).
    """
    def __init__(self, msg, *ctx):
        self.__ctx = ctx
        Error.__init__(self, msg, *ctx)
    def context(self):
        return self.__ctx

class Timeout(RestartableException):
    """
    Restartable Exception raised by rxStreamedLines when the timeout
    initially passed in the timeout arg of rxStreamedLines expires (no
    data has been received during this period of time while in
    rxStreamedLines).
    """
    pass

def makeNonBlocking(fobj):
    "Set the file descriptor behind fobj as non-blocking"
    fl = fcntl.fcntl(fobj, fcntl.F_GETFL)
    fcntl.fcntl(fobj, fcntl.F_SETFL, fl | os.O_NONBLOCK)

# warning: the rxStreamedLines() generator is not extremely
# scalable, do not use it for hundred of streams.
def rxStreamedLines(fobjs=None, timeout=None, ctx=None):
    """
    rxStreamedLines() is a generator function that iterates over lines
    received on multiple Python unbuffered non-blocking selectable file
    objects - suitable to be used through the standard Python select
    module.

    A file object can be made non-blocking using the makeNonBlocking()
    function of this module.

    NOTE: under Unix, Python file objects wrapping pipes, sockets, ttys,
    and some char devices are acceptable if open as unbuffured then
    configured as non-blocking, while under Windows only sockets are
    allowed.  See the manual of the Python select module for more details.

    There are two ways to call this generator function:

    * for lines in rxStreamedLines(fobjs, timeout):
        print lines

     - fobjs is a sequence of Python unbuffured non-blocking selectable
      file objects.  You can pass None instead of a file object, in which
      case None will always be yielded in the corresponding iteration
      output.
     - timeout is an optional timeout in seconds; it is reset at each
      iteration and when new data arrives on any file descriptor (not
      necessarily a complete line - any data do reset it); it is suspended
      when the code flow is outside of the blocking code implementing this
      iterator; when it triggers, a Timeout exception is raised and the
      exception instance contains the abstract context that you can pass
      back by calling this generator function once again to transparently
      resume operations.

    * assert isinstance(x, RestartableException)
      resume_ctx = x.context()
      del x
      for lines in rxStreamedLines(ctx=resume_ctx):
        print lines

     In this second form, a RestartableException (which x is the instance
     of) has previously been raised by a previous iteration over an
     iterator generated by rxStreamedLines().  For now, the only
     RestartableException class is Timeout.  It is advised to drop every
     references to the exception instance as soon as possible to allow the
     Python context of the function which raised the exception (body of
     rxStreamedLines() ) to be freed.

    During iterations, tuples are yielded in which the nth element
    correspond to the nth element in fobjs.  Each element of yielded tuples
    is a string or None.  When it is a string, this is the next line
    available on the corresponding file object, and it usually contains a
    unique '\\n' which ends it.  Lines are yielded as soon as they are
    available, one at a time for a given file object.  There are two cases
    where a yielded string element does not contain a unique '\\n' that
    ends it:
     - if a stream is not terminated by a '\\n' as its last character
      (emitted by the peer before breaking the pipe or ending the socket
      connection, for example) then its last correponding yielded string
      will contain the last chunk of consecutive data received from the
      peer without any '\\n' in it;
     - when EOF is received on a stream, it is represented as soon as
      possible (in order with regular lines and last chunk for the same
      stream) as an empty string element of the yielded tuple (only once).
    When an element of the yielded tuple is None, this can mean one of the
    following:
     - the corresponding element in the fobjs parameter is None;
     - an other element of the yielded tuple is a string, but nothing
      happened on the file object corresponding to the None (neither
      arrival of enough data to yield one complete line nor occurrence of
      EOF) in the period between the last event on it and the simultaneous
      or following event on a string's corresponding file object which was
      associated with the data conveyed in the yielded string element;
     - an EOF on the corresponding file object had previously been reported
      by yielding '' for this element.

    One can sequence the two call form in something like the following, for
    example:

    fobjs = (child1_stdout_pipe_read_side, child1_stderr_pipe_read_side)
    timeout = 42
    resume = None
    while fobjs or resume:
            try:
                    for lines in rxStreamedLines(fobjs, timeout, resume):
                            handle_lines(lines)
            except RestartableException, x:
                    handle_exceptional_condition(x)
                    fobjs, timeout, resume = None, None, x.context()
                    del x
            else:
                    fobjs, timeout, resume = None, None, None
    """
    if ((fobjs is None) and (ctx is None)) or \
       ((ctx is not None) and ((fobjs is not None) or (timeout is not None))):
        raise InvalidParam("bad arguments", fobjs, timeout, ctx)

    if ctx is None:
        initial_fobjs = list(fobjs)
        current_fobjs = initial_fobjs[:]
        buffers = [ [] for fobj in initial_fobjs ]
        eof_status = [ (False, None)[fobj is None] for fobj in initial_fobjs ]
    else:
        # -~= Xref 1 (a) =~-
        # The target list of the following assignment must match the
        # second arg upto the last one of Timeout instantiation
        # in Xref 1 (b)
        timeout, initial_fobjs, current_fobjs, buffers, eof_status = ctx

    while not only_in(None, eof_status):

        select_fobjs = [ fobj for fobj in current_fobjs if fobj ]

        if not select_fobjs:
            rlist = []
        elif timeout is None:
            rlist = select.select(select_fobjs, (), ())[0]
        else:
            rlist = select.select(select_fobjs, (), (), timeout)[0]

        if select_fobjs and not rlist:
            # -~= Xref 1 (b) =~-
            # Second arg upto the last one of Timeout instantiation
            # must match the target list of the assignment
            # in Xref 1 (a)
            raise Timeout(
                    "Timed out while waiting for data from %d fds" % len(select_fobjs),
                    timeout, initial_fobjs, current_fobjs, buffers, eof_status)

        for p, fobj in enumerate(current_fobjs):
            if fobj not in rlist:
                continue
            block = fobj.read()
            if not block:
                current_fobjs[p] = None
                eof_status[p] = True
            else:
                buffers[p].append(block)

        send = True
        while send:
            send = False
            ylines = [None] * len(buffers)
            for p, blocks in enumerate(buffers):

                nl = find(((bp, block, block.find('\n')) for (bp, block) in enumerate(blocks)),
                          lambda (bp, block, nlpos): nlpos > -1)
                if nl is None:
                    if current_fobjs[p]:
                        continue
                    elif blocks:
                        bp = len(blocks) - 1
                        block = blocks[bp]
                        nlp = len(block) - 1
                    elif eof_status[p]:
                        ylines[p] = ''
                        eof_status[p] = None
                        send = True
                        continue
                    else:
                        continue
                else:
                    bp, block, nlp = nl

                yblk = blocks[:bp]
                yblk.append(block[:nlp + 1])
                ystr = ''.join(yblk)

                remblk = [block[nlp + 1:]]
                if remblk == ['']:
                    remblk = []
                remblk.extend(blocks[bp + 1:])

                ylines[p] = ystr
                buffers[p] = remblk

            if send or any(ylines):
                send = True
                yield tuple(ylines)

if __name__ == '__main__':
    def testfunc():
        import subprocess, time, sys
	# pylint: disable-msg=W0333
        LEN_LAW = [(127, 1), (120, 16), (0, 0), (32, 64), (31, 137), (2039, 0)]
        def gen_lines(len_law):
            sz, b = 0, 0
            for nb, step in len_law:
                if step == 0:
                    if nb == 0:
                        yield ''
                    else:
                        yield chr(b % 95 + 32) * nb
                        return
                for i in xrange(nb): # pylint: disable-msg=W0612
                    yield chr(b % 95 + 32) * sz + '\n'
                    b += sz + 1
                    sz += step
        TEST_LINES = list(gen_lines(LEN_LAW))
        if len(sys.argv) == 1:
            child = subprocess.Popen(
                    (sys.argv[0], '--child'),
                    bufsize = 0,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    close_fds=True)
            makeNonBlocking(child.stdout)
            makeNonBlocking(child.stderr)
            fobjs = (child.stdout, child.stderr)
            nbf = 2 # len(fobjs), state machine in test loop bellow does not
                # support more than 2
            rcvd_eof = [False, False]
            timeout = 1
            resume = None
            def print_one(s):
                if s is None:
                    print '<None>',
                elif not s:
                    print '<EOF>',
                elif len(s) <= 10:
                    print s.strip(),
                else:
                    print s[:10] + "...%d" % len(s),
            nb = 0
            start = 0
            expect = iter(TEST_LINES)
            eofs = 0
            line_iter = None
            try:
                line_iter = rxStreamedLines(fobjs, timeout, resume)
                while fobjs or resume:
                    try:
                        for lines in line_iter:
                            nstart = start
                            ST = 0
                            for x in range(start, nbf) \
                                   + range(0, start):
                                nstart = x
                                if ST == 0:
                                    assert lines[x] is not None, `lines`
                                    ST = 1
                                elif ST == 1:
                                    if lines[x] is None:
                                        break
                                if eofs:
                                    assert not lines[x], `lines[x]`
                                if lines[x] == '':
                                    assert rcvd_eof[x] == False
                                    eofs = 1
                                    rcvd_eof[x] = True
                                try:
                                    l = expect.next()
                                    nb += 1
                                except StopIteration:
                                    assert eofs
                                else:
                                    assert not eofs
                                    assert l == lines[x], `(l, lines[x])`
                            else:
                                nstart = start
                            start = nstart
                            print_one(lines[0])
                            print_one(lines[1])
                            print
                            if nb == 59 or nb == 126:
                                time.sleep(1.-1/128.)
                    except RestartableException, x:
                        l = expect.next()
                        assert l == '', `l`
                        assert isinstance(x, Timeout)
                        print "############ RestartableException #############"
                        fobjs, timeout, resume = None, None, x.context()
                        del x
                        line_iter = rxStreamedLines(fobjs, timeout, resume)
                    else:
                        fobjs, timeout, resume = None, None, None
            except AssertionError:
                for lines in line_iter:
                    print "***", lines
                raise
            else:
                assert rcvd_eof == [True, True], `rcvd_eof`
                rv = child.wait()
                assert rv == 0
                print "EVERYTHING OK"
        elif len(sys.argv) > 1 and sys.argv[1] == '--child':
            from itertools import izip, islice
            def cut(l, sz):
                r = []
                while l:
                    r.append(l[:sz])
                    l = l[sz:]
                return r
            p = False
            std = (sys.stdout, sys.stderr)
            chunks = [cut(l, int(pow(len(l), 0.70710678118654757)))
                      for l in TEST_LINES]
            for cur, nxt in izip(chunks, islice(chunks, 1, None)):
                if not cur:
                    time.sleep(1.5)
                    continue
                while cur:
                    l = cur.pop(0)
                    std[p].write(l)
                    std[p].flush()
                    time.sleep(1/512.)
                    if cur and len(nxt) > 1:
                        l = nxt.pop(0)
                        std[not p].write(l)
                        std[not p].flush()
                        time.sleep(1/256.)
                p = not p
            while nxt:
                l = nxt.pop(0)
                std[p].write(l)
                std[p].flush()
            time.sleep(1/256.)
    testfunc()

__all__ = [
    'rxStreamedLines', 'makeNonBlocking',
    'Error', 'InvalidParam', 'RestartableException', 'Timeout'
]
