<?php

xivo_service_asterisk::required(array('meetmefeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_meetmefeatures_sqlite extends xivo_service_asterisk_meetmefeatures_abstract_sql
{
	function xivo_service_asterisk_meetmefeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
