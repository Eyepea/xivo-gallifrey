# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

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

import anysql
import csv
import md5
import urllib
from xivo_log import *

USERLIST_LENGTH = 12

## \class AsteriskConfig
# \brief Properties of an Asterisk server
class AsteriskConfig:
        ## \var astid
        # \brief Asterisk String ID
        
        ## \var userlist_url
        # \brief Asterisk's URL
        
        ## \var extrachannels
        # \brief Comma-separated List of the Channels not present in the SSO

        ## \var localaddr
        # \brief Local IP address

        ## \var remoteaddr
        # \brief Address of the Asterisk server

        ## \var ipaddress_webi
        # \brief IP address allowed to send CLI commands

        ## \var ami_port
        # \brief AMI port of the monitored Asterisk

        ## \var ami_login
        # \brief AMI login of the monitored Asterisk

        ## \var ami_pass
        # \brief AMI password of the monitored Asterisk
        
        ##  \brief Class initialization.
        def __init__(self,
                     astid,
                     userlist_url,
                     extrachannels,
                     localaddr = '127.0.0.1',
                     remoteaddr = '127.0.0.1',
                     ipaddress_webi = '127.0.0.1',
                     ami_port = 5038,
                     ami_login = 'xivouser',
                     ami_pass = 'xivouser',
                     userfeatures_db_uri = None,
                     capafeatures = [],
                     cdr_db_uri = None,
                     realm = 'asterisk',
                     parkingnumber = '700',
                     faxcallerid = 'faxcallerid',
                     linkestablished = ''):

                self.astid = astid
                self.userlist_url = userlist_url
                self.userlist_md5 = ""
                self.userlist_kind = userlist_url.split(':')[0]
                self.extrachannels = extrachannels
                self.localaddr = localaddr
                self.remoteaddr = remoteaddr
                self.ipaddress_webi = ipaddress_webi
                self.ami_port = ami_port
                self.ami_login = ami_login
                self.ami_pass = ami_pass
                self.capafeatures = capafeatures
                self.realm = realm
                self.parkingnumber = parkingnumber
                self.faxcallerid = faxcallerid
                self.linkestablished = linkestablished

                if userfeatures_db_uri is not None:
                        self.userfeatures_db_conn = anysql.connect_by_uri(userfeatures_db_uri)
                else:
                        self.userfeatures_db_conn = None

                if cdr_db_uri == userfeatures_db_uri:
                        self.cdr_db_conn = self.userfeatures_db_conn
                else:
                        self.cdr_db_conn = anysql.connect_by_uri(cdr_db_uri)


        ## \brief Function to load user file.
        # SIP, Zap, mISDN and IAX2 are taken into account there.
        # There would remain MGCP, CAPI, h323, ...
        # \param url the url where lies the sso, it can be file:/ as well as http://
        # \return the new phone numbers list
        # \sa update_phonelist
        def get_phonelist_fromurl(self):
                phonelist = {}
                try:
                        if self.userlist_kind == 'file':
                                f = urllib.urlopen(self.userlist_url)
                        else:
                                f = urllib.urlopen(self.userlist_url + "?sum=%s" % self.userlist_md5)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- %s : unable to open URL %s : %s" %(self.astid, self.userlist_url, str(exc)))
                        return phonelist

                t1 = time.time()
                try:
                        phone_list = []
                        mytab = []
                        for line in f:
                                mytab.append(line)
                        f.close()
                        fulltable = ''.join(mytab)
                        savemd5 = self.userlist_md5
                        self.userlist_md5 = md5.md5(fulltable).hexdigest()
                        csvreader = csv.reader(mytab, delimiter = '|')
                        # builds the phone_list
                        for line in csvreader:
                                if len(line) == USERLIST_LENGTH:
                                        if line[6] == "0":
                                                phone_list.append(line)
                                elif len(line) == USERLIST_LENGTH - 1:
                                        if line[6] == "0":
                                                line.append("0")
                                                phone_list.append(line)
                                elif len(line) == 1:
                                        if line[0] == 'XIVO-WEBI: no-data':
                                                log_debug(SYSLOG_INFO, "%s : received no-data from WEBI" % self.astid)
                                        elif line[0] == 'XIVO-WEBI: no-update':
                                                log_debug(SYSLOG_INFO, "%s : received no-update from WEBI" % self.astid)
                                                phonelist = None
                                                self.userlist_md5 = savemd5
                                else:
                                        pass
                        t2 = time.time()
                        log_debug(SYSLOG_INFO, "%s : URL %s has read %d bytes in %f seconds" %(self.astid, self.userlist_url, len(fulltable), (t2-t1)))
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- %s : a problem occured when building phone list : %s" %(self.astid, str(exc)))
                        return phonelist
                
                try:
                        # updates other accounts
                        for l in phone_list:
                                try:
                                        # line is protocol | username | password | rightflag |
                                        #         phone number | initialized | disabled(=1) | callerid |
                                        #         firstname | lastname | context | enable_hint
                                        [sso_tech, sso_phoneid, sso_passwd, sso_cti_allowed,
                                         sso_phonenum, isinitialized, sso_l6,
                                         fullname, firstname, lastname, sso_context, enable_hint] = l

                                        if sso_phonenum != '':
                                                fullname = '%s %s <b>%s</b>' % (firstname, lastname, sso_phonenum)
                                                if sso_tech == 'sip':
                                                        argg = 'SIP/%s' % sso_phoneid
                                                elif sso_tech == 'iax':
                                                        argg = 'IAX2/%s' % sso_phoneid
                                                elif sso_tech == 'misdn':
                                                        argg = 'mISDN/%s' % sso_phoneid
                                                elif sso_tech == 'zap':
                                                        argg = 'Zap/%s' % sso_phoneid
                                                else:
                                                        argg = ''
                                                if argg != '':
                                                        if bool(int(isinitialized)):
                                                                phonelist[argg] = fullname, firstname, lastname, sso_phonenum, sso_context, bool(int(enable_hint))
                                except Exception, exc:
                                        log_debug(SYSLOG_ERR, '--- exception --- %s : a problem occured when building phone list : %s' %(self.astid, str(exc)))
                                        return phonelist
                        if phonelist is not None:
                                log_debug(SYSLOG_INFO, '%s : found %d ids in phone list, among which %d ids are registered as users'
                                          %(self.astid, len(phone_list), len(phonelist)))
                finally:
                        f.close()
                return phonelist
