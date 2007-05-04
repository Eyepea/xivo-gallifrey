<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','extenumbers','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_extenumbers_sqlite extends xivo_service_asterisk_extenumbers_abstract_sql
{
	function xivo_service_asterisk_extenumbers_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
