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

class xivo_application_service_asterisk_realstatic_abstract extends xivo_application_asterisk_abstract
{
	var $_conf		= array();
	var $_filter		= false;
	var $_realstatic	= null;
	var $_multi		= false;

	function _init(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		$param = array();

		if(isset($this->_filename) === false)
			trigger_error('Missing filename attribute in '.__CLASS__,E_USER_ERROR);

		if(isset($this->_category,$this->_cat_metric) === false)
			trigger_error('Missing category or cat_metric attribute in '.__CLASS__,E_USER_ERROR);

		$param = array();
		$param['filename'] = $this->_filename;
		$param['category'] = $this->_category;
		$param['cat_metric'] = $this->_cat_metric;

		if(isset($this->_var_metric) === true)
			$param['var_metric'] = $this->_var_metric;

		if(isset($this->_var_name) === true)
			$param['var_name'] = $this->_var_name;

		$this->_realstatic = $this->_service->get_realstatic($this->_modulename,$param);

		if($this->_realstatic === false)
			return(false);

		$this->_load_config();

		return(true);
	}

	function get_info()
	{
		if(empty($this->_info) === true)
			return(null);
		else if(isset($this->_info[$this->_name]) === true)
			return($this->_info[$this->_name]);

		return(null);
	}

	function get_info_var($var)
	{
		$var = (string) $var;

		if(dwho_issa($this->_name,$this->_info) === false
		|| isset($this->_info[$this->_name][$var]) === false)
			return(null);

		return($this->_info[$this->_name][$var]);
	}

	function get_origin()
	{
		if(empty($this->_origin) === true)
			return(null);
		else if(isset($this->_origin[$this->_name]) === true)
			return($this->_origin[$this->_name]);

		return(null);
	}

	function get_origin_var($var)
	{
		$var = (string) $var;

		if(dwho_issa($this->_name,$this->_origin) === false
		|| isset($this->_origin[$this->_name][$var]) === false)
			return(null);

		return($this->_origin[$this->_name][$var]);
	}

	function get_result_var($var)
	{
		$var = (string) $var;

		if(dwho_issa($this->_name,$this->_result) === false
		|| isset($this->_result[$this->_name][$var]) === false)
			return(null);

		return($this->_result[$this->_name][$var]);
	}

	function get_return()
	{
		if(empty($this->_return) === true)
			return(null);
		else if(isset($this->_return[$this->_name]) === true)
			return($this->_return[$this->_name]);

		return(null);
	}

	function get_return_var($var)
	{
		$var = (string) $var;

		if(dwho_issa($this->_name,$this->_return) === false
		|| isset($this->_return[$this->_name][$var]) === false)
			return(null);

		return($this->_return[$this->_name][$var]);
	}

	function get_name()
	{
		return($this->_name);
	}

	function get($name,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if($this->_multi === false)
			$this->_realstatic_get($name,$disable);
		else
			$this->_realstatic_get_all($name,$disable);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_realstatic->get_nb($arr,$disable));
	}

	function get_all($disable=null,$prepare=true)
	{
		return($this->_realstatic->get_all($disable,$prepare));
	}

	function get_all_where($arr,$disable=null)
	{
		return($this->_realstatic->get_all_where($arr,$disable));
	}

	function get_all_by_category($disable=null)
	{
		return($this->_realstatic->get_all_by_category(null,$disable));
	}

	function get_all_val_by_category($disable=null)
	{
		return($this->_realstatic->get_all_val_by_category(null,$disable));
	}

	function enable($id)
	{
		return($this->_realstatic->enable($id));
	}

	function disable($id)
	{
		return($this->_realstatic->disable($id));
	}

	function get_elements()
	{
		return($this->get_element());
	}

	function _realstatic_prepare($data)
	{
		return($this->_realstatic->_prepare($data));
	}

	function _realstatic_get($name,$disable=null)
	{
		$this->_info[$this->_name] = false;
		$this->_origin[$this->_name] = false;

		$arr = array();
		$arr['var_name'] = $name;

		if(($this->_info[$this->_name] = $this->_realstatic->get_where($arr,$disable)) !== false)
			$this->_origin[$this->_name] = $this->_realstatic->get_origin();

		return($this->_info[$this->_name]);
	}

	function _realstatic_get_all($name,$disable=null)
	{
		$this->_info[$this->_name] = false;
		$this->_origin[$this->_name] = false;

		$arr = array();
		$arr['var_name'] = $name;

		if(($this->_info[$this->_name] = $this->_realstatic->get_all_where($arr,$disable)) !== false)
			$this->_origin[$this->_name] = $this->_realstatic->get_origin_list();

		return($this->_info[$this->_name]);
	}

	function set($arr)
	{
		$this->_reset();

		if(is_array($arr) === false || isset($arr['var_name']) === false)
			return(false);

		$this->get($arr['var_name']);
		$this->_realstatic_set($arr);

		return(($this->get_errnb() < 1));
	}

	function _realstatic_set($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['var_name'],$arr['var_val']) === false)
			return(false);

		$rs = array();
		$rs['var_name'] = $arr['var_name'];
		$rs['commented'] = null;

		if(isset($arr['commented']) === true)
			$rs['commented'] = (bool) $arr['commented'];

		if($this->_multi === true)
		{
			if(is_array($arr['var_val']) === false
			|| empty($arr['var_val']) === true)
			{
				$this->_multi = false;
				$arr['var_val'] = '';
			}
		}

		if($this->_multi === false)
		{
			if(($value = $this->set_chk_value($arr['var_name'],$arr['var_val'],true)) === false)
			{
				if(($value = $this->get_filter_result()) !== false)
					$rs['var_val'] = $value[$arr['var_name']];

				$this->_set_error($this->_name,$this->get_filter_error());
				$this->_set_result($this->_name,$rs);
				return(false);
			}

			$rs['var_val'] = $value;
			$this->_set_result($this->_name,$rs);
		}
		else
		{
			$res = $err = array();

			$i = 0;

			foreach($arr['var_val'] as $value)
			{
				$res[$i] = $rs;

				if(dwho_has_len($value) === false)
					$err[$i] = 'empty value';
				else if(($value = $this->set_chk_value($arr['var_name'],$value,true)) === false)
				{
					if(($value = $this->get_filter_result()) !== false)
						$res[$i]['var_val'] = $value[$arr['var_name']];

					$err[$i] = $this->get_filter_error();
				}
				else
					$res[$i]['var_val'] = $value;

				$i++;
			}

			$this->_set_result($this->_name,$res);

			if(empty($err) === false)
			{
				$this->_set_error($this->_name,$err);
				return(false);
			}
		}

		return(true);
	}

	function save()
	{
		$r = true;

		$this->_status = 'save';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| $this->_realstatic_save() === false)
		{
			$r = false;
			$this->_reverse_save();
		}

		$this->_status = '';

		return($r);
	}

	function _chk_pre_save()
	{
		return(null);
	}

	function set_save_all($arr,$disable=null)
	{
		if(is_array($arr) === false
		|| ($list = $this->get_element()) === false
		|| is_array($list) === false)
			return(false);
		else if($disable !== null)
			$disable = (bool) $disable;

		$r = array();
		$r['result'] = array();
		$r['error'] = array();

		$rs = array();
		$rs['commented'] = $disable;

		reset($list);

		while(list($key) = each($list))
		{
			if($this->unset_element_value($key) === true)
				continue;

			$rs['var_name'] = $key;

			if(isset($arr[$key]) === true)
				$rs['var_val'] = $arr[$key];
			else
				$rs['var_val'] = '';

			$this->_multi = (bool) dwho_ak('multi',$list[$key],true);

			if($this->set($rs) === false
			|| ($pre_save = $this->_chk_pre_save($rs)) === false
			|| ($pre_save !== 1 && $this->save() === false) === true)
				$r['error'][] = $key;

			$r['result'][$key] = $this->get_result($this->_name);
		}

		return($r);
	}

	function _realstatic_save()
	{
		if(isset($this->_info[$this->_name]) === false)
			return(false);
		else if($this->_info[$this->_name] === false)
			return($this->_realstatic_add());
		else if(($rs = $this->get_result($this->_name)) === null
		|| $rs === false)
			return(false);

		$where = array();
		$where['category'] = $this->_category;

		if($this->_multi === false)
		{
			$where['var_name'] = $rs['var_name'];

			if($this->_realstatic->edit_where($where,$rs) === false)
				$this->_return[$this->_name] = false;
			else
				$this->_return[$this->_name] = $rs['var_name'];

			return($this->_return[$this->_name]);
		}
		else if(is_array($rs) === false || isset($rs[0],$rs[0]['var_name']) === false)
			return(($this->_return[$this->_name] = false));

		$where['var_name'] = $rs[0]['var_name'];

		if($this->_realstatic->delete_where($where) === false)
			return(($this->_return[$this->_name] = false));

		$res = $where;

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(array_key_exists('commented',$rs[$i]) === true)
				$res['commented'] = $rs[$i]['commented'];
			else
				unset($res['commented']);

			if(array_key_exists('var_val',$rs[$i]) === true)
				$res['var_val'] = $rs[$i]['var_val'];
			else
				unset($res['var_val']);

			if($this->_realstatic->add($res) === false)
			{
				$this->_return[$this->_name] = $res['var_name'];
				return(false);
			}
		}

		return(true);
	}

	function _realstatic_add()
	{
		if(($rs = $this->get_result($this->_name)) === null
		|| $rs === false)
			return(false);
		else if($this->_multi === false)
		{
			if(($r = $this->_realstatic->add($rs)) === false)
				$this->_return[$this->_name] = false;
			else
				$this->_return[$this->_name] = $rs['var_name'];

			return($r);
		}
		else if(is_array($rs) === false || isset($rs[0],$rs[0]['var_name']) === false)
			return(($this->_return[$this->_name] = false));

		$res = array();
		$res['var_name'] = $rs[0]['var_name'];

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(array_key_exists('commented',$rs[$i]) === true)
				$res['commented'] = $rs[$i]['commented'];
			else
				unset($res['commented']);

			if(array_key_exists('var_val',$rs[$i]) === true)
				$res['var_val'] = $rs[$i]['var_val'];
			else
				unset($res['var_val']);

			if($this->_realstatic->add($res) === false)
			{
				$this->_return[$this->_name] = $res['var_name'];
				return(false);
			}
		}

		return(true);
	}

	function _reverse_save()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'save'
		|| is_array($this->_return) === false
		|| array_key_exists($this->_name,$this->_return) === false
		|| $this->_return[$this->_name] === false)
			return(false);

		return($this->_realstatic_reverse($this->_return[$this->_name]));
	}

	function _realstatic_reverse($val)
	{
		if(is_scalar($val) === false
		|| $val === false
		|| isset($this->_origin[$this->_name]) === false)
			return(false);
		else if($this->_multi === false
		&& isset($this->_origin[$this->_name]) === true
		&& $this->_origin[$this->_name] !== false)
		{
			$this->_realstatic->edit_origin($this->_origin[$this->_name]);
			return(true);
		}

		$where = array();
		$where['category'] = $this->_category;
		$where['var_name'] = $val;

		$this->_realstatic->delete_where($where);

		if($this->_multi === true
		&& isset($this->_origin[$this->_name]) === true
		&& $this->_origin[$this->_name] !== false)
			$this->_realstatic->add_origin_list($this->_origin[$this->_name]);

		return(true);
	}

	function delete_by_category()
	{
		$r = $this->_realstatic->delete_by_category($this->_category);

		if($this->_status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function add_origin($origin=null)
	{
		if(empty($this->_origin[$this->_name]) === false
		&& $origin === null)
			$origin = $this->_origin[$this->_name];

		if(is_array($origin) === false)
			return(false);

		return($this->_realstatic->add_origin($origin));
	}

	function edit_origin($origin=null)
	{
		if(empty($this->_origin[$this->_name]) === false
		&& $origin === null)
			$origin = $this->_origin[$this->_name];

		if(is_array($origin) === false)
			return(false);

		return($this->_realstatic->edit_origin($origin));
	}

	function add_origin_list($origin=null)
	{
		if(empty($this->_origin[$this->_name]) === false
		&& $origin === null)
			$origin = $this->_origin[$this->_name];

		if(is_array($origin) === false)
			return(false);

		return($this->_realstatic->add_origin_list($origin));
	}

	function edit_origin_list($origin=null)
	{
		if(empty($this->_origin[$this->_name]) === false
		&& $origin === null)
			$origin = $this->_origin[$this->_name];

		if(is_array($origin) === false)
			return(false);

		return($this->_realstatic->edit_origin_list($origin));
	}
}

?>
