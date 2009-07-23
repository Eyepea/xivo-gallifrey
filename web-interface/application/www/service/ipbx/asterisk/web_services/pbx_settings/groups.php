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

$access_category = 'pbx_settings';
$access_subcategory = 'groups';

include(xivo_file::joinpath(dirname(__FILE__),'..','_'.XIVO_TPL_WEBSERVICES_MODE.'.php'));

switch($_QRY->get_qs('act'))
{
	case 'add':
		$appgroup = &$ipbx->get_application('group');

		if($appgroup->add_from_json() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[grouplist,update]');
		}
		else
			$status = 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'delete':
		$appgroup = &$ipbx->get_application('group');

		if($appgroup->get($_QRY->get_qs('id')) !== false
		&& $appgroup->delete() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[grouplist,update]');
		}
		else
			$status = 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		$appgroup = &$ipbx->get_application('group',null,false);

		if(($groups = $appgroup->get_groups_search($_QRY->get_qs('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('groups',$groups);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');
		break;
	case 'list':
	default:
		$appgroup = &$ipbx->get_application('group',null,false);

		if(($groups = $appgroup->get_groups_list()) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('groups',$groups);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');
}

?>
