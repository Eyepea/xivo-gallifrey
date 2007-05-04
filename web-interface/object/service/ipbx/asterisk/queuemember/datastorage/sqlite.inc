<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','queuemember','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queuemember_sqlite extends xivo_service_asterisk_queuemember_abstract_sql
{
	function xivo_service_asterisk_queuemember_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
