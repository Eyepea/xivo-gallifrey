<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

xivo::load_class('xivo_accesswebservice',XIVO_PATH_OBJECT,null,false);
$_AWS = new xivo_accesswebservice();

$access = $_AWS->chk_http_access('pbx_settings','queues');

include(dirname(__FILE__).'/../restricted.php');

switch($_QRY->get_qs('act'))
{
	case 'add':
		$appqueue = &$ipbx->get_application('queue');

		if($appqueue->add_from_json() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[queuelist,update]');
		}
		else
			$status = 400;

		$http = new xivo_http();
		$http->set_status($status);
		$http->send(true);
		break;
	case 'delete':
		$appqueue = &$ipbx->get_application('queue');

		if($appqueue->get($_QRY->get_qs('id')) !== false
		&& $appqueue->delete() === true)
		{
			$status = 200;
			$ipbx->discuss('xivo[queuelist,update]');
		}
		else
			$status = 400;

		$http = new xivo_http();
		$http->set_status($status);
		$http->send(true);
		break;
	case 'list':
	default:
		$appqueue = &$ipbx->get_application('queue',null,false);

		if(($queues = $appqueue->get_queues_list()) === false)
		{
			xivo::load_class('xivo_http');
			$http = new xivo_http();
			$http->set_status(204);
			$http->send(true);
		}

		$_HTML->set_var('queues',$queues);
		$_HTML->set_var('sum',$_QRY->get_qs('sum'));
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/pbx_settings/queues');
}

?>
