<?php

xivo_service_asterisk::required(array('userfeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_userfeatures_mysql extends xivo_service_asterisk_userfeatures_abstract_sql
{
	function xivo_service_asterisk_userfeatures_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
