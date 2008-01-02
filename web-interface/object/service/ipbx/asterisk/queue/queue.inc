<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_queue extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queue';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_queue(&$sre,&$dso)
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
		$this->_origin = false;

		if(is_scalar($id) === false
		|| ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function _origin($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false)
			return(false);

		if($type === 'add')
			return($this->add($orig));
		else
			return($this->edit($orig['name'],$orig));
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);
			$where = array('name' => $arr['name']);

			if($this->get_id($where) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_scalar($id) === false || is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return($this->_dso->edit($id,$arr,$disable));
	}

	function delete($id,$disable=null)
	{
		if(is_scalar($id) === false)
			return(false);

		return($this->_dso->delete($id));
	}

	function enable($id,$value=false)
	{
		if(is_scalar($id) === false)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array('commented' => $value)));
	}

	function disable($id,$value=true)
	{
		if(is_scalar($id) === false)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array('commented' => $value)));
	}

	function get_all($disable=null,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(($list = $this->_dso->get_all($disable,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);
		else if(($nb = count($list)) === 0)
			return(false);

		$r = array();

		if($assoc === true && $prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[$list[$i]['name']] = $this->_prepare($list[$i]);
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[$list[$i]['name']] = $list[$i];
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}

		return($r);
	}

	function get_all_where($arr,$disable=null,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);
		else if(($nb = count($list)) === 0)
			return(false);

		$r = array();

		if($assoc === true && $prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[$list[$i]['name']] = $this->_prepare($list[$i]);
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[$list[$i]['name']] = $list[$i];
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}

		return($r);
	}
}

?>
