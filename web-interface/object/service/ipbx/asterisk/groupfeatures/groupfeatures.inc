<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_groupfeatures extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'groupfeatures';
	var $_filter		= null;
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

		if(xivo_empty($data['number']) === true)
			$data['identity'] .= ' ('.$data['context'].')';
		else
			$data['identity'] .= ' ('.$data['number'].'@'.$data['context'].')';

		$data['commented'] = (bool) $data['commented'];
		$data['deleted'] = (bool) $data['deleted'];

		return($data);
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

	function get_all_number()
	{
		if(($list = $this->get_all()) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if(xivo_empty($info['number']) === true)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
