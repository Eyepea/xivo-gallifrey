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

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$callback = isset($_QR['callback']) === true ? $_QR['callback'] : '';

$dhtml = &$_TPL->get_module('dhtml');

switch($act)
{
	case 'view':
		if(xivo_user::chk_acl('pbx_settings','users') === false)
			$dhtml->ajs_die('Error/403');

		$appvoicemail = &$ipbx->get_application('voicemail');

		if(isset($_QR['id']) === false
		|| ($info = $appvoicemail->get($_QR['id'])) === false)
			$dhtml->ajs_die('Error/404');
		else if(($callback = $dhtml->chk_function_name($callback)) === false)
			$dhtml->ajs_die('Error/Invalid callback function');

		$_TPL->set_var('info',$info);
		$_TPL->set_var('callback',$callback);
		break;
	default:
		$dhtml->ajs_die('Error/404');
}

$json = &$_TPL->get_module('json');
$json->display();

?>
