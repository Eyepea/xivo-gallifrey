<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_ldapfilter extends xivo_application_asterisk_abstract
{
	var $_ldapfilter	= false;
	var $_ldapserver	= false;

	function xivo_application_service_asterisk_ldapfilter(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_ldapfilter = &$this->_service->get_module('ldapfilter')) === false
		|| xivo::load_class('xivo_ldapserver',XIVO_PATH_OBJECT,null,false) === false
		|| ($this->_ldapserver = new xivo_ldapserver()) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['ldapfilter'] = $this->_ldapfilter->get($id,$disable)) === false)
			return(false);

		$this->_origin['ldapfilter'] = $this->_ldapfilter->get_origin();

		$this->_get_ldapserver($disable);

		return($this->_info);
	}

	function get_primary($arr,$disable=null)
	{
		return($this->_ldapfilter->get_primary($arr,$disable));
	}

	function _get_ldapserver($disable=null)
	{
		$this->_origin['ldapserver'] = false;

		if(($this->_info['ldapserver'] = $this->_ldapserver->get($this->_info['ldapfilter']['ldapserverid'],
									 $disable)) === false)
			return(false);

		$this->_origin['ldapserver'] = $this->_ldapserver->get_origin();

		return($this->_info['ldapserver']);
	}

	function get_elements()
	{
		$r = array();
		$r['ldapfilter'] = $this->_ldapfilter->get_element();
		$r['ldapserver'] = $this->_ldapserver->get_element();

		return($r);
	}

	function get_ldapfilters_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_ldapfilter->get_all($disable,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_ldapfilter->get_cnt();
			return(false);
		}

		$r = $info = array();

		$this->_cnt = $this->_ldapfilter->get_cnt();

		for($i = 0;$i < $nb;$i++)
		{
			$info['ldapfilter'] = $rs[$i];
			$info['identity'] = $info['ldapfilter']['name'];

			if(($info['ldapserver'] = $this->_ldapserver->get($info['ldapfilter']['ldapserverid'])) !== false)
				$info['identity'] .= ' ('.$info['ldapserver']['host'].')';

			$r[$info['ldapfilter']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_ldapservers_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_ldapserver->get_all($disable,true,$order,$limit,$assoc));
	}

	function _mk_unique_attribut($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		foreach($arr as $value)
		{
			if(is_scalar($value) === false)
				continue;

			$valuelower = strtolower($value);

			if(isset($r[$valuelower]) === false)
				$r[$valuelower] = $value;
		}

		return(implode(',',$r));
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| dwho_issa('ldapfilter',$arr) === false)
			return(false);

		$this->set_ldapfilter($arr['ldapfilter']);

		return(($this->get_errnb() < 1));
	}

	function set_ldapfilter($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('ldapfilter',null);
			$this->_set_error('ldapfilter','invalid data');
			return(false);
		}

		if(isset($arr['attrdisplayname']) === true
		&& ($arr['attrdisplayname'] = $this->_mk_unique_attribut($arr['attrdisplayname'])) === false)
			$arr['attrdisplayname'] = '';

		if(isset($arr['attrphonenumber']) === true
		&& ($arr['attrphonenumber'] = $this->_mk_unique_attribut($arr['attrphonenumber'])) === false)
			$arr['attrphonenumber'] = '';

		$arr['commented'] = false;

		if(($rs = $this->_ldapfilter->chk_values($arr)) === false)
		{
			$this->_set_result('ldapfilter',$this->_ldapfilter->get_filter_result());
			$this->_set_error('ldapfilter',$this->_ldapfilter->get_filter_error());
			return(false);
		}
		else if($this->_ldapserver->get_primary($rs['ldapserverid']) === false)
		{
			$this->_set_result('ldapfilter',$this->_ldapfilter->get_filter_result());
			$this->_set_error('ldapfilter','wrong ldapserver');
			return(false);
		}

		$this->_set_result('ldapfilter',$rs);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->add_ldapfilter() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function add_ldapfilter()
	{
		if(($rs = $this->get_result('ldapfilter')) === null
		|| $rs === false)
			return(false);

		$this->_return['ldapfilter'] = $this->_ldapfilter->add($rs);

		return($this->_return['ldapfilter']);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_ldapfilter() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function edit_ldapfilter()
	{
		if(dwho_issa('ldapfilter',$this->_info) === false
		|| ($rs = $this->get_result('ldapfilter')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['ldapfilter']['commented'];

		$this->_return['ldapfilter'] = $this->_ldapfilter->edit($this->_info['ldapfilter']['id'],$rs);

		return($this->_return['ldapfilter']);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_ldapfilter() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function delete_ldapfilter()
	{
		if(dwho_issa('ldapfilter',$this->_info) === false)
			return(false);

		$r = $this->_ldapfilter->delete($this->_info['ldapfilter']['id']);

		if($this->_status === 'delete')
			$this->_return['ldapfilter'] = $r;

		return($r);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_ldapfilter() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_ldapfilter() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_ldapfilter()
	{
		return($this->_enable_disable_ldapfilter(false));
	}

	function disable_ldapfilter()
	{
		return($this->_enable_disable_ldapfilter(true));
	}

	function _enable_disable_ldapfilter($disable=false)
	{
		if(dwho_issa('ldapfilter',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_ldapfilter->enable($this->_info['ldapfilter']['id']);
		else
			$r = $this->_ldapfilter->disable($this->_info['ldapfilter']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['ldapfilter'] = $r;

		return($r);
	}
}

?>
