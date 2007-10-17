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

	function get($id,$disable=null,$delete=false)
	{
		return(parent::get($id,$disable,$delete));
	}

	function get_id($arr,$disable=null,$delete=false)
	{
		return(parent::get_id($arr,$disable,$delete));
	}

	function get_where($arr,$disable=null,$delete=false)
	{
		return(parent::get_where($arr,$disable,$delete));
	}

	function _prepare($data)
	{
		$data['deleted'] = (bool) $data['deleted'];
		$data['commented'] = (bool) $data['commented'];

		if($this->get_defgroup() === (int) $data['id'])
			$data['deletable'] = false;
		else
			$data['deletable'] = true;

		return($data);
	}

	function get_defgroup()
	{
		if(isset($this->_conf['group_default']) === false
		|| ($r = xivo_uint($this->_conf['group_default'])) === 0)
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

			if($this->get_id($where) !== false)
				return(false);
		}

		if($replace === true)
			$r = $this->_dso->edit($last,$arr);
		else
			$r = $this->_dso->add($arr);

		return($r);
	}

	function edit($id,$arr,$disable=null,$delete=false)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable,$delete));
	}

	function edit_where($id,$arr,$disable=null,$delete=false)
	{
		return(parent::edit_where($id,$arr,$disable,$delete));
	}

	function delete($id,$disable=null)
	{
		return($this->edit($id,array('deleted' => true),$disable));
	}

	function delete_where($id,$disable=null)
	{
		return(parent::edit_where($id,array('deleted' => true),$disable));
	}

	function recover($id,$disable=null)
	{
		return($this->edit($id,array('deleted' => false),$disable,true));
	}

	function recover_where($id,$disable=null)
	{
		return(parent::edit_where($id,array('deleted' => true),$disable,true));
	}

	function get_list($disable=null,$delete=false,$order=null,$limit=null)
	{
		return(parent::get_list($disable,$delete,$order,$limit));
	}

	function get_list_where($arr,$disable=null,$delete=false,$order=null,$limit=null)
	{
		return(parent::get_list_where($arr,$disable,$delete,$order,$limit));
	}

	function get_all($disable=null,$delete=false,$prepare=true,$order=null,$limit=null)
	{
		return(parent::get_all($disable,$delete,$prepare,$order,$limit));
	}

	function get_all_where($arr,$disable=null,$delete=false,$prepare=true,$order=null,$limit=null)
	{
		return(parent::get_all_where($arr,$disable,$delete,$prepare,$order,$limit));
	}
}

?>
