<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_queuemember extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queuemember';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_queuemember(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($ids,$disable=null)
	{
		if(is_array($ids) === false || isset($ids['queue_name'],$ids['interface']) === false
		|| ($r = $this->_dso->get($ids,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

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
			$r = $this->add($this->_origin,array($this->_origin['queue_name'],$this->_origin['interface']));
		else
			$r = $this->edit(array($this->_origin['queue_name'],$this->_origin['interface']),$this->_origin);

		return($r);
	}

	function get_nb_by_name($name,$disable=null)
	{
		$name = (string) $name;

		return($this->_dso->get_nb_by_name($name,$disable));
	}

	function get_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->get_where($where));
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

	function get_all_by_category($arr=array(),$disable=null)
	{
		$arr = (array) $arr;
		$prepare = (bool) $prepare;

		if(empty($arr) === true)
		{
			if(($list = $this->_dso->get_all($disable)) === false)
				return(false);
		}
		else if(($where = $this->get_elements_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable)) === false)
				return(false);

		if(isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($r[$list[$i]['category']]) === false)
				$r[$list[$i]['category']] = array();

			$r[$list[$i]['category']][] = $list[$i];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);

		return($this->_dso->add($arr));
	}

	function add_list($arr,$values=null)
	{
		if(is_array($values) === false
		&& (($nb = xivo_uint($arr)) === 0 || ($values = $this->chk_set_values($values)) === false) === true)
			return(false);

		if(is_array($values) === true)
		{
			for($i = 0;$i < $nb;$i++)
				$this->add($values);
		}
		else if(is_array($arr) === false)
			return(false);

		$list = array_values($arr);

		if(($nb = count($list)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
			$this->add($list[$i]);

		return(true);
	}

	function edit($ids,$arr)
	{
		if(is_array($ids) === false || is_array($arr) === false
		|| isset($ids['queue_name'],$ids['interface']) === false)
			return(false);

		if(isset($arr['commented']) === true)
			$arr['commented'] = (bool) $arr['commented'];

		return($this->_dso->edit($ids,$arr));
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

	function delete($ids)
	{
		if(is_array($ids) === false || isset($ids['queue_name'],$ids['interface']) === false)
			return(false);

		return($this->_dso->delete($ids));
	}

	function delete_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->delete_where($where));
	}
}

?>
