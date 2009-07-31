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
$access_subcategory = 'users';

include(xivo_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act = $_QRY->get_qs('act');

switch($act)
{
	case 'view':
		$appuser = &$ipbx->get_application('user');

		$nocomponents = array('usermacro'		=> true,
				      'hints'			=> true,
				      'extenumbers'		=> true,
				      'contextnummember'	=> true);

		if(($info = $appuser->get($_QRY->get_qs('id'),
					  null,
					  false,
					  false,
					  $nocomponents)) === false)
		{
			$http->set_status(404);
			$http->send(true);
		}

		$_HTML->set_var('info',$info);
		break;
	case 'add':
		$appuser = &$ipbx->get_application('user');

		if($appuser->add_from_json() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[userlist,update]');
		}
		else
			$status = 400;

		$http->set_status($status);
		$http->send(true);
		break;
/*
	case 'edit':
		$appuser = &$ipbx->get_application('user');

		if($appuser->get($_QRY->get_qs('id')) === false)
			$status = 404;
		else if($appuser->edit_from_json() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[userlist,update]');
		}
		else
			$status = 400;

		$http->set_status($status);
		$http->send(true);
		break;
*/
	case 'delete':
		$appuser = &$ipbx->get_application('user');

		if($appuser->get($_QRY->get_qs('id')) === false)
			$status = 404;
		else if($appuser->delete() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[userlist,update]');
		}
		else
			$status = 500;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		$appuser = &$ipbx->get_application('user',null,false);

		if(($list = $appuser->get_users_search($_QRY->get_qs('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('list',$list);
		break;
	case 'list':
	default:
		$act = 'list';

		$appuser = &$ipbx->get_application('user',null,false);

		if(($list = $appuser->get_users_list()) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);
$_HTML->set_var('sum',$_QRY->get_qs('sum'));
$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>
