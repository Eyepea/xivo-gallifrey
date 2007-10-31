<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_userfeatures_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table'		=> 'userfeatures',
				'internal'	=> 0);

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

	function get_search($value,$disable=null)
	{
		$where = array();

		if(($rs = $this->_dso->search_contain('firstname',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('lastname',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('number',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('name',$value)) !== false)
			$where[] = $rs;

		if(($rs = $this->_dso->search_contain('context',$value)) !== false)
			$where[] = $rs;

		if(($where = implode(' OR ',$where)) === '')
			return(false);

		$where = '('.$where.')';

		if($disable !== null)
			$where .= ' AND commented = '.intval((bool) $disable);

		if(($internal = $this->_set_param_internal()) !== null)
			$where .= ' AND internal = '.$internal;

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		return($r);
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($id) === false)
		{
			if(($id = xivo_uint($id)) !== 0)
				$id = array('id' => $id);
			else
				return(false);
		}

		if(($id['internal'] = $this->_set_param_internal()) === null)
			unset($id['internal']);

		return(parent::edit($id,$arr,$disable));
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
