<?php

xivo_service_asterisk::required(array('trunkcustom','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunkcustom_sqlite extends xivo_service_asterisk_trunkcustom_abstract_sql
{
	function xivo_service_asterisk_trunkcustom_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
