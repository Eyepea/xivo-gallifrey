<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_outcalltrunk_sql extends xivo_service_asterisk_abstract_sql
{
	function get($arr)
	{
		if(is_array($arr) === false)
			return(false);

		return(parent::get($arr));
	}

	function get_id($arr)
	{
		if(is_array($arr) === false
		|| ($arr = parent::get($arr)) === false)
			return(false);

		$r = array();
		$r['outcallid'] = $arr['outcallid'];
		$r['trunkfeaturesid'] = $arr['trunkfeaturesid'];

		return($r);
	}

	function add($arr)
	{
		return(parent::add($arr,false));
	}

	function edit($arr,$value)
	{
		if(is_array($arr) === false)
			return(false);

		return(parent::edit($arr,$value));
	}

	function delete($arr)
	{
		if(is_array($arr) === false)
			return(false);

		return(parent::delete($arr));
	}
	
	function get_list($order=null,$limit=null)
	{
		if(($list = parent::get_list(null,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = array('outcallid' => $list[$i]['outcallid'],
				     'trunkfeaturesid' => $list[$i]['trunkfeaturesid']);

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_list_where($arr,$order=null,$limit=null)
	{
		if(($list = parent::get_list_where($arr,null,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = array('outcallid' => $list[$i]['outcallid'],
				     'trunkfeaturesid' => $list[$i]['trunkfeaturesid']);

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($order=null,$limit=null)
	{
		return(parent::get_list(null,$order,$limit,true));
	}

	function get_all_where($arr,$order=null,$limit=null)
	{
		return(parent::get_list_where($arr,null,$order,$limit,true));
	}
}

?>
