<?php

xivo_service_asterisk::required(array('staticconf','staticconf.inc'),true);

class xivo_service_asterisk_meetme extends xivo_service_asterisk_staticconf
{
	var $_dso	= null;
	var $_name	= 'meetme';
	var $_filename	= 'meetme.conf';
	var $_category	= 'rooms';
	var $_var_name	= 'conf';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_meetme(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_dso->set_param('filename',$this->_filename);

		$this->_load_config();
	}

	function get($id,$disable=null)
	{
		if(($r = parent::get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$val = explode(',',$data['var_val']);

		$data['number'] = isset($val[0]) === true ? $val[0] : '';
		$data['pin'] = isset($val[1]) === true ? $val[1] : '';
		$data['admin-pin'] = isset($val[2]) === true ? $val[2] : '';
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = parent::add($this->_origin,$this->_origin['id']);
		else
			$r = parent::edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function add($val,$id=0)
	{
		$arr = array();

		$arr['category'] = $this->_category;
		$arr['var_name'] = $this->_var_name;
		$arr['var_val'] = $val;

		return(parent::add($arr,$id));
	}

	function edit($id,$val)
	{
		$arr = array();

		$arr['category'] = $this->_category;
		$arr['var_name'] = $this->_var_name;
		$arr['var_val'] = $val;

		return(parent::edit($id,$arr));
	}
	
	function enable($id)
	{
		if(($r = parent::get($id,true)) === false)
			return(false);

		$r['commented'] = false;

		return(parent::edit($r['id'],$r));
	}

	function disable($id)
	{
		if(($r = parent::get($id,false)) === false)
			return(false);

		$r['commented'] = true;

		return(parent::edit($r['id'],$r));
	}
}

?>
