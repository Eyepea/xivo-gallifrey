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
            array('key' =>  'pf.ha.*'));
	}
	
    /*
	function set($arr)
	{
	    $conf = array();
	    foreach($arr as $k => $v)
	    {
	        if(strncmp($k, 'xivo-smtp', 9) === 0)
                $conf[str_replace('-', '.', $k)] = $v;
	    }
	    
        if($this->_sysconfd->request_post('/commonconf_set', 
            array('keyvalues' => $conf)) === false)
                return false;
            
        return $this->_sysconfd->request_get('/commonconf_genconfig');
	}*/
}

?>
