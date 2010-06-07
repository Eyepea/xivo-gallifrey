<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

class xivo_application_xivo_object_resolvconf extends xivo_application_xivo_object_abstract
{
	var $_resolvconf  = false;
	var $_sysconfd    = false;
	var $_commonconf  = false;

	function xivo_application_xivo_object_resolvconf(&$xobj)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_resolvconf = &$this->_xobj->get_module('resolvconf')) === false
		|| ($this->_sysconfd   = &$this->_xobj->get_module('sysconfd'))   === false
		|| ($this->_commonconf = &$this->_xobj->get_module('commonconf')) === false)
			return(false);

		return(true);
	}

	function get()
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['resolvconf'] = $this->_resolvconf->get(1)) === false)
			return(false);

		$this->_origin['resolvconf'] = $this->_resolvconf->get_origin();

		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['resolvconf'] = $this->_resolvconf->get_element();

		return($r);
	}

	function set($arr)
	{
		$this->_reset();
		$this->set_resolvconf($arr);

		return(($this->get_errnb() < 1));
	}

	function set_resolvconf($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('resolvconf',null);
			$this->_set_error('resolvconf','invalid data');
			return(false);
		}

		if(dwho_issa('search',$arr) === true)
			$arr['search'] = trim(dwho_implode(' ',$arr['search']));

		if(($rs = $this->_resolvconf->chk_values($arr)) === false)
		{
			$this->_set_result('resolvconf',$this->_resolvconf->get_filter_result());
			$this->_set_error('resolvconf',$this->_resolvconf->get_filter_error());
			return(false);
		}

		$this->_set_result('resolvconf',$rs);

		return(true);
	}

	function save()
	{
		$r = true;
		$this->_status = 'save';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->save_resolvconf()       === false
		|| $this->set_sysconfd()          === false
		|| $this->_commonconf->generate() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function save_resolvconf()
	{
		if(($rs = $this->get_result('resolvconf')) === null
		|| $rs === false)
			return(false);

		$this->_return['resolvconf'] = $this->_resolvconf->save($rs);

		return($this->_return['resolvconf']);
	}
	
	function set_sysconfd()
	{
		if(($rs = $this->get_result('resolvconf')) === null
		|| $rs === false)
			return(false);

		$ns = array();
		foreach(range(1, 3) as $i)
			if(!is_null($rs["nameserver$i"]))
				$ns[] = $rs["nameserver$i"];

		$args = array();
		if(count($ns) > 0)
			$args['nameservers'] = $ns;
		if(!is_null($rs['search']))
			$args['search']      = explode(' ', $rs['search']);

		return $this->_sysconfd->request_post('/resolv_conf', $args);
	}
	
	function save_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set($data) === false
		|| $this->save() === false)
			return(false);

		return(true);
	}
}

?>
