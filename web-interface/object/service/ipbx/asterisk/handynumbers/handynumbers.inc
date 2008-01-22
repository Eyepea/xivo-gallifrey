<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_handynumbers extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'handynumbers';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_handynumbers(&$sre,&$dso)
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
		$data['commented'] = (bool) $data['commented'];

		if(xivo_haslen($data,'exten') === true)
			$data['exten'] = $this->_sre->display_extension($data['exten']);

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(xivo_haslen($arr,'exten') === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if(xivo_haslen($arr,'exten') === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return(parent::edit($id,$arr,$disable));
	}
}

?>
