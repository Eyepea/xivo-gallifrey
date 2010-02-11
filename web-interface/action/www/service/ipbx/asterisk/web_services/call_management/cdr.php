<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$access_category = 'call_management';
$access_subcategory = 'cdr';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

switch($_QRY->get('act'))
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
				$http_response->set_status_line(204);
				$http_response->send(true);
			}
		}
		else
		{
			$http_response->set_status_line(400);
			$http_response->send(true);
		}

		$_TPL->set_var('result',$result);
		$_TPL->display('/service/ipbx/'.$ipbx->get_name().'/call_management/cdr');
}

?>
