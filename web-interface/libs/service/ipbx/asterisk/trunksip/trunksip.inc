<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_trunksip extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'trunksip';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_trunksip(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id,$disable=false)
	{
		$r = false;

		$disable = $disable !== null ? (bool) $disable : null;

		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return($r);

		$this->_origin = $r;

		$r['commented'] = (bool) $r['commented'];

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
		if(($type === 'add' && $type === 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		$arr['category'] = 'trunk';

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$arr['category'] = 'trunk';

		return($this->_dso->edit($id,$arr));
	}

	function disable($id,$value=false)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$value = (bool) $value === false ? 0 : 1;

		return($this->_dso->edit($id,array('commented' => $value)));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function get_list()
	{
		if(($r = $this->_dso->get_list()) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
