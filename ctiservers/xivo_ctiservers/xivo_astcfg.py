# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2010 Proformatique'
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

from xivo import anysql
import logging

log = logging.getLogger('astconfig')

## \class AsteriskConfig
# \brief Properties of an Asterisk server
class AsteriskConfig:
    ## \var astid
    # \brief Asterisk String ID

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
    def __init__(
        self,
        astid,
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
        linkestablished = '',
        aoriginate = 'AOriginate'
        ):

            self.astid = astid
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
            self.aoriginate = aoriginate

            self.userfeatures_db_conn = None
            try:
                if userfeatures_db_uri is not None:
                    self.userfeatures_db_conn = anysql.connect_by_uri(str(userfeatures_db_uri))
            except Exception:
                log.exception('(init userfeatures_db_conn for %s)' % astid)

            if cdr_db_uri == userfeatures_db_uri:
                self.cdr_db_conn = self.userfeatures_db_conn
            else:
                self.cdr_db_conn = None
                try:
                    self.cdr_db_conn = anysql.connect_by_uri(str(cdr_db_uri))
                except Exception:
                    log.exception('(init cdr_db_conn for %s)' % astid)
