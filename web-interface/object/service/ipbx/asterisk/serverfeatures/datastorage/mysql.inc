<?php

xivo_service_asterisk::required(array('serverfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_serverfeatures_mysql extends xivo_service_asterisk_serverfeatures_abstract_sql
{
	function xivo_service_asterisk_serverfeatures_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
