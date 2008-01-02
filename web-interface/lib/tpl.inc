<?php

define('XIVO_TPL_FILE_EXT','php');

class xivo_tpl
{
	var $_file	= array();
	var $_var	= array();
	var $_url	= array();
	var $_option	= array('overload_i18n'	=> null);
	var $_module	= array();

	function xivo_tpl($param=array(),$modules=array(),$url=null)
	{
		$param = (array) $param;
		$modules = (array) $modules;
		$script_root = '';

		if(isset($param['script_path']) === false || ($script_path = xivo_file::is_d_r($param['script_path'])) === false)
			trigger_error('Missing or invalid parameter script_path in '.__CLASS__,E_USER_ERROR);

		if(isset($param['application_path']) === false || ($application_path = xivo_file::is_d_r($param['application_path'])) === false)
			trigger_error('Missing or invalid parameter application_path in '.__CLASS__,E_USER_ERROR);

		if(isset($param['tpl_path']) === false || ($tpl_path = xivo_file::is_d_r($param['tpl_path'])) === false)
			trigger_error('Missing or invalid parameter tpl_path in '.__CLASS__,E_USER_ERROR);

		if(isset($param['script_root']) === true)
			$script_root = rtrim((string) $param['script_root'],'/');

		$pos = strrpos($tpl_path,XIVO_SEP_DIR);

		if($pos !== 0 && $pos !== false)
			$tpl_dir = substr($tpl_path,$pos+1);
		else
			$tpl_dir = '';

		$this->set_option('script_path',$script_path);
		$this->set_option('application_path',$application_path);
		$this->set_option('tpl_path',$tpl_path);
		$this->set_option('tpl_dir',$tpl_dir);
		$this->set_option('script_root',$script_root);
		$this->set_option('i18n_namespace','tpl::'.$tpl_dir);

		if(is_array($url) === true)
			$this->_url = $url;

		if(($arr = xivo_get_aks($modules)) === false)
			return(null);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$class = &$modules[$k];

			if(is_object($class) === true)
				$this->set_module($class);
			else if(is_string($class) === true)
				$this->_load_module($class);
		}
	}

	function load_i18n_file($file,$namespace='')
	{
		$namespace = (string) $namespace;

		if($namespace === '')
			$namespace = $this->get_option('i18n_namespace');

		if(($i18n = &xivo_gct::get('xivo_i18n')) === false)
			return(false);

		return($i18n->load_file($file,$namespace));
	}

	function bbf($name,$arr=array(),$namespace='')
	{
		$namespace = (string) $namespace;

		if($namespace === '')
			$namespace = $this->get_option('i18n_namespace');

		$r = xivo_i18n::babelfish($name,$arr,$namespace);

		return($r);
	}

	function set_option($name,$value)
	{
		$name = (string) $name;

		if($name === '')
			return(false);

		$this->_option[$name] = $value;

		return(true);
	}

	function get_option($name)
	{
		$r = null;

		$name = (string) $name;

		if(isset($this->_option[$name]) === true)
			$r = $this->_option[$name];

		return($r);
	}

	function set_module($value,$name='')
	{
		$name = (string) $name;

		if(is_object($value) === false)
			return(false);

		if($name === '')
			$name = get_class($value);

		$this->_module[$name] = &$value;

		return(true);
	}

	function &get_module($name)
	{
		$r = false;

		$name = (string) $name;

		if($name !== '')
		{
			if(isset($this->_module[$name]) === false)
				$this->_load_module($name);

			if(isset($this->_module[$name]) === true)
				$r = &$this->_module[$name];
		}

		return(($ref = &$r));
	}

	function call_module($name,$func,$args=null)
	{
		$func = (string) $func;
		$name = (string) $name;

		if($name === '' || $this->_load_module($name) === false)
			return(false);

		if($args === null)
			$r = call_user_func(array(&$this->_module[$name],$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array(&$this->_module[$name],$func),$args);
		}

		return($r);
	}

	function valid_module($name)
	{
		$name = (string) $name;

		return(isset($this->_module[$name]));
	}

	function _load_class($module)
	{
		$module = (string) $module;

		$file = $module.'.inc';

		$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module';
		$class = 'xivo_tpl_'.$module;

		if(xivo_file::load_file($file,$dir,true) === false)
			return(false);

		if(class_exists($class) === true)
			$r = $class;
		else
			$r = false;

		return($r);
	}

	function _load_module($name)
	{
		$name = (string) $name;

		if($this->valid_module($name) === true)
			return(true);

		if(($class = $this->_load_class($name)) === false)
		{
			trigger_error('Module does not exist in tpl',E_USER_ERROR);
			return(false);
		}

		$this->set_module(new $class($this),$name);

		return(true);
	}

	function set_var($name,$value)
	{
		$name = (string) $name;

		if($name === '')
			return(false);

		$this->_var[$name] = &$value;

		return(true);
	}

	function set_varra(&$a)
	{
		$a = (array) $a;

		if(($arr = xivo_get_aks($a)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$this->set_var($k,$a[$k]);
		}
	}

	function get_var($name)
	{
		$r = null;

		$name = (string) $name;

		if(isset($this->_var[$name]) === true)
			$r = &$this->_var[$name];

		return($r);
	}

	function get_varra($name,$keys)
	{
		$name = (string) $name;

		if(is_scalar($keys) === true)
		{
			$keys = (string) $keys;

			if(xivo_issa($name,$this->_var) === false || isset($this->_var[$name][$keys]) === false)
				return(null);

			return($this->_var[$name][$keys]);
		}
		
		$keys = array_values((array) $keys);

		if(xivo_issa($name,$this->_var) === false || ($nb = count($keys)) === 0)
			return(null);

		$ref = &$this->_var[$name];

		for($i = 0;$i < $nb;$i++)
		{
			if(xivo_ak($keys[$i],$ref) === false)
				return(null);

			$ref = &$ref[$keys[$i]];
		}

		return($ref);
	}

	function set_file($name,$value)
	{
		$name = (string) $name;
		$value = (string) $value;

		if($name === '')
			return(false);

		if(isset($this->_file[$name]) === false)
		{
			$this->_file[$name] = $value;
			return(true);
		}

		if(is_array($this->_file[$name]) === true)
			$this->_file[$name][] = $value;
		else
			$this->_file[$name] = array($this->_file[$name],$value);
	
		return(true);
	}

	function get_file($name)
	{
		$name = (string) $name;
	
		if(isset($this->_file[$name]) === false)
			return(null);

		if(is_array($this->_file[$name]) === true)
			$r = end($this->_file[$name]);
		else
			$r = $this->_file[$name];

		return($r);
	}

	function get_prepend($ext='')
	{
		$r = false;

		$ext = (string) $ext;

		if($ext === '')
			$ext = XIVO_TPL_FILE_EXT;

		$prepend = $this->get_option('script_path').XIVO_SEP_DIR.'prepend.'.$ext;

		if(xivo_file::is_i($prepend) === true)
			$r = $prepend;

		return($r);
	}

	function get_append($ext='')
	{
		$r = false;

		$ext = (string) $ext;

		if($ext === '')
			$ext = XIVO_TPL_FILE_EXT;

		$append = $this->get_option('script_path').XIVO_SEP_DIR.'append.'.$ext;

		if(xivo_file::is_i($append) === true)
			$r = $append;

		return($r);
	}

	function set_bloc($name,$file,$ext='')
	{
		$name = (string) $name;
		$ext = (string) $ext;

		if($ext === '')
			$ext = XIVO_TPL_FILE_EXT;

		$file = 'bloc'.XIVO_SEP_DIR.ltrim((string) $file,XIVO_SEP_DIR).'.'.$ext;

		if($name === '' || xivo_file::is_f_r($this->get_option('tpl_path').XIVO_SEP_DIR.$file) === false)
			return(false);

		$this->set_file('bloc_'.$name,$file);

		if($this->get_option('overload_i18n') !== false)
			$this->load_i18n_file($this->get_file('bloc_'.$name));

		return(true);
	}

	function mk_bloc($name)
	{
		$name = (string) $name;

		if(($file = $this->get_file('bloc_'.$name)) !== null)
			$this->file_include($file,null,null);
	}

	function set_struct($file,$ext='')
	{
		$ext = (string) $ext;

		if($ext === '')
			$ext = XIVO_TPL_FILE_EXT;

		$file = 'struct'.XIVO_SEP_DIR.ltrim((string) $file,XIVO_SEP_DIR).'.'.$ext;

		if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.$file) === false)
			return(false);

		$this->set_file('structure',$file);

		if($this->get_option('overload_i18n') !== false)
			$this->load_i18n_file($this->get_file('structure'));

		return(true);
	}

	function mk_struct()
	{
		if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.$this->get_file('structure')) === false)
			return(false);

		if($this->get_option('overload_i18n') !== false)
			$this->load_i18n_file($this->get_file('structure'));

		include($this->get_option('tpl_path').XIVO_SEP_DIR.$this->get_file('structure'));
	}

	function display($page,$overload_i18n=false,$ext='')
	{
		$overload_i18n = (bool) $overload_i18n;
		$ext = (string) $ext;

		if(xivo_haslen($page) === false)
			return(false);

		if($ext === '')
			$ext = XIVO_TPL_FILE_EXT;

		if($page{0} === XIVO_SEP_DIR)
			$page .= '.'.$ext;
		else
			$page = XIVO_SEP_DIR.'struct'.XIVO_SEP_DIR.'page'.XIVO_SEP_DIR.$page.'.'.$ext;

		$path = $this->get_option('tpl_path').$page;

		if($overload_i18n === true)
			$this->set_option('overload_i18n',true);

		if(xivo_file::is_i($path) === false)
			return(false);
		
		$this->set_file('display',$path);

		$this->load_i18n_file($page);

		unset($page,$path);
	
		if($this->get_option('overload_i18n') === true)
		{
			ob_start();

			if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.'prepend.'.$ext) === true)
				include($this->get_option('tpl_path').XIVO_SEP_DIR.'prepend.'.$ext);

			include($this->get_file('display'));

			if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.'append.'.$ext) === true)
				include($this->get_option('tpl_path').XIVO_SEP_DIR.'append.'.$ext);

			ob_end_clean();

			$this->set_option('overload_i18n',false);
		}

		ob_start();
		
		if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.'prepend.'.$ext) === true)
			include($this->get_option('tpl_path').XIVO_SEP_DIR.'prepend.'.$ext);

		include($this->get_file('display'));

		if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.'append.'.$ext) === true)
			include($this->get_option('tpl_path').XIVO_SEP_DIR.'append.'.$ext);

		ob_end_flush();
	}

	function file_include($file,$arr=array(),$ext='')
	{
		$file = ltrim((string) $file,XIVO_SEP_DIR);

		if($ext === null)
			$ext = '';
		else if($ext === '')
			$ext = '.'.XIVO_TPL_FILE_EXT;
		else
			$ext = '.'.(string) $ext;

		$path = $this->get_option('tpl_path').XIVO_SEP_DIR.$file.$ext;

		if(xivo_file::is_i($path) === false)
			return(false);

		$this->set_file('tpl',$path);
		$this->set_varra($arr);

		if($this->get_option('overload_i18n') !== false)
			$this->load_i18n_file($file);

		unset($file,$arr,$path,$ext);

		include($this->get_file('tpl'));
	}

	function file_time($file)
	{
		$r = (string) $file;
		$file = ltrim($r,XIVO_SEP_DIR);

		if(($time = xivo_file::get_time($this->get_option('script_path').XIVO_SEP_DIR.$file)) !== false)
			$r .= '?'.$time;

		return($r);
	}

	function url($url='',$full=false)
	{
		$r = '';

		$url = (string) $url;
		$full = (bool) $full;
		
		if(isset($url{0}) === false)
			return($r);

		if($url{0} === '#')
			return($url);

		if(isset($this->_url[$url]) === true)
			$url = $this->_url[$url];

		$r = $url{0} !== '/' ? $this->get_option('script_root').'/'.$url : $url;

		if($full === true)
			$r = rtrim(XIVO_URL,'/').'/'.ltrim($url,'/');

		return($r);
	}

	function get_application($section,$nb=0,$path='',$ext='')
	{
		$section = (string) $section;
		$nb = (int) $nb;
		$path = (string) $path;
		$ext = (string) $ext;

		if($nb !== 0)
			$nb = xivo_uint($nb,20);

		if($ext === '')
			$ext = XIVO_TPL_FILE_EXT;

		$search = array('@/+@','@^/@','@/$@','@\.\.@');
		$replace = array(XIVO_SEP_DIR,'','','');

		$section = preg_replace($search,$replace,$section);

		if($section === '')
			return(false);

		if($path === '' && isset($_SERVER['PATH_INFO']) === true)
			$path = $_SERVER['PATH_INFO'];

		if($path !== '')
		{
			$path = preg_replace($search,$replace,$path);

			if($nb !== 0)
			{
				if(($path = explode(XIVO_SEP_DIR,$path)) === false)
					return(false);

				$path = implode(XIVO_SEP_DIR,array_slice($path,0,$nb));
			}
		}

		$this->set_option('api_path_info',$path);
		$this->set_option('api_section_info',$section);

		$dir = XIVO_SEP_DIR.trim($section.XIVO_SEP_DIR.$path,XIVO_SEP_DIR).'.'.$ext;

		$r = xivo_file::is_f_r($this->get_option('application_path').$dir);

		return($r);
	}
}

?>
