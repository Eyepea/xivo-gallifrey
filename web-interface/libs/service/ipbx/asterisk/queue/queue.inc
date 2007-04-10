<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_queue extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queue';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_queue(&$sre,&$dso)
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
		if(($r = $this->_dso->get($id,$disable)) === false)
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
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			return($this->add($this->_origin));
		else
			return($this->edit($this->_origin['name'],$this->_origin));
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if($disable !== null)
			$arr['commented'] = (bool) $disable === false ? 0 : 1;

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		return($this->_dso->delete($id));
	}
}

?>
