<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_agentgroup extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'agentgroup';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_agentgroup(&$sre,&$dso)
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
		$data['deleted'] = (bool) $data['deleted'];
		$data['commented'] = (bool) $data['commented'];

		if($this->get_defgroup() === $data['id'])
			$data['deletable'] = false;
		else
			$data['deletable'] = true;

		return($data);
	}

	function get($id,$disable=null,$delete=false)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(($id = xivo_ulongint($id)) === 0
		|| ($r = $this->_dso->get($id,$disable,$delete)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		return($this->_prepare($r));
	}

	function get_primary($arr,$disable=null,$delete=false)
	{
		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_primary($arr,$disable,$delete));
	}

	function get_where($arr,$disable=null,$delete=false)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get($where,$disable,$delete)) === false)
			return(false);

		if(isset($this->_origin) === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_nb($arr=null,$disable=null,$delete=false)
	{
		if($arr !== null && is_array($arr) === false)
			return(false);

		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_nb($arr,$disable,$delete));
	}

	function get_defgroup()
	{
		if(isset($this->_conf['group_default']) === false
		|| ($r = xivo_ulongint($this->_conf['group_default'])) === 0)
			$r = false;

		return($r);
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
		$arr['commented'] = false;
		$arr['deleted'] = false;

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if($this->get_primary($where) !== false)
				return(false);
		}

		if($replace === true)
			return($this->_dso->edit($last,$arr));

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null,$delete=false)
	{
		if(($id = xivo_ulongint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_primary($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable,$delete));
	}

	function edit_where($id,$arr,$disable=null,$delete=false)
	{
		if(is_array($id) === false
		|| ($where = $this->get_element_where($id)) === false
		|| ($arr = $this->chk_values($arr,true,false,false)) === false)
			return(false);

		$r = $this->_dso->edit($where,$arr,$disable,$delete);

		return($r);
	}

	function delete($id,$disable=null)
	{
		return($this->edit($id,array('deleted' => true),$disable));
	}

	function delete_where($id,$disable=null)
	{
		return($this->edit_where($id,array('deleted' => true),$disable));
	}

	function recover($id,$disable=null)
	{
		return($this->edit($id,array('deleted' => false),$disable,true));
	}

	function recover_where($id,$disable=null)
	{
		return($this->edit_where($id,array('deleted' => true),$disable,true));
	}

	function get_list($disable=null,$delete=false,$order=null,$limit=null)
	{
		if(($r = $this->_dso->get_list($disable,$delete,$order,$limit)) !== false
		&& isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$delete=false,$order=null,$limit=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable,$delete,$order,$limit)) === false
		|| isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($disable=null,$delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(($list = $this->_dso->get_all($disable,$delete,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);

		return($this->_mk_list_assoc_prepare($list,$assoc,$prepare));
	}

	function get_all_where($arr,$disable=null,$delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable,$delete,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);

		if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);

		return($this->_mk_list_assoc_prepare($list,$assoc,$prepare));
	}
}

?>
