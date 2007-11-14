<?php

xivo_service_asterisk::required(array('handynumbers','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_handynumbers_mysql extends xivo_service_asterisk_handynumbers_abstract_sql
{
	function xivo_service_asterisk_handynumbers_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
