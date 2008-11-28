<?php

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'datastorage','helper','abstract.inc'));

class xivo_application_service_asterisk_abstract_sql extends xivo_datastorage_helper_abstract
{
	var $_cnt	= 0;
	var $_dsoconf	= array();
	var $_table	= array();

	function xivo_application_service_asterisk_abstract_sql(&$dso,&$dsoconf)
	{
		parent::_init(&$dso);

		if(is_array($dsoconf) === false)
			trigger_error('Invalid Datastorage configuration array',E_USER_ERROR);

		$this->_dsoconf = &$dsoconf;

		$this->_set_table();	
	}

	function _set_table()
	{
		if(is_array($this->_table) === false
		|| empty($this->_table) === true)
			return(false);

		reset($this->_table);

		while(list($key) = each($this->_table))
		{
			if($this->_get_table($key) === false)
				trigger_error('Invalid or missing Datastorage table '.$key,E_USER_ERROR);
		}
		
		return(true);
	}

	function get_cnt()
	{
		return(xivo_ulongint($this->_cnt));
	}

	function _get_table($module)
	{
		if(isset($this->_table[$module]) === true
		&& xivo_haslen($this->_table[$module]) === true)
			return($this->_table[$module]);
		else if(($table = $this->get_modconf_key($module,'table')) === false)
			return(false);

		$this->_table[$module] = $table;

		return($table);
	}

	function get_modconf_key($module,$key)
	{
		if(($modconf = $this->get_modconf($module)) === false
		|| isset($modconf[$key]) === false)
			return(false);

		return($modconf[$key]);
	}

	function get_modconf($module)
	{
		if(is_array($this->_dsoconf) === true
		&& isset($this->_dsoconf[$module]) === true
		&& is_array($this->_dsoconf[$module]) === true)
			return($this->_dsoconf[$module]);

		return(false);
	}
}

?>
