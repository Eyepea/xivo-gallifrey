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
	var $_sysconfd	    = false;
	var $_ha_svc        = false;
	var $_uname_svc     = false;
	var $_ping_svc      = false;
	var $_virtnet_svc   = false;
	var $_peer_svc      = false;

	function xivo_application_xivo_object_ha(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_sysconfd    = &$this->_xobj->get_module('sysconfd'))   === false
		|| ($this->_ha_svc      = &$this->_xobj->get_module('ha'))         === false
		|| ($this->_uname_svc   = &$this->_xobj->get_module('ha_uname'))   === false
		|| ($this->_ping_svc    = &$this->_xobj->get_module('ha_ping'))    === false
		|| ($this->_virtnet_svc = &$this->_xobj->get_module('ha_virtnet')) === false
		|| ($this->_peer_svc    = &$this->_xobj->get_module('ha_peer'))    === false)
			return(false);

		return(true);
	}

	function get()
	{
	    $arr = array();
        $arr['global'] = $this->_ha_svc->get(1);

        // uname
        $arr['uname_node'] = array();
        foreach($this->_uname_svc->get_all() as $node)
            $arr['uname_node'][] = $node['uname_node'];
            
        // ping_ipaddr
        $arr['ping_ipaddr'] = array();
        foreach($this->_ping_svc->get_all() as $node)
            $arr['ping_ipaddr'][] = $node['ping_ipaddr'];
            
        // virtnet
        $arr['virtnet'] = $this->_virtnet_svc->get_all();
        // peer
        $arr['peer'] = $this->_peer_svc->get_all();
        
        return $arr;
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
	    $this->_reset();
	    $conf = array('global' => array());

        // 1. TEST DATAS
        // services
        $services = array('apache2', 'asterisk', 'dhcp', 'monit', 'mysql', 
            'ntp', 'rsync', 'smokeping', 'mailto');
        foreach($services as $svc)
            $conf['global'][$svc] = array_key_exists($svc, $arr);
        
        $conf['global']['alert_emails'] = 
            strlen(trim($arr['alert_emails']))==0?null:trim($arr['alert_emails']);
        
        $settings = array('serial', 'authkeys', 'com_mode',
            'user', 'password', 'dest_user', 'dest_password');
        foreach($settings as $setting)
            $conf['global'][$setting] = $arr[$setting];
        
        if($this->_ha_svc->chk_values($conf['global']) === false)
            $this->_set_errors($this->_ha_svc->get_filter_error());
            
        // uname_node array
        $conf['uname_node'] = $arr['uname_node'];
        array_pop($conf['uname_node']);
        
        for($i = 0; $i < count($conf['uname_node']); $i++)
        {
            if($this->_uname_svc->chk_values(array('uname_node' => $conf['uname_node'][$i])) === false)
               $this->_set_errors('pf_ha_uname_node', $i, $this->_uname_svc->get_filter_error());
        }

         // ping_ipaddr
        $conf['ping_ipaddr'] = $arr['ping_ipaddr'];
        array_pop($conf['ping_ipaddr']);
        
        for($i = 0; $i < count($conf['ping_ipaddr']); $i++)
        {
            if($this->_ping_svc->chk_values(array('ping_ipaddr' => $conf['ping_ipaddr'][$i])) === false)
               $this->_set_errors('pf_ha_ping_ipaddr', $i, $this->_ping_svc->get_filter_error());
        }            

        // virtnet
        $conf['virtnet'] = array();
        for($i = 0; $i < count($arr['virtnet_ipaddr']) - 1; $i++)
        {
            $virtnet = array(
                'ipaddr'        => $arr['virtnet_ipaddr'][$i],
                'netmask'       => $arr['virtnet_netmask'][$i],
                'broadcast'     => $arr['virtnet_broadcast'][$i],
            );

            if($this->_virtnet_svc->chk_values($virtnet) === false)
                $this->_set_errors("virtnet[$i]", $this->_virtnet_svc->get_filter_error());
                
            $conf['virtnet'][] = $virtnet;
        }

        // peer
        $conf['peer'] = array();
        for($i = 0; $i < count($arr['peer_iface']) - 1; $i++)
        {
            $virtnet = array(
                'iface'        => $arr['peer_iface'][$i],
                'host'         => $arr['peer_host'][$i],
                'transfer'     => $arr['peer_xfer'][$i],
            );

            if($this->_peer_svc->chk_values($virtnet) === false)
                $this->_set_errors("peer", $i, $this->_peer_svc->get_filter_error());
                
            $conf['peer'][] = $virtnet;
        }

        $this->_set_result('ha', $conf);
        if($this->get_errnb() > 0)
            return false;

        // IF ALL IT'S OK, SAVE DATAS
        if($this->_ha_svc->edit(1, $conf['global']) === false)
            return false;
        
        if($this->_uname_svc->delete_all() === false)
            return false;
        foreach($conf['uname_node'] as $node)
            $this->_uname_svc->add(array('uname_node' => $node));

        if($this->_ping_svc->delete_all() === false)
            return false;

        foreach($conf['ping_ipaddr'] as $ip)
            $this->_ping_svc->add(array('ping_ipaddr' => $ip));

        if($this->_virtnet_svc->delete_all() === false)
            return false;

        for($i = 0; $i < count($conf['virtnet']); $i++)
            $this->_virtnet_svc->add($conf['virtnet'][$i]);

        if($this->_peer_svc->delete_all() === false)
            return false;

        foreach($conf['peer'] as $peer)
            $this->_peer_svc->add($peer);
        
        if($this->get_errnb() > 0)
            return false;
        
        return $this->generate();
    }
    
    function generate()
    {
        $arr  = $this->get();
        $conf = array();
        
        $services = array('apache2', 'asterisk', 'dhcp', 'monit', 'mysql', 
            'ntp', 'rsync', 'smokeping', 'mailto');
        foreach($services as $svc)
            $conf["pf.ha.$svc"] = ($arr['global'][$svc] == 1);
        
        $settings = array('alert_emails', 'serial', 'authkeys', 'com_mode',
            'user', 'password', 'dest_user', 'dest_password');
        foreach($settings as $setting)
            $conf["pf.ha.$setting"] = $arr['global'][$setting];

        $conf['pf.ha.uname_node'] = array();
        foreach($arr['uname_node'] as $node)
            $conf['pf.ha.uname_node'][] = $node;

        $conf['pf.ha.ping_ipaddr'] = array();
        foreach($arr['ping_ipaddr'] as $node)
            $conf['pf.ha.ping_ipaddr'][] = $node;

        $conf['pf.ha']      = $arr['virtnet'];
        if($conf['pf.ha'] === false)
            $conf['pf.ha'] = array();
            
        $conf['pf.ha.dest'] = $arr['peer'];
        if($conf['pf.ha.dest'] === false)
            $conf['pf.ha.dest'] = array();
            
        for($i = 0; $i < count($conf['pf.ha.dest']); $i++)
            $conf['pf.ha.dest'][$i]['transfer'] = ($conf['pf.ha.dest'][$i]['transfer'] == '1');
            
        return $this->_sysconfd->request_post('/ha_generate', $conf);
    }
}

?>
