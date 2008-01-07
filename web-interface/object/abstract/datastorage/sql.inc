<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'datastorage.inc');

class xivo_abstract_datastorage_sql extends xivo_abstract_datastorage
{
	var $_cnt		= 0;
	var $_disable_varname	= 'disable';

	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso);

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function get($arr,$disable=null,$id=false)
	{
		$id = (bool) $id;

		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$id = $id === true ? 'id' : null;

		$r = $this->_dso->select_one($this->_param['table'],$id,$where);

		if($r !== false && $id !== null)
			$r = (int) $r['id'];

		return($r);
	}

	function get_id($arr,$disable=null)
	{
		return($this->get($arr,$disable,true));
	}

	function get_cnt()
	{
		return(xivo_uint($this->_cnt));
	}

	function get_nb($arr=null,$disable=null)
	{
		if($arr !== null
		&& (is_array($arr) === false || empty($arr) === true) === true)
			return(false);

		$where = '';

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if(is_array($arr) === true && ($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->select_count($this->_param['table'],null,$where);

		return($r);
	}

	function add($arr,$insertid=true)
	{
		$r = false;

		$insertid = (bool) $insertid;

		if(is_array($arr) === false)
			return(false);

		if(array_key_exists($this->_disable_varname,$arr) === true)
			$arr[$this->_disable_varname] = intval((bool) $arr[$this->_disable_varname]);

		if($this->_dso->insert($this->_param['table'],$arr) === true)
			$r = $insertid === true ? $this->_dso->insert_id() : true;

		return($r);
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if(is_array($id) === false)
		{
			if(($id = xivo_uint($id)) !== 0)
				$id = array('id' => $id);
			else
				return(false);
		}

		if($disable !== null)
			$id[$this->_disable_varname] = intval((bool) $disable);

		if(($where = $this->_dso->where($id)) === false)
			return(false);

		if(array_key_exists($this->_disable_varname,$arr) === true)
			$arr[$this->_disable_varname] = intval((bool) $arr[$this->_disable_varname]);

		$r = $this->_dso->update($this->_param['table'],$arr,$where);

		return($r);
	}

	function delete($arr,$disable=null)
	{
		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],$where);

		return($r);
	}

	function get_list($disable=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		$where = '';

		$arr = array();

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if(empty($arr) === false && ($where = $this->_dso->where($arr)) === false)
			return(false);

		$this->_dso->new_order($order);
		$this->_dso->new_limit($limit);

		$col = $all === true ? null : 'id';

		$r = $this->_dso->select_all($this->_param['table'],$col,$where);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		$this->_cnt = $this->_dso->select_count($this->_param['table'],null,$where);

		if($this->_cnt === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$this->_dso->new_order($order);
		$this->_dso->new_limit($limit);

		$col = $all === true ? null : 'id';

		$r = $this->_dso->select_all($this->_param['table'],$col,$where);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		$this->_cnt = $this->_dso->select_count($this->_param['table'],null,$where);

		if($this->_cnt === false)
			$r = false;

		return($r);
	}

	function get_all($disable=null,$order=null,$limit=null)
	{
		return($this->get_list($disable,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$order=null,$limit=null)
	{
		return($this->get_list_where($arr,$disable,$order,$limit,true));
	}
}

?>
