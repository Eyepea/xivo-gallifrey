<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_groupfeatures_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'groupfeatures');

	function get($arr,$delete=null,$id=false)
	{
		$id = (bool) $id;

		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$id = $id === true ? 'id' : null;

		$r = $this->_dso->select_one($this->_param['table'],$id,$where);

		if($r !== false && $id !== null)
			$r = (int) $r['id'];

		return($r);
	}

	function get_nb($arr=null,$delete=null)
	{
		if($arr !== null
		&& (is_array($arr) === false || empty($arr) === true) === true)
			return(false);

		$where = '';

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

		if(is_array($arr) === true && ($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->select_count($this->_param['table'],null,$where);

		return($r);
	}

	function get_last()
	{
		$r = $this->_dso->select_one($this->_param['table'],'COUNT(*) AS cnt, MAX(id) AS id');

		if($r !== false)	
			$r = $r['cnt'] > 0 ? $r['id'] : 0;

		return($r);
	}

	function get_free()
	{
		$r = false;

		$this->_dso->new_order(array('id' => SORT_ASC));

		$r = $this->_dso->select_one($this->_param['table'],'id','deleted = 1');

		$this->_dso->reset_order();

		if($r !== false)
			$r = $r['id'];

		return($r);
	}

	function add($arr,$insertid=true)
	{
		$r = false;

		$insertid = (bool) $insertid;

		if(is_array($arr) === false)
			return(false);

		if(array_key_exists('deleted',$arr) === true)
			$arr['deleted'] = intval((bool) $arr['deleted']);

		if($this->_dso->insert($this->_param['table'],$arr) === true)
			$r = $insertid === true ? $this->_dso->insert_id() : true;

		return($r);
	}

	function edit($id,$arr,$delete=null)
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

		if($delete !== null)
			$id['deleted'] = intval((bool) $delete);

		if(($where = $this->_dso->where($id)) === false)
			return(false);

		if(array_key_exists('deleted',$arr) === true)
			$arr['deleted'] = intval((bool) $arr['deleted']);

		$r = $this->_dso->update($this->_param['table'],$arr,$where);

		return($r);
	}

	function delete($arr)
	{
		return(parent::delete($arr,null));
	}

	function get_list($delete=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		$where = '';

		$arr = array();

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

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

	function get_list_where($arr,$delete=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

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
}

?>
