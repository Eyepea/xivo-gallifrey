<?php

xivo_service_asterisk::required(array('extensions','datastorage','mysql.inc'),true);

class xivo_service_asterisk_extenfeatures_mysql extends xivo_service_asterisk_extensions_mysql
{
	function xivo_service_asterisk_extenfeatures_mysql(&$dso,$param=array())
	{
		$this->xivo_service_asterisk_extensions_mysql(&$dso,$param);
	}
}

?>
