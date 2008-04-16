<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_contextnumbers_sql extends xivo_service_asterisk_abstract_sql
{
	var $_autoincrement	= false;
	var $_unique		= array(array('context','type','numberbeg','numberend'));
}

?>
