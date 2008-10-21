<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_callerid extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'callerid';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_callerid(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['callerdisplay']) === true
		&& xivo_haslen($arr['callerdisplay']) === false)
		{
			$arr['mode'] = null;
			$arr['callerdisplay'] = '';
		}

		return(parent::add($arr));
	}

	function edit($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['callerdisplay']) === true
		&& xivo_haslen($arr['callerdisplay']) === false)
		{
			$arr['mode'] = null;
			$arr['callerdisplay'] = '';
		}

		return(parent::edit($id,$arr));
	}
}

?>
