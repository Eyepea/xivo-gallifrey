<?php

xivo_service_asterisk::required(array('trunkfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunkfeatures_mysql extends xivo_service_asterisk_trunkfeatures_abstract_sql
{
	function xivo_service_asterisk_trunkfeatures_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
