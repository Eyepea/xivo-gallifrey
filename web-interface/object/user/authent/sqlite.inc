<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'user','authent','abstract','sql.inc'),true);

class xivo_authent_sqlite extends xivo_authent_abstract_sql
{
	function xivo_authent_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
