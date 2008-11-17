<?php

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'config.inc');

define('XIVO_PATH_OBJECT',XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'object');
define('XIVO_PATH_OBJECTCONF',XIVO_PATH_OBJECT.DIRECTORY_SEPARATOR.'objectconf');

require_once(XIVO_PATH_OBJECTCONF.DIRECTORY_SEPARATOR.'xivo.inc');

require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'blop.inc');

class xivo extends blop
{
	function xivo()
	{
		if(($this->_conf['user'] = xivo_gat::load_get('user',XIVO_PATH_OBJECTCONF)) === false)
			die('ERR: Failed to load user Array');

		$this->blop();

		$this->_init_cfg();
		$this->_set_error();
		$this->_set_query();
		$this->_set_session();
		$this->_set_i18n();
		$this->_set_service();
		$this->_set_user();

		xivo_gat::set('_CF',$this->_conf);
	}

	function _init_cfg()
	{
		if(($xivo_ini = blop::load_init(XIVO_PATH_CONF.XIVO_SEP_DIR.'xivo.ini')) === false)
			die('ERR: XIVO init file not found or unreadable');

		if(xivo_issa('locale',$xivo_ini) === true)
			$this->_cfg_ini_locale($xivo_ini['locale']);
		else
			$this->_cfg_ini_locale();

		if(xivo_issa('session',$xivo_ini) === true)
			$this->_cfg_ini_session($xivo_ini['session']);
		else
			$this->_cfg_ini_session();

		if(xivo_issa('error',$xivo_ini) === true)
			$this->_cfg_ini_error($xivo_ini['error']);

		$this->_cfg_ini_set();

		if(xivo_issa('general',$xivo_ini) === true && isset($xivo_ini['general']['datastorage']) === true)
			$this->_cfg_ini_datastorage($xivo_ini['general']['datastorage']);
		else if(xivo_issa('datastorage',$xivo_ini) === true)
			$this->_cfg_ini_datastorage($xivo_ini['datastorage']);
		else
			die('ERR: Missing datastorage configuration in XIVO init file');

		if(xivo_issa('service',$xivo_ini) === true)
			$this->_cfg_ini_service($xivo_ini['service']);
		else
			die('ERR: Missing service configuration in XIVO init file');

		if(xivo_issa('template',$xivo_ini) === true)
			$this->_cfg_ini_template($xivo_ini['template']);
	}

	function _set_service()
	{
		xivo::load_class('xivo_service',XIVO_PATH_OBJECT,null,false);
		xivo_gct::set_get(new xivo_service($this->_conf['xivo']['sys']['service']));
	}

	function _set_user()
	{
		xivo::load_class('xivo_user',XIVO_PATH_OBJECT,null,false);
		new xivo_user();
	}

	function _cfg_ini_service($service)
	{
		if(is_array($service) === false)
			die('ERR: Invalid service configuration in XIVO init file');

		$this->_conf['xivo']['sys']['service'] = $service;
	}
}

?>
