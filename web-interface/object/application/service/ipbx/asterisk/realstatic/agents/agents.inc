<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic.inc'),true);

class xivo_application_service_asterisk_realstatic_agents extends xivo_application_service_asterisk_realstatic
{
	var $_name	= 'agents';
	var $_module	= array('general'	=> false,
				'group'		=> false,
				'agent'		=> false,
				'agentoptions'	=> false);
}

?>
