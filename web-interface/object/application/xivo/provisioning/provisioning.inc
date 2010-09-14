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

class xivo_application_xivo_object_provisioning extends xivo_application_xivo_object_abstract
{
	var $_commonconf = false;
	var $_prov       = false;

	function xivo_application_xivo_object_provisioning(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_commonconf = &$this->_xobj->get_module('commonconf'))    === false
		|| ($this->_prov       = &$this->_xobj->get_module('provisioning'))  === false)
			return(false);

		return(true);
	}

	function get()
	{
		$data = $this->_prov->get(1);
		return $data;
	}

	function set($arr)
    {
        $this->_reset();
        if(($arr = $this->_prov->chk_values($arr)) === false)
            $this->_set_errors($this->_prov->get_filter_error());

				if($this->get_errnb() > 0)
            return false;

        $keys = array('registrar_main', 'registrar_backup', 'proxy_main', 'proxy_backup', 'vlan_id');
        foreach(array_keys($arr) as $key)
        {
            if(!in_array($key, $keys))
							unset($arr[$key]);
				}

				if($this->_prov->edit(1, $arr) === false)
					return false;

        // update commonconf via sysconfd
        return $this->_commonconf->generate();
    }
}

?>
