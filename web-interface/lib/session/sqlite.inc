<?php

xivo_file::required(array(XIVO_PATH_LIB,'session','abstract','sql.inc'),true);

class xivo_session_sqlite extends xivo_session_abstract_sql
{
	function xivo_session_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
