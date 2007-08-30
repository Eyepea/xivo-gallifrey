#!/usr/bin/python
# $Revision$
# $Date$

# Thomas Bernard

import ConfigParser
import getopt
import ldap
import sys
import syslog
import threading
import encodings.utf_8

import generefiche

# XIVO lib-python modules initialization
from xivo import ConfigPath
from xivo.ConfigPath import *
xivoconffile            = "/etc/asterisk/xivo_daemon.conf"
GETOPT_SHORTOPTS        = 'dc:'
GETOPT_LONGOPTS         = ["daemon", "config="]
CONFIG_LIB_PATH         = 'py_lib_path'
def config_path():
        global xivoconffile
        for opt, arg in getopt.getopt(sys.argv[1:], "dc:", ["daemon", "config="])[0]:
                if opt == "-c":
                        xivoconffile = arg
        ConfiguredPathHelper(xivoconffile, CONFIG_LIB_PATH)
config_path()
debug_mode = (sys.argv.count('-d') > 0)

# XIVO lib-python modules imports
import anysql
from BackSQL import backmysql
from BackSQL import backsqlite

import xivo_ldap



#
## \class Info
class Info:
        def __init__(self, ititle, itype, ivalue):
                self.title = ititle
                self.type = itype
                self.value = ivalue
        def __str__(self):
                return self.title + '(' + self.type + ')="' + self.value + '"'
        def __repr__(self):
                return str(self)
        def getTitle(self):
                return self.title
        def getType(self):
                return self.type
        def getValue(self):
                return self.value

def varlog(string):
        syslog.syslog(syslog.LOG_NOTICE, "sendfiche : " + string)
        return 0

def log_debug(string):
#        if debug_mode:
        print "#debug# (sendfiche) " + string
        return varlog(string)

## \brief Returns informations fetched from the LDAP database
# \param config
# \param cid
def get_ldap_infos(cid, ctxinfos):
        str_cidm = []
        for cidm in ctxinfos.sheet_callidmatch:
                if cidm != "":
                        str_cidm.append('(%s=%s)' %(cidm, cid))
        reply_by_field = {}

        try:
                ldapid = xivo_ldap.xivo_ldap(ctxinfos.uri)
                result = ldapid.getldap("(|%s)" % (''.join(str_cidm)),
                                        ctxinfos.sheet_matching_fields)
        except Exception, exc:
                log_debug('Connection to LDAP <%s> failed : %s' % (ctxinfos.uri, str(exc)))
                return reply_by_field

        if len(result) > 0:
                try:
                        for [dispname, dbnames_list, dummy] in ctxinfos.sheet_valid_fields:
                                field_value = ""
                                for dbname in dbnames_list:
                                        if dbname in result[0][1] and field_value is "":
                                                field_value = result[0][1][dbname][0]
                                        reply_by_field[dispname] = field_value
                except Exception, exc:
                        log_debug('--- exception --- in LDAP : %s' %(str(exc)))
        else:
                log_debug('No callerid in LDAP <%s> for <%s>' %(ctxinfos.uri, cid))
	return reply_by_field


## \brief Returns informations fetched from the SQL database
# \param config configuration informations
# \param cid callerid requested
def get_sql_infos(cid, ctxinfos):
        str_cidm = []
        for cidm in ctxinfos.sheet_callidmatch:
                if cidm != "":
                        str_cidm.append("%s REGEXP '%s'" %(cidm, cid))
        reply_by_field = {}

        if ctxinfos.sqltable == '':
                log_debug('No SQL table info given')
                return reply_by_field


	try:
                sqlrequest = "SELECT %s FROM %s WHERE %s LIMIT 1;" % (', '.join(ctxinfos.sheet_matching_fields),
                                                                      ctxinfos.sqltable,
                                                                      ' OR '.join(str_cidm))
                conn = anysql.connect_by_uri(ctxinfos.uri)
                cursor = conn.cursor()
                cursor.execute(sqlrequest)
                results = [cursor.fetchone()] # vs. fetchall() if needed
                conn.close()
	except Exception, exc:
                log_debug('Connection to SQL <%s> failed : %s' % (ctxinfos.uri, str(exc)))
                return reply_by_field

        try:
                xx = {}
                n = 0
                for t in ctxinfos.sheet_matching_fields:
                        xx[t] = results[0][n]
                        n += 1
                for [dispname, dbnames_list, dummy] in ctxinfos.sheet_valid_fields:
                        field_value = ""
                        for dbname in dbnames_list:
                                if dbname in xx and field_value is "":
                                        field_value = xx[dbname]
                                reply_by_field[dispname] = field_value
        except Exception, exc:
                log_debug('--- exception --- in anysql : %s' %(str(exc)))

	return reply_by_field


## \brief Builds the contents of the Sheet according to titles and default values
# \param items
# \param formats
# \param flds
def make_fields(items, formats, flds):
	fields_list = []
	for field_to_display in items:
		# a "|" character splits the order of the field in the displayed popup
		# and its kind
                if len(field_to_display) == 2:
                        lhs = field_to_display[0].split('|')
                        rhs = field_to_display[1].split('|')
                        if len(lhs) == 2 and len(rhs) == 3:
                                kindoffield  = lhs[1]
                                fieldtype    = rhs[0]
                                argum        = rhs[1]
                                defaultvalue = rhs[2]

                                prestr = ""
                                poststr = ""
                                if kindoffield in formats.keys():
                                        fmat = formats[kindoffield].split("|")
                                        if len(fmat) == 2:
                                                prestr  = fmat[0]
                                                poststr = fmat[1]
                                value = prestr + defaultvalue + poststr
                                if kindoffield in flds:
                                        if flds[kindoffield] != "":
                                                value = prestr + flds[kindoffield] + poststr
                                fields_list.append(Info(argum, fieldtype, value))
                        else:
                                log_debug('not the right number of fields (2+3) : %s' % (str(field_to_display)))
	return fields_list


# opens the xivo_daemon.conf config file
config = ConfigParser.ConfigParser()
try:
	config.readfp(open(xivoconffile))
except:
	try:
		config.readfp(open("xivo_daemon.conf"))
	except:
                log_debug('no xivo_daemon.conf file found')
		sys.exit(2)

# reads the kind of database and the sheet's format
fitems = {}
if config.has_section('sheet_display') :
        fitems = config.items('sheet_display')
        fitems.sort()
else:
        fitems = [("01|req_cid", "phone|Numero|Inconnu")]
fformats = {}
if config.has_section('sheet_extraformat') :
	for x in config.items('sheet_extraformat'):
		fformats[x[0]] = x[1]


def retrieve_callerid_data(callerid, ctxinfos):
        fields = {}
        fields["req_cid"] = callerid
        fields['callidname'] = callerid
        fields_formatted = []

        # database-specific calls
        if ctxinfos.uri != "":
                databasekind = ctxinfos.uri.split(':')[0]
                log_debug('callerid=<%s> databasekind=<%s>' % (callerid, databasekind))
                if databasekind == "ldap":
                        try:
                                nfields = get_ldap_infos(callerid, ctxinfos)
                                for n, v in nfields.iteritems():
                                        fields[n] = v
                                log_debug('fields = %s' % str(fields))
                        except Exception, exc:
                                log_debug('--- exception --- (in %s) %s' % (databasekind, str(exc)))
                else:
                        try:
                                nfields = get_sql_infos(callerid, ctxinfos)
                                for n, v in nfields.iteritems():
                                        fields[n] = v
                                log_debug('fields = %s' % str(fields))
                        except Exception, exc:
                                log_debug('--- exception --- (in %s) %s' % (databasekind, str(exc)))
        else:
                log_debug("WARNING - The db_uri of the context has not been defined")

        # request for pictures in 'sheet_pictures' section
        if config.has_section('sheet_pictures') and config.has_option('sheet_pictures', callerid):
                fields["picture"] = config.get('sheet_pictures', callerid)

        # formats output according to filled-in values
        try:
                fields_formatted = make_fields(fitems, fformats, fields)
                log_debug('fields_formatted = %s' % str(fields_formatted))
        except Exception, exc:
                log_debug('--- exception --- when calling make_fields : %s' % str(exc))

        return [fields['callidname'], fields_formatted]



class FicheSender:
        def __call__(self, sessionid, address, state, callerid, msg, tcpmode, socket, todisplay):
                # print 'FicheSend.__class__(%s, %s, %s)' % (sessionid, address, state)

                # sessionid, address, state, msg, tcpmode, socket
                if state == 'available':
                        log_debug('address = %s' % str(address))
                        fiche = generefiche.Fiche(sessionid)
                        fiche.setmessage(msg)
                        for x in todisplay:
                                fiche.addinfo(x.getTitle(), x.getType(), x.getValue())
                        if tcpmode:
                                # fs = socket.makefile('w')
                                # fs.write(fiche.getxml())
                                # fs.flush()
                                # fs.close()
                                fxml = fiche.getxml()
                                socket.write(fxml)
                                socket.flush()
                        else:
                                fiche.sendtouser(address)
                        log_debug('the customer info has been sent')
                else:
                        log_debug('the customer info has not been sent because inavailable state %s' % state)


def sendficheasync(userinfo, ctxinfos, callerid, msg):
        params = {}
        if userinfo is not None:
                sender = FicheSender()
                log_debug('sendficheasync : %s callerid=<%s> msg=<%s>' % (userinfo, callerid, msg))
                params = {'sessionid': userinfo.get('sessionid'),
                          'address':   (userinfo.get('ip'), int(userinfo.get('port'))),
                          'state':     userinfo.get('state'),
                          'callerid':  callerid,
                          'msg':       msg,
                          'tcpmode':   userinfo.get('tcpmode')}
                if userinfo['tcpmode']:
                        params['socket'] = userinfo.get('socket')
                else:
                        params['socket'] = None

        [calleridname, params['todisplay']] = retrieve_callerid_data(callerid, ctxinfos)

        if userinfo is not None:
                t = threading.Thread(None, sender, None, (), params)
                t.start()

        return calleridname
