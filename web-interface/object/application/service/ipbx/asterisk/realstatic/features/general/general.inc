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

class xivo_application_service_asterisk_realstatic_features_general extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'generalfeatures';
	var $_modulename	= 'features';
	var $_extenumbers	= null;
	var $_sound		= array();
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'features.conf';

	function xivo_application_service_asterisk_realstatic_features_general(&$service)
	{
		if($this->_init(&$service) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => $this->_name))) === false)
			return(false);

		return(true);
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function get($name,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_realstatic_get($name);

		if($this->_chk_exists_cfg('exten',$name) === true)
			$this->_get_extenumbers($name);

		return($this->_info);
	}

	function get_sound()
	{
		if(empty($this->_sound) === false)
			return($this->_sound);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);
		else if(($this->_sound = $sounds->get_list('features','pathnoext')) !== false)
			asort($this->_sound);

		return($this->_sound);
	}

	function get_config_exten()
	{
		if(isset($this->_conf) === false
		|| dwho_issa('exten',$this->_conf) === false)
			return(false);

		return($this->_conf['exten']);
	}

	function get_config_sound()
	{
		if(isset($this->_conf) === false
		|| dwho_issa('sound',$this->_conf) === false)
			return(false);

		return($this->_conf['sound']);
	}

	function chk_sound($value)
	{
		if(dwho_has_len($value) === false)
			return(true);
		else if(is_string($value) === false)
			return(false);

		$this->get_sound();

		if($this->_sound === false || isset($this->_sound[$value]) === false)
			return(false);

		return($value);
	}

	function set($arr)
	{
		parent::set($arr);

		if(is_array($arr) === false || isset($arr['var_name']) === false)
			return(false);
		else if($this->_chk_exists_cfg('exten',$arr['var_name']) === true)
		{
			$number = (string) $this->get_result_var('var_val');
			$context = '';

			$this->set_extenumbers('save',$number,$context);
		}

		return(($this->get_errnb() < 1));
	}

	function _realstatic_set($arr)
	{
		if(parent::_realstatic_set($arr) === false)
			return(false);
		else if(($value = $this->get_result_var('var_val')) === null)
			$value = $arr['var_val'];

		if($this->_chk_exists_cfg('sound',$arr['var_name']) === true
		&& $this->chk_sound($value) === false)
		{
			$this->_set_error($this->_name,'wrong sound file');
			return(false);
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
		|| ($name = $this->_realstatic_save()) === false
		|| $this->save_extenumbers($name) === false)
		{
			$r = false;
			$this->_reverse_save();
		}

		$this->_status = '';

		return($r);
	}

	function save_extenumbers($name)
	{
		if($this->_chk_exists_cfg('exten',$name) === false)
			return(null);
		else if(($r = parent::save_extenumbers($name)) === null)
			$r = false;

		return($r);
	}

	function _reverse_save()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'save'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case $this->_name:
					$this->_realstatic_reverse($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
			}
		}

		return(true);
	}
}

?>
