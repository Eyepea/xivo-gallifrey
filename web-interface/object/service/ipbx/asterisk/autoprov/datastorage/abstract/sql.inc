<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_autoprov_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'phone');
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function get($id)
	{
		if(is_string($id) === false)
			return(false);

		$arr = array('macaddr' => $id);

		return(parent::get($arr));
	}

	function get_id($arr)
	{
		if(($r = parent::get($arr)) === false)
			return(false);

		return($r['macaddr']);
	}
	
	function get_list($disable=null,$order=null,$limit=null)
	{
		if(($list = parent::get_list($disable,null,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $list[$i]['macaddr'];

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(($list = parent::get_list_where($arr,$disable,null,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $list[$i]['macaddr'];

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($disable=null,$order=null,$limit=null)
	{
		return(parent::get_list($disable,null,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$order=null,$limit=null)
	{
		return(parent::get_list_where($arr,$disable,null,$order,$limit,true));
	}
}

?>
