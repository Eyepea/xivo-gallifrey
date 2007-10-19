<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic.inc'),true);

class xivo_application_service_asterisk_realstatic_features extends xivo_application_service_asterisk_realstatic
{
	var $_name	= 'features';
	var $_module	= array(
				'general'		=> false,
				'featuremap'		=> false,
				'applicationmap'	=> false);
}

?>
