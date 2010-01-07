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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_agents_agentoptions extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'agentoptions';
	var $_modulename	= 'staticagent';
	var $_category		= 'agents';
	var $_cat_metric	= 1;
	var $_filename		= 'agents.conf';
	var $_musiconhold	= array();
	var $_beep		= array();
	var $_goodbye		= array();
	var $_var_metric_range	= XIVO_SRE_IPBX_AST_AGENT_VAR_METRIC_RANGE;

	function xivo_application_service_asterisk_realstatic_agents_agentoptions(&$service)
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
		|| ($var_metric % 2) !== 1
		|| $var_metric < $this->_var_metric_range)
			return(false);

		return($var_metric);
	}

	function get_beep()
	{
		if(empty($this->_beep) === false)
			return($this->_beep);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);
		else if(($this->_beep = $sounds->get_list('beep','pathnoext')) !== false)
			asort($this->_beep);

		return($this->_beep);
	}

	function get_goodbye()
	{
		if(empty($this->_goodbye) === false)
			return($this->_goodbye);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);
		else if(($this->_goodbye = $sounds->get_list('goodbye','pathnoext')) !== false)
			asort($this->_goodbye);

		return($this->_goodbye);
	}

	function get_agentoptions($var_metric,$disable=null)
	{
		$this->_info[$this->_name] = false;
		$this->_origin[$this->_name] = false;

		if(($var_metric = $this->chk_var_metric($var_metric)) === false)
			return(false);

		$arr = array();
		$arr['var_metric'] = $var_metric;

		if(($info = $this->_realstatic->get_all_where($arr,$disable)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_origin[$this->_name] = $this->_realstatic->get_origin_list();

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$info[$i]['var_name']] = $info[$i]['var_val'];

		return(($this->_info[$this->_name] = $r));
	}

	function edit_var_metric($var_metric,$old_var_metric)
	{
		if(($var_metric = $this->chk_var_metric($var_metric)) === false
		|| ($old_var_metric = $this->chk_var_metric($old_var_metric)) === false)
			return(false);

		$where = array();
		$where['category'] = $this->_category;
		$where['var_metric'] = $old_var_metric;

		$value = array();
		$value['var_metric'] = $var_metric;

		return(($this->_return[$this->_name] = $this->_realstatic->edit_where($where,$value)));
	}

	function set_agentoptions($arr)
	{
		$this->_reset();

		if(is_array($arr) === false)
		{
			$this->_set_result($this->_name,null);
			$this->_set_error($this->_name,'invalid data');
			return(false);
		}

		$this->get_musiconhold();

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		$this->get_beep();

		if(isset($arr['custom_beep'],$this->_beep[$arr['custom_beep']]) === false)
			$arr['custom_beep'] = '';

		$this->get_goodbye();

		if(isset($arr['goodbye'],$this->_goodbye[$arr['goodbye']]) === false)
			$arr['goodbye'] = '';

		if(($rs = $this->chk_values($arr)) === false)
		{
			$this->_set_result($this->_name,$this->get_filter_result());
			$this->_set_error($this->_name,$this->get_filter_error());
			return(false);
		}

		$this->_set_result($this->_name,$rs);

		return(true);
	}

	function add_agentoptions($var_metric)
	{
		if(($var_metric = $this->chk_var_metric($var_metric)) === false)
			return(false);
		else if(($rs = $this->get_result($this->_name)) === null)
			return($rs);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return[$this->_name] = $var_metric;

		$arr = array();
		$arr['var_metric'] = $var_metric;

		foreach($rs as $key => $val)
		{
			$arr['var_name'] = $key;
			$arr['var_val'] = $val;

			if($this->_realstatic->add($arr) === false)
				return(false);
		}

		return(true);
	}

	function edit_agentoptions($status,$var_metric,$old_var_metric)
	{
		if(($var_metric = $this->chk_var_metric($var_metric)) === false
		|| ($this->_return[$this->_name] = $this->delete_agentoptions($status,$old_var_metric)) === null
		|| $this->_return[$this->_name] === false)
			return(($this->_return[$this->_name] = false));

		return($this->add_agentoptions($var_metric));
	}

	function delete_agentoptions($status,$var_metric)
	{
		if(($var_metric = $this->chk_var_metric($var_metric)) === false)
			return(null);

		$arr = array();
		$arr['category'] = $this->_category;
		$arr['cat_metric'] = $this->_cat_metric;
		$arr['var_metric'] = $var_metric;

		$r = $this->_realstatic->delete_where($arr);

		if($status === 'delete')
			$this->_return[$this->_name] = $r;

		return($r);
	}
}

?>
