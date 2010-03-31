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

xivo_object::required(array('abstract','abstract.inc'),true);

class xivo_object_commonconf extends xivo_object_abstract
{
	var $_name		= 'commonconf';
	var $_sysconfd	 = false;
	var $_dhcp       = false;
	var $_monitoring = false;
	var $_mail       = false;
	var $_netiface   = false;
	var $_resolvconf = false;
	
	function xivo_object_commonconf(&$xobj)
	{
		$this->xivo_object_abstract(&$xobj);

		if(($this->_sysconfd    = &$this->_xobj->get_module('sysconfd'))    === false
		|| ($this->_dhcp        = &$this->_xobj->get_module('dhcp'))        === false
		|| ($this->_monitoring  = &$this->_xobj->get_module('monitoring'))  === false
		|| ($this->_mail        = &$this->_xobj->get_module('mail'))        === false)
			return(false);

		return(true);
	}
	
	function generate()
	{
        $conf    = array();

        // dhcp
        $rawdata = $this->_dhcp->get(1);
        $conf = array_merge($conf, array(
            'xivo.dhcp.pool'         => $rawdata['pool_start'].' '.$rawdata['pool_end'],
            'xivo.dhcp.extra_ifaces' => $rawdata['extra_ifaces'],
            'xivo.dhcp.active'       => $rawdata['active']==0?false:true,
        ));

        // mail (postfix)
        $rawdata = $this->_mail->get(1);
        var_dump($rawdata);
        $conf = array_merge($conf, array(
            'xivo.smtp.mydomain'           => 
                strlen($rawdata['mydomain'])==0?null:$rawdata['mydomain'],
            'xivo.smtp.origin'             => $rawdata['origin'],
            'xivo.smtp.relayhost'          => $rawdata['relayhost'],
            'xivo.smtp.fallback_relayhost' => $rawdata['fallback_relayhost'],
            'xivo.smtp.canonical'          => $rawdata['canonical'],
        ));

        // monitoring
        $rawdata = $this->_monitoring->get(1);
        $conf = array_merge($conf, array(
            'xivo.maintenance'             => $rawdata['maintenance']==0?false:true,
            'alert_emails'                 => 
                strlen($rawdata['alert_emails'])?null:$rawdata['alert_emails'],
            'dahdi_monitor_ports'          => 
                strlen($rawdata['dahdi_monitor_ports'])==0?null:$rawdata['dahdi_monitor_ports'],
            'max_call_duration'            => 
                strlen($rawdata['max_call_duration'])==0?null:$rawdata['max_call_duration'],
        ));

        //resolvconf
        // we can't load module at start, or we'll have an infinite loop
        $this->_resolvconf  = &$this->_xobj->get_application('resolvconf');
        if($this->_resolvconf == false)
            return false;
            
        $rawdata = $this->_resolvconf->get(1);
        $conf = array_merge($conf, array(
            'xivo.hostname'                => 
                is_null($rawdata['resolvconf']['hostname'])?'':$rawdata['resolvconf']['hostname'],
            'xivo.domain'                  => 
                is_null($rawdata['resolvconf']['domain'])?'':$rawdata['resolvconf']['domain'],
            'xivo.extra.dns.search'        => '',
            'xivo.nameservers'             => '',
        ));

        // netiface
        // we can't load module at start, or we'll have an infinite loop
        $this->_netiface    = &$this->_xobj->get_application('netiface');
        if($this->_netiface == false)
            return false;
            
        $list = $this->_netiface->get_netifaces_list();
        
        // we filter network devices list, keeping only 'voip' interfaces:
        //  1. 1st vlan
        //  2. 1st voip iface
        //  3. voip ifaces list
        $vlanid     = 0;        //0 means no vlan
        $ifaces     = "";
        $cidr_id    = -1;
        
        for($i = 0; $i < count($list); $i++)
        {
            $item = $list[$i];
            if(!$item['actionable']
            || $item['netiface']['disable']
            || strcmp($item['netiface']['networktype'], 'voip') != 0)
                continue;
                
            $ifaces .= $item['netiface']['ifname'] . ' ';

            if($vlanid == 0 && $item['netinfo']['vlanif'])
                $vlanid = intval($item['netiface']['vlanid']);
                
            if($cidr_id < 0 && strcmp($item['netinfo']['method'], 'static') == 0)
                $cidr_id = $i;
        }

        $ifaces = trim($ifaces);
        
##        var_dump($list);
#        var_dump($ifaces);
#        var_dump($vlanid);
#        var_dump($cidr_id);
        
        $subnet = null;
        if($cidr_id >= 0)
        {
            $dotted_ip = split('\.', $list[$cidr_id]['netiface']['address']);
            $dotted_nm = split('\.', $list[$cidr_id]['netiface']['netmask']);
        
            $subnet = sprintf("%d.%d.%d.%d",
                (((int)$dotted_ip[0]) & ((int)$dotted_nm[0])),
                (((int)$dotted_ip[1]) & ((int)$dotted_nm[1])),
                (((int)$dotted_ip[2]) & ((int)$dotted_nm[2])),
                (((int)$dotted_ip[3]) & ((int)$dotted_nm[3]))
            );
        }
        
        $conf = array_merge($conf, array(
            'xivo.voip.ifaces'      => $ifaces,
            'xivo.voip.vlan.id'     => $vlanid,
            
            'xivo.net4.ip'          => ($cidr_id < 0)?null:$list[$cidr_id]['netiface']['address'],
            'xivo.net4.netmask'     => $cidr_id < 0?null:$list[$cidr_id]['netiface']['netmask'],
            'xivo.net4.broadcast'   => $cidr_id < 0?null:$list[$cidr_id]['netiface']['broadcast'],
            'xivo.net4.subnet'      => $subnet,
        ));
  
        return $this->_sysconfd->request_post('/commonconf_generate', $conf);
	}
}

?>
