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

class xivo_application_xivo_object_mail extends xivo_application_xivo_object_abstract
{
	var $_mail           = false;
	var $_commonconf     = false;

	function xivo_application_xivo_object_mail(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_mail       = &$this->_xobj->get_module('mail'))       === false
        || ($this->_commonconf = &$this->_xobj->get_module('commonconf')) === false)
			return(false);

		return(true);
	}

	function get()
	{
	    return $this->_mail->get(1);
	}

	function set($arr)
	{
        $this->_reset();

        if($this->_mail->chk_values($arr) === false)
            $this->_set_errors($this->_mail->get_filter_error());
        if($this->get_errnb() > 0)
            return false;

        $keys = array('mydomain', 'origin', 'relayhost', 'fallback_relayhost', 'canonical');
        foreach(array_keys($arr) as $key)
            if(!in_array($key, $keys))
                unset($arr[$key]);

        if($this->_mail->edit(1, $arr) === false)
            return false;

        // update commonconf via sysconfd
        return $this->_commonconf->generate();
    }
}

?>
