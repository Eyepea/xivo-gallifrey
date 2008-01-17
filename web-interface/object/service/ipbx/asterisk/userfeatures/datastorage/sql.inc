<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_userfeatures_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('internal'	=> 0);

	function _set_param_internal()
	{
		if($this->_param['internal'] !== null)
			$this->_param['internal'] = intval((bool) $this->_param['internal']);

		return($this->_param['internal']);
	}

	function get($arr,$disable=null)
	{
		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			unset($arr['internal']);

		return(parent::get($arr,$disable));
	}

	function get_id($arr,$disable=null)
	{
		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			unset($arr['internal']);

		return(parent::get($arr,$disable,true));
	}

	function get_nb($arr=null,$disable=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			unset($arr['internal']);

		return(parent::get_nb($arr,$disable));
	}

	function get_search($value,$disable=null,$order=null,$limit=null,$number=false)
	{
		$this->_cnt = 0;

		$number = (bool) $number;

		$this->_dso->new_select($this->_param['table']);
		$this->_dso->wherecontain('firstname',$value);
		$this->_dso->orwherecontain('lastname',$value);
		$this->_dso->orwherecontain('number',$value);
		$this->_dso->orwherecontain('name',$value);

		if($number === false)
			$this->_dso->orwherecontain('context',$value);

		if($this->aggregate_where() === false)
			return(false);

		if($disable !== null)
			$this->_dso->andwhere(array($this->_disable_varname => intval((bool) $disable)));

		if(($internal = $this->_set_param_internal()) !== null)
			$this->_dso->andwhere(array('internal' => $internal));

		if($number === true)
			$this->_dso->andwhere('IFNULL(number,\'\') != \'\'');

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

	function get_all_context($disable=null,$order=null,$limit=null)
	{
		$this->_cnt = 0;

		$this->_dso->new_select($this->_param['table'],'context');

		$arr = array();

		if($disable !== null)
			$arr[$this->_disable_varname] = intval((bool) $disable);

		if(($internal = $this->_set_param_internal()) !== null)
			$arr['internal'] = $internal;

		if(empty($arr) === false)
			$this->_dso->where($arr);

		$this->_dso->new_group('context');
		$this->_dso->new_order($order);
		$this->_dso->limit($limit);

		$r = $this->_dso->select_all();
	
		$this->_dso->reset_group();
		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count()) === false
		|| isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function edit($arr,$value,$disable=null)
	{
		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			unset($arr['internal']);

		return(parent::edit($arr,$value,$disable));
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

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			unset($arr['internal']);

		return(parent::delete($arr,$disable));
	}


	function get_list($disable=null,$order=null,$limit=null)
	{
		$arr = array();

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			$r = parent::get_list($disable,$order,$limit);
		else
			$r = parent::get_list_where($arr,$disable,$order,$limit);

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			unset($arr['internal']);

		return(parent::get_list_where($arr,$disable,$order,$limit));
	}

	function get_all($disable=null,$order=null,$limit=null)
	{
		$arr = array();

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			$r = parent::get_list($disable,$order,$limit,true);
		else
			$r = parent::get_list_where($arr,$disable,$order,$limit,true);

		return($r);
	}

	function get_all_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		if(($arr['internal'] = $this->_set_param_internal()) === null)
			unset($arr['internal']);

		return(parent::get_list_where($arr,$disable,$order,$limit,true));
	}
}

?>
