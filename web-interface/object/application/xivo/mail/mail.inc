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
	var $_mail		= false;
	var $_sysconfd	= false;
	var $_resolvconf = false;

	function xivo_application_xivo_object_mail(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_mail = &$this->_xobj->get_module('mail')) === false
		|| ($this->_sysconfd = &$this->_xobj->get_module('sysconfd')) === false
		|| ($this->_resolvconf = &$this->_xobj->get_module('resolvconf')) === false)
			return(false);

		return(true);
	}

	function get()
	{
		$this->_info = $this->_origin = array();

		// we have only 1 mail configuration, stored with ID == 1
		if(($this->_info['mail'] = $this->_mail->get(1)) === false)
			return(false);

		$this->_origin['mail'] = $this->_mail->get_origin();
		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['mail'] = $this->_mail->get_element();

		return($r);
	}

	function set($arr)
	{
		$this->_reset();
		$this->set_mail($arr);

		return(($this->get_errnb() < 1));
	}

	function set_mail($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('mail',null);
			$this->_set_error('mail','invalid data');
			return(false);
		}

		if(($rs = $this->_mail->chk_values($arr)) === false)
		{
			$this->_set_result('mail', $this->_mail->get_filter_result());
			$this->_set_error('mail' , $this->_mail->get_filter_error());
			return(false);
		}

		$this->_set_result('mail',$rs);
		return(true);
	}

	function save()
	{
		$r = true;
		$this->_status = 'save';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->save_mail() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function save_mail()
	{
		if(($rs = $this->get_result('mail')) === null
		|| $rs === false)
			return(false);

		$this->_return['mail'] = $this->_mail->save($rs);
		
		if($this->_return['mail'] !== false)
		{
	        $resolvconf = $this->_resolvconf->get(1);
	        if($resolvconf === false)
	        { $this->_return['mail'] = false; }
		    else
		    {
		        if(strlen($rs['origin']) == 0)
		            $rs['origin'] = sprintf("%s.%s", $resolvconf['hostname'], $resolvconf['domain']);

                // if($smtp_domain is defined) => $smtp_mydomain = $smtp_domain
                // where is $smtp_domain defined ??
                $rs['mydomain'] = $resolvconf['domain'];
    		    $this->_return['mail'] = (bool) $this->_sysconfd->request_post('/mailconfig_set', $rs);
    		}
        }

		return($this->_return['mail']);
	}

    /*
	function save_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set($data) === false
		|| $this->save() === false)
			return(false);

		return(true);
	}
	*/
}

?>
