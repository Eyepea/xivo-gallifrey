<?php

xivo_service_asterisk::required(array('realstatic','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_generalqueue_abstract_sql extends xivo_service_asterisk_realstatic_abstract_sql
{
	var $_param	= array('table' => 'generalqueue');
}

?>
