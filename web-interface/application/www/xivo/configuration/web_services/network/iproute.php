<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$access_category = 'network';
$access_subcategory = 'iproute';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act = $_QRY->get('act');

switch($act)
{
	case 'view':
		$appiproute = &$_XOBJ->get_application('iproute',null,false);

		if(($info = $appiproute->get($_QRY->get('id'))) === false)
		{
			$http_response->set_status_line(404);
			$http_response->send(true);
		}

		$_TPL->set_var('info',$info['iproute']);
		break;
	case 'add':
		$appiproute = &$_XOBJ->get_application('iproute',null,false);

		$status = $appiproute->add_from_json() === true ? 200 : 400;

		$http_response->set_status_line($status);
		$http_response->send(true);
		break;
	case 'delete':
		$appiproute = &$_XOBJ->get_application('iproute');

		if(($info = $appiproute->get($_QRY->get('id'))) === false)
			$status = 404;
		else if($appiproute->delete() === true)
			$status = 200;
		else
			$status = 500;

		$http_response->set_status_line($status);
		$http_response->send(true);
		break;
	case 'search':
		$appiproute = &$_XOBJ->get_application('iproute');

		if(($list = $appiproute->get_iproutes_search($_QRY->get('search'),null,null,null,false,false)) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}

		$_TPL->set_var('list',$list);
		break;
	case 'list':
	default:
		$act = 'list';

		$appiproute = &$_XOBJ->get_application('iproute');

		if(($list = $appiproute->get_iproutes_list(null,null,null,false,false)) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}

		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/xivo/configuration/generic');

?>
