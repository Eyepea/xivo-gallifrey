<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','trunkcustom','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunkcustom_mysql extends xivo_service_asterisk_trunkcustom_abstract_sql
{
	function xivo_service_asterisk_trunkcustom_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
