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

class xivo_application_service_asterisk_realstatic_agents_agent extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'agent';
	var $_modulename	= 'staticagent';
	var $_category		= 'agents';
	var $_cat_metric	= 1;
	var $_var_name		= 'agent';
	var $_filename		= 'agents.conf';
	var $_var_metric_range	= XIVO_SRE_IPBX_AST_AGENT_VAR_METRIC_RANGE;

	function xivo_application_service_asterisk_realstatic_agents_agent(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function chk_var_metric($var_metric)
	{
		if(($var_metric = dwho_uint($var_metric)) === 0
		|| ($var_metric % 2) !== 0
		|| $var_metric < $this->_var_metric_range)
			return(false);

		return($var_metric);
	}

	function edit_group($id,$numgroup)
	{
		if(($id = dwho_ulongint($id)) === 0
		|| ($var_metric = $this->_get_var_metric($numgroup)) === false)
			return(false);

		$arr = array();
		$arr['var_metric'] = $var_metric;

		if(($this->_return[$this->_name] = $this->_realstatic->edit($id,$arr)) !== false)
			$this->_return[$this->_name] = $var_metric;

		return($this->_return[$this->_name]);
	}

	function get_agent($id,$disable=null)
	{
		$this->_info = $this->_origin = false;

		if(($info = $this->_realstatic->get($id,$disable)) === false)
			return(false);

		$this->_info = $this->_origin = array();

		$this->_info[$this->_name] = $info;
		$this->_origin[$this->_name] = $this->_realstatic->get_origin();

		return($this->_info[$this->_name]);
	}

	function set_agent($arr)
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

	function _get_var_metric($numgroup)
	{
		if(($var_metric = $this->_realstatic->get_last_var_metric($numgroup)) !== false)
			$var_metric += 2;
		else if(($free = $this->_realstatic->get_free_var_metric($numgroup)) !== false)
			$var_metric = $free['var_metric'];
		else
			return(false);

		$max = (($numgroup + 1) * $this->_var_metric_range) - 1;

		return(($var_metric >= $max ? false : $var_metric));
	}

	function _prepare_result($numgroup=null,$var_metric=null)
	{
		if(($rs = $this->get_result($this->_name)) === null || $rs === false)
			return($rs);
		else if(($numgroup === null
		   || (string) $numgroup !== (string) $rs['numgroup']) === true
		&& ($var_metric = $this->_get_var_metric($rs['numgroup'])) === false)
			return(false);
		else if(($var_metric = $this->chk_var_metric($var_metric)) === false)
			return(false);

		$arr = array();
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_name'] = $this->_var_name;
		$arr['var_metric'] = $var_metric;

		$arr['var_val'] = $rs['number'].',';

		if(dwho_has_len($rs,'passwd') === true)
			$arr['var_val'] .= $rs['passwd'];

		$agentname = ',';

		if(dwho_has_len($rs,'firstname') === true)
			$agentname .= $rs['firstname'].' ';

		if(dwho_has_len($rs,'lastname') === true)
			$agentname .= $rs['lastname'];

		$arr['var_val'] .= rtrim($agentname,' ');
		$arr['var_val'] = rtrim($arr['var_val'],',');

		return($arr);
	}

	function add_agent()
	{
		if(($rs = $this->_prepare_result()) === null || $rs === false)
			return($rs);
		else if(isset($rs['var_metric']) === false)
			return(false);

		$this->_return[$this->_name] = array();
		$this->_return[$this->_name]['var_metric'] = $rs['var_metric'];

		if(($this->_return[$this->_name]['id'] = $this->_realstatic->add($rs)) === false)
			$this->_return[$this->_name] = false;

		return($this->_return[$this->_name]);
	}

	function edit_agent($id,$numgroup,$var_metric)
	{
		if(($id = dwho_ulongint($id)) === 0 || ($numgroup = dwho_uint($numgroup)) === 0)
			return(false);
		else if(($rs = $this->_prepare_result($numgroup,$var_metric)) === null || $rs === false)
			return($rs);
		else if(isset($rs['var_metric']) === false
		|| $this->chk_var_metric($rs['var_metric']) === false)
			return(false);
		else if(($this->_return[$this->_name] = $this->_realstatic->edit($id,$rs)) !== false)
			$this->_return[$this->_name] = $rs['var_metric'];

		return($this->_return[$this->_name]);
	}

	function delete_agent($status,$id)
	{
		if(($id = dwho_ulongint($id)) === 0)
			return(null);

		$arr = array();
		$arr['commented'] = true;
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_name'] = 'deleted';
		$arr['var_val'] = null;

		$r = $this->_realstatic->edit($id,$arr);

		if($status === 'delete')
			$this->_return[$this->_name] = $r;

		return($r);
	}

	function add_deleted_agent($var_metric)
	{
		if(($var_metric = $this->chk_var_metric($var_metric)) === false)
			return(null);

		$arr = array();
		$arr['commented'] = true;
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $var_metric;
		$arr['var_name'] = 'deleted';
		$arr['var_val'] = null;

		return($this->_realstatic->add($arr));
	}
}

?>
