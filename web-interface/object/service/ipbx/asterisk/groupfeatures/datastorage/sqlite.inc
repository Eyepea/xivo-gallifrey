<?php

xivo_service_asterisk::required(array('groupfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_groupfeatures_sqlite extends xivo_service_asterisk_groupfeatures_abstract_sql
{
	function xivo_service_asterisk_groupfeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
