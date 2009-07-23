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

xivo::load_class('xivo_http');
$http = new xivo_http();

if(defined('XIVO_TPL_WEBSERVICES_MODE') === false
|| (XIVO_TPL_WEBSERVICES_MODE !== 'private'
   && XIVO_TPL_WEBSERVICES_MODE !== 'restricted') === true)
{
	$http->set_status(403);
	$http->send(true);
}

$access_category = 'call_management';
$access_subcategory = 'incall';

include(xivo_file::joinpath(dirname(__FILE__),'..','_'.XIVO_TPL_WEBSERVICES_MODE.'.php'));

switch($_QRY->get_qs('act'))
{
	case 'add':
		$appincall = &$ipbx->get_application('incall');
		$status = $appincall->add_from_json() === true ? 200 : 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'delete':
		$appincall = &$ipbx->get_application('incall');

		if($appincall->get($_QRY->get_qs('id')) !== false
		&& $appincall->delete() === true)
			$status = 200;
		else
			$status = 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		$appincall = &$ipbx->get_application('incall',null,false);

		if(($incall = $appincall->get_incalls_search($_QRY->get_qs('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('incall',$incall);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/call_management/incall');
		break;
	case 'list':
	default:
		$appincall = &$ipbx->get_application('incall',null,false);

		if(($incall = $appincall->get_incalls_list()) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('incall',$incall);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/call_management/incall');
}

?>
