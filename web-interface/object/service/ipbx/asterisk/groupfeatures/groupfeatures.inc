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

	function get($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$this->_origin = $r;
		$r = $this->_prepare($r);

		return($r);
	}

	function _prepare($data)
	{
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
		$last++;

		if($last > 63 && ($last = $this->_dso->get_free()) === false)
			return(false);

		$arr['id'] = $last;
		$arr['commented'] = false;

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_where($where)) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['commented']) === true)
			$arr['commented'] = (bool) $arr['commented'];

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
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function enable($id)
	{
		if(($group = $this->get($id,true)) === false)
			return(false);

		$id = $group['id'];

		$r = array();

		if(($pos = strrpos($group['name'],'|')) !== false)
			$r['name'] = substr($group['name'],0,$pos);
		else
			$r['name'] = $group['name'];

		$r['commented'] = false;

		return($this->edit($id,$r));
	}

	function disable($id)
	{
		if(($group = $this->get($id)) === false)
			return(false);

		$id = $group['id'];

		$r = array();
		$r['name'] = $group['name'].'|'.md5(uniqid(rand(),true));
		$r['commented'] = true;

		return($this->edit($id,$r));
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

	function get_where($arr)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false)
			return(false);

		return($this->_dso->get_where($where));
	}

	function get_all($disable=null)
	{
		if(($list = $this->_dso->get_all($disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_number($disable=null)
	{
		if(($list = $this->_dso->get_all($disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $this->_prepare($list[$i]);

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
