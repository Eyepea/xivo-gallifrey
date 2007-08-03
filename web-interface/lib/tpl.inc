<?php

class xivo_tpl
{
	var $_file	= array();
	var $_var	= array();
	var $_url	= array();
	var $_option	= array('overload_i18n'	=> null);
	var $_module	= array();

	function xivo_tpl($param=array(),$modules=array())
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

		if(($this->_url = xivo_gat::load_get('url',XIVO_PATH_CONF)) === false)
			trigger_error('Failed to load url Array',E_USER_ERROR);

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

		$i18n = &xivo_gct::get('xivo_i18n');

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
		$r = false;

		$name = (string) $name;

		if(is_object($value) === false)
			return($r);

		$r = true;

		if($name === '')
			$name = get_class($value);

		$this->_module[$name] = &$value;

		return($r);
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
		$r = false;

		$func = (string) $func;
		$name = (string) $name;

		if($name === '' || $this->_load_module($name) === false)
			return($r);

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
		$r = false;

		$module = (string) $module;

		$file = $module.'.inc';

		$dir = XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module';
		$class = 'xivo_tpl_'.$module;

		if(xivo_file::load_file($file,$dir,true) === false)
			return($r);

		if(class_exists($class) === true)
			$r = $class;

		return($r);
	}

	function _load_module($name)
	{
		$r = true;

		$name = (string) $name;

		if($this->valid_module($name) === false)
		{
			if(($class = $this->_load_class($name)) !== false)
				$this->set_module(new $class($this),$name);
			else
			{
				trigger_error('Module does not exist in tpl',E_USER_ERROR);
				$r = false;
			}
		}

		return($r);
	}

	function assign($name,$value)
	{
		$name = (string) $name;

		if($name === '')
			return(false);

		$this->_var[$name] = &$value;

		return(true);
	}

	function assign_array(&$a)
	{
		$a = (array) $a;

		if(($arr = xivo_get_aks($a)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$this->assign($k,$a[$k]);
		}
	}

	function varra($name,$keys)
	{
		$r = null;

		$name = (string) $name;

		if(is_scalar($keys) === true)
		{
			$keys = (string) $keys;

			if(xivo_issa($name,$this->_var) === false || isset($this->_var[$name][$keys]) === false)
				return($r);

			return($this->_var[$name][$keys]);
		}
		
		$keys = array_values((array) $keys);

		if(xivo_issa($name,$this->_var) === false || ($nb = count($keys)) === 0)
			return($r);

		$ref = &$this->_var[$name];

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($ref[$keys[$i]]) === false)
				return($r);

			$ref = &$ref[$keys[$i]];
		}

		return($ref);
	}

	function vars($name)
	{
		$r = null;

		$name = (string) $name;

		if(isset($this->_var[$name]) === true)
			$r = &$this->_var[$name];

		return($r);
	}

	function set_file($name,$value)
	{
		$name = (string) $name;
		$value = (string) $value;

		if($name === '')
			return(false);

		if(isset($this->_file[$name]) === true)
		{
			if(is_array($this->_file[$name]) === true)
				$this->_file[$name][] = $value;
			else
			{
				$pfile = $this->_file[$name];
				$this->_file[$name] = array($pfile,$value);
			}
		}
		else
			$this->_file[$name] = $value;
	
		return(true);
	}

	function get_file($name)
	{
		$r = null;

		$name = (string) $name;
	
		if(isset($this->_file[$name]) === false)
			return($r);

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
			$ext = 'php';

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
			$ext = 'php';

		$append = $this->get_option('script_path').XIVO_SEP_DIR.'append.'.$ext;

		if(xivo_file::is_i($append) === true)
			$r = $append;

		return($r);
	}

	function set_struct($file,$ext='')
	{
		$ext = (string) $ext;

		if($ext === '')
			$ext = 'php';

		$file = 'struct'.XIVO_SEP_DIR.ltrim((string) $file,XIVO_SEP_DIR).'.'.$ext;

		if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.$file) === true)
			$this->set_file('structure',$file);
	}

	function mk_struct()
	{
		if(xivo_file::is_i($this->get_option('tpl_path').XIVO_SEP_DIR.$this->get_file('structure')) === false)
			return(false);

		if($this->get_option('overload_i18n') !== false)
			$this->load_i18n_file($this->get_file('structure'));

		include($this->get_option('tpl_path').XIVO_SEP_DIR.$this->get_file('structure'));
	}

	function display($page,$overload_i18n=false,$sep_dir=false,$ext='')
	{
		$sep_dir = (bool) $sep_dir;
		$page = $sep_dir === false ? ltrim((string) $page,XIVO_SEP_DIR) : (string) $page;
		$overload_i18n = (bool) $overload_i18n;
		$ext = (string) $ext;

		if($ext === '')
			$ext = 'php';

		if($sep_dir === false)
			$page = XIVO_SEP_DIR.'struct'.XIVO_SEP_DIR.'page'.XIVO_SEP_DIR.$page.'.'.$ext;
		else
			$page .= '.'.$ext;

		$path = $this->get_option('tpl_path').$page;

		if($overload_i18n === true)
			$this->set_option('overload_i18n',true);

		if(xivo_file::is_i($path) === true)
		{
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
	}

	function file_include($file,$arr=array(),$ext='')
	{
		$file = ltrim((string) $file,XIVO_SEP_DIR);

		if($ext !== null)
		{
			if($ext === '')
				$ext = 'php';
			else
				$ext = (string) $ext;
		}

		$path = $this->get_option('tpl_path').XIVO_SEP_DIR.$file.($ext === null ? '' : '.'.$ext);

		if(xivo_file::is_i($path) === false)
			return(false);

		$this->set_file('tpl',$path);
		$this->assign_array($arr);

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

	function url($url='')
	{
		$r = '';

		$url = (string) $url;
		
		if(isset($url{0}) === false)
			return($r);

		if($url{0} === '#')
			return($url);

		if(isset($this->_url[$url]) === true)
			$url = $this->_url[$url];

		$r = $url{0} !== '/' ? $this->get_option('script_root').'/'.$url : $url;

		return($r);
	}

	function get_application($section,$nb=0,$path='',$ext='')
	{
		$r = false;

		$section = (string) $section;
		$nb = (int) $nb;
		$path = (string) $path;
		$ext = (string) $ext;

		if($nb !== 0)
			$nb = xivo_uint($nb,20);

		if($ext === '')
			$ext = 'php';

		$search = array('@/+@','@^/@','@/$@','@\.\.@');
		$replace = array(XIVO_SEP_DIR,'','','');

		$section = preg_replace($search,$replace,$section);

		if($section === '')
			return($r);

		if($path === '' && isset($_SERVER['PATH_INFO']) === true)
			$path = $_SERVER['PATH_INFO'];

		if($path !== '')
		{
			$path = preg_replace($search,$replace,$path);

			if($nb !== 0)
			{
				if(($path = explode(XIVO_SEP_DIR,$path)) === false)
					return($r);

				$path = implode(XIVO_SEP_DIR,array_slice($path,0,$nb));
			}
		}

		$this->set_option('api_path_info',$path);
		$this->set_option('api_section_info',$section);

		$dir = XIVO_SEP_DIR.trim($section.XIVO_SEP_DIR.$path,XIVO_SEP_DIR).'.'.$ext;

		$r = xivo_file::is_f_r($this->get_option('application_path').$dir);

		return($r);
	}

	function chk_policy($cat='',$func='',$section='')
	{
		$r = false;

		$section = (string) $section;
		$cat = $cat === true ? true : (string) $cat;
		$func = (string) $func;

		if($section === '')
		{
			$script_root = $this->get_option('script_root');

			if(($len = strlen($script_root)) === 0)
				$section = $_SERVER['SCRIPT_NAME'];
			else if(strncmp($script_root,$_SERVER['SCRIPT_NAME'],$len) === 0)
				$section = substr($_SERVER['SCRIPT_NAME'],$len);
			else
				return($r);

			if(($pos = strrpos($section,'/')) !== false)
				$section = substr($section,0,$pos);
		}

		if($cat === true)
		{
			if(isset($_SERVER['PATH_INFO']) === true)
				$section .= $_SERVER['PATH_INFO'];
		}
		else if($cat !== '')
		{
			$section .= '/'.$cat;

			if($func !== '')
				$section .= '/'.$func;
		}

		return(xivo_user::policy_chk($section));
	}
}

?>
