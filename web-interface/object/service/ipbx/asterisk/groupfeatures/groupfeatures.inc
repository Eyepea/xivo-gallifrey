<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

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

		return($r);
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

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['commented']) === true)
			$r['commented'] = (bool) $arr['commented'];

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

	function get_all_number($disable=null)
	{
		$r = false;

		if(($list = $this->get_list()) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false || $info['number'] === '' || $info['number'] === null)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
