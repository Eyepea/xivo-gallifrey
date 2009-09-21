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

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'config.inc');

define('XIVO_PATH_LIBCONF',XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'libconf');

require_once(XIVO_PATH_LIBCONF.DIRECTORY_SEPARATOR.'define.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'func.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'file.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'sysinfo.inc');

define('XIVO_OS_NAME',xivo_sysinfo::os_name());
define('XIVO_OS_META',xivo_sysinfo::os_meta());
define('XIVO_PHP_VER',xivo_sysinfo::php_version());
define('XIVO_SEP_ARG_IN',xivo_sysinfo::argin_separator());
define('XIVO_SEP_ARG_OUT',xivo_sysinfo::argout_separator());
define('XIVO_SEP_DIR',xivo_sysinfo::directory_separator());
define('XIVO_SEP_PATH',xivo_sysinfo::path_separator());
define('XIVO_EOL',xivo_sysinfo::eol());
define('XIVO_SAPI_NAME',xivo_sysinfo::sapi_name());
define('XIVO_SAPI_MODE',xivo_sysinfo::sapi_mode());

require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gateclass.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gatearray.inc');

class blop
{
	var $_conf	= array();

	function blop()
	{
		if(($this->_conf['blop'] = xivo_gat::get_file(XIVO_PATH_LIBCONF.DIRECTORY_SEPARATOR.'blop')) === false)
			die('ERR: Failed to load BLOP Array');
		else if(($this->_conf['datastorage'] = xivo_gat::load_get('datastorage',XIVO_PATH_LIBCONF)) === false)
			die('ERR: Failed to load datastorage Array');
		else if(function_exists('version_compare') === false
		|| version_compare(PHP_VERSION,XIVO_PHP_VER_MIN,'<') === true
		|| version_compare(PHP_VERSION,XIVO_PHP_VER_MAX,'>=') === true)
			die('ERR: Please update your PHP');

		$this->_cfg_ini_get();
		$this->_cfg_url();
		$this->_cfg_infos_extension();
	}

	function _set_error()
	{
		xivo::load_class('xivo_tracerror');
		$_ERR = &xivo_gct::set_get(new xivo_tracerror($this->_conf['blop']['ini']['error']));
		set_error_handler(array(&$_ERR,'report_e_err'));
	}

	function _set_query()
	{
		blop::load_class('xivo_query');
		$_QUERY = &xivo_gct::set_get(new xivo_query());
		xivo_gat::set_get('_QR',$_QUERY->raw);
	}

	function _set_session()
	{
		blop::load_class('xivo_session');
		xivo_gct::set(new xivo_session());
	}

	function _set_i18n()
	{
		blop::load_class('xivo_i18n');
		xivo_gct::set(new xivo_i18n());
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

		define('XIVO_URL',$url);
	}

	function _cfg_ini_get()
	{
		define('XIVO_PHP_SAFE',(bool) ini_get('safe_mode'));

		$this->_conf['blop']['ini']['safe_mode'] = XIVO_PHP_SAFE;
		$this->_conf['blop']['ini']['enable_dl'] = (bool) ini_get('enable_dl');
		$this->_conf['blop']['ini']['file_uploads'] = (bool) ini_get('file_uploads');
	}

	function _cfg_ini_set()
	{
		ini_set('register_globals',false);
		ini_set('magic_quotes_gpc',false);
		ini_set('magic_quotes_sybase',false);
		ini_set('magic_quotes_runtime',false);
		ini_set('register_argc_argv',true);

		if(XIVO_PHP_SAFE === false)
			ini_set('max_execution_time',0);

		ini_set('include_path',XIVO_PATH_ROOT.XIVO_SEP_PATH.XIVO_PATH_LIB.XIVO_SEP_PATH.ini_get('include_path'));
		ini_set('default_charset',XIVO_LC_CHARSET);

		if($this->load_extension('mbstring') !== false)
		{
			ini_set('mbstring.internal_encoding',XIVO_LC_CHARSET);
			ini_set('mbstring.language',XIVO_LC_CHARSET);
			ini_set('mbstring.http_input',XIVO_LC_CHARSET);
			ini_set('mbstring.http_output',XIVO_LC_CHARSET);
		}
	}

	function _cfg_ini_locale($locale=null)
	{
		if(is_array($locale) === false)
			$locale = $this->_conf['blop']['ini']['locale'];

		if(isset($locale['language']) === false)
			$locale['language'] = $this->_conf['blop']['ini']['language'];

		define('XIVO_LC_LANGUAGE',(string) $locale['language']);

		if(isset($locale['territory']) === false)
			$locale['territory'] = $this->_conf['blop']['ini']['territory'];

		define('XIVO_LC_TERRITORY',(string) $locale['territory']);

		if(isset($locale['charset']) === false)
			$locale['charset'] = $this->_conf['blop']['ini']['charset'];

		define('XIVO_LC_CHARSET',(string) $locale['charset']);

		define('XIVO_LC_LABEL',setlocale(LC_ALL,0));
	}

	function _cfg_ini_session($session=null)
	{
		if(is_array($session) === false)
			$session = $this->_conf['blop']['ini']['session'];

		if(isset($session['name']) === false)
			$session['name'] = $this->_conf['blop']['ini']['session']['name'];

		define('XIVO_SESS_NAME',(string) $session['name']);

		if(isset($session['time']) === false)
			$session['time'] = $this->_conf['blop']['ini']['session']['time'];

		define('XIVO_SESS_TIME',xivo_uint($session['time']));

		if(isset($session['path']) === false)
			$session['path'] = $this->_conf['blop']['ini']['session']['path'];

		define('XIVO_SESS_PATH',(string) $session['path']);
	}

	function _cfg_ini_error($error=null)
	{
		if(xivo_issa('error',$this->_conf['blop']['ini']) === false)
			$this->_conf['blop']['ini']['error'] = array();

		if(is_array($error) === false)
			return(null);
		else if(isset($error['level']) === true)
			$this->_conf['blop']['ini']['error']['level'] = (string) $error['level'];

		if(isset($error['report_type']) === true)
			$this->_conf['blop']['ini']['error']['report_type'] = (string) $error['report_type'];

		if(isset($error['report_mode']) === true)
			$this->_conf['blop']['ini']['error']['report_mode'] = (string) $error['report_mode'];

		if(isset($error['report_func']) === true)
			$this->_conf['blop']['ini']['error']['report_func'] = (string) $error['report_func'];

		if(isset($error['email']) === true)
			$this->_conf['blop']['ini']['error']['email'] = (string) $error['email'];

		if(isset($error['file']) === true)
			$this->_conf['blop']['ini']['error']['file'] = (string) $error['file'];
	}

	function _cfg_infos_extension()
	{
		$prefix = $suffix = '';

		if(defined('PHP_SHLIB_PREFIX') === true)
			$prefix = PHP_SHLIB_PREFIX;

		if(defined('PHP_SHLIB_SUFFIX') === true)
			$suffix = PHP_SHLIB_SUFFIX;

		switch(XIVO_OS_META)
		{
			default:
				if($suffix === '')
					$suffix = 'so';
		}

		if($this->_conf['blop']['ini']['safe_mode'] === true
		|| $this->_conf['blop']['ini']['enable_dl'] === false)
			define('XIVO_EXT_ENABLE',false);
		else
			define('XIVO_EXT_ENABLE',true);

		define('XIVO_EXT_PRE',$prefix);
		define('XIVO_EXT_SUF',$suffix);
	}

	function _cfg_ini_datastorage($dso)
	{
		if(xivo_issa('datastorage',$this->_conf['blop']['ini']) === false)
			$this->_conf['blop']['ini']['datastorage'] = array();

		blop::load_class('xivo_datastorage');

		if(is_string($dso) === true)
			$dso = xivo_datastorage::set_param_from_uri($dso);

		if(is_array($dso) === false)
			die('ERR: Invalid datastorage configuration in init file');
		else if(isset($dso['type']) === false)
			die('ERR: Missing datastorage "type" configuration in init file');

		if(xivo_datastorage::is_valid($dso['type']) === false)
			die('ERR: Invalid datastorage "type" configuration in init file');
		else if(xivo_datastorage::chk_extension_loaded($dso['type'],true) === false)
			die('ERR: Datastorage '.$dso['type'].' extension is not loaded');
		else if(($datastorage = &xivo_gct::set_get(
						xivo_datastorage::factory($dso['type'],
									  $dso),
						'XIVO_DSO')) === false)
			die('ERR: Datastorage class '.$dso['type'].' does not exist');

		define('XIVO_DSO_TYPE',$datastorage->get_type());

		$this->_conf['blop']['ini']['datastorage']['type'] = XIVO_DSO_TYPE;
		$this->_conf['blop']['ini']['datastorage']['param'] = $dso;
	}

	function _cfg_ini_template($template)
	{
		if(is_array($template) === false || empty($template) === true)
			die('ERR: Invalid template configuration in init file');

		$def_script_path = $def_application_path = $def_tpl_path = false;

		if(isset($template['script_path']) === true
		&& ($def_script_path = xivo_file::is_d_r($template['script_path'])) === false)
			die('ERR: Missing or invalid script_path for template configuration in init file');

		if(isset($template['application_path']) === true
		&& ($def_application_path = xivo_file::is_d_r($template['application_path'])) === false)
			die('ERR: Missing or invalid application_path for template configuration in init file');

		if(isset($template['tpl_path']) === true
		&& ($def_tpl_path = xivo_file::is_d_r($template['tpl_path'])) === false)
			die('ERR: Missing or invalid tpl_path for template configuration in init file');

		unset($template['script_path'],$template['application_path'],$template['tpl_path']);

		if(empty($template) === true)
			die('ERR: Invalid template configuration in init file');

		foreach($template as $key => $value)
		{
			if(is_array($value) === false)
				die('ERR: Missing information in '.$key.' for template configuration in init file');
			else if(isset($value['script_path']) === true && ($script_path = xivo_file::is_d_r($value['script_path'])) !== false)
				$this->_conf['tpl'][$key]['script_path'] = $script_path;
			else if($def_script_path !== false)
				$this->_conf['tpl'][$key]['script_path'] = $def_script_path;
			else
				die('ERR: Missing or invalid script_path in '.$key.' for template configuration in init file');

			if(isset($value['application_path']) === true && ($application_path = xivo_file::is_d_r($value['application_path'])) !== false)
				$this->_conf['tpl'][$key]['application_path'] = $application_path;
			else if($def_application_path !== false)
				$this->_conf['tpl'][$key]['application_path'] = $def_application_path;
			else
				die('ERR: Missing or invalid application_path in '.$key.' for template configuration in init file');

			if(isset($value['tpl_path']) === true && ($tpl_path = xivo_file::is_d_r($value['tpl_path'])) !== false)
				$this->_conf['tpl'][$key]['tpl_path'] = $tpl_path;
			else if($def_tpl_path !== false)
				$this->_conf['tpl'][$key]['tpl_path'] = $def_tpl_path;
			else
				die('ERR: Missing or invalid tpl_path in '.$key.' for template configuration in init file');

			if(isset($value['script_root']) === true)
				$this->_conf['tpl'][$key]['script_root'] = rtrim($value['script_root'],DIRECTORY_SEPARATOR);
			else
				$this->_conf['tpl'][$key]['script_root'] = '';
		}
	}

	function chk_extension_loaded($ext)
	{
		if(xivo_has_len($ext) === false)
			return(false);

		return(extension_loaded($ext));
	}

	function load_extension($ext)
	{
		if(xivo_has_len($ext) === false
		|| preg_match('/^[a-z0-9_-]+$/',$ext) !== 1)
			return(false);
		else if(extension_loaded($ext) === true)
			return(null);
		else if(defined('XIVO_EXT_ENABLE') === false || XIVO_EXT_ENABLE === false)
			return(false);

		$file = XIVO_EXT_PRE.$ext.'.'.XIVO_EXT_SUF;

		$path = ini_get('extension_dir').DIRECTORY_SEPARATOR.$file;

		if(xivo_file::is_f_r($path) === false)
			return(false);

		dl($file);

		return(extension_loaded($ext));
	}

	function load_class($class,$dir='',$file='',$lib=true)
	{
		$class = (string) $class;

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

			if(substr($path,0,5) === 'xivo'.DIRECTORY_SEPARATOR)
				$dir = XIVO_PATH_LIB.DIRECTORY_SEPARATOR.dirname(substr($path,5));
			else
				$dir = dirname($path);

			$file = basename($path).'.inc';
		}
		else if($dir === '' && $path !== $class)
		{
			$dir = XIVO_PATH_LIB.DIRECTORY_SEPARATOR.dirname($path);
			$file .= '.inc';
		}
		else if(substr($class,0,5) === 'xivo_')
		{
			if((bool) $lib === true)
				$dir = rtrim(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.ltrim($dir,DIRECTORY_SEPARATOR),DIRECTORY_SEPARATOR.'.');
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

		if(xivo_file::load_file($file,$dir,true) === false)
			return(false);

		return(class_exists($class));
	}

	function load_init($file,$name='')
	{
		$name = (string) $name;

		if(($file = xivo_file::is_f_r($file)) === false
		|| ($init = parse_ini_file($file,true)) === false
		|| empty($init) === true
		|| ($name !== '' && isset($init[$name]) === false) === true)
			return(false);

		$r = array();

		foreach($init as $k => $v)
		{
			if($name !== '' && $name !== $k)
				continue;
			else if(is_array($v) === false)
			{
				$r[$k] = $v;
				continue;
			}
			else if(empty($v) === true)
				continue;

			foreach($v as $ks => $vs)
			{
				if(($pos = strpos($ks,'.')) === false)
				{
					if(isset($r[$k]) === false)
						$r[$k] = array();

					$r[$k][$ks] = $vs;
					continue;
				}
				else if($pos === 0 || ($key2 = substr($ks,$pos+1)) === '')
					continue;

				$key1 = substr($ks,0,$pos);

				if(isset($r[$k]) === false)
					$r[$k] = array();

				if(isset($r[$k][$key1]) === false)
					$r[$k][$key1] = array();

				$r[$k][$key1][$key2] = $vs;
			}
		}

		if($name !== '')
		{
			if(isset($r[$name]) === true)
				$r = &$r[$name];
			else
				$r = false;
		}

		return($r);
	}

	function load_array($file,$once=true)
	{
		return(xivo_gat::get_file($file,$once));
	}
}

?>
