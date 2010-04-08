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

$access_category = 'support';
$access_subcategory = 'monitoring';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$app = &$_XOBJ->get_application('monitoring');
$act = $_QRY->get('act');

switch($act)
{
	case 'edit':
	    $data = $app->_get_data_from_json();
	    $data['maintenance']            = (int) $data['maintenance'];
	    $data['alert_emails']           = implode(' ', $data['alert_emails']);
	    $data['dahdi_monitor_ports']    = implode(' ', $data['dahdi_monitor_ports']);
	    
		$status = ($data !== false && $app->set_monitoring($data) === true) ? 200 : 400;

		$http_response->set_status_line($status);
		$http_response->send(true);
		break;

	default:
		$act = 'view';

		if(($info = $app->get_monitoring()) === false)
		{
			$http_response->set_status_line(204);
			$http_response->send(true);
		}

        $info['maintenance']    = (bool) $info['maintenance'];
        
        $info['alert_emails']   = preg_split('/\s+/', $info['alert_emails']);
        if(count($info['alert_emails']) == 1 && strlen($info['alert_emails'][0]) == 0)
            $info['alert_emails'] = array();

        $info['dahdi_monitor_ports']   = split(' ', $info['dahdi_monitor_ports']);
        if(count($info['dahdi_monitor_ports']) == 1 && strlen($info['dahdi_monitor_ports'][0]) == 0)
            $info['dahdi_monitor_ports'] = array();
        for($i = 0; $i < count($info['dahdi_monitor_ports']); $i++)
            $info['dahdi_monitor_ports'][$i] = (int) $info['dahdi_monitor_ports'][$i];
        
        if(!is_null($info['max_call_duration']))
            $info['max_call_duration'] = intval($info['max_call_duration']);
        
		$_TPL->set_var('info',$info);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/xivo/configuration/generic');

?>
