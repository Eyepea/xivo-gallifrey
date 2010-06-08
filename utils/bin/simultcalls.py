#!/usr/bin/python
# -*- coding: utf-8 -*-

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2009-2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import cjson
import csv
import getopt
import os
import sys
import time
import urllib

def zero_seconds(val):
    global histo_seconds
    if val not in histo_seconds:
        histo_seconds[val] = 0

def zero_minutes(val):
    global histo_minutes
    if val not in histo_minutes:
        histo_minutes[val] = 0

def gettimelength(start, length, chan1, chan2, timefmt):
    cond = True
    ret = []
    if sc_channel:
        cond = (chan1.startswith(sc_channel) or chan2.startswith(sc_channel))
    if cond:
        try:
            tt = time.strptime(start, timefmt)
            ret = [int(time.strftime('%s', tt)), int(length)]
        except Exception, exc:
            print '--- exception (%s) : %s ---' % (exc, start)
            ret = []
    return ret

sc_mode = None
sc_channel = None
sc_begin = None
sc_end = None

sc_image = False
sc_ps = False
sc_x11 = False
sc_keep = False
sc_help = False

infile_or_ip = None

GETOPT_SHORTOPTS = 'm:c:b:e:ipxkh'
GETOPT_LONGOPTS = ['mode=', 'channel=', 'begin=', 'end=', 'image', 'ps', 'x11', 'keep', 'help']
try:
    opts = getopt.gnu_getopt(sys.argv[1:],
                             GETOPT_SHORTOPTS,
                             GETOPT_LONGOPTS)
except getopt.GetoptError, exc:
    opts = ([], [])

for opt, arg in opts[0]:
    if opt in ['-m', '--mode']:
        sc_mode = arg
    elif opt in ['-c', '--channel']:
        sc_channel = arg
    elif opt in ['-b', '--begin']:
        sc_begin = arg
    elif opt in ['-e', '--end']:
        sc_end = arg
        
    elif opt in ['-i', '--image']:
        sc_image = True
    elif opt in ['-p', '--ps']:
        sc_ps = True
    elif opt in ['-x', '--x11']:
        sc_x11 = True
    elif opt in ['-k', '--keep']:
        sc_keep = True
    elif opt in ['-h', '--help']:
        sc_help = True
        
if opts[1]:
    infile_or_ip = opts[1][0]

if not infile_or_ip or not sc_mode or sc_help:
    codename = 'simultcalls.py' # sys.argv[0]
    print
    print 'Usage : you should use this tool in either mode :'
    print '  %s [options] -m file xivo_cdr_XXX.csv (exported CSV)' % codename
    print '  %s [options] -m ip   192.168.0.4      (web service request)' % codename
    print
    print 'where [options] can be :'
    print '   -c <channel>    : the first characters of a channel name (DAHDI, SIP, IAX)'
    print '   -b <begin date> : in "ip" mode, the begin date,'
    print '                     format like 2009-11-27 (default is current day)'
    print '   -e <end date>   : in "ip" mode, the end date,'
    print '                     format like 2009-11-27 (default is current day)'
    print '   -i              : outputs a PNG graph under simultcalls_%Y%m%d_%H%M%S.png'
    print '   -p              : outputs a PostScript graph under simultcalls_%Y%m%d_%H%M%S.ps'
    print '   -x              : displays very briefly an X11 graph'
    print '   -k              : keep the temporary data files for further processing'
    print '   -h              : this help'
    print
    if not infile_or_ip or not sc_mode:
        sys.exit(1)

mystats = []
now = time.strftime('%Y%m%d_%H%M%S', time.localtime())
today = time.strftime('%Y-%m-%d', time.localtime())
t1 = time.time()
nl = 0

if sc_mode == 'ip':
    # 'webservice' mode
    remoteip = infile_or_ip
    mode = 'restricted'
    if remoteip in ['127.0.0.1', 'localhost']:
        mode = 'private'
    print 'request to %s' % remoteip
    if not sc_begin:
        sc_begin = today
    if not sc_end:
        sc_end = today
    title = '%s from %s to %s' % (remoteip, sc_begin, sc_end)
    w = urllib.urlopen('https://%s/service/ipbx/json.php/%s/call_management/cdr/?act=search&dbeg=%s&dend=%s'
                       % (remoteip, mode, sc_begin, sc_end))
    wr = w.readlines()
    if wr:
        for z in cjson.decode(''.join(wr)):
            nl += 1
            start = z.get('calldate')
            length = z.get('duration')
            chan1 = z.get('channel')
            chan2 = z.get('dstchannel')
            gtl = gettimelength(start, length, chan1, chan2, '%Y-%m-%d %H:%M:%S')
            if gtl:
                mystats.append(gtl)
    else:
        print 'It seems the Web Service returned an empty result :'
        print ' - did you give some meaningful begin and end dates ? (%s -> %s)' % (sc_begin, sc_end)
        print ' - are you on a sufficiently high XiVO version (1.0 or greater) ?'
        sys.exit(1)
        
elif sc_mode == 'file':
    # file/url mode
    if infile_or_ip.startswith('http:'):
        f = urllib.urlopen(infile_or_ip)
        title = 'url : %s' % infile_or_ip
    else:
        f = open(infile_or_ip)
        title = 'file : %s' % infile_or_ip
    print 'reading csv file from %s' % infile_or_ip
    csvreader = csv.reader(f, delimiter = ';')
    for line in csvreader:
        nl += 1
        if len(line) == 16:
            [start, length, chan1, chan2] = [line[0], line[3], line[4], line[11]]
            gtl = gettimelength(start, length, chan1, chan2, '%d/%m/%Y %H:%M:%S')
            if gtl:
                mystats.append(gtl)
    f.close()

else:
    print 'unknown mode "%s" - exiting' % sc_mode
    sys.exit(1)

t2 = time.time()
print '... (%.1f seconds spent for %d items)\n' \
      'filling histogram' % (t2-t1, len(mystats))

if not mystats:
    print 'no data found matching your channel "%s" - exiting' % sc_channel
    sys.exit(1)

# mystats is filled now

starttime_seconds = mystats[0][0]
starttime_minutes = mystats[0][0]/60
histo_seconds = {}
histo_minutes = {}

# fill the seconds' bins
for st in mystats:
    time_s = st[0] - starttime_seconds
    deltatime_s = st[1]
    for ttime_s in xrange(time_s, time_s + deltatime_s + 1):
        if ttime_s in histo_seconds:
            histo_seconds[ttime_s] = histo_seconds[ttime_s] + 1
        else:
            histo_seconds[ttime_s] = 1
    zero_seconds(time_s - 1)
    zero_seconds(time_s + deltatime_s + 1)

t3 = time.time()
print '... (%.1f seconds spent, %d bins filled)\n' \
      'sorting and writing to files' % (t3-t2, len(histo_seconds))

datfilename_s = '/tmp/stat_seconds_%s.dat' % now
datfilename_m = '/tmp/stat_minutes_%s.dat' % now
datfile_s = open(datfilename_s, 'w')
datfile_m = open(datfilename_m, 'w')

# fill the minutes' bins and keep data around the maximal values
max_s_value = 0
max_s_list = []
hsk = histo_seconds.keys()
hsk.sort()
for h in hsk:
    ttime = starttime_seconds + h
    h0 = histo_seconds[h]
    time_min = ttime/60 - starttime_minutes
    if time_min in histo_minutes:
        histo_minutes[time_min] = histo_minutes[time_min] + h0 / 60.0
    else:
        histo_minutes[time_min] = h0 / 60.0
        
    if h0 == max_s_value:
        for hbin in [h-1, h, h+1]:
            if hbin not in max_s_list:
                max_s_list.append(hbin)
    if h0 > max_s_value:
        max_s_value = h0
        max_s_list = [h-1, h, h+1]
        
    zero_minutes(time_min - 1)
    zero_minutes(time_min + 1)
    strtime = time.strftime('%d/%m/%Y,%H:%M:%S', time.localtime(ttime))
    datfile_s.write('%s %d\n' % (strtime, h0))
datfile_s.close()

# display a (possibly) short summary
print 'max simultaneous calls is %s - values around max are :' % (max_s_value)
for mm in max_s_list:
    h = mm
    ttime = starttime_seconds + h
    strtime = time.strftime('%d/%m/%Y,%H:%M:%S', time.localtime(ttime))
    print ' %s %10d calls' % (strtime, histo_seconds[h])

hmk = histo_minutes.keys()
hmk.sort()
for h in hmk:
    ttime = (starttime_minutes + h) * 60
    h0 = histo_minutes[h]
    strtime = time.strftime('%d/%m/%Y,%H:%M', time.localtime(ttime))
    datfile_m.write('%s %.1f\n' % (strtime, h0))
datfile_m.close()

t4 = time.time()
print '... (%.1f seconds spent)' % (t4-t3)

# make plots if requested
pstmpplot = None
if sc_ps or sc_x11 or sc_image:
    import Gnuplot
    g = Gnuplot.Gnuplot()
    g('set timestamp')
    g('set title "%s"' % title)
    g('set xdata time')
    g('set timefmt "%d/%m/%Y,%H:%M:%S"')
    
    g('set terminal postscript color solid')
    pstmpplot = '/tmp/simultcalls_%s.ps' % now
    g('set output "%s"' % pstmpplot)
    
    g('plot '
      '"%s" using 1:2 with lines title "Nombre d\'Appels (s)" 1,'
      '"%s" using 1:2 with lines title "Nombre d\'Appels (min)" 3'
      % (datfilename_s, datfilename_m))
    
    if sc_ps:
        g('set terminal postscript color solid')
        g('set output "simultcalls_%s.ps"' % now)
        g('replot')
    if sc_x11:
        g('set terminal x11')
        g('replot')
    if sc_image:
        g('set terminal png')
        g('set output "simultcalls_%s.png"' % now)
        g('replot')
    g.close()

# remove temporary files if not requested otherwise
if not sc_keep:
    os.unlink(datfilename_s)
    os.unlink(datfilename_m)
    if pstmpplot:
        os.unlink(pstmpplot)
