#!/usr/bin/python

# XIVO Daemon
# Copyright (C) 2007, 2008  Proformatique
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Alternatively, XIVO Daemon is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XIVO Daemon
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, you will find one at
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

# $Revision$
# $Date$

# Thomas Bernard

import ConfigParser
import getopt
import ldap
import sys
import syslog
import threading
import urllib

# XIVO lib-python modules initialization
import xivo.to_path
import generefiche

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
        print "#debug# (sendfiche) " + string
        return varlog(string)


def callerid_match(dbfield, cid):
        domatch = (dbfield[-9:] == cid[-9:])
        return domatch


## \brief Returns informations fetched from the LDAP database
# \param cid callerid requested
# \param ctxinfos context informations
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
# \param cid callerid requested
# \param ctxinfos context informations
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
                whereline = 'WHERE ' + ' OR '.join(str_cidm)
                conn = anysql.connect_by_uri(ctxinfos.uri)
                cursor = conn.cursor()
                cursor.query("SELECT ${columns} FROM " + ctxinfos.sqltable + " " + whereline + " LIMIT 1",
                             tuple(ctxinfos.sheet_matching_fields),
                             None)
                results = [cursor.fetchone()] # vs. fetchall() if needed
                conn.close()
	except Exception, exc:
                log_debug('Connection to SQL <%s> failed : %s' % (ctxinfos.uri, str(exc)))
                return reply_by_field

        if results[0] is not None:
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


## \brief Returns informations fetched from a CSV file
# \param cid callerid requested
# \param ctxinfos context informations
def get_csv_infos(cid, ctxinfos):
        str_cidm = []
        reply_by_field = {}
        results = []

	try:
                csv = xivo_ldap.xivo_csv(ctxinfos.uri)
                if csv.open():
                        for cidm in ctxinfos.sheet_callidmatch:
                                if cidm in csv.keys:
                                        str_cidm.append(csv.index(cidm))
                        for items in csv.items:
                                for idx in str_cidm:
                                        if callerid_match(items[idx], cid):
                                                results.append(items)
	except Exception, exc:
                log_debug('Connection to URL <%s> failed : %s' % (ctxinfos.uri, str(exc)))
                return reply_by_field

        if len(results) > 0:
                try:
                        for [dispname, dbnames_list, dummy] in ctxinfos.sheet_valid_fields:
                                field_value = ''
                                for dbname in dbnames_list:
                                        if dbname in csv.keys and field_value is '':
                                                field_value = results[0][csv.index(dbname)]
                                        reply_by_field[dispname] = field_value.strip('"')
                except Exception, exc:
                        log_debug('--- exception --- in anysql : %s' %(str(exc)))

	return reply_by_field


## \brief Builds the contents of the Sheet according to titles and default values
# \param items
# \param formats
# \param flds
def make_fields(items, formats, flds, localdir):
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

                                prestr = ''
                                poststr = ''
                                if kindoffield in formats.keys():
                                        fmat = formats[kindoffield].split("|")
                                        if len(fmat) == 2:
                                                prestr  = fmat[0]
                                                poststr = fmat[1]
                                if localdir is not None and len(localdir) == 3:
                                        if kindoffield == 'firstname':
                                                defaultvalue = localdir[1]
                                        elif kindoffield == 'lastname':
                                                defaultvalue = localdir[2]
                                value = prestr + defaultvalue + poststr
                                if kindoffield in flds:
                                        if flds[kindoffield] != "":
                                                value = prestr + flds[kindoffield] + poststr
                                fields_list.append(Info(argum, fieldtype, value))
                        else:
                                log_debug('not the right number of fields (2+3) : %s' % (str(field_to_display)))
	return fields_list



def retrieve_callerid_data(callerid, ctxinfos, xdconfig, localdir):
        fields = {}
        fields['req_cid'] = callerid
        fields['callidname'] = ''
        fields_formatted = []

        # database-specific calls
        if ctxinfos.uri != "":
                databasekind = ctxinfos.uri.split(':')[0]
                log_debug('callerid=<%s> databasekind=<%s>' % (callerid, databasekind))
                if databasekind == 'ldap':
                        try:
                                nfields = get_ldap_infos(callerid, ctxinfos)
                                for n, v in nfields.iteritems():
                                        fields[n] = v
                                log_debug('fields = %s' % str(fields))
                        except Exception, exc:
                                log_debug('--- exception --- (in %s) %s' % (databasekind, str(exc)))
                elif databasekind == 'file' or databasekind == 'http':
                        try:
                                nfields = get_csv_infos(callerid, ctxinfos)
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

        # reads the sheet informations from xivo_daemon config file
        fitems = {}
        if xdconfig.has_section('sheet_display') :
                fitems = xdconfig.items('sheet_display')
                fitems.sort()
        else:
                fitems = [("01|req_cid", "phone|Numero|Inconnu")]

        fformats = {}
        if xdconfig.has_section('sheet_extraformat') :
                for x in xdconfig.items('sheet_extraformat'):
                        fformats[x[0]] = x[1]


        # request for pictures in 'sheet_pictures' section
        if xdconfig.has_section('sheet_pictures') and xdconfig.has_option('sheet_pictures', callerid):
                fields["picture"] = xdconfig.get('sheet_pictures', callerid)

        # formats output according to filled-in values
        try:
                fields_formatted = make_fields(fitems, fformats, fields, localdir)
                log_debug('fields_formatted = %s' % str(fields_formatted))
        except Exception, exc:
                log_debug('--- exception --- when calling make_fields : %s' % str(exc))

        return [fields, fields_formatted]



class FicheSender:
        def __call__(self, sessionid, address, state, callerid, msg, tcpmode, socket, todisplay, sheetui):
                # print 'FicheSend.__class__(%s, %s, %s)' % (sessionid, address, state)

                # sessionid, address, state, msg, tcpmode, socket
                if state == 'available' or state == 'nopresence':
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
                                if sheetui is None:
                                        fxml = fiche.getxml()
                                else:
                                        fxml = sheetui
                                socket.write(fxml)
                                socket.flush()
                        else:
                                fiche.sendtouser(address, sheetui)
                        log_debug('the customer info has been sent')
                else:
                        log_debug('the customer info has not been sent because unavailable state <%s>' % state)


def sendficheasync(userinfo, ctxinfos, callerid, msg, xdconfig, localdir):
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

        [fields, params['todisplay']] = retrieve_callerid_data(callerid, ctxinfos, xdconfig, localdir)
        callidname = fields['callidname']
        params['sheetui'] = None

        if userinfo is not None:
                t = threading.Thread(None, sender, None, (), params)
                t.start()

        return callidname


def senduiasync(userinfo, ctxinfos, callerid, xdconfig):
        if len(ctxinfos.sheetui) == 0:
                log_debug('senduiasync : sheetui not defined for this context')
                return ''
                
        f = urllib.urlopen(ctxinfos.sheetui)
        full = []
        listvars = []
        for line in f:
                ls = line.strip()
                full.append(ls)
                if ls.find('XIVOFIELD-') >= 0:
                        listvars.append(ls.split('<string>XIVOFIELD-')[1].split('</string>')[0])
        f.close()
        base_sheetui = ''.join(full) + '\n'
        msg = 'UI'
        
        params = {}
        if userinfo is not None:
                sender = FicheSender()
                log_debug('senduiasync : %s callerid=<%s> msg=<%s>' % (userinfo, callerid, msg))
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

        [fields, params['todisplay']] = retrieve_callerid_data(callerid, ctxinfos, xdconfig, None)
        callidname = fields['callidname']
        for lv in listvars:
                if lv in ctxinfos.sheet_callidmatch:
                        value = callerid
                else:
                        value = ''
                if lv in fields:
                        value = fields[lv]
                nn = base_sheetui.replace('XIVOFIELD-%s' % lv, value)
                base_sheetui = nn
        params['sheetui'] = base_sheetui

        if userinfo is not None:
                t = threading.Thread(None, sender, None, (), params)
                t.start()

        return callidname
