<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','abstract.inc'),true);

class xivo_service_asterisk_groupfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'groupfeatures';
	var $_filter	= null;
	var $_origin	= null;

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

	function get($id,$disable=null,$delete=false)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable,$delete)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
		$data['deleted'] = (bool) $data['deleted'];
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

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

			if(($whereid = $this->get_where($where)) !== false)
				return(false);
		}

		if($replace === true)
			$r = $this->_dso->edit($last,$arr);
		else
			$r = $this->_dso->add($arr);

		return($r);
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_where($where)) !== false && (int) $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		return($this->edit($id,array('deleted' => true)));
	}

	function recover($id)
	{
		return($this->edit($id,array('deleted' => false)));
	}

	function disable($id)
	{
		return($this->edit($id,array('commented' => true)));
	}

	function enable($id)
	{
		return($this->edit($id,array('commented' => false)));
	}

	function is_valid($arr)
	{
		$r = '';

		$arr = array_values((array) $arr);

		if(($nb = count($arr)) === 0)
			return($r);

		if(($r = $this->_dso->is_valid($arr)) !== false && $nb !== count($r))
			$r = false;

		return($r);
	}

	function get_where($arr,$disable=null,$delete=false)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->get_where($where,$disable,$delete));
	}

	function get_all($disable=null,$delete=false)
	{
		if(($list = $this->_dso->get_all($disable,$delete)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_number($disable=null,$delete=false)
	{
		if(($list = $this->_dso->get_all($disable,$delete)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $this->_prepare($list[$i]);

			if(xivo_empty($info['number']) === true)
				continue;

			if($info['context'] !== '')
				$info['number-context'] = $info['number'].'@'.$info['context'];
			else
				$info['number-context'] = $info['number'];

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
