<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_phonefunckey_sql extends xivo_service_asterisk_abstract_sql
{
	var $_disable		= null;
	var $_autoincrement	= false;
}

?>
