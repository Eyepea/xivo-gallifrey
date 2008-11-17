<?php

require_once(XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'config.inc');

define('XIVO_PATH_LIBCONF',XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'libconf');

require_once(XIVO_PATH_LIBCONF.DIRECTORY_SEPARATOR.'define.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'func.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'file.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gateclass.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gatearray.inc');

class blop
{
	var $_conf	= array();

	function blop()
	{
		if(($this->_conf['xivo'] = xivo_gat::load_get('blop',XIVO_PATH_LIBCONF)) === false)
			die('ERR: Failed to load BLOP Array');
		else if(($this->_conf['datastorage'] = xivo_gat::load_get('datastorage',XIVO_PATH_LIBCONF)) === false)
			die('ERR: Failed to load datastorage Array');
		else if(function_exists('version_compare') === false
		|| version_compare(PHP_VERSION,$this->_conf['xivo']['sys']['php_version']['min'],'<') === true
		|| version_compare(PHP_VERSION,$this->_conf['xivo']['sys']['php_version']['max'],'>=') === true)
			die('ERR: Please update your PHP');

		$this->_cfg_os();
		$this->_cfg_php_version();
		$this->_cfg_ini_get();
		$this->_cfg_separator();
		$this->_cfg_eol();
		$this->_cfg_sapi();
		$this->_cfg_url();
		$this->_cfg_infos_extension();

		xivo::load_class('xivo_gateclass');
		xivo::load_class('xivo_gatearray');
	}

	function _set_error()
	{
		xivo::load_class('xivo_tracerror');
		$_ERR = &xivo_gct::set_get(new xivo_tracerror($this->_conf['xivo']['sys']['error']));
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

	function _cfg_os()
	{
		$sys = strtolower(substr(PHP_OS,0,3));

		$this->_conf['xivo']['sys']['os'] = array();
		$this->_conf['xivo']['sys']['os']['name'] = PHP_OS;
		$this->_conf['xivo']['sys']['os']['meta'] = $sys;

		define('XIVO_OS_NAME',PHP_OS);
		define('XIVO_OS_META',$sys);
	}

	function _cfg_php_version()
	{
		$this->_conf['xivo']['sys']['php_version']['ver'] = PHP_VERSION;

		define('XIVO_PHP_VER',$this->_conf['xivo']['sys']['php_version']['ver']);
	}

	function _cfg_ini_get()
	{
		$this->_conf['xivo']['ini']['safe_mode'] = (bool) ini_get('safe_mode');
		$this->_conf['xivo']['ini']['enable_dl'] = (bool) ini_get('enable_dl');
		$this->_conf['xivo']['ini']['file_uploads'] = (bool) ini_get('file_uploads');

		define('XIVO_PHP_SAFE',$this->_conf['xivo']['ini']['safe_mode']);
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

	function _cfg_separator()
	{
		if(($sep = ini_get('arg_separator.input')) !== false && $sep !== '' && $sep !== '&amp;')
			$this->_conf['xivo']['sys']['separator']['arg_in'] = $sep;

		if(($sep = ini_get('arg_separator.ouput')) !== false && $sep !== '' && $sep !== '&amp;')
			$this->_conf['xivo']['sys']['separator']['arg_out'] = $sep;

		$this->_conf['xivo']['sys']['separator']['dir'] = DIRECTORY_SEPARATOR;
		$this->_conf['xivo']['sys']['separator']['path'] = PATH_SEPARATOR;

		define('XIVO_SEP_ARG_IN',$this->_conf['xivo']['sys']['separator']['arg_in']);
		define('XIVO_SEP_ARG_OUT',$this->_conf['xivo']['sys']['separator']['arg_out']);

		define('XIVO_SEP_DIR',$this->_conf['xivo']['sys']['separator']['dir']);
		define('XIVO_SEP_PATH',$this->_conf['xivo']['sys']['separator']['path']);
	}

	function _cfg_eol()
	{
		$this->_conf['xivo']['sys']['eol'] = PHP_EOL;

		define('XIVO_EOL',$this->_conf['xivo']['sys']['eol']);

		return(XIVO_EOL);
	}

	function _cfg_sapi()
	{
		$this->_conf['xivo']['sys']['sapi'] = array();
		$this->_conf['xivo']['sys']['sapi']['name'] = PHP_SAPI;

		switch(PHP_SAPI)
		{
			case 'cli':
				$this->_conf['xivo']['sys']['sapi']['mode'] = 'cli';
				break;
			default:
				$this->_conf['xivo']['sys']['sapi']['mode'] = 'default';
		}

		define('XIVO_SAPI_NAME',$this->_conf['xivo']['sys']['sapi']['name']);
		define('XIVO_SAPI_MODE',$this->_conf['xivo']['sys']['sapi']['mode']);
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

	function _cfg_ini_locale($locale=null)
	{
		if(is_array($locale) === false)
			$locale = $this->_conf['xivo']['sys']['locale'];

		if(isset($locale['language']) === false)
			$locale['language'] = $this->_conf['xivo']['sys']['language'];

		define('XIVO_LC_LANGUAGE',(string) $locale['language']);

		if(isset($locale['territory']) === false)
			$locale['territory'] = $this->_conf['xivo']['sys']['territory'];

		define('XIVO_LC_TERRITORY',(string) $locale['territory']);

		if(isset($locale['charset']) === false)
			$locale['charset'] = $this->_conf['xivo']['sys']['charset'];

		define('XIVO_LC_CHARSET',(string) $locale['charset']);

		define('XIVO_LC_LABEL',setlocale(LC_ALL,0));
	}

	function _cfg_ini_session($session=null)
	{
		if(is_array($session) === false)
			$session = $this->_conf['xivo']['sys']['session'];

		if(isset($session['name']) === false)
			$session['name'] = $this->_conf['xivo']['sys']['session']['name'];

		define('XIVO_SESS_NAME',(string) $session['name']);

		if(isset($session['time']) === false)
			$session['time'] = $this->_conf['xivo']['sys']['session']['time'];

		define('XIVO_SESS_TIME',xivo_uint($session['time']));

		if(isset($session['path']) === false)
			$session['path'] = $this->_conf['xivo']['sys']['session']['path'];

		define('XIVO_SESS_PATH',(string) $session['path']);
	}

	function _cfg_ini_error($error=null)
	{
		if(xivo_issa('error',$this->_conf['xivo']['sys']) === false)
			$this->_conf['xivo']['sys']['error'] = array();

		if(is_array($error) === false)
			return(null);
		else if(isset($error['level']) === true)
			$this->_conf['xivo']['sys']['error']['level'] = (string) $error['level'];

		if(isset($error['report_type']) === true)
			$this->_conf['xivo']['sys']['error']['report_type'] = (string) $error['report_type'];

		if(isset($error['report_mode']) === true)
			$this->_conf['xivo']['sys']['error']['report_mode'] = (string) $error['report_mode'];

		if(isset($error['report_func']) === true)
			$this->_conf['xivo']['sys']['error']['report_func'] = (string) $error['report_func'];

		if(isset($error['email']) === true)
			$this->_conf['xivo']['sys']['error']['email'] = (string) $error['email'];

		if(isset($error['file']) === true)
			$this->_conf['xivo']['sys']['error']['file'] = (string) $error['file'];
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
	
		if($this->_conf['xivo']['ini']['safe_mode'] === true
		|| $this->_conf['xivo']['ini']['enable_dl'] === false)
			define('XIVO_EXT_ENABLE',false);
		else
			define('XIVO_EXT_ENABLE',true);

		define('XIVO_EXT_PRE',$prefix);
		define('XIVO_EXT_SUF',$suffix);
	}

	function _cfg_ini_datastorage($dso)
	{
		if(xivo_issa('datastorage',$this->_conf['xivo']['sys']) === false)
			$this->_conf['xivo']['sys']['datastorage'] = array();

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
		else if(($dso = &xivo_gct::set_get(xivo_datastorage::factory($dso['type'],$dso),'XIVO_DSO')) === false)
			die('ERR: Datastorage class '.$dso['type'].' does not exist');

		define('XIVO_DSO_TYPE',$dso->get_type());

		$this->_conf['xivo']['sys']['datastorage']['type'] = $dso['type'];
		$this->_conf['xivo']['sys']['datastorage']['param'] = $dso;
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
				$this->_conf['tpl'][$key]['script_root'] = rtrim($value['script_root'],XIVO_SEP_DIR);
			else
				$this->_conf['tpl'][$key]['script_root'] = '';
		}
	}

	function chk_extension_loaded($ext)
	{
		if(xivo_haslen($ext) === false)
			return(false);

		return(extension_loaded($ext));
	}

	function load_extension($ext)
	{
		if(xivo_haslen($ext) === false
		|| preg_match('/^[a-z0-9_-]+$/',$ext) !== 1)
			return(false);
		else if(extension_loaded($ext) === true)
			return(null);
		else if(defined('XIVO_EXT_ENABLE') === false || XIVO_EXT_ENABLE === false)
			return(false);

		$file = XIVO_EXT_PRE.$ext.'.'.XIVO_EXT_SUF;

		$path = ini_get('extension_dir').XIVO_SEP_DIR.$file;

		if(xivo_file::is_f_r($path) === false)
			return(false);

		dl($file);

		return(extension_loaded($ext));
	}

	function load_class($class,$dir='',$file='',$lib=true)
	{
	       	$class = (string) $class;

		if(is_array($dir) === true)
			$dir = implode(XIVO_SEP_DIR,$dir);
		else
			$dir = (string) $dir;

		$file = (string) $file;

		if(class_exists($class) === true)
			return(true);

		$path = str_replace('::',XIVO_SEP_DIR,$class);
		$dir = str_replace('::',XIVO_SEP_DIR,$dir);

		if($file === '' && $dir === '' && $path !== $class)
		{
			$class = str_replace('::','_',$class);

			if(substr($path,0,5) === 'xivo'.XIVO_SEP_DIR)
				$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.dirname(substr($path,5));
			else
				$dir = dirname($path);

			$file = basename($path).'.inc';
		}
		else if($dir === '' && $path !== $class)
		{
			$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.dirname($path);
			$file .= '.inc';
		}
		else if(substr($class,0,5) === 'xivo_')
		{
			if((bool) $lib === true)
				$dir = rtrim(XIVO_PATH_LIB.XIVO_SEP_DIR.ltrim($dir,XIVO_SEP_DIR),XIVO_SEP_DIR.'.');
			else
				$dir = rtrim($dir,XIVO_SEP_DIR.'.');

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
		if(is_array($file) === true)
			$file = implode(XIVO_SEP_DIR,$file);

		if(($file = xivo_file::is_f_r($file)) === false)
			return(false);
		else if((bool) $once === true)
			require_once($file);
		else
			require($file);

		if(isset($array) === true && is_array($array) === true)
			$r = &$array;

		return($r);
	}
}

?>
