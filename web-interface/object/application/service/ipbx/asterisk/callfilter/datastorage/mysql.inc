<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','callfilter','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_callfilter_mysql extends xivo_application_service_asterisk_callfilter_abstract_sql
{
	function xivo_application_service_asterisk_callfilter_mysql(&$dso,&$dsoconf)
	{
		$this->_init(&$dso,&$dsoconf);
	}
}

?>
