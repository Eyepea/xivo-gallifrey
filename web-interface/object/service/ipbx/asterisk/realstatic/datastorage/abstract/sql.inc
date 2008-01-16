<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'datastorage.inc');

class xivo_service_asterisk_realstatic_abstract_sql extends xivo_abstract_datastorage
{
	function xivo_service_asterisk_realstatic_abstract_sql(&$dso,$param=array())
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

		$this->_dso->new_select($this->_param['table'],($id === true ? 'id' : '*'));

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

		$this->_dso->where($arr);

		if(($r = $this->_dso->select_single()) !== false && $id === true)
			$r = (int) $r['id'];

		return($r);
	}

	function get_id($arr,$disable=null)
	{
		return($this->get($arr,$disable,true));
	}

	function get_nb($arr=null,$disable=null)
	{
		if($arr !== null
		&& (is_array($arr) === false || empty($arr) === true) === true)
			return(false);

		if(is_array($arr) === false)
			$arr = array();

		$arr['filename'] = $this->_param['filename'];

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$this->_dso->new_select($this->_param['table']);

		if(is_array($arr) === true)
			$this->_dso->where($arr);

		return((int) $this->_dso->select_count());
	}

	function add($arr,$insertid=true)
	{
		$insertid = (bool) $insertid;

		if(is_array($arr) === false)
			return(false);

		if(array_key_exists('commented',$arr) === true)
			$arr['commented'] = intval((bool) $arr['commented']);

		$arr['filename'] = $this->_param['filename'];

		if($this->_dso->insert($this->_param['table'],$arr) === false)
			return(false);

		return(($insertid === true ? $this->_dso->insert_id() : true));
	}

	function edit($arr,$value,$disable=null)
	{
		if(is_array($value) === false)
			return(false);

		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		$value['filename'] = $arr['filename'] = $this->_param['filename'];

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		if(array_key_exists('commented',$value) === true)
			$value['commented'] = intval((bool) $value['commented']);

		return($this->_dso->update($this->_param['table'],$value,$arr));
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

		$arr['filename'] = $this->_param['filename'];

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		return($this->_dso->delete($this->_param['table'],$arr));
	}

	function get_list($disable=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		$this->_dso->new_select($this->_param['table'],($all === true ? '*' : 'id'));

		$arr = array();

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

		$this->_dso->where($arr);

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		$this->_cnt = 0;

		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$this->_dso->new_select($this->_param['table'],($all === true ? '*' : 'id'));

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$arr['filename'] = $this->_param['filename'];

		$this->_dso->where($arr);

		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
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
