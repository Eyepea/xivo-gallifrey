<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'sql.inc');

class xivo_server_abstract_sql extends xivo_abstract_datastorage_sql
{
	var $_param	= array('table'	=> 'server');

	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
