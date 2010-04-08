<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

$monitoring = &$_SRE->get('monitoring');

if(isset($_QR['service'],$_QR['action']) === true)
{
	switch($_QR['action'])
	{
		case 'start':
			$monitoring->start_service($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'stop':
			$monitoring->stop_service($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'restart':
			$monitoring->restart_service($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'monitor':
			$monitoring->enable_monitor($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		case 'unmonitor':
			$monitoring->disable_monitor($_QR['service']);
			$_QRY->go($_TPL->url('xivo'));
			break;
		default:
			break;
	}
}

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/uri.js');
$dhtml->set_js('js/dwho/http.js');
$dhtml->set_js('js/xivo/monitoring.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));

$_TPL->set_bloc('main','xivo/index');
$_TPL->set_struct('xivo/index');
$_TPL->display('simple');

?>
