<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_meetmefeatures_abstract_sql extends xivo_service_asterisk_abstract_sql
{
	var $_param	= array('table' => 'meetmefeatures');
}

?>
