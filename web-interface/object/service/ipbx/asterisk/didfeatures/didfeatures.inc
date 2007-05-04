<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','abstract.inc'),true);

class xivo_service_asterisk_didfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'didfeatures';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_didfeatures(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id)) === false)
			return(false);

		$this->_origin = $r;

		$r['commented'] = (bool) $r['commented'];

		return($r);
	}

	function get_where($arr)
	{
		$arr = (array) $arr;

		$where = array();

		if(isset($arr['extenid']) === true)
			$where['extenid'] = $arr['extenid'];

		if(isset($arr['type']) === true)
			$where['type'] = $arr['type'];

		if(isset($arr['typeid']) === true)
			$where['typeid'] = $arr['typeid'];

		if(isset($arr['custom']) === true)
			$where['custom'] = $arr['custom'];

		if(($id = $this->_dso->get_where($where)) === false)
			return(false);

		return($this->get($id));
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
			$r = $this->add($this->_origin);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function get_by_extenid($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$r = $this->get_where(array('extenid' => $id));

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function edit_where($arr,$values)
	{
		$arr = (array) $arr;

		if(is_array($values) === false || ($values = $this->chk_set_values($values,true,true)) === false)
			return(false);

		$where = array();

		if(isset($arr['id']) === true)
			$where['id'] = $arr['id'];

		if(isset($arr['extenid']) === true)
			$where['extenid'] = $arr['extenid'];

		if(isset($arr['type']) === true)
			$where['type'] = $arr['type'];

		if(isset($arr['typeid']) === true)
			$where['typeid'] = $arr['typeid'];

		if(isset($arr['custom']) === true)
			$where['custom'] = $arr['custom'];

		if(isset($arr['commented']) === true)
			$where['commented'] = $arr['commented'];

		return($this->_dso->edit_where($where,$values));
	}

	function edit_list_where($arr,$values)
	{
		$r = false;

		if(is_array($arr) === false || ($values = $this->chk_set_values($values,true,true)) === false)
			return($r);

		$arr_values = array_values($arr);

		if(($nb = count($arr_values)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
			$this->_dso->edit_where(array('id' => $arr[$i]),$values);

		return(true);
	}

	function get_list_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;

		$where = array();

		if(isset($arr['extenid']) === true)
			$where['extenid'] = $arr['extenid'];

		if(isset($arr['type']) === true)
			$where['type'] = $arr['type'];

		if(isset($arr['typeid']) === true)
			$where['typeid'] = $arr['typeid'];

		if(isset($arr['custom']) === true)
			$where['custom'] = $arr['custom'];

		if(($r = $this->_dso->get_list_where($where,$disable)) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
