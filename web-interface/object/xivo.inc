<?php

define('XIVO_PATH_LIB',XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'lib');
define('XIVO_PATH_LIBCONF',XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'libconf');
define('XIVO_PATH_OBJECT',XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'object');
define('XIVO_PATH_OBJECTCONF',XIVO_PATH_OBJECT.DIRECTORY_SEPARATOR.'objectconf');
define('XIVO_PATH_CONF',XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'conf');
define('XIVO_PATH_I18N',XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'i18n');
define('XIVO_PATH_DS',XIVO_PATH_ROOT.DIRECTORY_SEPARATOR.'datastorage');

require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'func.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'file.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gateclass.inc');
require_once(XIVO_PATH_LIB.DIRECTORY_SEPARATOR.'gatearray.inc');

class xivo
{
	var $_conf	= array();

	function xivo()
	{
		if(($this->_conf['xivo'] = xivo_gat::load_get('xivo',XIVO_PATH_OBJECTCONF)) === false)
			die('ERR: Failed to load XIVO Array');

		$this->_conf['xivo']['ini']['extension_loaded'] = 0;

		if(($this->_conf['extension'] = xivo_gat::load_get('extension',XIVO_PATH_LIBCONF)) === false)
			die('ERR: Failed to load extension Array');

		if(($this->_conf['datastorage'] = xivo_gat::load_get('datastorage',XIVO_PATH_LIBCONF)) === false)
			die('ERR: Failed to load datastorage Array');

		if(($this->_conf['user'] = xivo_gat::load_get('user',XIVO_PATH_OBJECTCONF)) === false)
			die('ERR: Failed to load user Array');

		if(function_exists('version_compare') === false
		|| version_compare(PHP_VERSION,$this->_conf['xivo']['sys']['php_version']['min'],'<') === true
		|| version_compare(PHP_VERSION,$this->_conf['xivo']['sys']['php_version']['max'],'>=') === true)
			die('ERR: Please update your PHP');

		$this->_cfg_general();
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
		if(($xivo_ini = xivo::load_init(XIVO_PATH_CONF.XIVO_SEP_DIR.'xivo.ini')) === false)
			die('ERR: XIVO ini file not found or unreadable');

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

		if(xivo_issa('extension',$xivo_ini) === true)
			$this->_cfg_ini_extension($xivo_ini['extension']);

		$this->_load_ini_extension();
		$this->_cfg_ini_set();

		if(xivo_issa('datastorage',$xivo_ini) === true)
			$this->_cfg_ini_datastorage($xivo_ini['datastorage']);
		else
			die('ERR: Missing datastorage configuration in XIVO ini file');

		if(xivo_issa('service',$xivo_ini) === true)
			$this->_cfg_ini_service($xivo_ini['service']);
		else
			die('ERR: Missing service configuration in XIVO ini file');

		if(xivo_issa('template',$xivo_ini) === true)
			$this->_cfg_ini_template($xivo_ini['template']);
	}

	function _set_error()
	{
		xivo::load_class('xivo_tracerror');
		$_ERR = &xivo_gct::set_get(new xivo_tracerror($this->_conf['xivo']['sys']['error']));
		set_error_handler(array(&$_ERR,'report_e_err'));
	}

	function _set_query()
	{
		xivo::load_class('xivo_query');
		$_QUERY = &xivo_gct::set_get(new xivo_query());
		xivo_gat::set_get('_QR',$_QUERY->raw);
	}

	function _set_session()
	{
		xivo::load_class('xivo_session');
		xivo_gct::set(new xivo_session());
	}

	function _set_i18n()
	{
		xivo::load_class('xivo_i18n');
		xivo_gct::set(new xivo_i18n());
	}

	function _set_service()
	{
		xivo::load_class('xivo_service',XIVO_PATH_OBJECT,null,false);
		$_SRE = &xivo_gct::set_get(new xivo_service($this->_conf['xivo']['sys']['service']));
	}

	function _set_user()
	{
		xivo::load_class('xivo_user',XIVO_PATH_OBJECT,null,false);
		new xivo_user();
	}

	function _cfg_general()
	{
		define('XIVO_SOFT_NAME',$this->_conf['xivo']['sys']['general']['soft_name']);
		define('XIVO_SOFT_LABEL',$this->_conf['xivo']['sys']['general']['soft_label']);
		define('XIVO_SOFT_VERSION',$this->_conf['xivo']['sys']['general']['soft_version']);
		define('XIVO_SOFT_URL',$this->_conf['xivo']['sys']['general']['soft_url']);
		define('XIVO_CORP_ADDRESS',$this->_conf['xivo']['sys']['general']['corp_address']);
		define('XIVO_CORP_ZIPCODE',$this->_conf['xivo']['sys']['general']['corp_zipcode']);
		define('XIVO_CORP_CITY',$this->_conf['xivo']['sys']['general']['corp_city']);
		define('XIVO_CORP_COUNTRY',$this->_conf['xivo']['sys']['general']['corp_country']);
		define('XIVO_CORP_PHONE',$this->_conf['xivo']['sys']['general']['corp_phone']);
		define('XIVO_CORP_FAX',$this->_conf['xivo']['sys']['general']['corp_fax']);
		define('XIVO_CORP_NAME',$this->_conf['xivo']['sys']['general']['corp_name']);
		define('XIVO_CORP_LABEL',$this->_conf['xivo']['sys']['general']['corp_label']);
		define('XIVO_CORP_EMAIL',$this->_conf['xivo']['sys']['general']['corp_email']);
		define('XIVO_CORP_URL',$this->_conf['xivo']['sys']['general']['corp_url']);
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
		$this->_conf['xivo']['ini']['extension_dir'] = ini_get('extension_dir');

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

		if($this->chk_extension_loaded(XIVO_EXT__MBSTRING) === true)
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

	function _cfg_ini_locale($a=null)
	{
		if(is_array($a) === false)
			$a = $this->_conf['xivo']['sys']['locale'];

		if(isset($a['language']) === false)
			$a['language'] = $this->_conf['xivo']['sys']['language'];

		define('XIVO_LC_LANGUAGE',(string) $a['language']);

		if(isset($a['territory']) === false)
			$a['territory'] = $this->_conf['xivo']['sys']['territory'];

		define('XIVO_LC_TERRITORY',(string) $a['territory']);

		if(isset($a['charset']) === false)
			$a['charset'] = $this->_conf['xivo']['sys']['charset'];

		define('XIVO_LC_CHARSET',(string) $a['charset']);

		define('XIVO_LC_LABEL',setlocale(LC_ALL,0));
	}

	function _cfg_ini_session($a=null)
	{
		if(is_array($a) === false)
			$a = $this->_conf['xivo']['sys']['session'];

		if(isset($a['name']) === false)
			$a['name'] = $this->_conf['xivo']['sys']['session']['name'];

		define('XIVO_SESS_NAME',(string) $a['name']);

		if(isset($a['time']) === false)
			$a['time'] = $this->_conf['xivo']['sys']['session']['time'];

		define('XIVO_SESS_TIME',xivo_uint($a['time']));

		if(isset($a['path']) === false)
			$a['path'] = $this->_conf['xivo']['sys']['session']['path'];

		define('XIVO_SESS_PATH',(string) $a['path']);
	}

	function _cfg_ini_error($a=null)
	{
		if(xivo_issa('error',$this->_conf['xivo']['sys']) === false)
			$this->_conf['xivo']['sys']['error'] = array();

		if(is_array($a) === false)
			return(null);

		if(isset($a['level']) === true)
			$this->_conf['xivo']['sys']['error']['level'] = (string) $a['level'];

		if(isset($a['report_type']) === true)
			$this->_conf['xivo']['sys']['error']['report_type'] = (string) $a['report_type'];

		if(isset($a['report_mode']) === true)
			$this->_conf['xivo']['sys']['error']['report_mode'] = (string) $a['report_mode'];

		if(isset($a['report_func']) === true)
			$this->_conf['xivo']['sys']['error']['report_func'] = (string) $a['report_func'];

		if(isset($a['email']) === true)
			$this->_conf['xivo']['sys']['error']['email'] = (string) $a['email'];

		if(isset($a['file']) === true)
			$this->_conf['xivo']['sys']['error']['file'] = (string) $a['file'];
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
	
		$this->_conf['xivo']['sys']['extension']['prefix'] = $prefix;
		$this->_conf['xivo']['sys']['extension']['suffix'] = $suffix;

		if($this->_conf['xivo']['ini']['safe_mode'] === true || $this->_conf['xivo']['ini']['enable_dl'] === false)
			$this->_conf['xivo']['sys']['extension']['enable'] = false;
		else
			$this->_conf['xivo']['sys']['extension']['enable'] = true;

		define('XIVO_EXT_ENABLE',$this->_conf['xivo']['sys']['extension']['enable']);
		define('XIVO_EXT_CONST',$this->_conf['xivo']['sys']['extension']['const']);
		define('XIVO_EXT_PRE',$this->_conf['xivo']['sys']['extension']['prefix']);
		define('XIVO_EXT_SUF',$this->_conf['xivo']['sys']['extension']['suffix']);
	}

	function _cfg_ini_datastorage($a)
	{
		if(xivo_issa('datastorage',$this->_conf['xivo']['sys']) === false)
			$this->_conf['xivo']['sys']['datastorage'] = array();

		if(is_array($a) === false)
			die('ERR: Invalid datastorage configuration in XIVO ini file');

		if(isset($a['type']) === false)
			die('ERR: Missing datastorage "type" configuration in XIVO ini file');

		xivo::load_class('xivo_datastorage');

		if(xivo_datastorage::is_valid($a['type']) === false)
			die('ERR: Invalid datastorage "type" configuration in XIVO ini file');

		if(xivo_datastorage::chk_extension_exists($a['type']) === true
		&& $this->chk_extension_loaded(xivo_datastorage::get_extension_name($a['type'])) === false)
			die('ERR: Datastorage '.$a['type'].' extension is not loaded');
		
		if(($dso = &xivo_gct::set_get(xivo_datastorage::factory($a['type'],$a),'XIVO_DSO')) === false)
			die('ERR: Datastorage class '.$a['type'].' does not exist');

		define('XIVO_DSO_TYPE',$dso->get_type());

		$this->_conf['xivo']['sys']['datastorage']['type'] = $a['type'];
		$this->_conf['xivo']['sys']['datastorage']['param'] = $a;
	}

	function _cfg_ini_service($a)
	{
		if(is_array($a) === false)
			die('ERR: Invalid service configuration in XIVO ini file');

		$this->_conf['xivo']['sys']['service'] = $a;
	}

	function _cfg_ini_extension($a)
	{
		$r = false;

		if(($arr = xivo_get_aks($a)) === false)
			return($r);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = $arr['keys'][$i];
			$v = (bool) $a[$k];

			if(($const = $this->chk_ini_extension($k)) === false)
				continue;

			if($this->_conf['extension'][$const]['optionnal'] !== 1)
				continue;

			if($v === true)
				$this->_conf['extension'][$const]['load'] = 1;
			else
				$this->_conf['extension'][$const]['load'] = 2;
		}
	}

	function _cfg_ini_template($a)
	{
		if(($arr = xivo_get_aks($a)) === false)
			die('ERR: Invalid template configuration in XIVO ini file');

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = $arr['keys'][$i];
			$v = &$a[$k];

			if(is_array($v) === false)
				die('ERR: Missing information in '.$k.' for template configuration in XIVO ini file');

			if(isset($v['script_path']) === true && ($script_path = xivo_file::is_d_r($v['script_path'])) !== false)
				$this->_conf['tpl'][$k]['script_path'] = $script_path;
			else
				die('ERR: Missing or invalid script_path in '.$k.' for template configuration in XIVO ini file');

			if(isset($v['application_path']) === true && ($application_path = xivo_file::is_d_r($v['application_path'])) !== false)
				$this->_conf['tpl'][$k]['application_path'] = $application_path;
			else
				die('ERR: Missing or invalid application_path in '.$k.' for template configuration in XIVO ini file');

			if(isset($v['tpl_path']) === true && ($tpl_path = xivo_file::is_d_r($v['tpl_path'])) !== false)
				$this->_conf['tpl'][$k]['tpl_path'] = $tpl_path;
			else
				die('ERR: Missing or invalid tpl_path in '.$k.' for template configuration in XIVO ini file');

			if(isset($v['script_root']) === true)
				$this->_conf['tpl'][$k]['script_root'] = rtrim((string) $v['script_root'],XIVO_SEP_DIR);
			else
				$this->_conf['tpl'][$k]['script_root'] = '';
		}
	}

	function _load_ini_extension()
	{
		if(($arr = xivo_get_aks($this->_conf['extension'])) === false)
		{
			define('XIVO_EXT_INI_LD',$this->_conf['xivo']['ini']['extension_loaded']);
			return(null);
		}

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$ref_ext = &$this->_conf['extension'][$k];

			if(extension_loaded($ref_ext['meta']) === true)
				$this->_conf['xivo']['ini']['extension_loaded'] |= $k;
			else if($ref_ext['load'] === 1 && XIVO_EXT_ENABLE === true)
			{
				$file = XIVO_EXT_PRE.$this->_conf['extension'][$k]['meta'].'.'.XIVO_EXT_SUF;
				$path = $this->_conf['xivo']['ini']['extension_dir'].XIVO_SEP_DIR.$file;

				if(xivo_file::is_f_r($path) === false)
					continue;

				dl($file);

				if(extension_loaded($this->_conf['extension'][$k]['meta']) === true)
					$this->_conf['xivo']['ini']['extension_loaded'] |= $k;
			}
		}

		define('XIVO_EXT_INI_LD',$this->_conf['xivo']['ini']['extension_loaded']);
	}

	function chk_ini_extension($meta)
	{
		$r = false;

		$meta = strtoupper((string) $meta);

		$name = XIVO_EXT_CONST.$meta;

		if(defined($name) === false)
			return($r);

		$const = constant($name);
		$extension = &xivo_gat::get('extension');

		if(xivo_issa($const,$extension) === true)
			$r = $const;

		return($r);
	}

	function chk_extension_loaded($meta)
	{
		$extension = &xivo_gat::get('extension');

		if(($ext = xivo_uint($meta)) !== 0)
		{
			if(xivo_issa($ext,$extension) === true)
				$name = $extension[$ext]['meta'];
			else
				return(false);
		}
		else if(($ext = xivo::chk_ini_extension($meta)) !== false)
			$name = $extension[$ext]['meta'];
		else
			$name = (string) $meta;

		return(extension_loaded($name));
	}

	function load_extension($ext)
	{
		if(is_string($ext) === true)
			$ext = (array) $ext;

		if(is_array($ext) === false)
			return(false);

		$nb = count($ext);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($ext[$i]) === false || preg_match('/^[a-z0-9\-_]+$/',$ext[$i]) !== 1)
				continue;

			$meta = $ext[$i];

			if(extension_loaded($meta) === false && XIVO_EXT_ENABLE === true)
				dl(XIVO_EXT_PRE.$meta.'.'.XIVO_EXT_SUF);
		}
	}

	function load_class($class,$dir='',$file='',$lib=true)
	{
	       	$class = (string) $class;

		if(is_array($dir) === true)
			$dir = implode(XIVO_SEP_DIR,$dir);
		else
			$dir = (string) $dir;

		$file = (string) $file;
		$lib = (bool) $lib;

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
			if($lib === true)
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
		$r = false;

		$file = (string) $file;
		$name = (string) $name;

		if(xivo_file::is_i($file) === false
		|| ($init = parse_ini_file($file,true)) === false
		|| ($arr = xivo_get_aks($init)) === false)
			return($r);

		if($name !== '' && isset($init[$name]) === false)
			return($r);

		$r = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];

			if($name !== '' && $name !== $k)
				continue;
			
			$v = &$init[$k];

			if(is_array($v) === false)
			{
				$r[$k] = $v;
				continue;
			}

			if(($a = xivo_get_aks($v)) === false)
				continue;

			for($j = 0;$j < $a['cnt'];$j++)
			{
				$ks = &$a['keys'][$j];
				$vs = &$v[$ks];

				if(($pos = strpos($ks,'.')) === false)
				{
					if(isset($r[$k]) === false)
						$r[$k] = array();

					$r[$k][$ks] = $vs;
					continue;
				}

				if($pos === 0 || ($key2 = substr($ks,$pos+1)) === '')
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
		$r = false;

		$file = (string) $file;
		$once = (bool) $once;

		if(xivo_file::is_i($file) === false)
			return($r);

		if($once === true)
			require_once($file);
		else
			require($file);

		if(isset($array) === true && is_array($array) === true)
			$r = &$array;

		return($r);
	}
}

?>
