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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','xivo.inc'),true);

class xivo_application_xivo_object_ha extends xivo_application_xivo_object_abstract
{
	var $_sysconfd	 = false;

	function xivo_application_xivo_object_ha(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_sysconfd = &$this->_xobj->get_module('sysconfd'))     === false)
			return(false);

		return(true);
	}

	function get()
	{
        return $this->_sysconfd->request_post('/ha_get', 
            array('key' =>  'pf.ha*'));
	}

	
	function get_netifaces()
	{
	    $netifaces = $this->_sysconfd->request_get('/discover_netifaces');
	    if($netifaces === false)
	    { return false; }
	    
	    $netifaces = array_keys($netifaces);
	    asort($netifaces);
	    return $netifaces;
	}
	
	function set($arr)
	{
	    $conf = array();

        // services
        $services = array('apache2', 'asterisk', 'dhcp', 'monit', 'mysql', 
            'ntp', 'rsync', 'smokeping', 'mailto');

        foreach($services as $svc)
            $conf["pf.ha.$svc"] = array_key_exists("pf-ha-$svc", $arr);
        
        $settings = array('alert_emails', 'serial', 'authkeys', 'com_mode',
            'user', 'password', 'dest_user', 'dest_password');
        foreach($settings as $setting)
            $conf["pf.ha.$setting"] = $arr["pf-ha-$setting"];
        
        $lists = array('uname_node', 'ping_ipaddr');
        foreach($lists as $list)
        {
            $conf["pf.ha.$list"] = $arr["pf-ha-$list"];
            array_pop($conf["pf.ha.$list"]);
        }
        
        //var_dump($conf);
        if($this->_sysconfd->request_post('/ha_set', $conf) === false)
            return false;
            
        return $this->_sysconfd->request_get('/ha_generate');
	}
}

?>
