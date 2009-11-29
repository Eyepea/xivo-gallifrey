<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

if(defined('DWHO_PATH_ROOT') === false)
	define('DWHO_PATH_ROOT',dirname(__FILE__));

define('DWHO_PATH_CONF',DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'libconf');

require_once(DWHO_PATH_CONF.DIRECTORY_SEPARATOR.'define.inc');
require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'func.inc');
require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'file.inc');
require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'sysinfo.inc');

define('DWHO_OS_NAME',dwho_sysinfo::os_name());
define('DWHO_OS_META',dwho_sysinfo::os_meta());
define('DWHO_PHP_VER',dwho_sysinfo::php_version());
define('DWHO_SEP_ARG_IN',dwho_sysinfo::argin_separator());
define('DWHO_SEP_ARG_OUT',dwho_sysinfo::argout_separator());
define('DWHO_SEP_DIR',dwho_sysinfo::directory_separator());
define('DWHO_SEP_PATH',dwho_sysinfo::path_separator());
define('DWHO_EOL',dwho_sysinfo::eol());
define('DWHO_SAPI_NAME',dwho_sysinfo::sapi_name());
define('DWHO_SAPI_MODE',dwho_sysinfo::sapi_mode());

require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'gateclass.inc');
require_once(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.'gatearray.inc');

class dwho
{
	var $_conf	= array();

	function dwho()
	{
		if(($this->_conf['dwho'] = dwho_gat::get_file(DWHO_PATH_CONF.DIRECTORY_SEPARATOR.'dwho')) === false)
			die('ERR: Failed to load DWHO Array');
		else if(($this->_conf['datastorage'] = dwho_gat::load_get('datastorage',DWHO_PATH_CONF)) === false)
			die('ERR: Failed to load datastorage Array');
		else if(function_exists('version_compare') === false
		|| version_compare(PHP_VERSION,DWHO_PHP_VER_MIN,'<') === true
		|| version_compare(PHP_VERSION,DWHO_PHP_VER_MAX,'>=') === true)
			die('ERR: Please update your PHP');

		$this->_cfg_ini_get();
		$this->_cfg_url();
		$this->_cfg_infos_extension();
	}

	function _set_error()
	{
		dwho::load_class('dwho_tracerror');
		$_ERR = &dwho_gct::set_get(new dwho_tracerror($this->_conf['dwho']['ini']['error']));
		set_error_handler(array(&$_ERR,'report_e_err'));
	}

	function _set_query()
	{
		dwho::load_class('dwho_query');
		$_QUERY = &dwho_gct::set_get(new dwho_query());
		dwho_gat::set_get('_QR',$_QUERY->raw);
	}

	function _set_session()
	{
		dwho::load_class('dwho_session');
		dwho_gct::set(new dwho_session());
	}

	function _set_i18n()
	{
		dwho::load_class('dwho_i18n');
		dwho_gct::set(new dwho_i18n());
	}

	function _cfg_url()
	{
		$url = $scheme = '';

		if(isset($_SERVER['SERVER_ADDR'],$_SERVER['SERVER_PORT']) === true)
		{
			$url = $_SERVER['SERVER_ADDR'].':'.$_SERVER['SERVER_PORT'];

			if(isset($_SERVER['HTTP_HOST']) === true)
			{
				$scheme = 'http';
				if(isset($_SERVER['HTTPS']) === true
				&& (bool) $_SERVER['HTTPS'] === true)
					$scheme .= 's';

				$url = $scheme.'://'.$url;
			}
		}

		define('DWHO_URL',$url);
	}

	function _cfg_ini_get()
	{
		define('DWHO_PHP_SAFE',(bool) ini_get('safe_mode'));

		$this->_conf['dwho']['ini']['safe_mode'] = DWHO_PHP_SAFE;
		$this->_conf['dwho']['ini']['enable_dl'] = (bool) ini_get('enable_dl');
		$this->_conf['dwho']['ini']['file_uploads'] = (bool) ini_get('file_uploads');
	}

	function _cfg_ini_set()
	{
		ini_set('register_globals',false);
		ini_set('magic_quotes_gpc',false);
		ini_set('magic_quotes_sybase',false);
		ini_set('magic_quotes_runtime',false);
		ini_set('register_argc_argv',true);

		if(DWHO_PHP_SAFE === false)
			ini_set('max_execution_time',0);

		ini_set('include_path',DWHO_PATH_ROOT.DWHO_SEP_PATH.ini_get('include_path'));
		ini_set('default_charset',DWHO_LC_CHARSET);

		if($this->load_extension('mbstring') !== false)
		{
			ini_set('mbstring.internal_encoding',DWHO_LC_CHARSET);
			ini_set('mbstring.language',DWHO_LC_CHARSET);
			ini_set('mbstring.http_input',DWHO_LC_CHARSET);
			ini_set('mbstring.http_output',DWHO_LC_CHARSET);
		}
	}

	function _cfg_ini_locale($locale=null)
	{
		if(is_array($locale) === false)
			$locale = $this->_conf['dwho']['ini']['locale'];

		if(isset($locale['language']) === false)
			$locale['language'] = $this->_conf['dwho']['ini']['language'];

		define('DWHO_LC_LANGUAGE',(string) $locale['language']);

		if(isset($locale['territory']) === false)
			$locale['territory'] = $this->_conf['dwho']['ini']['territory'];

		define('DWHO_LC_TERRITORY',(string) $locale['territory']);

		if(isset($locale['charset']) === false)
			$locale['charset'] = $this->_conf['dwho']['ini']['charset'];

		define('DWHO_LC_CHARSET',(string) $locale['charset']);

		define('DWHO_LC_LABEL',setlocale(LC_ALL,0));
	}

	function _cfg_ini_session($session=null)
	{
		if(is_array($session) === false)
			$session = $this->_conf['dwho']['ini']['session'];

		if(isset($session['name']) === false)
			$session['name'] = $this->_conf['dwho']['ini']['session']['name'];

		define('DWHO_SESS_NAME',(string) $session['name']);

		if(isset($session['time']) === false)
			$session['time'] = $this->_conf['dwho']['ini']['session']['time'];

		define('DWHO_SESS_TIME',dwho_uint($session['time']));

		if(isset($session['path']) === false)
			$session['path'] = $this->_conf['dwho']['ini']['session']['path'];

		define('DWHO_SESS_PATH',(string) $session['path']);
	}

	function _cfg_ini_error($error=null)
	{
		if(dwho_issa('error',$this->_conf['dwho']['ini']) === false)
			$this->_conf['dwho']['ini']['error'] = array();

		if(is_array($error) === false)
			return(null);
		else if(isset($error['level']) === true)
			$this->_conf['dwho']['ini']['error']['level'] = (string) $error['level'];

		if(isset($error['report_type']) === true)
			$this->_conf['dwho']['ini']['error']['report_type'] = (string) $error['report_type'];

		if(isset($error['report_mode']) === true)
			$this->_conf['dwho']['ini']['error']['report_mode'] = (string) $error['report_mode'];

		if(isset($error['report_func']) === true)
			$this->_conf['dwho']['ini']['error']['report_func'] = (string) $error['report_func'];

		if(isset($error['email']) === true)
			$this->_conf['dwho']['ini']['error']['email'] = (string) $error['email'];

		if(isset($error['file']) === true)
			$this->_conf['dwho']['ini']['error']['file'] = (string) $error['file'];
	}

	function _cfg_infos_extension()
	{
		$prefix = $suffix = '';

		if(defined('PHP_SHLIB_PREFIX') === true)
			$prefix = PHP_SHLIB_PREFIX;

		if(defined('PHP_SHLIB_SUFFIX') === true)
			$suffix = PHP_SHLIB_SUFFIX;

		switch(DWHO_OS_META)
		{
			default:
				if($suffix === '')
					$suffix = 'so';
		}

		if($this->_conf['dwho']['ini']['safe_mode'] === true
		|| $this->_conf['dwho']['ini']['enable_dl'] === false)
			define('DWHO_EXT_ENABLE',false);
		else
			define('DWHO_EXT_ENABLE',true);

		define('DWHO_EXT_PRE',$prefix);
		define('DWHO_EXT_SUF',$suffix);
	}

	function _cfg_ini_datastorage($dso)
	{
		if(dwho_issa('datastorage',$this->_conf['dwho']['ini']) === false)
			$this->_conf['dwho']['ini']['datastorage'] = array();

		dwho::load_class('dwho_datastorage');

		if(is_string($dso) === true)
			$dso = dwho_datastorage::set_param_from_uri($dso);

		if(is_array($dso) === false)
			die('ERR: Invalid datastorage configuration');
		else if(isset($dso['type']) === false)
			die('ERR: Missing datastorage "type" configuration');

		if(dwho_datastorage::is_valid($dso['type']) === false)
			die('ERR: Invalid datastorage "type" configuration');
		else if(dwho_datastorage::chk_extension_loaded($dso['type'],true) === false)
			die('ERR: Datastorage '.$dso['type'].' extension is not loaded');
		else if(($datastorage = &dwho_gct::set_get(
						dwho_datastorage::factory($dso['type'],
									  $dso),
						'DWHO_DSO')) === false)
			die('ERR: Datastorage class '.$dso['type'].' does not exist');

		define('DWHO_DSO_TYPE',$datastorage->get_type());

		$this->_conf['dwho']['ini']['datastorage']['type'] = DWHO_DSO_TYPE;
		$this->_conf['dwho']['ini']['datastorage']['param'] = $dso;
	}

	function _cfg_ini_location($location)
	{
		if(is_array($location) === false || empty($location) === true)
			die('ERR: Invalid location configuration');
		else if(dwho_issa('general',$location) === false)
			die('ERR: Missing general section for location configuration');

		$this->_conf['location'] = array(
					'script'	=> null,
					'application'	=> null,
					'root_url'	=> null);

		if(isset($location['general']['script']) === false)
			die('ERR: Missing script option in general for location configuration');
		else if(($this->_conf['location']['script'] = dwho_file::is_d_r($location['general']['script'])) === false)
			die('ERR: Invalid script option in general for path configuration');

		if(isset($location['general']['application']) === false)
			die('ERR: Missing application option in general for location configuration');
		else if(($this->_conf['location']['application'] = dwho_file::is_d_r($location['general']['application'])) === false)
			die('ERR: Invalid application option in general for path configuration');

		if(isset($location['general']['root_url']) === true)
		{
			if(is_string($location['general']['root_url']) === true)
				$this->_conf['location']['root_url'] = rtrim($location['general']['root_url'],'/');
			else
				die('ERR: Invalid root_url option in general for location configuration');
		}

		return(true);
	}

	function _cfg_ini_template($template)
	{
		if(is_array($template) == false)
			die('ERR: Invalid template configuration');

		$this->_conf['template'] = array();

		foreach($template as $key => $value)
		{
			if(isset($value['path']) === true
			&& ($path = dwho_file::is_d_r($value['path'])) !== false)
				$this->_conf['template'][$key]['path'] = $path;
			else
				die('ERR: Missing or invalid option path in '.$key.' for template configuration');
		}

		return(true);
	}

	function chk_extension_loaded($ext)
	{
		if(dwho_has_len($ext) === false)
			return(false);

		return(extension_loaded($ext));
	}

	function load_extension($ext)
	{
		if(dwho_has_len($ext) === false
		|| preg_match('/^[a-z0-9_-]+$/',$ext) !== 1)
			return(false);
		else if(extension_loaded($ext) === true)
			return(null);
		else if(defined('DWHO_EXT_ENABLE') === false || DWHO_EXT_ENABLE === false)
			return(false);

		$file = DWHO_EXT_PRE.$ext.'.'.DWHO_EXT_SUF;

		$path = ini_get('extension_dir').DIRECTORY_SEPARATOR.$file;

		if(dwho_file::is_f_r($path) === false)
			return(false);

		dl($file);

		return(extension_loaded($ext));
	}

	function load_class($class,$dir='',$file='',$lib=true)
	{
		if(is_array($dir) === true)
			$dir = implode(DIRECTORY_SEPARATOR,$dir);
		else
			$dir = (string) $dir;

		$file = (string) $file;

		if(class_exists($class) === true)
			return(true);

		$path = str_replace('::',DIRECTORY_SEPARATOR,$class);
		$dir = str_replace('::',DIRECTORY_SEPARATOR,$dir);

		if($file === '' && $dir === '' && $path !== $class)
		{
			$class = str_replace('::','_',$class);

			if(substr($path,0,5) === 'dwho'.DIRECTORY_SEPARATOR)
				$dir = DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.dirname(substr($path,5));
			else
				$dir = dirname($path);

			$file = basename($path).'.inc';
		}
		else if($dir === '' && $path !== $class)
		{
			$dir = DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.dirname($path);
			$file .= '.inc';
		}
		else if(strncmp('dwho_',$class,5) === 0)
		{
			if((bool) $lib === true)
				$dir = rtrim(DWHO_PATH_ROOT.DIRECTORY_SEPARATOR.ltrim($dir,DIRECTORY_SEPARATOR),DIRECTORY_SEPARATOR.'.');
			else
				$dir = rtrim($dir,DIRECTORY_SEPARATOR.'.');

			if($file === '')
				$file = substr($class,5).'.inc';
			else
				$file .= '.inc';
		}
		else if($file !== '')
			$file .= '.inc';
		else
			$file = $class.'.inc';

		if(dwho_file::load_file($file,$dir,true) === false)
			return(false);

		return(class_exists($class));
	}

	function load_init($file,$name='')
	{
		$name = (string) $name;

		if(($file = dwho_file::is_f_r($file)) === false
		|| ($init = parse_ini_file($file,true)) === false
		|| empty($init) === true
		|| ($name !== '' && isset($init[$name]) === false) === true)
			return(false);

		$r = array();

		foreach($init as $section => $options)
		{
			if($name !== '' && $name !== $section)
				continue;
			else if(is_array($options) === false)
			{
				$r[$section] = $options;
				continue;
			}
			else if(empty($options) === true)
				continue;

			foreach($options as $optname => $optvalue)
			{
				if(($pos = strpos($optname,'.')) === false)
				{
					if(isset($r[$section]) === false)
						$r[$section] = array();

					$r[$section][$optname] = $optvalue;
					continue;
				}
				else if($pos === 0 || ($optnamepart2 = substr($optname,$pos + 1)) === '')
					continue;

				$optnamepart1 = substr($optname,0,$pos);

				if(isset($r[$section]) === false)
					$r[$section] = array();

				if(isset($r[$section][$optnamepart1]) === false)
					$r[$section][$optnamepart1] = array();

				$r[$section][$optnamepart1][$optnamepart2] = $optvalue;
			}
		}

		if($name === '')
			return($r);
		else if(isset($r[$name]) === true)
			return($r[$name]);

		return(false);
	}

	function load_array($file,$once=true)
	{
		return(dwho_gat::get_file($file,$once));
	}
}

?>
