<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agentgroup_sql extends xivo_service_asterisk_abstract_sql
{
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

		$this->_dso->new_select($this->_param['table'],($id === true ? 'id' : '*'));

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

		$this->_dso->where($arr);

		if(($r = $this->_dso->select_single()) !== false && $id === true)
			$r = (int) $r['id'];

		return($r);
	}

	function get_id($arr,$disable=null,$delete=null)
	{
		return($this->get($arr,$disable,$delete,true));
	}

	function get_nb($arr=null,$disable=null,$delete=null)
	{
		if($arr !== null
		&& (is_array($arr) === false || empty($arr) === true) === true)
			return(false);

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

		$this->_dso->new_select($this->_param['table']);

		if(is_array($arr) === true)
			$this->_dso->where($arr);

		return((int) $this->_dso->select_count());
	}

	function get_last()
	{
		$this->_dso->new_select($this->_param['table'],array('cnt' => (object) 'COUNT(*)',
								     'id' => (object) 'MAX(id)'));

		if(($r = $this->_dso->select_single()) !== false)
			$r = $r['cnt'] > 0 ? $r['id'] : 0;

		return($r);
	}

	function get_free()
	{
		$this->_dso->new_select($this->_param['table'],'id');
		$this->_dso->where(array('deleted' => 1));
		$this->_dso->new_order(array('id' => SORT_ASC));

		$r = $this->_dso->select_one();

		$this->_dso->reset_order();

		return($r);
	}

	function add($arr,$insertid=true)
	{
		$insertid = (bool) $insertid;

		if(is_array($arr) === false)
			return(false);

		if(array_key_exists($this->_disable_varname,$arr) === true)
			$arr[$this->_disable_varname] = intval((bool) $arr[$this->_disable_varname]);

		if(array_key_exists('deleted',$arr) === true)
			$arr['deleted'] = intval((bool) $arr['deleted']);

		if($this->_dso->insert($this->_param['table'],$arr) === false)
			return(false);

		return(($insertid === true ? $this->_dso->insert_id() : true));
	}

	function edit($arr,$value,$disable=null,$delete=null)
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

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

		if(array_key_exists($this->_disable_varname,$value) === true)
			$value[$this->_disable_varname] = intval((bool) $value[$this->_disable_varname]);

		if(array_key_exists('deleted',$value) === true)
			$value['deleted'] = intval((bool) $value['deleted']);

		return($this->_dso->update($this->_param['table'],$value,$arr));
	}

	function get_list($disable=null,$delete=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		$this->_dso->new_select($this->_param['table'],($all === true ? '*' : 'id'));

		$arr = array();

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

		if(empty($arr) === false)
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

	function get_list_where($arr,$disable=null,$delete=null,$order=null,$limit=null,$all=false)
	{
		$all = (bool) $all;

		$this->_cnt = 0;

		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$this->_dso->new_select($this->_param['table'],($all === true ? '*' : 'id'));

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if($delete !== null)
			$arr['deleted'] = intval((bool) $delete);

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

	function get_all($disable=null,$delete=null,$order=null,$limit=null)
	{
		return($this->get_list($disable,$delete,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$delete=null,$order=null,$limit=null)
	{
		return($this->get_list_where($arr,$disable,$delete,$order,$limit,true));
	}
}

?>
