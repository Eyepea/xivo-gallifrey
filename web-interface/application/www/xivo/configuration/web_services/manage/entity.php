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

$access_category = 'manage';
$access_subcategory = 'entity';

include(xivo_file::joinpath(dirname(__FILE__),'..','_'.XIVO_TPL_WEBSERVICES_MODE.'.php'));

xivo::load_class('xivo_entity',XIVO_PATH_OBJECT,null,false);
$_ETT = new xivo_entity();

switch($_QRY->get_qs('act'))
{
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

		if(($info = $_ETT->get($_QRY->get_qs('id'))) !== false
		&& $context->get_where(array('entity' => $info['name'])) === false
		&& $_ETT->delete($info['id']) === true)
			$status = 200;
		else
			$status = 400;

		$http->set_status($status);
		$http->send(true);
		break;
	case 'search':
		if(($entities = $_ETT->get_entities_search($_QRY->get_qs('search'))) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('entities',$entities);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/xivo/configuration/manage/entity');
		break;
	case 'list':
	default:
		if(($entities = $_ETT->get_entities_list()) === false)
		{
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('entities',$entities);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/xivo/configuration/manage/entity');
}

?>
