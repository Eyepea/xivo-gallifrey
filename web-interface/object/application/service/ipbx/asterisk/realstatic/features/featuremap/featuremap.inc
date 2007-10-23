<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_features_featuremap extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'featuremap';
	var $_modulename	= 'features';
	var $_extenumbers	= null;
	var $_category		= 'featuremap';
	var $_cat_metric	= 1;
	var $_filename		= 'features.conf';

	function xivo_application_service_asterisk_realstatic_features_featuremap(&$service)
	{
		if($this->_init(&$service) === false
		|| ($this->_extenumbers = &$this->get_abstract('extenumbers')) === false)
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

		$this->_get_realstatic($name);
		$this->_get_extenumbers();

		return($this->_info);
	}

	function _get_extenumbers()
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		if(isset($this->_info[$this->_name]['var_name']) === false)
			return(false);

		$r = parent::_get_extenumbers('featuremap',$this->_info[$this->_name]['var_name']);

		return($r);
	}

	function set($arr)
	{
		parent::set($arr); 

		if(is_array($arr) === false || isset($arr['var_name']) === false)
			return(false);

		$number = (string) $this->get_result_var($this->_name,'var_val');
		$context = '';

		$this->set_extenumbers('save','featuremap',$number,$context);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function save()
	{
		$this->_status = 'save';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| ($name = $this->save_realstatic()) === false
		|| $this->save_extenumbers($name) === false)
		{
			$this->_reverse_save();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function save_extenumbers($name)
	{
		if(($r = parent::save_extenumbers($name)) === null)
			$r = false;

		return($r);
	}

	function _reverse_save()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'save'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case $this->_name:
					$this->_reverse_realstatic($val);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === false)
						continue;

					if($this->_origin['extenumbers'] === false)
						$this->_extenumbers->delete($this->_status,$val);
					else
						$this->_extenumbers->edit_origin($this->_origin['extenumbers']);
					break;
			}
		}
	}
}

?>
