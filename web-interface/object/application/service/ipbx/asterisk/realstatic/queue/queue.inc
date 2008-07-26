<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic.inc'),true);

class xivo_application_service_asterisk_realstatic_queue extends xivo_application_service_asterisk_realstatic
{
	var $_name	= 'queue';
	var $_module	= array('general'	=> false);
}

?>
