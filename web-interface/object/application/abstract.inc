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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'abstract','libconfig.inc'));

class xivo_application_abstract extends xivo_libconfig_abstract
{
	var $_abstract		= array();
	var $_cnt		= 0;
	var $_components	= false;
	var $_errnb		= 0;
	var $_error		= array();
	var $_info		= array();
	var $_origin		= array();
	var $_result		= array();
	var $_return		= array();

	function _load_abstract_class($abstract)
	{
		$file = $abstract.'.inc';

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   $this->_application_name,
					   'abstract');

		$class = 'xivo_application_'.$this->_application_name.'_'.$abstract.'_abstract';

		if(dwho_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_abstract($name,$param=null)
	{
		$name = (string) $name;

		if(isset($this->_abstract[$name]) === true)
		{
			if(is_array($param) === true)
				$this->_abstract[$name]->load_init($param);

			return(true);
		}
		else if(($class = $this->_load_abstract_class($name)) === false)
		{
			trigger_error('Abstract Application '.$name.' does not exist in '.$this->_application_name,E_USER_ERROR);
			return(false);
		}

		$this->_abstract[$name] = new $class($param);

		return(true);
	}

	function &get_abstract($name,$param=null)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_abstract($name,$param) === true)
			$r = &$this->_abstract[$name];

		return(($ref = &$r));
	}

	function load_init($param=null)
	{
		$this->_reset_info();
		$this->_reset_params();

		if(is_array($param) === true)
			$this->set_params($param);

		return(null);
	}

	function load_components()
	{
		return(null);
	}

	function _reset_params()
	{
		if(isset($this->_param) === false)
			return(false);

		$vars = get_class_vars(get_class($this));

		if(isset($vars['param']) === true)
			$this->_param = $vars['param'];

		return(true);
	}

	function _reset_info()
	{
		$this->_info	= array();
		$this->_origin	= array();
	}

	function _reset()
	{
		$this->_errnb	= 0;
		$this->_error	= array();
		$this->_status	= '';
		$this->_result	= array();
		$this->_return	= array();
	}

	function get_cnt()
	{
		return(dwho_ulongint($this->_cnt));
	}

	function get_info($name='')
	{
		$name = (string) $name;

		if(empty($this->_info) === true)
			return(null);
		else if($name === '')
			return($this->_info);
		else if(isset($this->_info[$name]) === true)
			return($this->_info[$name]);

		return(null);
	}

	function get_info_var($name,$var)
	{
		$name = (string) $name;
		$var = (string) $var;

		if(dwho_issa($name,$this->_info) === false
		|| isset($this->_info[$name][$var]) === false)
			return(null);

		return($this->_info[$name][$var]);
	}

	function get_origin($name='')
	{
		$name = (string) $name;

		if(empty($this->_origin) === true)
			return(null);
		else if($name === '')
			return($this->_origin);
		else if(isset($this->_origin[$name]) === true)
			return($this->_origin[$name]);

		return(null);
	}

	function get_origin_var($name,$var)
	{
		$name = (string) $name;
		$var = (string) $var;

		if(dwho_issa($name,$this->_origin) === false
		|| isset($this->_origin[$name][$var]) === false)
			return(null);

		return($this->_origin[$name][$var]);
	}

	function _set_result($name,$value)
	{
		$name = (string) $name;
		$this->_result[$name] = $value;
	}

	function _set_result_var($name,$var,$value)
	{
		$name = (string) $name;
		$var = (string) $var;

		$this->_result[$name][$var] = $value;
	}

	function get_result($name='')
	{
		$name = (string) $name;

		if(empty($this->_result) === true)
			return(null);
		else if($name === '')
			return($this->_result);
		else if(isset($this->_result[$name]) === true)
			return($this->_result[$name]);

		return(null);
	}

	function get_result_var($name,$var)
	{
		$name = (string) $name;
		$var = (string) $var;

		if(dwho_issa($name,$this->_result) === false
		|| isset($this->_result[$name][$var]) === false)
			return(null);

		return($this->_result[$name][$var]);
	}

	function get_return($name='')
	{
		$name = (string) $name;

		if(empty($this->_return) === true)
			return(null);
		else if($name === '')
			return($this->_return);
		else if(isset($this->_return[$name]) === true)
			return($this->_return[$name]);

		return(null);
	}

	function get_return_var($name,$var)
	{
		$name = (string) $name;
		$var = (string) $var;

		if(dwho_issa($name,$this->_return) === false
		|| isset($this->_return[$name][$var]) === false)
			return(null);

		return($this->_return[$name][$var]);
	}

	function _set_error()
	{
		if(($argc = func_num_args()) === 0)
			return(null);

		$this->_errnb++;

		$name = (string) func_get_arg(0);

		if($argc === 1)
			$this->_error[$name] = false;
		else if($argc === 2)
			$this->_error[$name] = func_get_arg(1);
		else
		{
			if(dwho_issa($name,$this->_error) === false)
				$this->_error[$name] = array();

			$ref = &$this->_error[$name];

			for($i = 1,$nb = ($argc - 1);$i < $nb;$i++)
			{
				$key = func_get_arg($i);

				if(is_scalar($key) === false || is_bool($key) === true)
					return(false);
				else if(dwho_issa($key,$ref) === false)
					$ref[$key] = array();

				$ref = &$ref[$key];
			}

			$ref = func_get_arg($i);
		}

		return(true);
	}

	function _set_errors()
	{
		if(($argc = func_num_args()) === 0)
			return(null);

		$args = func_get_args();
		$arr = $args[$argc - 1];
		unset($args[$argc - 1]);

		if(is_array($arr) === false)
			return(false);

		foreach($arr as $key => $value)
		{
			$tmp = $args;
			$tmp[] = $key;
			$tmp[] = $value;
			call_user_func_array(array(&$this,'_set_error'),$tmp);
		}

		return(true);
	}

	function get_error($name='')
	{
		$name = (string) $name;

		if($name === '')
			return($this->_error);
		else if(isset($this->_error[$name]) === true)
			return($this->_error[$name]);

		return(null);
	}

	function get_errnb()
	{
		return($this->_errnb);
	}

	function set_params($params)
	{
		if(is_array($params) === false
		|| empty($params) === true
		|| isset($this->_param) === false
		|| is_array($this->_param) === false)
			return(false);

		foreach($params as $key => $value)
		{
			if(isset($this->_param[$key]) === true)
				$this->_param[$key] = $value;
		}

		return(true);
	}

	function get_params()
	{
		if(isset($this->_param) === false)
			return(false);

		return($this->_param);
	}

	function set_param($name,$value)
	{
		if(isset($this->_param) === false
		|| is_array($this->_param) === false
		|| isset($this->_param[$name]) === false)
			return(false);

		$this->_param[$name] = $value;

		return(true);
	}

	function get_param($name)
	{
		if(isset($this->_param) === false
		|| is_array($this->_param) === false
		|| isset($this->_param[$name]) === false)
			return(false);

		return($this->_param[$name]);
	}

	function _get_data_from_json()
	{
		$_QRY = &dwho_gct::get('dwho_query');

		if(dwho::load_class('dwho_json') === false
		|| ($data = dwho_json::decode($_QRY->get_input(),true)) === false
		|| is_array($data) === false)
			return(false);

		return($data);
	}
}

?>
