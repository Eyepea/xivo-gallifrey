<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_queuemember extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'queuemember';
	var $_filter		= null;
	var $_origin		= false;
	var $_origin_list	= false;

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

	function get($id,$disable=null)
	{
		if(is_array($id) === false
		|| isset($id['queue_name'],$id['interface']) === false
		|| is_scalar($id['queue_name']) === false
		|| is_scalar($id['interface']) === false
		|| ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin);
		else
			$r = $this->edit(array($this->_origin['queue_name'],$this->_origin['interface']),$this->_origin);

		return($r);
	}

	function _origin_list($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($this->_origin_list) === false
		|| ($nb = count($this->_origin_list)) === 0)
			return(false);

		if($type === 'add')
		{
			for($i = 0;$i < $nb;$i++)
				$this->add($this->_origin_list[$i]);
		}
		else
		{
			$id = array();

			for($i = 0;$i < $nb;$i++)
			{
				$id[0] = $this->_origin_list[$i]['queue_name'];
				$id[1] = $this->_origin_list[$i]['interface'];

				$this->edit($id,$this->_origin_list[$i]);
			}
		}

		return(true);
	}

	function get_all_by_category($arr=array(),$disable=null)
	{
		$arr = (array) $arr;
		$prepare = (bool) $prepare;

		if(empty($arr) === true)
		{
			if(($list = $this->get_all($disable)) === false)
				return(false);
		}
		else if(($list = $this->get_all_where($arr,$disable)) === false)
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
		&& (($nb = xivo_uint($arr)) === 0
		   || ($values = $this->chk_values($values,true,false,false)) === false) === true)
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

	function edit($id,$arr,$disable=null)
	{
		if(is_array($id) === false
		|| is_array($arr) === false
		|| isset($id['queue_name'],$id['interface']) === false
		|| is_scalar($id['queue_name']) === false
		|| is_scalar($id['interface']) === false)
			return(false);

		return($this->_dso->edit($id,$arr,$disable));
	}

	function delete($id,$disable=null)
	{
		if(is_array($id) === false
		|| isset($id['queue_name'],$id['interface']) === false
		|| is_scalar($id['queue_name']) === false
		|| is_scalar($id['interface']) === false)
			return(false);

		return($this->_dso->delete($id,$disable));
	}
}

?>
