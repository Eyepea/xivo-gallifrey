<?php

xivo_service_asterisk::required(array('realstatic','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_staticagent_sql extends xivo_service_asterisk_realstatic_abstract_sql
{
	var $_forcecolumn	= array('filename' => 'agents.conf');
}

?>
