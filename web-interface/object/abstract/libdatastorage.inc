<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'libconfig.inc');

class xivo_libdatastorage_abstract extends xivo_libconfig_abstract
{
	function set_dso_param($name,$value)
	{
		if(is_object($this->_dso) === false)
			return(false);

		return($this->_dso->set_param($name,$value));
	}

	function get_dso_param($name,$value)
	{
		if(is_object($this->_dso) === false)
			return(false);

		return($this->_dso->get_param($name,$value));
	}

	function _prepare(&$data)
	{
		return($data);
	}

	function set_origin($arr)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;
		else
			return(false);

		if(is_array($arr) === false)
			return(false);

		$this->_origin = $arr;

		return(true);
	}

	function get_origin()
	{
		if(isset($this->_origin) === false
		|| is_array($this->_origin) === false)
			return(false);

		return($this->_origin);
	}

	function get_origin_list()
	{
		if(isset($this->_origin_list) === false
		|| is_array($this->_origin_list) === false)
			return(false);

		return($this->_origin_list);
	}

	function add_origin($val=null)
	{
		return($this->_origin('add',$val));
	}

	function add_origin_list($val=null)
	{
		return($this->_origin_list('add',$val));
	}

	function edit_origin($val=null)
	{
		return($this->_origin('edit',$val));
	}

	function edit_origin_list($val=null)
	{
		return($this->_origin_list('edit',$val));
	}

	function _origin($type,$val=null)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| isset($this->_origin) === false)
			return(false);

		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(is_array($orig) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($orig,$orig['id']);
		else
			$r = $this->edit($orig['id'],$orig);

		return($r);
	}

	function _origin_list($type,$val=null)
	{
		if(($type !== 'add' && $type !== 'edit') === true
		|| isset($this->_origin_list) === false)
			return(false);

		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin_list;

		if(is_array($orig) === false || ($nb = count($orig)) === 0)
			return(false);

		if($type === 'add')
		{
			for($i = 0;$i < $nb;$i++)
				$this->add($orig[$i],$orig[$i]['id']);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$this->edit($orig[$i]['id'],$orig[$i]);
		}

		return(true);
	}

	function get($id,$disable=null)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(($id = xivo_uint($id)) === 0
		|| ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_id($arr,$disable=null,$id=null)
	{
		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		if($id !== null
		&& (is_array($arr) === false || ($arr['id'] = xivo_uint($id)) === 0) === true)
			return(false);

		return($this->_dso->get_id($arr,$disable));
	}

	function get_where($arr,$disable=null,$id=null)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($id !== null && ($where['id'] = xivo_uint($id)) === 0) === true
		|| ($r = $this->_dso->get($where,$disable)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_nb($arr=null,$disable=null)
	{
		if($arr !== null && is_array($arr) === false)
			return(false);

		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_nb($arr,$disable));
	}

	function get_cnt()
	{
		return($this->_dso->get_cnt());
	}

	function get_list($disable=null,$order=null,$limit=null)
	{
		if(($r = $this->_dso->get_list($disable,$order,$limit)) !== false
		&& isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable,$order,$limit)) === false
		|| isset($r[0]) === false)
			return(false);

		return($r);
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
				$r[$list[$i]['id']] = $this->_prepare($list[$i]);
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[$list[$i]['id']] = $list[$i];
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
				$r[$list[$i]['id']] = $this->_prepare($list[$i]);
		}
		else if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[$list[$i]['id']] = $list[$i];
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[] = $this->_prepare($list[$i]);
		}

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

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = $this->_dso->edit($id,$arr,$disable);
		
		return($r);
	}

	function edit_where($id,$arr,$disable=null)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false
		|| ($arr = $this->chk_values($arr,true,false,false)) === false)
			return(false);

		$r = $this->_dso->edit($where,$arr,$disable);

		return($r);
	}

	function delete($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id,$disable));
	}

	function delete_where($id,$disable=null)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		return($this->_dso->delete($where,$disable));
	}

	function enable($id,$value=false)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array('disable' => $value)));
	}

	function disable($id,$value=true)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$value = intval((bool) $value);

		return($this->_dso->edit($id,array('disable' => $value)));
	}

	function enable_where($id,$value=false)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		$value = intval((bool) $value);

		$r = $this->_dso->edit($where,array('disable' => $value));

		return($r);
	}

	function disable_where($id,$value=true)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false)
			return(false);

		$value = intval((bool) $value);

		$r = $this->_dso->edit($where,array('disable' => $value));

		return($r);
	}
}

?>
