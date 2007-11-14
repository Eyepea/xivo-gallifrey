<?php

xivo_service_asterisk::required(array('handynumbers','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_handynumbers_sqlite extends xivo_service_asterisk_handynumbers_abstract_sql
{
	function xivo_service_asterisk_handynumbers_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
