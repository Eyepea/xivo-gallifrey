<?php

xivo_service_asterisk::required(array('extenumbers','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_extenumbers_mysql extends xivo_service_asterisk_extenumbers_abstract_sql
{
	function xivo_service_asterisk_extenumbers_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
