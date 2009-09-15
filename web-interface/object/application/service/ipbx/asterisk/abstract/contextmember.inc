<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_contextmember_abstract extends xivo_application_service_asterisk_abstract
{
	var $_context		= false;
	var $_contextmember	= false;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_contextmember_abstract(&$service,$param=array())
	{
		$this->xivo_application_service_asterisk_abstract(&$service);

		if(($this->_context = &$this->_service->get_module('context')) === false
		|| ($this->_contextmember = &$this->_service->get_module('contextmember')) === false)
			return(false);

		return($this->load_init($param));
	}

	function load_init($param=array())
	{
		$this->_reset_info();
		$this->_reset_params();
		$this->_reset();

		if($this->set_params($param) === false
		|| isset($param['type']) === false
		|| $this->_contextmember->chk_element_value('type',$param['type']) === false)
		{
			$this->_param['type'] = '';
			return(false);
		}

		$this->_param['type'] = $param['type'];

		return(true);
	}

	function get($typeval='',$varname='')
	{
		$this->_info = false;
		$this->_origin = false;

		$where = array();
		$where['type'] = $this->_param['type'];

		if(xivo_has_len($typeval) === true)
			$where['typeval'] = $typeval;
		else if(xivo_has_len($varname) === true)
			$where['varname'] = $varname;
		else
			return(false);

		if(($this->_info = $this->_contextmember->get_all_where($where)) === false
		|| isset($this->_info[0]) === false)
			return(false);

		$this->_origin = $this->_contextmember->get_origin_list();

		return($this->_info);
	}

	function set($list)
	{
		$this->_reset();

		if(is_array($list) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$info = array();
		$info['type'] = $this->_param['type'];

		$tmp = $reslist = $errlist = array();

		for($i = 0;$i < $nb;$i++)
		{
			$context = '';

			if(xivo_has_len($list[$i],'context') === false)
				continue;
			else if(($context = $this->_context->get_primary($list[$i]['context'])) === false)
			{
				$this->_set_error('invalid context');
				continue;
			}

			if(xivo_has_len($list[$i],'typeval') === true)
				$info['typeval'] = $list[$i]['typeval'];
			else
				$info['typeval'] = '';

			if(xivo_has_len($list[$i],'varname') === true)
				$info['varname'] = $list[$i]['varname'];
			else
				$info['varname'] = '';

			$uniq = $context.'-'.$info['type'].'-'.$info['typeval'].'-'.$info['varname'];
			$info['context'] = $context;

			if(($rs = $this->_contextmember->chk_values($info)) === false)
			{
				$this->_set_error('invalid context');
				continue;
			}
			else if(isset($tmp[$uniq]) === true)
				continue;

			$tmp[$uniq] = 1;
			$reslist[] = $rs;
		}

		if($this->get_errnb() > 0)
		{
			$this->_set_result(false);
			return(false);
		}
		else if(isset($reslist[0]) === false)
			$reslist = null;

		$this->_set_result($reslist);

		return(true);
	}

	function add($typeval='')
	{
		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		$typevalhaslen = xivo_has_len($typeval);

		$this->_return = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if($typevalhaslen === true)
				$rs[$i]['typeval'] = $typeval;
			else if(xivo_has_len($rs[$i],'typeval') === false
			&& xivo_has_len($rs[$i],'varname') === false)
				return(false);

			if(($this->_return[] = $this->_contextmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		return($this->_contextmember->add_origin_list($this->_origin));
	}

	function edit($status,$typeval='')
	{
		if(xivo_issa(0,$this->_origin) === false)
			return($this->add($typeval));

		$this->_return = $this->delete($status);

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if($rs === false)
			return(false);

		$this->_return = array();

		$nb = count($rs);

		if(xivo_has_len($this->_origin[0],'typeval') === true)
			$typeval = $this->_origin[0]['typeval'];
		else
			$typeval = '';

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['typeval'] = $typeval;

			if(($this->_return[] = $this->_contextmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[0]) === false)
			$this->_return = false;

		return(true);
	}

	function delete($status)
	{
		if(xivo_issa(0,$this->_origin) === false)
			return(null);

		$where = array();
		$where['type'] = $this->_origin[0]['type'];

		if(xivo_has_len($this->_origin[0],'typeval') === true)
			$where['typeval'] = $this->_origin[0]['typeval'];
		else if(xivo_has_len($this->_origin[0],'varname') === true)
			$where['varname'] = $this->_origin[0]['varname'];
		else
			return(false);

		$r = $this->_contextmember->delete_where($where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false || ($nb = count($this->_return)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$this->_contextmember->delete($this->_return[$i]);
		}

		return(true);
	}

	function reverse_edit($status)
	{
		$this->delete($status);

		return($this->add_origin_list());
	}
}

?>
