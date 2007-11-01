<?php

xivo_service_asterisk::required(array('serverfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_serverfeatures_sqlite extends xivo_service_asterisk_serverfeatures_abstract_sql
{
	function xivo_service_asterisk_serverfeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
