<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queuemember_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'queuemember');
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

	function get($arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		return(parent::get($arr,$disable));
	}

	function get_id($arr,$disable=null)
	{
		if(($arr = parent::get($arr,$disable)) === false)
			return(false);

		$r = array();
		$r['queue_name'] = $arr['queue_name'];
		$r['interface'] = $arr['interface'];

		return($r);
	}

	function add($arr)
	{
		return(parent::add($arr,false));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($id) === false)
			return(false);

		return(parent::edit($id,$arr,$disable));
	}

	function delete($arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		return(parent::delete($arr,$disable));
	}
	
	function get_list($disable=null,$order=null,$limit=null)
	{
		if(($list = parent::get_list($disable,null,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = array('queue_name' => $list[$i]['queue_name'],
				     'interface' => $list[$i]['interface']);

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
			$r[] = array('queue_name' => $list[$i]['queue_name'],
				     'interface' => $list[$i]['interface']);

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
