<?php

xivo_service_asterisk::required(array('didfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_didfeatures_sqlite extends xivo_service_asterisk_didfeatures_abstract_sql
{
	function xivo_service_asterisk_didfeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
