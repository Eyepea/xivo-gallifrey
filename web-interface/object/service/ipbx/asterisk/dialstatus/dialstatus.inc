<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_dialstatus extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'dialstatus';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_dialstatus(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		$data['endcall'] = '';
		$data['user'] = '';
		$data['group'] = '';
		$data['queue'] = '';
		$data['meetme'] = '';
		$data['schedule'] = '';
		$data['application'] = '';
		$data['sound'] = '';
		$data['custom'] = ''; 

		if(isset($data[$data['type']]) === true)
			$data[$data['type']] = $data['typeval'];

		$data['linked'] = (bool) $data['linked'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if($arr['type'] !== 'application')
			$arr['applicationval'] = '';

		if($this->exists($arr['status'],$arr['category'],$arr['categoryval']) !== false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0
		|| is_array($arr) === false
		|| ($info = $this->get($id)) === false)
			return(false);

		if(isset($arr['status']) === false)
			$status = $info['status'];
		else
			$status = $arr['status'];

		if(isset($arr['category']) === false)
			$category = $info['category'];
		else
			$category = $arr['category'];

		if(isset($arr['type']) === false)
			$arr['type'] = $info['type'];

		if($arr['type'] !== 'application')
			$arr['applicationval'] = '';

		if(isset($arr['categoryval']) === false)
			$categoryval = $info['categoryval'];
		else
			$categoryval = $arr['categoryval'];

		if($this->exists($status,$category,$categoryval,$id) !== false)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function linked($id)
	{
		return($this->edit($id,array('linked' => true)));
	}

	function unlinked($id)
	{
		return($this->edit($id,array('linked' => false)));
	}

	function linked_where($id)
	{
		return($this->edit_where($id,array('linked' => true)));
	}

	function unlinked_where($id)
	{
		return($this->edit_where($id,array('linked' => false)));
	}

	function is_valid($category,$categoryval,$type,$typeval,$appval='',$disable=null)
	{
		if(is_string($category) === false || is_string($type) === false)
			return(false);

		if($category === $type)
			$id = $categoryval;
		else
			$id = null;

		switch($type)
		{
			case 'endcall':
				return($this->_chk_dest_value($type,$typeval));
			case 'application':
				return($this->chk_dest_application($typeval,$appval));
			case 'custom':
				return($this->chk_dest_custom($typeval));
			case 'schedule':
				return($this->chk_dest_schedule($typeval,null,$disable,false));
			case 'user':
				return($this->chk_dest_user($typeval,$id,$disable,false));
			case 'group':
				return($this->chk_dest_group($typeval,$id,$disable,false));
			case 'queue':
				return($this->chk_dest_queue($typeval,null,$disable,false));
			case 'meetme':
				return($this->chk_dest_meetme($typeval,null,$disable,false));
			case 'sound':
				return($this->chk_dest_sound('playback',$typeval));
		}

		return(false);
	}

	function exists($status,$category,$categoryval,$id=null)
	{
		$id = $id !== null ? xivo_uint($id) : null;

		if(is_string($status) === false
		|| xivo_haslen($status) === false
		|| $this->chk_element_value('category',$category) === false
		|| xivo_haslen($categoryval) === false)
			return(null);

		$where = array();
		$where['status'] = $status;
		$where['category'] = $category;
		$where['categoryval'] = $categoryval;

		if(($whereid = $this->get_id($where)) !== false
		&& ($id === null || $whereid !== $id) === true)
			return(true);

		return(false);
	}

	function get_all($category='',$categoryval='',$prepare=true,$order=null,$limit=null)
	{
		$category = strval($category);
		$categoryval = strval($categoryval);

		if(($list = parent::get_all(null,$prepare,$order,$limit)) === false)
			return(false);

		if($id === 0)
			return($list);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($category === (string) $info['category']
			&& $categoryval === (string) $info['categoryval'])
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
