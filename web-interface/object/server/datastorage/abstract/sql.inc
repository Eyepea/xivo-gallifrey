<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'helper'.XIVO_SEP_DIR.'table.inc');

class xivo_server_abstract_sql extends xivo_datastorage_helper_table
{
	var $_table	= 'server';
	var $_primary	= array('id' => 0);
	var $_disable	= 'disable';
	var $_unique	= array('name',array('host','port'));

	function xivo_server_abstract_sql(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}
}

?>
