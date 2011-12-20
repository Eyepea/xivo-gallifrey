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
	var $_prov       = false;
	var $_general    = false;
	
	function xivo_object_commonconf(&$xobj)
	{
		$this->xivo_object_abstract(&$xobj);
		$_SRE = &dwho_gct::get('xivo_service');

		if(($this->_sysconfd    = &$this->_xobj->get_module('sysconfd'))     === false
		|| ($this->_dhcp        = &$this->_xobj->get_module('dhcp'))         === false
		|| ($this->_monitoring  = &$this->_xobj->get_module('monitoring'))   === false
		|| ($this->_mail        = &$this->_xobj->get_module('mail'))         === false
		|| ($this->_prov        = &$this->_xobj->get_module('provisioning')) === false
		|| ($this->_general     = &$_SRE->get_module('ipbx', 'general')) === false)
			return(false);

		return(true);
	}
	
	function _ip2subnet($ip, $netmask)
	{
		$dotted_ip = split('\.', $ip);
		$dotted_nm = split('\.', $netmask);

		$subnet = sprintf("%d.%d.%d.%d",
			(((int)$dotted_ip[0]) & ((int)$dotted_nm[0])),
			(((int)$dotted_ip[1]) & ((int)$dotted_nm[1])),
			(((int)$dotted_ip[2]) & ((int)$dotted_nm[2])),
			(((int)$dotted_ip[3]) & ((int)$dotted_nm[3]))
		);

		return($subnet);
	}
	
	function generate($onlystatic=true)
	{
        $conf    = array();

        // dhcp
        $rawdata = $this->_dhcp->get(1);
        $conf = array_merge($conf, array(
            'xivo.dhcp.pool'         =>
                strlen($rawdata['pool_start'])==0?'':$rawdata['pool_start'].' '.$rawdata['pool_end'],
            'xivo.dhcp.extra_ifaces' => $rawdata['extra_ifaces'],
            'xivo.dhcp.active'       => $rawdata['active']==0?false:true,
        ));

        // mail (postfix)
        $rawdata = $this->_mail->get(1);
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
                strlen($rawdata['alert_emails']) == 0?null:str_replace("\r\n", " ", $rawdata['alert_emails']),
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
        $ns = '';
        if(strlen($rawdata['resolvconf']['nameserver1']) > 0)
            $ns .= $rawdata['resolvconf']['nameserver1'] . " ";
        if(strlen($rawdata['resolvconf']['nameserver2']) > 0)
            $ns .= $rawdata['resolvconf']['nameserver2'] . " ";
        if(strlen($rawdata['resolvconf']['nameserver3']) > 0)
            $ns .= $rawdata['resolvconf']['nameserver3'];
        $ns = trim($ns);
            
        $conf = array_merge($conf, array(
            'xivo.hostname'                => 
                is_null($rawdata['resolvconf']['hostname'])?'':$rawdata['resolvconf']['hostname'],
            'xivo.domain'                  => 
                is_null($rawdata['resolvconf']['domain'])?'':$rawdata['resolvconf']['domain'],
            'xivo.extra.dns.search'        => '',
            'xivo.nameservers'             => $ns,
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
        //$vlanid     = 0;        //0 means no vlan
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

						/*
            if($vlanid == 0 && $item['netinfo']['vlanif'])
                $vlanid = intval($item['netiface']['vlanid']);
						*/
            if($cidr_id < 0 && (!$onlystatic || strcmp($item['netinfo']['method'], 'static') == 0))
                $cidr_id = $i;
        }

				// no VOIP netiface defined
        if($cidr_id < 0)
					return false;

        $net = array(
            'ipaddr'    => $list[$cidr_id]['netiface']['address'], 
            'netmask'   => $list[$cidr_id]['netiface']['netmask'], 
            'broadcast' => $list[$cidr_id]['netiface']['broadcast']);

        $ifaces = trim($ifaces);
        $subnet = $this->_ip2subnet($list[$cidr_id]['netiface']['address'], $list[$cidr_id]['netiface']['netmask']);

        /* checking with HA configuration
             If HA is activated, we must use HA ip address instead of server one
#        */
        if(($havirtnet_mod = &$this->_xobj->get_module('ha_virtnet')) === false)
            return false;
        
        $virtnets = $havirtnet_mod->get_all();
        if(is_array($virtnets) && count($virtnets) > 0)
        {
            $hafound = false;
            
            foreach($virtnets as $virtnet)
            {
                $virtsubnet = $this->_ip2subnet($virtnet['ipaddr'], $virtnet['netmask']);
                if($virtsubnet == $subnet)
                {
                    $net    = $virtnet;
                    $subnet = $virtsubnet;
                    $hafound = true;
                    break;
                }
            }
            
            // no HA virtnet set for VoIP interface
            if(!$hafound)
                return false;
        }

				
				$general = $this->_general->get(1);

        $conf = array_merge($conf, array(
            'xivo.voip.ifaces'      => $ifaces,
            //'xivo.voip.vlan.id'     => $vlanid,
            
            'xivo.net4.ip'          => $net['ipaddr'],
            'xivo.net4.netmask'     => $net['netmask'],
            'xivo.net4.broadcast'   => $net['broadcast'],
            'xivo.net4.subnet'      => $subnet,
            'xivo.externip'         => $general['externip'],
        ));

        // load database uri
        // 1. xivo
        $config  = dwho::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.'xivo.ini');
        $db_xivo = $config['general']['datastorage'];

        // 2. asterisk
        $config  = dwho::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.'ipbx.ini');
        $db_ast  = $config['general']['datastorage'];

        $conf = array_merge($conf, array(
            'xivo.astdb'            => $db_ast,
            'xivo.xivodb'           => $db_xivo,
					));

				//provisioning
				$rawdata = $this->_prov->get(1);
				$conf = array_merge($conf, array(
						'xivo.registrar.main.ip'   => $rawdata['registrar_main'],
            'xivo.registrar.backup.ip' => $rawdata['registrar_backup'],
            'xivo.proxy.main.ip'       => $rawdata['proxy_main'],
            'xivo.proxy.backup.ip'     => $rawdata['proxy_backup'],
            'xivo.voip.vlan.id'        => intval($rawdata['vlan_id'])==0?'':intval($rawdata['vlan_id']),
					));

        return $this->_sysconfd->request_post('/commonconf_generate', $conf);
	}

    function apply()
    {
        return $this->_sysconfd->request_get('/commonconf_apply');
    }
    
    function last_status_code()
    {
        return $this->_sysconfd->last_status_code();
    }
}

?>
