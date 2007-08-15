<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_didfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'didfeatures';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_didfeatures(&$sre,&$dso)
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

		return($data);
	}

	function edit_list($arr,$values)
	{
		$r = false;

		if(is_array($arr) === false
		|| ($values = $this->chk_values($values,true,false,false)) === false)
			return($r);

		$list = array_values($arr);

		if(($nb = count($list)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
			$this->edit($list[$i],$values);

		return(true);
	}
}

?>
