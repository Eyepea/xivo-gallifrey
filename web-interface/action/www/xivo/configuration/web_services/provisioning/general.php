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

$access_category    = 'provisioning';
$access_subcategory = 'general';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act     = $_QRY->get('act');
$appprov = &$_XOBJ->get_application('provisioning',null,false);

switch($act)
{
	case 'edit':
		$status = $appprov->edit_from_json() === true ? 200 : 400;

		$http_response->set_status_line($status);
		$http_response->send(true);

		break;
	case 'view':
	default:
		if(($info = $appprov->get($_QRY->get(1))) === false)
		{
			$http_response->set_status_line(404);
			$http_response->send(true);
		}

		$_TPL->set_var('info',$info);
		break;
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/xivo/configuration/generic');

?>
