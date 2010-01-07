<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'config.inc');

define('XIVO_PATH_OBJECT',XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'object');
define('XIVO_PATH_OBJECTCONF',XIVO_PATH_OBJECT.DIRECTORY_SEPARATOR.'objectconf');

require_once(XIVO_PATH_OBJECTCONF.DIRECTORY_SEPARATOR.'xivo.inc');

require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'dwho.inc');

class xivo extends dwho
{
	function xivo()
	{
		if(($this->_conf['user'] = dwho_gat::load_get('user',XIVO_PATH_OBJECTCONF)) === false)
			die('ERR: Failed to load user Array');

		$this->dwho();
		$this->_conf['xivo'] = $this->_conf['dwho'];

		$this->_init_cfg();
		$this->_set_error();
		$this->_set_query();
		$this->_set_session();
		$this->_set_i18n();

		if(dwho_constant('XIVO_WEBI_CONFIGURED',false) === true)
		{
			$this->_set_xivo_object();
			$this->_set_service();
			$this->_set_user();
		}

		dwho_gat::set('_CF',$this->_conf);
	}

	function _init_cfg()
	{
		if(($xivo_ini = dwho::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.'xivo.ini')) === false)
			define('XIVO_WEBI_CONFIGURED',false);
		else
			define('XIVO_WEBI_CONFIGURED',true);

		if(dwho_issa('locale',$xivo_ini) === true)
			$this->_cfg_ini_locale($xivo_ini['locale']);
		else
			$this->_cfg_ini_locale();

		if(dwho_issa('session',$xivo_ini) === true)
			$this->_cfg_ini_session($xivo_ini['session']);
		else
			$this->_cfg_ini_session();

		if(dwho_issa('error',$xivo_ini) === true)
			$this->_cfg_ini_error($xivo_ini['error']);

		$this->_cfg_ini_set();

		ini_set('include_path',XIVO_PATH_ROOT.DWHO_SEP_PATH.ini_get('include_path'));

		if(($location_ini = dwho::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.'location.ini')) !== false)
			$this->_cfg_ini_location($location_ini);
		else
			die('ERR: Missing location init file');

		if(($template_ini = dwho::load_init(XIVO_PATH_CONF.DWHO_SEP_DIR.'template.ini')) !== false)
			$this->_cfg_ini_template($template_ini);
		else
			die('ERR: Missing template init file');

		if(XIVO_WEBI_CONFIGURED === false)
			return(null);

		if(dwho_issa('general',$xivo_ini) === true && isset($xivo_ini['general']['datastorage']) === true)
			$this->_cfg_ini_datastorage($xivo_ini['general']['datastorage']);
		else if(dwho_issa('datastorage',$xivo_ini) === true)
			$this->_cfg_ini_datastorage($xivo_ini['datastorage']);
		else
			die('ERR: Missing datastorage configuration in XIVO init file');

		if(dwho_issa('service',$xivo_ini) === true)
			$this->_cfg_ini_service($xivo_ini['service']);
		else
			die('ERR: Missing service configuration in XIVO init file');

		return(true);
	}

	function _set_xivo_object()
	{
		xivo::load_class('xivo_object',XIVO_PATH_OBJECT,'xivo',false);
		dwho_gct::set(new xivo_object());
	}

	function _set_service()
	{
		xivo::load_class('xivo_service',XIVO_PATH_OBJECT,null,false);
		dwho_gct::set(new xivo_service($this->_conf['xivo']['ini']['service']));
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

		$this->_conf['xivo']['ini']['service'] = $service;
	}

	function load_class($class,$dir='',$file='',$lib=true)
	{
		if(strncmp('xivo_',$class,5) === 0
		&& dwho_has_len($file) === false)
			$file = substr($class,5);

		return(dwho::load_class($class,$dir,$file,$lib));
	}
}

?>
