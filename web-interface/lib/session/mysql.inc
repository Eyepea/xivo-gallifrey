<?php

xivo_file::required(array(XIVO_PATH_LIB,'session','abstract','sql.inc'),true);

class xivo_session_mysql extends xivo_session_abstract_sql
{
	function xivo_session_mysql(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
