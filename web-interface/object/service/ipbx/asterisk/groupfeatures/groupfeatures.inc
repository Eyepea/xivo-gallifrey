<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_groupfeatures extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'groupfeatures';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_groupfeatures(&$sre,&$dso)
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
		if($data['context'] === '')
			$data['context'] = 'default';

		$data['identity'] = trim($data['name']);

		if($data['identity'] === '')
			$data['identity'] = '-';

		if(xivo_haslen($data['number']) === false)
			$data['identity'] .= ' ('.$data['context'].')';
		else
			$data['identity'] .= ' ('.$data['number'].'@'.$data['context'].')';

		$data['deleted'] = (bool) $data['deleted'];

		return($data);
	}

	function get($id,$delete=false)
	{
		return(parent::get($id,$delete));
	}

	function get_id($arr,$delete=false)
	{
		return(parent::get_id($arr,$delete));
	}

	function get_where($arr,$delete=false)
	{
		return(parent::get_where($arr,$delete));
	}

	function add($arr)
	{
		if(is_array($arr) === false || ($last = $this->_dso->get_last()) === false)
			return(false);

		$replace = false;

		$last++;

		if($last > 63)
		{
			if(($last = $this->_dso->get_free()) === false)
				return(false);

			$replace = true;
		}

		$arr['id'] = $last;
		$arr['deleted'] = false;

		if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);
			$where = array('name' => $arr['name']);

			if($this->get_id($where) !== false)
				return(false);
		}

		if($replace === true)
			$r = $this->_dso->edit($last,$arr);
		else
			$r = $this->_dso->add($arr);

		return($r);
	}

	function edit($id,$arr,$delete=false)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$delete));
	}

	function edit_where($id,$arr,$delete=false)
	{
		return(parent::edit_where($id,$arr,$delete));
	}

	function delete($id)
	{
		return($this->edit($id,array('deleted' => true)));
	}

	function delete_where($id)
	{
		return(parent::edit_where($id,array('deleted' => true)));
	}

	function recover($id)
	{
		return($this->edit($id,array('deleted' => false),true));
	}

	function recover_where($id)
	{
		return(parent::edit_where($id,array('deleted' => true),true));
	}

	function get_nb($arr=null,$delete=false)
	{
		return(parent::get_nb($arr,$delete));
	}

	function get_list($delete=false,$order=null,$limit=null)
	{
		return(parent::get_list($delete,$order,$limit));
	}

	function get_list_where($arr,$delete=false,$order=null,$limit=null)
	{
		return(parent::get_list_where($arr,$delete,$order,$limit));
	}

	function get_all($delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		return(parent::get_all($delete,$prepare,$order,$limit,$assoc));
	}

	function get_all_where($arr,$delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		return(parent::get_all_where($arr,$delete,$prepare,$order,$limit,$assoc));
	}

	function get_all_number($id=0)
	{
		$id = xivo_uint($id);

		if(($list = $this->get_all()) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($id === (int) $info['id'] || xivo_haslen($info['number']) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
