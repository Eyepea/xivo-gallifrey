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
//dwho_file::required(array(DWHO_PATH_ROOT  ,'network.inc'),true);

class xivo_application_xivo_object_ha extends xivo_application_xivo_object_abstract
{
	var $_sysconfd	    = false;
	var $_ping_svc      = false;
	var $_virtnet_svc   = false;
	var $_nodes_svc     = false;

	function xivo_application_xivo_object_ha(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_sysconfd = &$this->_xobj->get_module('sysconfd'))      === false
		|| ($this->_ping_svc = &$this->_xobj->get_module('ha_ping'))       === false
		|| ($this->_virtnet_svc = &$this->_xobj->get_module('ha_virtnet')) === false
		|| ($this->_nodes_svc   = &$this->_xobj->get_module('ha_nodes'))   === false)
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
	
	function get_status()
    {
	    $status = $this->_sysconfd->request_get('/ha_status');
	    if($status === false)
	    { return false; }
	    
	    return $status;
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

        // validate ip addresses
        $this->_reset();
        foreach($conf['pf.ha.ping_ipaddr'] as $ipaddr)
        {
            if($this->_ping_svc->chk_values(array('ipaddr' => $ipaddr)) === false)
                $this->_set_error("ping_ipaddr[$ipaddr]", 
                    $this->_ping_svc->get_filter_error());
        }
        
        $conf['pf.ha'] = array();
        for($i = 0; $i < count($arr['pf_ha_ipaddr']) - 1; $i++)
        {
            $virtnet = array(
                'ipaddr'    => $arr['pf_ha_ipaddr'][$i],
                'netmask'   => $arr['pf_ha_netmask'][$i],
                'broadcast' => $arr['pf_ha_broadcast'][$i],
            );
            
            $conf['pf.ha'][] = $virtnet;
            if($this->_virtnet_svc->chk_values($virtnet) === false)
                $this->_set_error("virtnet[$i]", $this->_virtnet_svc->get_filter_error());
        }

        $conf['pf.ha.dest'] = array();
        for($i = 0; $i < count($arr['pf_ha_iface']) - 1; $i++)
        {
            $nodes = array(
                'iface'     => $arr['pf_ha_iface'][$i],
                'host'      => $arr['pf_ha_host'][$i],
                'transfer'  => strcmp($arr['pf_ha_xfer'][$i], '1') === 0?True:False,
            );
            
            $conf['pf.ha.dest'][] = $nodes;
            if($this->_nodes_svc->chk_values($nodes) === false)
                $this->_set_error("nodes[$i]", $this->_nodes_svc->get_filter_error());
        }
        
        $this->_result = $conf;
        

        if($this->get_errnb() > 0)
            return false;
            
        if($this->_sysconfd->request_post('/ha_set', $conf) === false)
            return false;
            
        return $this->_sysconfd->request_get('/ha_generate');
	}
}

?>
