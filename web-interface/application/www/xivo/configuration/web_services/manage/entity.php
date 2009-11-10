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

$access_category = 'manage';
$access_subcategory = 'entity';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act = $_QRY->get('act');

switch($act)
{
	case 'view':
		$appentity = &$_XOBJ->get_application('entity',null,false);

		if(($info = $appentity->get($_QRY->get('id'))) === false)
		{
			$http->set_status(404);
			$http->send(true);
		}

		$_TPL->set_var('info',$info['entity']);
		break;
	case 'add':
		$appentity = &$_XOBJ->get_application('entity',null,false);

		$status = $appentity->add_from_json() === true ? 200 : 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'delete':
		$appentity = &$_XOBJ->get_application('entity');

		if(($info = $appentity->get($_QRY->get('id'))) === false)
			$status = 404;
		else if(isset($info['deletable']) === true && $info['deletable'] === false)
			$status = 405;
		else if($appentity->delete() === true)
			$status = 200;
		else
			$status = 500;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		$appentity = &$_XOBJ->get_application('entity');

		if(($list = $appentity->get_entities_search($_QRY->get('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_TPL->set_var('list',$list);
		break;
	case 'list':
	default:
		$act = 'list';

		$appentity = &$_XOBJ->get_application('entity');

		if(($list = $appentity->get_entities_list()) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/xivo/configuration/manage/entity');

?>
