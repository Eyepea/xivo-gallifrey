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

$access_category = 'call_center';
$access_subcategory = 'queueskills';

$appqueue = &$ipbx->get_application('queue',null,false);

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$act = $_QRY->get('act');

switch($act)
{
	case 'delete':
		if(dwho_is_uint($_QRY->get('id')) === false)
		    $status = 400;
		else if($appqueue->skills_json_get($_QRY->get('id')) === false)
			$status = 404;
		else if($appqueue->skills_json_delete($_QRY->get('id')) === false)
			$status = 500;
		else
			$status = 200;

		$http_response->set_status_line($status);
		$http_response->send(true);
		break;

	case 'add':
	    $status = 200;
        if(($id = $appqueue->skills_json_add()) === false)
            $status = 500;

		$http_response->set_status_line($status);
#		$http_response->send(true);
		$_TPL->set_var('list',$id);
		break;
	
	case 'view':
		if(dwho_is_uint($_QRY->get('id')) === false
		|| ($info = $appqueue->skills_json_get($_QRY->get('id'))) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}

		$_TPL->set_var('info',$info);
		break;

	case 'search':
		if(dwho_has_len($_QRY->get('search')) === false
		|| ($list = $appqueue->skills_json_get(null, $_QRY->get('search'))) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}

		$_TPL->set_var('list',$list);
		break;

	case 'list':
	default:
		if(($list = $appqueue->skills_json_get()) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}
 
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/generic');

?>
