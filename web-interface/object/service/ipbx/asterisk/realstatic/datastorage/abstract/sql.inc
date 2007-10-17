<?php

xivo_service_asterisk::required(array('realstatic','datastorage','abstract','datastorage.inc'),true);

class xivo_service_asterisk_realstatic_abstract_sql extends xivo_service_asterisk_realstatic_abstract_datastorage
{
	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso);

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function get($arr,$disable=null,$delete=null,$id=false)
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
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

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
		return($this->get($arr,$disable,null,true));
	}

	function get_nb($arr=null,$disable=null)
	{
		if($arr !== null
		&& (is_array($arr) === false || empty($arr) === true) === true)
			return(false);

		$where = '';

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

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

		if(array_key_exists('commented',$arr) === true)
			$arr['commented'] = intval((bool) $arr['commented']);

		$arr['filename'] = $this->_param['filename'];

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
			$id['commented'] = intval((bool) $disable);

		$id['filename'] = $this->_param['filename'];

		if(($where = $this->_dso->where($id)) === false)
			return(false);

		if(array_key_exists('commented',$arr) === true)
			$arr['commented'] = intval((bool) $arr['commented']);

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
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		$r = $this->_dso->delete($this->_param['table'],$where);

		return($r);
	}

	function get_list($disable=null,$delete=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		$where = '';

		$arr = array();

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

		if(empty($arr) === false && ($where = $this->_dso->where($arr)) === false)
			return(false);

		if($order !== null)
			$this->_dso->order($order);

		if($limit !== null)
			$this->_dso->limit($limit);

		$col = $all === true ? null : 'id';

		$r = $this->_dso->select_all($this->_param['table'],$col,$where);

		if($order !== null)
			$this->_dso->order();

		if($limit !== null)
			$this->_dso->limit();

		return($r);
	}

	function get_list_where($arr,$disable=null,$delete=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

		if(($where = $this->_dso->where($arr)) === false)
			return(false);

		if($order !== null)
			$this->_dso->order($order);

		if($limit !== null)
			$this->_dso->limit($limit);

		$col = $all === true ? null : 'id';

		$r = $this->_dso->select_all($this->_param['table'],$col,$where);

		if($order !== null)
			$this->_dso->order();

		if($limit !== null)
			$this->_dso->limit();

		return($r);
	}

	function get_all($disable=null,$delete=null,$order=null,$limit=null)
	{
		return($this->get_list($disable,null,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$delete=null,$order=null,$limit=null)
	{
		return($this->get_list_where($arr,null,$disable,$order,$limit,true));
	}
}

?>
