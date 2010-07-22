"""Return the current UTC offset and DST rules of arbitrary timezones.
"""

__version__ = "$Revision: 8668 $ $Date: 2010-07-21 10:39:22 -0400 (Wed, 21 Jul 2010) $"
__license__ = """
    Copyright (c) 2010  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import os.path


class TimezoneNotFoundError(Exception):
    pass


class Time(object):
    def __init__(self, raw_seconds):
        self._raw_seconds = raw_seconds
    
    @property
    def as_seconds(self):
        return self._raw_seconds
    
    @property
    def as_minutes(self):
        return self._raw_seconds // 60
    
    @property
    def as_hours(self):
        return self._raw_seconds // 3600
    
    @property
    def as_hms(self):
        """Return the time decomposed into hours, minutes and seconds.
        
        Note that if the time is negative, only the leftmost non-zero value will be
        negative.
        
        >>> Time(3602).as_hms   # 1 hour, 0 minutes and 2 seconds
        [1, 0, 2]
        >>> Time(-3602).as_hms  # -(1 hour, 0 minutes and 2 seconds)
        [-1, 0, 2]
        >>> Time(-2).as_hms
        [0, 0, -2]
        """
        if self._raw_seconds < 0:
            result = self._compute_positive_hms()
            for i in xrange(len(result)):
                if result[i]:
                    result[i] = -result[i]
                    break
            return result
        else:
            return self._compute_positive_hms()
    
    def _compute_positive_hms(self):
        seconds = abs(self._raw_seconds)
        return [seconds // 3600, seconds // 60 % 60, seconds % 60]

    
class TextTimezoneInfoDB(object):
    """Instances of TextTimeZoneInfoDB return timezone information read from a
    text file. The file format is the same as the one created by default for
    the tzdataexport tool.
    """
    
    _TZ_DEFAULT_FILENAME = os.path.join(os.path.dirname(__file__), 'tzinform/tzdatax')
    
    def __init__(self, filename=None):
        if filename is None:
            filename = self._TZ_DEFAULT_FILENAME
        self._read_file(filename)
        
    def _read_file(self, filename):
        fobj = open(filename)
        try:
            self._db = {}
            for line in fobj:
                if line and not line.startswith('#'):
                    name, offset, dst_rule = line.rstrip().split()
                    self._db[name] = {'utcoffset': Time(int(offset)),
                                      'dst': self._parse_dst_rule(dst_rule)}
        finally:
            fobj.close()

    @classmethod
    def _parse_dst_rule(cls, str):
        if str == '-':
            return None
        else:
            tokens = str.split(';')
            return {'start': cls._parse_dst_change(tokens[0]),
                    'end': cls._parse_dst_change(tokens[1]),
                    'save': Time(int(tokens[2])),
                    'as_string': str}
    
    @classmethod
    def _parse_dst_change(cls, str):
        tokens = str.split('/')
        return {'month': int(tokens[0]),
                'day': tokens[1],
                'time': Time(int(tokens[2]))}

    def get_timezone_info(self, timezone_name):
        """Return timezone information for the timezone named timezone_name.
        
        The method returns a dictionary with the following key:
        - 'utcoffset':    the offset from UTC as a Time object
        - 'dst':          a dictionary containing the DST rules, or None if the timezone has no DST
          - 'start'       a dictionary containing the DST start rule
            - 'month'     the month number
            - 'day'       the day. Can be either something like 'D24' or 'W1/6'
            - 'time'      the time of the day, as a Time object
          - 'end'         a dictionary containing the DST end rule
          - 'save'        an offset from standard time as a Time object
          - 'as_string'   the original DST string
        
        Raise a TimezoneNotFoundError is no information for the timezone is
        found.
        """
        try:
            return self._db[timezone_name]
        except KeyError:
            raise TimezoneNotFoundError(timezone_name)


class DefaultTimezoneInfoDB(object):
    """Instances of DefaultTimezoneInfoDB returns timezone information from
    another TimezoneInfoDB, or a default timezone information in the case the
    timezone can't be found.
    
    >>> tz_db = DefaultTimezoneInfoDB('Europe/Paris', TextTimezoneInfoDB())
    >>> tz_db.get_timezone_info('Moon/Sea_of_Tranquility')['utcoffset'].as_hours
    1
    """
    def __init__(self, default_tz, db):
        self.db = db
        self.default = db.get_timezone_info(default_tz)
        
    def get_timezone_info(self, timezone_name):
        try:
            return self.db.get_timezone_info(timezone_name)
        except TimezoneNotFoundError:
            return self.default


get_timezone_info = DefaultTimezoneInfoDB('Europe/Paris', TextTimezoneInfoDB()).get_timezone_info


def week_start_on_monday(weekday):
    """Convert weekday so that monday is the first day of the week (instead of sunday).
    
    >>> week_start_on_monday(1)  # sunday is now the last day of the week
    7
    >>> week_start_on_monday(2)  # ...and monday is the first
    1
    """
    return (weekday - 1 + 6) % 7 + 1


if __name__ == '__main__':
    import doctest
    doctest.testmod(verbose=True)
