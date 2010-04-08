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
$access_category = 'network';
$access_subcategory = 'dhcp';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$app = &$_XOBJ->get_application('dhcp');
$act = $_QRY->get('act');

switch($act)
{
	case 'edit':
	    $data = $app->_get_data_from_json();
        $data['active']         = $data['active']?1:0;
        $data['extra_ifaces']   = implode(' ', $data['extra_ifaces']);
	    
		$status = ($data !== false && $app->set($data) === true) ? 200 : 400;

		$http_response->set_status_line($status);
		$http_response->send(true);
		break;

	default:
		$act = 'view';

		if(($info = $app->get()) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}

        $info['active']         = $info['active'] == 0?False:True;
        $info['extra_ifaces']   = split(' ', $info['extra_ifaces']);
        if(count($info['extra_ifaces']) == 1 && strlen($info['extra_ifaces'][0]) == 0)
            $info['extra_ifaces'] = array();
            
		$_TPL->set_var('info',$info);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/xivo/configuration/generic');

?>
