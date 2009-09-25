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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_agents_group extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'agent';
	var $_modulename	= 'staticagent';
	var $_category		= 'agents';
	var $_cat_metric	= 1;
	var $_var_name		= 'group';
	var $_filename		= 'agents.conf';
	var $_var_metric_range	= XIVO_SRE_IPBX_AST_AGENT_VAR_METRIC_RANGE;

	function xivo_application_service_asterisk_realstatic_agents_group(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function chk_group_number($group)
	{
		if(($group = dwho_uint($group)) === 0
		|| $group < 1
		|| $group > 63)
			return(false);

		return($group);
	}

	function get_group($id,$disable=null)
	{
		$this->_info = $this->_origin = false;

		if(($info = $this->_realstatic->get($id,$disable)) === false)
			return(false);

		$this->_info = $this->_origin = array();

		$this->_info[$this->_name] = $info;
		$this->_origin[$this->_name] = $this->_realstatic->get_origin();

		return($this->_info[$this->_name]);
	}

	function set_group($arr)
	{
		$this->_reset();

		if(is_array($arr) === false)
		{
			$this->_set_result($this->_name,null);
			$this->_set_error($this->_name,'invalid data');
			return(false);
		}
		else if(($rs = $this->chk_values($arr)) === false)
		{
			$this->_set_result($this->_name,$this->get_filter_result());
			$this->_set_error($this->_name,$this->get_filter_error());
			return(false);
		}

		$this->_set_result($this->_name,$rs);

		return(true);
	}

	function _prepare_result($numgroup=null)
	{
		if(($rs = $this->get_result($this->_name)) === null || $rs === false)
			return($rs);
		else if($numgroup !== null)
			$rs['group'] = $numgroup;

		if(($rs['group'] = $this->chk_group_number($rs['group'])) === false)
			return(false);

		$arr = array();
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_name'] = $this->_var_name;
		$arr['var_metric'] = $rs['group'] * $this->_var_metric_range;
		$arr['var_val'] = $rs['group'];

		return($arr);
	}

	function add_group($numgroup)
	{
		if(($rs = $this->_prepare_result($numgroup)) === null || $rs === false)
			return($rs);

		return(($this->_return[$this->_name] = $this->_realstatic->add($rs)));
	}

	function edit_group($id,$numgroup)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(false);
		else if(($rs = $this->_prepare_result($numgroup)) === null || $rs === false)
			return($rs);

		return(($this->_return[$this->_name] = $this->_realstatic->edit($id,$rs)));
	}

	function delete_group($status,$id)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(null);

		$r = $this->_realstatic->delete($id);

		if($status === 'delete')
			$this->_return[$this->_name] = $r;

		return($r);
	}
}

?>
