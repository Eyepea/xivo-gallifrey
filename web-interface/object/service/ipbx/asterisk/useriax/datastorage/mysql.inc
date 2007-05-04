<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','useriax','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_useriax_mysql extends xivo_service_asterisk_useriax_abstract_sql
{
	function xivo_service_asterisk_useriax_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
