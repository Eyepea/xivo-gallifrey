<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_voicemailfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'voicemailfeatures';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_voicemailfeatures(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}
}

?>
