<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_webservices_abstract extends xivo_application_asterisk_abstract
{
	var $_conf		= array();

	function _init(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		$this->_load_config();

		return(true);
	}
}

?>
