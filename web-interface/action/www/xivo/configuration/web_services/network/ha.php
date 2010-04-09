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
$access_subcategory = 'ha';

include(dwho_file::joinpath(dirname(__FILE__),'..','_common.php'));

$app = &$_XOBJ->get_application('ha');
$act = $_QRY->get('act');

switch($act)
{
	case 'edit':
	    $data = $app->_get_data_from_json();
	    if($data === false)
	    {
		    $http_response->set_status_line(400);
		    $http_response->send(true);
	    }

	    $data['global'] = array();
	    $skip = array('global', 'uname_node', 'ping_ipaddr', 'virtnet', 'peer', 'alert_emails');
	    foreach(array_keys($data) as $key)
	    {
	        if(in_array($key, $skip))
	            continue;
	            
	        $data['global'][$key] = $data[$key];
	        unset($data[$key]);
	    }
	    $data['global']['alert_emails'] = implode(' ', $data['alert_emails']);
	    unset($data['alert_emails']);
        
        $status = 200;
	    if($app->_set_check($data) === false
	    || $app->_set_save($data)  === false)
	    {
	        $status = 400;
	        var_dump($app->get_error());
	    }

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
		
		// formatting result
		$result = array();
        $services = array('apache2', 'asterisk', 'dhcp', 'monit', 'mysql', 
            'ntp', 'rsync', 'smokeping', 'mailto');
        foreach($services as $svc)
            $result[$svc] = $info['global'][$svc] != 0;

        $settings = array('serial', 'authkeys', 'com_mode',
            'user', 'password', 'dest_user', 'dest_password');
        foreach($settings as $setting)
            $result[$setting] = &$info['global'][$setting];

        $result['alert_emails'] = preg_split('/\s+/', $info['global'][alert_emails]);
        if(count($result['alert_emails']) == 1 && strlen($result['alert_emails'][0]) == 0)
            array_pop($result['alert_emails']);

        foreach(array('uname_node', 'ping_ipaddr', 'peer') as $key)
            $result[$key] = &$info[$key];
        
        foreach($result['peer'] as &$peer)
            $peer['transfer'] = $peer['transfer'] == 0?false:true;

		$_TPL->set_var('info',$result);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('sum',$_QRY->get('sum'));
$_TPL->display('/xivo/configuration/generic');

?>
