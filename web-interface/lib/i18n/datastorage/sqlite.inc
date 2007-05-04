<?php

xivo_file::required(array(XIVO_PATH_LIB,'i18n','datastorage','abstract','sql.inc'),true);

class xivo_i18n_sqlite extends xivo_i18n_abstract_sql
{
	function xivo_i18n_sqlite(&$dso,$param=array())
	{
		$this->_init(&$dso,$param);
	}
}

?>
