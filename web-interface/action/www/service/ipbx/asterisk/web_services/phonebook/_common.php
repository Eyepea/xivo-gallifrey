<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$_ERR = &dwho_gct::get('dwho_tracerror');
$_ERR->set_param('report_type',
		 $_ERR->get_param('report_type') & ~DWHO_TE_RTYPE_SCREEN);

dwho::load_class('dwho_http');
$http_response = dwho_http::factory('response');

$phonebook = &$ipbx->get_appcustom('webservices','phonebook');

if(isset($_SERVER['REMOTE_ADDR']) === false
|| $phonebook->chk_host_access($_SERVER['REMOTE_ADDR']) === false)
{
	$http_response->set_status_line(403);
	$http_response->send(true);
}

?>
