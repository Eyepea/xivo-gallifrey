<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_agentfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'agentfeatures';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_agentfeatures(&$sre,&$dso)
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
		if(is_array($id) === true && ($id = $this->get_where($id)) === false)
			return(false);

		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];
		$data['silent'] = (bool) $data['silent'];

		return($data);
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

		if(isset($arr['number']) === true)
		{
			$where = array('number' => $arr['number']);

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

		if(isset($arr['number']) === true)
		{
			$where = array('number' => $arr['number']);

			if(($whereid = $this->get_where($where)) !== false && (int) $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr));
	}

	function edit_where($arr,$values)
	{
		$arr = (array) $arr;

		if(is_array($values) === false
		|| ($where = $this->get_elements_where($arr)) === false
		|| ($values = $this->chk_set_values($values)) === false)
			return(false);

		return($this->_dso->edit_where($where,$values));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function get_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->get_where($where));
	}

	function get_nb_by_group($group,$disable=null)
	{
		if(($group = xivo_uint($group)) === 0)
			return(false);

		return($this->_dso->get_nb_by_group($group,$disable));
	}

	function get_all_where($arr=array(),$disable=null,$prepare=true)
	{
		$arr = (array) $arr;
		$prepare = (bool) $prepare;

		if(($where = $this->get_elements_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $prepare === true ? $this->_prepare($list[$i]) : $list[$i];

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
