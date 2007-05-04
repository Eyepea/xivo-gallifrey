<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','abstract.inc'),true);

class xivo_service_asterisk_trunkfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'trunkfeatures';
	var $_filter	= null;

	function xivo_service_asterisk_trunkfeatures(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id)) === false)
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

	function get_by_trunk($id,$trunk)
	{
		if(($id = xivo_uint($id)) === 0
		|| $this->_sre->chk_protocol($trunk) === false
		|| ($id = $this->_dso->get_by_trunk($id,$trunk)) === false)
			return(false);

		return($this->get($id));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if($this->_sre->chk_protocol($arr['trunk']) === false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['trunk']) === true && $this->_sre->chk_protocol($arr['trunk']) === false)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}
}

?>
