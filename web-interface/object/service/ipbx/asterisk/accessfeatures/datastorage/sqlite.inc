<?php

xivo_service_asterisk::required(array('accessfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_accessfeatures_sqlite extends xivo_service_asterisk_accessfeatures_abstract_sql
{
	function xivo_service_asterisk_accessfeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
