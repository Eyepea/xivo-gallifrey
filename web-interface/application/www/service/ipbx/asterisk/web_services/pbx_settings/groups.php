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

$access_category = 'pbx_settings';
$access_subcategory = 'groups';

include(xivo_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act = $_QRY->get('act');

switch($act)
{
	case 'view':
		$appgroup = &$ipbx->get_application('group');

		$nocomponents = array('groupmacro'		=> true,
				      'extenumbers'		=> true,
				      'contextnummember'	=> true);

		if(($info = $appgroup->get($_QRY->get('id'),
					   null,
					   $nocomponents)) === false)
		{
			$http->set_status(404);
			$http->send(true);
		}

		$_TPL->set_var('info',$info);
		break;
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

		if($appgroup->get($_QRY->get('id')) === false)
			$status = 404;
		else if($appgroup->delete() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[grouplist,update]');
		}
		else
			$status = 500;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		$appgroup = &$ipbx->get_application('group',null,false);

		if(($list = $appgroup->get_groups_search($_QRY->get('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_TPL->set_var('list',$list);
		break;
	case 'list':
	default:
		$act = 'list';

		$appgroup = &$ipbx->get_application('group',null,false);

		if(($list = $appgroup->get_groups_list()) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>
