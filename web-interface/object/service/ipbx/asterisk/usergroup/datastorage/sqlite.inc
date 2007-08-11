<?php

xivo_service_asterisk::required(array('usergroup','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_usergroup_sqlite extends xivo_service_asterisk_usergroup_abstract_sql
{
	function xivo_service_asterisk_usergroup_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
