<?php

xivo_service_asterisk::required(array('queuefeatures','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_queuefeatures_sqlite extends xivo_service_asterisk_queuefeatures_abstract_sql
{
	function xivo_service_asterisk_queuefeatures_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
