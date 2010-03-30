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

	function xivo_object_commonconf(&$xobj)
	{
		$this->xivo_object_abstract(&$xobj);

		if(($this->_sysconfd    = &$this->_xobj->get_module('sysconfd'))    === false
		|| ($this->_dhcp        = &$this->_xobj->get_module('dhcp'))    === false)
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
            'xivo.dhcp.active'       => $rawdata['active'],
        ));

        return $this->_sysconfd->request_post('/commonconf_generate', $conf);
	}
}

?>
