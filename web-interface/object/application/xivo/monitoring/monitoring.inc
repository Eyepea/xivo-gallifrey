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

class xivo_application_xivo_object_monitoring extends xivo_application_xivo_object_abstract
{
	var $_sysconfd	 = false;

	function xivo_application_xivo_object_monitoring(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_sysconfd    = &$this->_xobj->get_module('sysconfd'))    === false)
			return(false);

		return(true);
	}

	function get_xivo_maintenance()
	{
        $data = $this->_sysconfd->request_post('/commonconf_get', 
            array('key' =>  'xivo.maintenance'));
  
        return $data;
		
	}
	
	function set_xivo_maintenance($value)
    {
        if($this->_sysconfd->request_post('/commonconf_set', 
            array('key' => 'xivo.maintenance', 'value' => $value)) === false)
            return false;
            
        return $this->_sysconfd->request_get('/commonconf_genconfig');
    }

    function get_alerts()
    {
        return $this->_sysconfd->request_post('/commonconf_get', 
            array('key' => array('alert_emails', 'dahdi_monitor_ports')));    
    }
    
    
    function set_alerts($mails, $dahdi_ports)
    {
        if($this->_sysconfd->request_post('/commonconf_set', 
            array('keyvalues' => array(
                'alert_emails'        => $mails, 
                'dahdi_monitor_ports' => $dahdi_ports
        ))) === false)
            return false;
            
        return $this->_sysconfd->request_get('/commonconf_genconfig');
    }
    
    function get_max_call_duration()
    {
        return $this->_sysconfd->request_post('/commonconf_get', 
            array('key' => 'max_call_duration'));
    }

    function set_max_call_duration($duration)
    {
        if($this->_sysconfd->request_post('/commonconf_set', 
            array('key' => 'max_call_duration', 'value' => $duration)) === false)
            return false;
            
        return $this->_sysconfd->request_get('/commonconf_genconfig');
    }
}

?>
