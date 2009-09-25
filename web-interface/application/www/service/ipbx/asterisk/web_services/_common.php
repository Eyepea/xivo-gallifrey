<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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
$http = new dwho_http();

header(XIVO_WS_HEADER_NAME_VERSION.': '.XIVO_WS_VERSION);

if(defined('XIVO_TPL_WEBSERVICES_MODE') === false
|| (XIVO_TPL_WEBSERVICES_MODE !== 'private'
   && XIVO_TPL_WEBSERVICES_MODE !== 'restricted') === true)
{
	$http->set_status(403);
	$http->send(true);
}

include(dwho_file::joinpath(dirname(__FILE__),'_'.XIVO_TPL_WEBSERVICES_MODE.'.php'));

?>
