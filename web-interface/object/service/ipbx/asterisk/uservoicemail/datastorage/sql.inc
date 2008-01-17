<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_uservoicemail_sql extends xivo_service_asterisk_abstract_sql
{
	function get($arr,$disable=null)
	{
		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('uniqueid' => $arr);
			else
				return(false);
		}

		return(parent::get($arr,$disable));
	}

	function get_id($arr,$disable=null)
	{
		if(($r = parent::get($arr,$disable)) === false)
			return(false);

		return((int) $r['uniqueid']);
	}

	function edit($arr,$value,$disable=null)
	{
		if(($arr = xivo_uint($arr)) === 0)
			return(false);

		$arr = array('uniqueid' => $arr);

		return(parent::edit($arr,$value,$disable));
	}

	function delete($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$id = array('uniqueid' => $id);

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
			$r[] = $list[$i]['uniqueid'];

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
			$r[] = $list[$i]['uniqueid'];

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
