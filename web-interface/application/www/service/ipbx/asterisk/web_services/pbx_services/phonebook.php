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

$access_category = 'pbx_services';
$access_subcategory = 'phonebook';

include(xivo_file::joinpath(dirname(__FILE__),'..','_common.php'));

$appphonebook = &$ipbx->get_application('phonebook');

$act = $_QRY->get_qs('act');

switch($act)
{
	case 'get':
		if(($info = $appphonebook->get($_QRY->get_qs('id'))) === false)
		{
			$http->set_status(404);
			$http->send(true);
		}

		$_HTML->set_var('info',$info);
		break;
	case 'add':
		$status = $appphonebook->add_from_json() === true ? 200 : 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'delete':
		if($appphonebook->get($_QRY->get_qs('id')) === false)
			$status = 404;
		else if($appphonebook->delete() === true)
			$status = 200;
		else
			$status = 500;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		if(($list = $appphonebook->get_phonebook_search($_QRY->get_qs('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('list',$list);
		break;
	case 'list':
	default:
		$act = 'list';

		if(($list = $appphonebook->get_phonebook_list()) === false)
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
