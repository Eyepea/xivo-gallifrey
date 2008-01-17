<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'abstract'.XIVO_SEP_DIR.'datastorage'.XIVO_SEP_DIR.'datastorage.inc');

class xivo_application_service_asterisk_abstract_sql extends xivo_abstract_datastorage
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
		if(($arr = xivo_get_aks($this->_table)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			if($this->_get_table($arr['keys'][$i]) === false)
				trigger_error('Invalid or missing Datastorage table '.$arr['keys'][$i],E_USER_ERROR);
		}
		
		return(true);
	}

	function get_cnt()
	{
		return(xivo_uint($this->_cnt));
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
