<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_queuemember extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'queuemember';
	var $_filter		= false;
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
		$this->_origin = false;

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
			$r = $this->add($orig);
		else
			$r = $this->edit(array($orig['queue_name'],$orig['interface']),$orig);

		return($r);
	}

	function _origin_list($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin_list;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false
		|| ($nb = count($orig)) === 0)
			return(false);

		if($type === 'add')
		{
			for($i = 0;$i < $nb;$i++)
				$this->add($orig[$i]);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$this->edit(array($orig[$i]['queue_name'],$orig[$i]['interface']),$orig[$i]);
		}

		return(true);
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);

		return($this->_dso->add($arr));
	}

	function add_list($arr)
	{
		if(is_array($arr) === false)
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

	function enable($id,$value=false)
	{
		if(is_array($id) === false
		|| isset($id['queue_name'],$id['interface']) === false
		|| is_scalar($id['queue_name']) === false
		|| is_scalar($id['interface']) === false)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array('commented' => $value)));
	}

	function disable($id,$value=true)
	{
		if(is_array($id) === false
		|| isset($id['queue_name'],$id['interface']) === false
		|| is_scalar($id['queue_name']) === false
		|| is_scalar($id['interface']) === false)
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
			{
				$key = $list[$i]['queue_name'].'-'.$list[$i]['interface'];
				$r[$key] = $this->_prepare($list[$i]);
			}
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$key = $list[$i]['queue_name'].'-'.$list[$i]['interface'];
				$r[$key] = $list[$i];
			}
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
			{
				$key = $list[$i]['queue_name'].'-'.$list[$i]['interface'];
				$r[$key] = $this->_prepare($list[$i]);
			}
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$key = $list[$i]['queue_name'].'-'.$list[$i]['interface'];
				$r[$key] = $list[$i];
			}
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
