#!/usr/bin/python
# vim: set expandtab ts=4 sw=4 sts=4 fileencoding=utf-8:

import sys
import time
from optparse import OptionParser

from xivo import daemonize
from xivo_queuelogger.ami_conn import *
from xivo_queuelogger.ami_logger import *

DAEMONNAME = 'queues-logger'
PIDFILE = '/var/run/%s.pid' % DAEMONNAME

usage = \
'''
%prog [options]

 return code:
 * 0: you typed --help
 * 1: unable to connect to server
 * 2: this server is not a valid AMI server
 * 3: your credential are wrong - verify your login/password
 * 4: *impossible*
'''
def parse_cmd_line():
# {
    parser = OptionParser(usage)

    parser.add_option("-u", "--uri", dest="anysql_uri", type="string",
                      help="an anysql uri", metavar="",
                      default="sqlite3:/var/lib/pf-xivo-queues-logger/sqlite3/queuestat.db")

    parser.add_option("-i", "--ip", dest="ip", type="string",
                      help="monitor AMI ip", metavar="127.0.0.1",
                      default="127.0.0.1")

    parser.add_option("-p", "--port", dest="port", type="int",
                      help="monitor AMI port", metavar="5038", default=5038)

    parser.add_option("-l", "--userlogin", dest="user", type="string",
                      help="AMI user for login", metavar="xivouser",
                      default="xivouser")

    parser.add_option("-x", "--password", dest="password", type="string",
                      help="AMI user password", metavar="xivouser",
                      default="xivouser")

    parser.add_option("-d", "--debug",
                      dest = "debug",
                      action = "store_true",
                      help = "debug mode",
                      default = False)

    #parser.add_option("-c", "--config", dest="config_file", type="string",
    #                  help="specify a config file'", metavar="FILE", default=None)

    (options, args) = parser.parse_args()


    return options
# }

def main():
# (
    options = parse_cmd_line()

    if not options.debug:
        daemonize.daemonize()
    daemonize.lock_pidfile_or_die(PIDFILE)
    
    try:
        ami_conn(options)
    except socket.error, msg:
        sys.stderr.write("Oops, Couldn t connect to AMI check,"\
                         "ami ip/port (%s)\n" % msg)
        return 1

    time_before_retry = 1

    while time_before_retry:
        loop_ret = ami_logger.loop(options)

        if loop_ret and loop_ret[1]>1:
            sys.stderr.write(loop_ret[0])
            return loop_ret[1]
        elif loop_ret and loop_ret[1] != -3:
            sys.stderr.write(loop_ret[0])
            
            

        time.sleep(time_before_retry)
        time_before_retry += 2

        try:
            ami_conn(options)
        except:
            sys.stderr.write("%s:%d| Unable to reconnect, next try scheduled "\
                             "in %d sec.\n" %\
                             (options.ip, options.port, time_before_retry))

    daemonize.unlock_pidfile(PIDFILE)
    
    return 4
# }


try:
    sys.exit(main())
except KeyboardInterrupt:
    sys.stderr.write("\rbye")
