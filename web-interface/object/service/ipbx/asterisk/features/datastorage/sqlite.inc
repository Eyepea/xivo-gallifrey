<?php

xivo_service_asterisk::required(array('features','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_features_sqlite extends xivo_service_asterisk_features_abstract_sql
{
	var $_param	= array('table' => 'features');

	function xivo_service_asterisk_features_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
