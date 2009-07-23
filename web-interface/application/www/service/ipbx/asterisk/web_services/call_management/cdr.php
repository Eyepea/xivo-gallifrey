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

$access_category = 'call_management';
$access_subcategory = 'cdr';

include(xivo_file::joinpath(dirname(__FILE__),'..','_'.XIVO_TPL_WEBSERVICES_MODE.'.php'));

switch($_QRY->get_qs('act'))
{
	case 'search':
	default:
		$result = false;

		$cdr = &$ipbx->get_module('cdr');

		if(($info = $cdr->chk_values($_QRY->request_meth_raw(),false)) !== false
		&& ($result = $cdr->search($info,'calldate')) !== false)
		{
			if($result === null)
			{
				$http->set_status(204);
				$http->send(true);
			}
		}
		else
		{
			$http->set_status(400);
			$http->send(true);
		}

		$_HTML->set_var('result',$result);
		$_HTML->display('/service/ipbx/'.$ipbx->get_name().'/call_management/cdr');	
}

?>
