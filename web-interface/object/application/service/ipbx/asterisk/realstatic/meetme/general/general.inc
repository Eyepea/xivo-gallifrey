<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_meetme_general extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'generalmeetme';
	var $_modulename	= 'meetme';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'meetme.conf';

	function xivo_application_service_asterisk_realstatic_meetme_general(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}
}

?>
