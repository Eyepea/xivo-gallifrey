<?php

xivo_service_asterisk::required(array('callfiltermember','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_callfiltermember_mysql extends xivo_service_asterisk_callfiltermember_abstract_sql
{
	function xivo_service_asterisk_callfiltermember_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
