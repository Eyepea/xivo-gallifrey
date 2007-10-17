<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_features_general extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_extenumbers	= null;
	var $_features		= null;
	var $_sound		= false;
	var $_conf		= array();
	var $_filter		= false;
	var $_category		= 'general';
	var $_filename		= 'features.conf';

	function xivo_application_service_asterisk_features_general(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_extenumbers = &$this->get_abstract('extenumbers')) === false)
			return(false);

		$param = array();
		$param['filename'] = $this->_filename;

		if(($this->_features = &$this->_service->get_realstatic('features',$param)) === false)
			return(false);

		$this->_load_config(dirname(__FILE__));

		return(true);
	}

	function get($name,$disable=null)
	{
		$this->_info = $this->_origin = array();

		$this->_get_generalfeatures($name);

		if($this->_chk_exists_cfg('exten',$name) === true)
			$this->_get_extenumbers();

		return($this->_info);
	}

	function get_all_by_category($disable=null)
	{
		$this->_info = $this->_origin = array();

		$arr = array();
		$arr['category'] = $this->_category;

		$r = $this->_features->get_all_by_category($arr,$disable);

		return($r);
	}

	function get_sound()
	{
		if($this->_sound !== false)
			return($this->_sound);

		if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		$this->_sound = $sounds->get_list('features',true);

		return($this->_sound);
	}

	function get_config_exten()
	{
		if(isset($this->_conf) === false
		|| xivo_issa('exten',$this->_conf) === false)
			return(false);

		return($this->_conf['exten']);
	}

	function get_config_sound()
	{
		if(isset($this->_conf) === false
		|| xivo_issa('sound',$this->_conf) === false)
			return(false);

		return($this->_conf['sound']);
	}

	function chk_sound($value)
	{
		if(xivo_haslen($value) === false)
			return(true);
		else if(is_string($value) === false)
			return(false);

		$this->get_sound();

		if($this->_sound === false || isset($this->_sound[$value]) === false)
			return(false);

		return($value);
	}

	function get_elements()
	{
		return($this->get_element());
	}

	function _get_generalfeatures($name,$disable=null)
	{
		$this->_info['generalfeatures'] = false;
		$this->_origin['generalfeatures'] = false;

		$arr = array();
		$arr['var_name'] = $name;
		$arr['category'] = $this->_category;

		$this->_info['generalfeatures'] = $this->_features->get_where($arr,$disable);
		$this->_origin['generalfeatures'] = $this->_features->get_origin();

		return($this->_info['generalfeatures']);
	}

	function _get_extenumbers()
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		if(isset($this->_info['generalfeatures']['var_name']) === false)
			return(false);

		$r = parent::_get_extenumbers('generalfeatures',$this->_info['generalfeatures']['var_name']);

		return($r);
	}

	function set($arr)
	{
		$this->_reset();

		if(is_array($arr) === false || isset($arr['var_name']) === false)
			return(false);

		$this->get($arr['var_name']);

		$this->set_generalfeatures($arr);

		if($this->_chk_exists_cfg('exten',$arr['var_name']) === true)
		{
			$number = (string) $this->get_result_var('generalfeatures','var_val');
			$context = '';

			$this->set_extenumbers('save','generalfeatures',$number,$context);
		}

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_generalfeatures($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['var_name'],$arr['var_val']) === false)
			return(false);

		$rs = array();
		$rs['var_name'] = $arr['var_name'];
		$rs['commented'] = null;

		if(isset($arr['commented']) === true)
			$rs['commented'] = (bool) $arr['commented'];

		if(($value = $this->set_chk_value($arr['var_name'],$arr['var_val'],true)) === false)
		{
			if(($value = $this->get_filter_result()) !== false)
				$rs['var_val'] = $value[$arr['var_name']];

			$this->_set_result('generalfeatures',$rs);
			$this->_set_error('generalfeatures',$this->get_filter_error());

			return(false);
		}

		$rs['var_val'] = $value;

		if($this->_chk_exists_cfg('sound',$arr['var_name']) === true
		&& $this->chk_sound($value) === false)
		{
			$this->_set_result('generalfeatures',$rs);
			$this->_set_error('generalfeatures','wrong sound file');
			
			return(false);
		}

		$this->_set_result('generalfeatures',$rs);

		return(true);
	}

	function save()
	{
		$this->_status = 'save';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| ($name = $this->save_generalfeatures()) === false
		|| $this->save_extenumbers($name) === false)
		{
			$this->_reverse_save();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function save_generalfeatures()
	{
		if(isset($this->_info['generalfeatures']) === false)
			return(false);

		if($this->_info['generalfeatures'] === false)
			return($this->_add_generalfeatures());

		if(($rs = $this->get_result('generalfeatures')) === null
		|| $rs === false)
			return(false);

		$generalfeatures_where = array();
		$generalfeatures_where['category'] = $this->_category;
		$generalfeatures_where['var_name'] = $rs['var_name'];

		$r = $this->_features->edit_where($generalfeatures_where,$rs);

		if($r === false)
			$this->_return['generalfeatures'] = false;
		else
			$this->_return['generalfeatures'] = $rs['var_name'];

		return($this->_return['generalfeatures']);
	}

	function _add_generalfeatures()
	{
		if(($rs = $this->get_result('generalfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['category'] = $this->_category;

		if($this->_features->add($rs) === false)
			$this->_return['generalfeatures'] = false;
		else
			$this->_return['generalfeatures'] = $rs['var_name'];

		return($this->_return['generalfeatures']);
	}

	function save_extenumbers($name)
	{
		if($this->_chk_exists_cfg('exten',$name) === false)
			return(null);

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
				case 'generalfeatures':
					if(isset($this->_origin['generalfeatures']) === false)
						continue;

					if($this->_origin['generalfeatures'] === false)
					{
						$generalfeatures_where = array();
						$generalfeatures_where['category'] = $this->_category;
						$generalfeatures_where['var_name'] = $val;

						$this->_features->delete_where($generalfeatures_where);
					}
					else
						$this->_features->edit_origin($this->_origin['generalfeatures']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === false
					|| ($extenumbers = &$this->_service->get_module('extenumbers')) === false)
						continue;

					if($this->_origin['extenumbers'] === false)
						$extenumbers->delete($val);
					else
						$extenumbers->edit_origin($this->_origin['extenumbers']);
					break;
			}
		}
	}
}

?>
