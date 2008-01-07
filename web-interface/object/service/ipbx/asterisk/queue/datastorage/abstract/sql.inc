<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queue_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'queue');

	function get($arr,$disable=null)
	{
		if(is_array($arr) === false)
		{
			if(is_scalar($arr) === true)
				$arr = array('name' => $arr);
			else
				return(false);
		}

		return(parent::get($arr,$disable));
	}

	function get_id($arr,$disable=null)
	{
		if(($r = parent::get($arr,$disable)) === false)
			return(false);

		return($r['name']);
	}

	function add($arr)
	{
		return(parent::add($arr,false));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_scalar($id) === false)
			return(false);

		$id = array('name' => $id);

		return(parent::edit($id,$arr,$disable));
	}

	function delete($id,$disable=null)
	{
		if(is_scalar($id) === false)
			return(false);

		$id = array('name' => $id);

		return(parent::delete($id,$disable));
	}
	
	function get_list($disable=null,$order=null,$limit=null)
	{
		if(($list = parent::get_list($disable,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $list[$i]['name'];

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(($list = parent::get_list_where($arr,$disable,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $list[$i]['name'];

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($disable=null,$order=null,$limit=null)
	{
		return(parent::get_list($disable,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$order=null,$limit=null)
	{
		return(parent::get_list_where($arr,$disable,$order,$limit,true));
	}
}

?>
