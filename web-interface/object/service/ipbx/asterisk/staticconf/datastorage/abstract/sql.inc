<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'datastorage.inc');

class xivo_service_asterisk_staticconf_abstract_sql extends xivo_abstract_datastorage
{
	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso);

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function get($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_param['table']);

		$arr = array();
		$arr['id'] = $id;
		$arr['filename'] = $this->_param['filename'];

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$this->_dso->where($arr);

		return($this->_dso->select_single());
	}

	function get_where($arr,$disable=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$this->_dso->new_select($this->_param['table'],'id');

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$this->_dso->where($arr);

		return($this->_dso->select_one());
	}

	function add($arr)
	{
		$r = false;

		if($this->_dso->insert($this->_param['table'],$arr) === true)
			$r = $this->_dso->insert_id();

		return($r);
	}

	function edit($id,$value)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->update($this->_param['table'],$value,array('id' => $id)));
	}

	function edit_where($arr,$value,$disable=null)
	{
		if(is_array($arr) === false || is_array($value) === false)
			return(false);

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		return($this->_dso->update($this->_param['table'],$value,$arr));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($this->_param['table'],array('id' => $id)));
	}

	function delete_where($arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

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
