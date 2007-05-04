<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'user','datastorage','abstract','sql.inc'),true);

class xivo_user_mysql extends xivo_user_abstract_sql
{
	function xivo_user_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
