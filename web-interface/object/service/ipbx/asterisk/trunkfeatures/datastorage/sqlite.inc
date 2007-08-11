<?php

xivo_service_asterisk::required(array('trunkfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_trunkfeatures_sqlite extends xivo_service_asterisk_trunkfeatures_abstract_sql
{
	function xivo_service_asterisk_trunkfeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
