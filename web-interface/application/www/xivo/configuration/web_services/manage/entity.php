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

include(xivo_file::joinpath(dirname(__FILE__),'..','_common.php'));

xivo::load_class('xivo_entity',XIVO_PATH_OBJECT,null,false);
$_ETT = new xivo_entity();

$act = $_QRY->get_qs('act');

switch($act)
{
	case 'view':
		if(($info = $_ETT->get($_QRY->get_qs('id'))) === false)
		{
			$http->set_status(404);
			$http->send(true);
		}

		$_HTML->set_var('info',$info);
		break;
	case 'add':
		if(xivo::load_class('xivo_json') === false
		|| ($data = xivo_json::decode($_QRY->get_input(),true)) === false
		|| is_array($data) === false
		|| $_ETT->chk_values($data) === false
		|| ($result = $_ETT->get_filter_result()) === false
		|| $_ETT->add($result) === false)
			$status = 400;
		else
			$status = 200;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'delete':
		$ipbx = &$_SRE->get('ipbx');
		$context = &$ipbx->get_module('context');

		if(($info = $_ETT->get($_QRY->get_qs('id'))) === false)
			$status = 404;
		else if($context->get_where(array('entity' => $info['name'])) !== false)
			$status = 405;
		else if($_ETT->delete($info['id']) !== false)
			$status = 200;
		else
			$status = 500;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		if(($list = $_ETT->get_entities_search($_QRY->get_qs('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('list',$list);
		break;
	case 'list':
	default:
		$act = 'list';

		if(($list = $_ETT->get_entities_list()) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);
$_HTML->set_var('sum',$_QRY->get_qs('sum'));
$_HTML->display('/xivo/configuration/manage/entity');

?>
