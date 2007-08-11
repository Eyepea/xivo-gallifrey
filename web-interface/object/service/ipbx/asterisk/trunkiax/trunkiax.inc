<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_trunkiax extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'trunkiax';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_trunkiax(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id,$disable=null)
	{
		$r = false;

		if($disable !== null)
			$disable = (bool) $disable;

		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return($r);

		$this->_origin = $r;

		$r['commented'] = (bool) $r['commented'];

		return($r);
	}

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if($this->get_where($where) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$arr['category'] = 'trunk';

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_where($where)) !== false && (int) $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr));
	}

	function disable($id,$value=false)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$value = (bool) $value === false ? 0 : 1;

		return($this->_dso->edit($id,array('commented' => $value)));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function get_list()
	{
		if(($r = $this->_dso->get_list()) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->get_where($where));
	}
}

?>
