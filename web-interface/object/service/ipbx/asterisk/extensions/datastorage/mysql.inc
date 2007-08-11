<?php

xivo_service_asterisk::required(array('extensions','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_extensions_mysql extends xivo_service_asterisk_extensions_abstract_sql
{
	function xivo_service_asterisk_extensions_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
