<?php

xivo_service_asterisk::required(array('extenumbers','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_extenumbers_sqlite extends xivo_service_asterisk_extenumbers_abstract_sql
{
	function xivo_service_asterisk_extenumbers_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
