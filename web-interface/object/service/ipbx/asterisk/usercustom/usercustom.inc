<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_usercustom extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'usercustom';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_usercustom(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}
	
	function _prepare($data)
	{
		$data['initialized'] = true;
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		$arr['name'] = null;
		$arr['category'] = 'user';

		if(isset($arr['interface']) === true)
		{
			$where = array();
			$where['interface'] = strtolower($arr['interface']);
			$where['category'] = $arr['category'];

			if($this->get_id($where) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$arr['name'] = null;
		$arr['category'] = 'user';

		if(isset($arr['interface']) === true)
		{
			$where = array();
			$where['interface'] = strtolower($arr['interface']);
			$where['category'] = $arr['category'];

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable));
	}
}

?>
