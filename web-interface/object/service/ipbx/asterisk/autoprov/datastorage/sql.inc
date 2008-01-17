<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_autoprov_sql extends xivo_service_asterisk_abstract_sql
{
	function get($arr)
	{
		if(is_array($arr) === false)
		{
			if(is_string($arr) === true)
				$arr = array('macaddr' => $arr);
			else
				return(false);
		}

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
		if(($list = parent::get_list($disable,$order,$limit)) === false
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
		if(($list = parent::get_list_where($arr,$disable,$order,$limit)) === false
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
		return(parent::get_list($disable,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$order=null,$limit=null)
	{
		return(parent::get_list_where($arr,$disable,$order,$limit,true));
	}
}

?>
