<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunkcustom_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'usercustom');

	function get($arr,$disable=null)
	{
		if(is_array($arr) === false)
		{
			if(($arr = xivo_uint($arr)) !== 0)
				$arr = array('id' => $arr);
			else
				return(false);
		}

		$arr['category'] = 'trunk';

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

		$arr['category'] = 'trunk';

		return(parent::get($arr,$disable,true));
	}

	function get_nb($arr=null,$disable=null)
	{
		if(is_array($arr) === false)
			$arr = array();
			
		$arr['category'] = 'trunk';

		return(parent::get_nb($arr,$disable));
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

		$id['category'] = 'trunk';

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

		$arr['category'] = 'trunk';

		return(parent::delete($arr,$disable));
	}

	function get_list($disable=null,$order=null,$limit=null)
	{
		$arr = array('category' => 'trunk');

		return(parent::get_list_where($arr,$disable,$order,$limit));
	}

	function get_list_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$arr['category'] = 'trunk';

		return(parent::get_list_where($arr,$disable,$order,$limit));
	}

	function get_all($disable=null,$order=null,$limit=null)
	{
		$arr = array('category' => 'trunk');

		return(parent::get_list_where($arr,$disable,$order,$limit,true));
	}

	function get_all_where($arr,$disable=null,$order=null,$limit=null)
	{
		if(is_array($arr) === false || empty($arr) === true)
			return(false);

		$arr['category'] = 'trunk';

		return(parent::get_list_where($arr,$disable,$order,$limit,true));
	}
}

?>
