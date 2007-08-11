<?php

xivo_service_asterisk::required(array('agentgroup','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agentgroup_mysql extends xivo_service_asterisk_agentgroup_abstract_sql
{
	function xivo_service_asterisk_agentgroup_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
