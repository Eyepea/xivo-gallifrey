<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_realstatic_features_featuremap extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_extenumbers	= null;
	var $_features		= null;
	var $_conf		= array();
	var $_filter		= false;
	var $_category		= 'featuremap';
	var $_cat_metric	= 1;
	var $_filename		= 'features.conf';

	function xivo_application_service_asterisk_realstatic_features_featuremap(&$service)
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

		$this->_get_featuremap($name);
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

	function get_elements()
	{
		return($this->get_element());
	}

	function _get_featuremap($name,$disable=null)
	{
		$this->_info['featuremap'] = false;
		$this->_origin['featuremap'] = false;

		$arr = array();
		$arr['var_name'] = $name;
		$arr['category'] = $this->_category;

		$this->_info['featuremap'] = $this->_features->get_where($arr,$disable);
		$this->_origin['featuremap'] = $this->_features->get_origin();

		return($this->_info['featuremap']);
	}

	function _get_extenumbers()
	{
		$this->_info['extenumbers'] = false;
		$this->_origin['extenumbers'] = false;

		if(isset($this->_info['featuremap']['var_name']) === false)
			return(false);

		$r = parent::_get_extenumbers('featuremap',$this->_info['featuremap']['var_name']);

		return($r);
	}

	function set($arr)
	{
		$this->_reset();

		if(is_array($arr) === false || isset($arr['var_name']) === false)
			return(false);

		$this->get($arr['var_name']);

		$this->set_featuremap($arr);

		$number = (string) $this->get_result_var('featuremap','var_val');
		$context = '';

		$this->set_extenumbers('save','featuremap',$number,$context);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_featuremap($arr)
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

			$this->_set_result('featuremap',$rs);
			$this->_set_error('featuremap',$this->get_filter_error());

			return(false);
		}

		$rs['var_val'] = $value;

		$this->_set_result('featuremap',$rs);

		return(true);
	}

	function save()
	{
		$this->_status = 'save';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| ($name = $this->save_featuremap()) === false
		|| $this->save_extenumbers($name) === false)
		{
			$this->_reverse_save();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function save_featuremap()
	{
		if(isset($this->_info['featuremap']) === false)
			return(false);

		if($this->_info['featuremap'] === false)
			return($this->_add_featuremap());

		if(($rs = $this->get_result('featuremap')) === null
		|| $rs === false)
			return(false);

		$featuremap_where = array();
		$featuremap_where['category'] = $this->_category;
		$featuremap_where['var_name'] = $rs['var_name'];

		$r = $this->_features->edit_where($featuremap_where,$rs);

		if($r === false)
			$this->_return['featuremap'] = false;
		else
			$this->_return['featuremap'] = $rs['var_name'];

		return($this->_return['featuremap']);
	}

	function _add_featuremap()
	{
		if(($rs = $this->get_result('featuremap')) === null
		|| $rs === false)
			return(false);

		$rs['category'] = $this->_category;
		$rs['cat_metric'] = $this->_cat_metric;

		if($this->_features->add($rs) === false)
			$this->_return['featuremap'] = false;
		else
			$this->_return['featuremap'] = $rs['var_name'];

		return($this->_return['featuremap']);
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
				case 'featuremap':
					if(isset($this->_origin['featuremap']) === false)
						continue;

					if($this->_origin['featuremap'] === false)
					{
						$featuremap_where = array();
						$featuremap_where['category'] = $this->_category;
						$featuremap_where['var_name'] = $val;

						$this->_features->delete_where($featuremap_where);
					}
					else
						$this->_features->edit_origin($this->_origin['featuremap']);
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
