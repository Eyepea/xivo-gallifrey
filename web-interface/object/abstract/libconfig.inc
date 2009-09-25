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

class xivo_libconfig_abstract
{
	function _load_config($dir)
	{
		$dir = (string) $dir;

		if(isset($this->_conf) === false)
			return(false);
		else if(empty($this->_conf) === false)
			return(null);
		else if(($this->_conf = dwho_gat::get_file($dir.DWHO_SEP_DIR.'config')) !== false)
			return(true);

		return(false);
	}

	function _chk_exists_cfg($type,$name,$ret=false)
	{
		$r = false;

		$type = (string) $type;
		$name = (string) $name;

		if(isset($this->_conf) === true
		&& dwho_issa($type,$this->_conf) === true
		&& isset($this->_conf[$type][$name]) === true)
			$r = (bool) $ret === false ? true : $this->_conf[$type][$name];

		return($r);
	}

	function get_option()
	{
		if(isset($this->_conf) === false || dwho_issa('option',$this->_conf) === false)
			return(false);

		return($this->_conf['option']);
	}

	function get_option_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('option',$this->_conf) === false
		|| isset($this->_conf['option'][$key]) === false)
			return(false);

		return($this->_conf['option'][$key]);
	}

	function get_element()
	{
		if(isset($this->_conf) === false || dwho_issa('element',$this->_conf) === false)
			return(false);

		return($this->_conf['element']);
	}

	function get_element_where($where)
	{
		if(is_array($where) === false
		|| isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| empty($this->_conf['element']) === true)
			return(false);

		$r = array();

		reset($this->_conf['element']);

		while(list($key) = each($this->_conf['element']))
		{
			if(array_key_exists($key,$where) === true
			&& $this->unset_element_value($key) === false)
				$r[$key] = $where[$key];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['value']) === false)
			return(false);

		return($this->_conf['element'][$key]['value']);
	}

	function chk_element_value($key,$value)
	{
		if(($values = $this->get_element_value($key)) === false
		|| in_array($value,$values,true) === false)
			return(false);

		return(true);
	}

	function _set_element_default($key,$value)
	{
		$key = (string) $key;
		$value = (string) $value;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false)
			return(false);

		$this->_conf['element'][$key]['default'] = $value;

		return(true);
	}

	function get_element_default($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['default']) === false)
			return(false);

		return($this->_conf['element'][$key]['default']);
	}

	function isset_element_default($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| dwho_ak('setdef',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function unset_element_default($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === true
		&& dwho_issa('element',$this->_conf) === true
		&& dwho_issa($key,$this->_conf['element']) === true
		&& isset($this->_conf['element'][$key]['setdef']) === true
		&& $this->_conf['element'][$key]['setdef'] === false)
			return(true);

		return(false);
	}

	function force_element_default($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| dwho_ak('forcedef',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function isset_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| dwho_ak('set',$this->_conf['element'][$key],true) === false)
			return(false);

		return(true);
	}

	function unset_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['unset']) === false)
			return(false);

		return($this->_conf['element'][$key]['unset']);
	}

	function isnull_element_value($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['null']) === false)
			return(false);

		return($this->_conf['element'][$key]['null']);
	}

	function element_is_multi($key)
	{
		$key = (string) $key;

		if(isset($this->_conf) === false
		|| dwho_issa('element',$this->_conf) === false
		|| dwho_issa($key,$this->_conf['element']) === false
		|| isset($this->_conf['element'][$key]['multi']) === false)
			return(false);

		return($this->_conf['element'][$key]['multi']);
	}

	function _load_filter()
	{
		if(isset($this->_conf,$this->_filter) === false
		|| dwho::load_class('dwho_filter') === false
		|| dwho_issa('filter',$this->_conf) === false)
			return(false);

		$this->_filter = new dwho_filter($this->_conf['filter']);

		return(true);
	}

	function get_filter_error()
	{
		if(isset($this->_filter) === false || $this->_filter === false)
			return(false);

		return($this->_filter->get_error());
	}

	function get_filter_errnb()
	{
		if(isset($this->_filter) === false || $this->_filter === false)
			return(false);

		return($this->_filter->get_errnb());
	}

	function get_filter_result()
	{
		if(isset($this->_filter) === false || $this->_filter === false)
			return(false);

		return($this->_filter->get_result());
	}

	function chk_values($arr,$set=true,$setval=true,$empty=true)
	{
		if(isset($this->_filter) === false
		|| ($this->_filter === false && $this->_load_filter() === false) === true
		|| $this->_filter->chk($arr,(bool) $setval) === false)
			return(false);

		$rs = $this->_filter->get_result();

		if((bool) $set === false
		|| is_array($rs) === false
		|| empty($rs) === true)
			return($rs);

		$r = array();

		foreach($rs as $key => $val)
		{
			if($this->unset_element_value($key) === true)
				continue;
			else if(($sval = $this->_set_value($key,$val)) !== false)
				$val = $sval;

			if(($defval = $this->get_element_default($key)) !== false
			&& $this->force_element_default($key) === true)
				$val = $defval;

			if($val === '' && $this->isnull_element_value($key) === true)
				$val = null;
			else if($val === null && $this->isnull_element_value($key) === false)
				$val = '';

			if($val === '' && $defval !== false && $this->isset_element_default($key) === true)
				$val = $defval;

			$r[$key] = $val;
		}

		if((bool) $empty === false && empty($r) === true)
			return(false);

		return($r);
	}

	function chk_value($key,$value)
	{
		if(isset($this->_filter) === false
		|| ($this->_filter === false && $this->_load_filter() === false) === true)
			return(false);
		else if($this->_chk_exists_cfg('filter',$key) === false)
			return(null);

		return($this->_filter->chk_value($key,$value));
	}

	function _set_value($key,$val)
	{
		if($this->isset_element_value($key) === false)
			return($val);
		else if(($value = $this->get_element_value($key)) === false)
			return(false);
		else if(is_scalar($value) === true)
			return($value);
		else if(dwho_ak($val,$value) === true)
			return($value[$val]);

		return(false);
	}

	function set_chk_value($key,$value,$valid=false)
	{
		$r = '';

		if(($val = $this->chk_value($key,$value)) !== false)
		{
			if(($setval = $this->_set_value($key,$val)) !== false)
				$r = $setval;
			else
				$r = $val;
		}
		else if((bool) $valid === true)
			return(false);

		if(($defval = $this->get_element_default($key)) !== false
		&& $this->force_element_default($key) === true)
			$r = $defval;

		if($r === '' && $this->isnull_element_value($key) === true)
			$r = null;
		else if($r === null && $this->isnull_element_value($key) === false)
			$r = '';

		if($r !== '' || $defval === false || $this->unset_element_value($key) === true)
			return($r);

		return($defval);
	}

	function _chk_upload_file()
	{
		$_CF = &dwho_gat::get('_CF');

		if($_CF['xivo']['ini']['file_uploads'] !== true)
		{
			trigger_error('file_uploads php.ini directive is set at "false"',E_USER_ERROR);
			return(false);
		}

		$upload = dwho_get_memory('upload_max_filesize');

		if(($size = $this->_get_config_file_maxsize()) !== false && $upload < $size)
		{
			trigger_error('file max size in configuration file shoud be less than '.
				      'upload_max_filesize php.ini directive',E_USER_ERROR);
			return(false);
		}
		else
			$size = $upload;

		return($size);
	}
}

?>
