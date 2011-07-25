# vim: set fileencoding=utf-8 :
# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XiVO CTI Server is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XiVO CTI Server
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import csv
import logging
import urllib
from itertools import chain

log = logging.getLogger('directories_csv')

def match_dict(t, lookup_list, searchpattern):
    match = False
    for k in lookup_list:
        if t.get(k):
            if t.get(k).lower().find(searchpattern.lower()) >= 0:
                match = True
                break
    return match

def csv_extractlines(uri, match_list, searchpattern, delimiter):
    d = csv.excel
    d.delimiter = delimiter
    # d.doublequote

    f = urllib.urlopen(uri)
    csvreader = csv.DictReader(f, dialect = d)
    first_line = csvreader.next() # needed in python <= 2.5 in order to fill csvreader.fieldnames
    lookup_list = list()
    for fieldname in csvreader.fieldnames:
        if fieldname in match_list:
            lookup_list.append(fieldname)

    allmatches = []
    if lookup_list:
        for t in chain([first_line], csvreader):
            match = match_dict(t, lookup_list, searchpattern)
            if match:
                allmatches.append(t)
    else:
        log.warning('%s : csv file fieldnames (%s) and match_list (%s) do not cross'
                    % (uri, csvreader.fieldnames, match_list))
    f.close()
    return allmatches

def lookup(searchpattern, uri, matchlist, tofill_dict, delimiter, dirname):
    futurelist = list()
    matches = csv_extractlines(uri, matchlist, searchpattern, delimiter)
    for am in matches:
        futureitem = { 'xivo-directory' : dirname }
        for k, vs in tofill_dict.iteritems():
            for v in vs:
                if not futureitem.get(k):
                    futureitem[k] = am.get(v)
        futurelist.append(futureitem)
    if not futurelist:
        log.warning('lookup : found no match for %s in %s' % (searchpattern, uri))
    return futurelist
