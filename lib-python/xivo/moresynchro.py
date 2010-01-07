"""Supplementary synchronization primitives not provided by 'threading'

Copyright (C) 2007-2010  Proformatique

- RWLock                simple implementation with timeouts, without promotion

    Highly inspired from
        http://code.activestate.com/recipes/502283/
    which does promotion, and behave differently on timeouts / failures
    (raising exceptions)

    This code contains portions of the one linked above, which is :
        Copyright (C) 2007, Heiko Wundram.
        Released under the BSD-license.

- ListLock              simple dictionary used to atomically test and insert
                        implemented locks are reentrant

    Useful to lock basing on dynamic symbolic identifiers instead of
    already known computer objects.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2010  Proformatique

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

import time
import thread
import threading

class RWLock:
    """
    Simple RWLock with timeouts, without promotion.

    Writers are always preferred by this implementation: if there are
    blocked threads waiting for a write lock, current readers may request
    more read locks (which they eventually should free, as they starve the
    waiting writers otherwise), but a new thread requesting a read lock
    will not be granted one, and block. This might mean starvation for
    readers if two writer threads interweave their calls to acquireWrite()
    without leaving a window only for readers.

    Maybe optimisations could be done by adding another condition to
    distinguish between waiting readers and waiting writers?
    """

    def __init__(self):
        self.__condition = threading.Condition(threading.Lock())
        self.__readers = {}
        self.__writer_lock_count = 0
        self.__writer = None
        self.__pending_writers = []

    def acquire_read(self, timeout=None):
        """
        Acquire a read lock for the current thread, waiting at most
        timeout seconds or doing a non-blocking check in case timeout
        is <= 0.

        In case timeout is None, the call to acquire_read blocks until
        the lock request can be serviced.

        If the lock has been successfully acquired, this function
        returns True, on a timeout it returns None.
        """
        if timeout is not None:
            endtime = time.time() + timeout
        me = threading.currentThread()
        self.__condition.acquire()
        try:
            if self.__writer is me:
                self.__writer_lock_count += 1
                return True
            while True:
                if self.__writer is None:
                    if self.__pending_writers:
                        if me in self.__readers:
                            # Grant the lock anyway if we already
                            # hold one, because this would otherwise
                            # cause a deadlock between the pending
                            # writers and ourself.
                            self.__readers[me] += 1
                            return True
                        # else: does nothing, will wait below
                        # writers are given priority
                    else:
                        self.__readers[me] = self.__readers.get(me, 0) + 1
                        return True
                if timeout is not None:
                    remaining = endtime - time.time()
                    if remaining <= 0:
                        return None
                    self.__condition.wait(remaining)
                else:
                    self.__condition.wait()
        finally:
            self.__condition.release()

    def acquire_write(self, timeout=None):
        """
        Acquire a write lock for the current thread, waiting at most
        timeout seconds or doing a non-blocking check in case timeout
        is <= 0.

        In case timeout is None, the call to acquire_write blocks until
        the lock request can be serviced.

        If the lock has been successfully acquired, this function
        returns True. On a timeout it returns None. In case a trivial
        deadlock condition is detected (the current thread already hold
        a reader lock) it returns False.
        """
        if timeout is not None:
            endtime = time.time() + timeout
        me = threading.currentThread()
        self.__condition.acquire()
        try:
            if self.__writer is me:
                self.__writer_lock_count += 1
                return True
            if me in self.__readers:
                # trivial deadlock detected (we do not handle promotion)
                return False
            self.__pending_writers.append(me)
            while True:
                if not self.__readers and self.__writer is None and self.__pending_writers[0] is me:
                    self.__writer = me
                    self.__writer_lock_count = 1
                    self.__pending_writers = self.__pending_writers[1:]
                    return True
                if timeout is not None:
                    remaining = endtime - time.time()
                    if remaining <= 0:
                        self.__pending_writers.remove(me)
                        return None
                    self.__condition.wait(remaining)
                else:
                    self.__condition.wait()
        finally:
            self.__condition.release()

    def release(self):
        """
        Release the currently held lock.

        In case the current thread holds no lock, a thread.error
        is thrown.
        """
        me = threading.currentThread()
        self.__condition.acquire()
        try:
            if self.__writer is me:
                self.__writer_lock_count -= 1
                if self.__writer_lock_count == 0:
                    self.__writer = None
                    self.__condition.notifyAll()
            elif me in self.__readers:
                self.__readers[me] -= 1
                if self.__readers[me] == 0:
                    del self.__readers[me]
                    if not self.__readers:
                        self.__condition.notifyAll()
            else:
                raise thread.error, "release unlocked lock"
        finally:
            self.__condition.release()

class ListLock:

    "This class let us lock on a given object identifier in a set"

    def __init__(self):
        self.lock = threading.Lock()
        self.locked = {}

    def try_acquire(self, elt):
        """
        elt must be hashable.

        If not yet locked, elt is captured for the current thread and
        True is returned.

        If already locked by the current thread its recursive counter
        is incremented and True is returned.

        If already locked by another thread False is returned.
        """
        me = threading.currentThread()
        self.lock.acquire()
        try:
            if elt not in self.locked:
                self.locked[elt] = [me, 1]
                return True
            elif self.locked[elt][0] == me:
                self.locked[elt][1] += 1
                return True
        finally:
            self.lock.release()
        return False

    def release(self, elt):
        """
        elt must be already locked (if it is not a thread.error
        exception will be raised)

        The recursive counter for elt is decremented and when it
        reaches zero the lock is released.
        """
        self.lock.acquire()
        try:
            if elt not in self.locked:
                raise thread.error, "release unlocked lock for %s" % `elt`
            self.locked[elt][1] -= 1
            if not self.locked[elt][1]:
                del self.locked[elt]
        finally:
            self.lock.release()
