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

$_ERR = &xivo_gct::get('xivo_tracerror');

if((($report_type = $_ERR->get_param('report_type')) & XIVO_TE_RTYPE_SCREEN) !== 0)
	$_ERR->set_param('report_type',($report_type ^ XIVO_TE_RTYPE_SCREEN));

xivo::load_class('xivo_http');
$http = new xivo_http();

header(XIVO_WS_HEADER_NAME_VERSION.': '.XIVO_WS_VERSION);

if(defined('XIVO_TPL_WEBSERVICES_MODE') === false
|| (XIVO_TPL_WEBSERVICES_MODE !== 'private'
   && XIVO_TPL_WEBSERVICES_MODE !== 'restricted') === true)
{
	$http->set_status(403);
	$http->send(true);
}

include(xivo_file::joinpath(dirname(__FILE__),'_'.XIVO_TPL_WEBSERVICES_MODE.'.php'));

?>
