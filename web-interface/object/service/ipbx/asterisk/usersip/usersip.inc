<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_usersip extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'usersip';
	var $_filter	= false;	
	var $_origin	= false;

	function xivo_service_asterisk_usersip(&$sre,&$dso)
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
		$initialized = $this->_conf['initialized'].':'.$data['name'].'@';

		$data['initialized'] = strncmp($initialized,$data['fullcontact'],strlen($initialized)) === 0 ? true : false;
		$data['commented'] = (bool) $data['commented'];
		$data['callgroup'] = empty($data['callgroup']) === false ? explode(',',$data['callgroup']) : '';
		$data['pickupgroup'] = empty($data['pickupgroup']) === false ? explode(',',$data['pickupgroup']) : '';

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		$arr['pickupgroup'] = $arr['callgroup'];
		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if($this->get_id($where) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['callgroup']) === true)
			$arr['pickupgroup'] = $arr['callgroup'];

		$arr['category'] = 'user';

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable));
	}
}

?>
