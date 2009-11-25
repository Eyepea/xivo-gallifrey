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

define('DWHO_TPL_FILE_EXT','php');

class dwho_tpl
{
	var $_file	= array();
	var $_var	= array();
	var $_url	= array();
	var $_api_misc	= array();
	var $_option	= array('preload_i18n'	=> null);
	var $_module	= array();
	var $_i18n	= null;

	function dwho_tpl($param=array(),$modules=array(),$url=null,$api_misc=null)
	{
		$param = (array) $param;
		$modules = (array) $modules;
		$script_root = '';

		if(isset($param['script_path']) === false
		|| ($script_path = dwho_file::is_d_r($param['script_path'])) === false)
			trigger_error('Missing or invalid parameter script_path in '.__CLASS__,E_USER_ERROR);

		if(isset($param['application_path']) === false
		|| ($application_path = dwho_file::is_d_r($param['application_path'])) === false)
			trigger_error('Missing or invalid parameter application_path in '.__CLASS__,E_USER_ERROR);

		if(isset($param['tpl_path']) === false
		|| ($tpl_path = dwho_file::is_d_r($param['tpl_path'])) === false)
			trigger_error('Missing or invalid parameter tpl_path in '.__CLASS__,E_USER_ERROR);

		if(isset($param['script_root']) === true)
			$script_root = rtrim($param['script_root'],'/');

		if(($pos = strrpos($tpl_path,DWHO_SEP_DIR)) !== 0 && $pos !== false)
			$tpl_dir = substr($tpl_path,$pos + 1);
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

		if(is_array($api_misc) === true)
			$this->_api_misc = $api_misc;

		if(empty($modules) === true)
			return(null);

		foreach($modules as $class)
		{
			if(is_object($class) === true)
				$this->set_module($class);
			else if(is_string($class) === true)
				$this->_load_module($class);
		}
	}

	function load_i18n_file($file,$namespace='')
	{
		if(dwho_has_len($namespace) === false)
			$namespace = $this->get_option('i18n_namespace');

		if($this->_i18n === null)
			$this->_i18n = &dwho_gct::get('dwho_i18n');

		if(is_object($this->_i18n) === false)
			return(false);

		return($this->_i18n->load_file($file,$namespace));
	}

	function bbf($name,$arr=array(),$namespace='')
	{
		if(dwho_has_len($namespace) === false)
			$namespace = $this->get_option('i18n_namespace');

		return(dwho_i18n::babelfish($name,$arr,$namespace));
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
		$name = (string) $name;

		if(isset($this->_option[$name]) === true)
			return($this->_option[$name]);

		return(null);
	}

	function get_file_extension($ext)
	{
		if($ext === null)
			$ext = '';
		else if(dwho_has_len($ext) === false)
			$ext = '.'.DWHO_TPL_FILE_EXT;
		else
			$ext = '.'.$ext;

		return($ext);
	}

	function set_module($value,$name='')
	{
		$name = (string) $name;

		if(is_object($value) === false)
			return(false);
		else if($name === '')
			$name = get_class($value);

		$this->_module[$name] = &$value;

		return(true);
	}

	function &get_module($name,$param=null)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_module($name,$param) === true)
			$r = &$this->_module[$name];

		return(($ref = &$r));
	}

	function _load_class($module)
	{
		$file = $module.'.inc';

		$dir = dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module');
		$class = 'dwho_tpl_'.$module;

		if(dwho_file::load_file($file,$dir,true) === false || class_exists($class) === false)
			return(false);

		return($class);
	}

	function _load_module($name,$param=null)
	{
		$name = (string) $name;

		if(isset($this->_module[$name]) === true)
		{
			if(is_array($param) === true)
				$this->_module[$name]->load_init($param);
			return(true);
		}
		else if(($class = $this->_load_class($name)) === false)
		{
			trigger_error('Module does not exist in tpl',E_USER_ERROR);
			return(false);
		}

		$this->set_module(new $class($this,$param),$name);

		return(true);
	}

	function set_var($name,$value)
	{
		if(dwho_has_len($name) === false)
			return(false);

		$this->_var[(string) $name] = &$value;

		return(true);
	}

	function set_vars(&$a)
	{
		$a = (array) $a;

		if(empty($a) === true)
			return(false);

		foreach($a as $k => $v)
			$this->set_var($k,$v);
	}

	function get_var()
	{
		if(($argc = func_num_args()) === 0)
			return(null);

		$name = (string) func_get_arg(0);

		if(isset($this->_var[$name]) === false)
			return(null);

		$ref = &$this->_var[$name]; 

		if($argc === 1)
			return($ref);
		else if(is_array($this->_var[$name]) === false)
			return(null);

		for($i = 1;$i < $argc;$i++)
		{
			$key = func_get_arg($i);

			if(dwho_ak($key,$ref) === false)
				return(null);

			$ref = &$ref[$key];
		}

		return($ref);
	}

	function get_var_default()
	{
		if(($argc = func_num_args()) < 2)
			return(false);

		$args = func_get_args();
		$default = $args[$argc - 1];
		unset($args[$argc - 1]);

		if(($r = call_user_func_array(array(&$this,'get_var'),$args)) !== null)
			return($r);

		return($default);
	}

	function set_file($name,$value,$preload_i18n=false)
	{
		$name = (string) $name;
		$value = (string) $value;

		if($name === '')
			return(false);
		else if(isset($this->_file[$name]) === false)
			$this->_file[$name] = $value;
		else if(is_array($this->_file[$name]) === true)
			$this->_file[$name][] = $value;
		else
			$this->_file[$name] = array($this->_file[$name],
						    $value);

		if((bool) $preload_i18n === true)
			$this->load_i18n_file($value);

		return(true);
	}

	function get_file($name)
	{
		$name = (string) $name;

		if(isset($this->_file[$name]) === false)
			return(null);
		else if(is_array($this->_file[$name]) === true)
			return(end($this->_file[$name]));

		return($this->_file[$name]);
	}

	function get_prepend($ext='')
	{
		return(dwho_file::is_f_r($this->get_option('script_path').
					 DWHO_SEP_DIR.
					 'prepend'.
					 $this->get_file_extension($ext)));
	}

	function get_append($ext='')
	{
		return(dwho_file::is_f_r($this->get_option('script_path').
					 DWHO_SEP_DIR.
					 'append'.
					 $this->get_file_extension($ext)));
	}

	function set_bloc($name,$file,$preload_i18n=false,$ext='')
	{
		$file = 'bloc'.
			DWHO_SEP_DIR.
			ltrim($file,DWHO_SEP_DIR).
			$this->get_file_extension($ext);

		if(dwho_has_len($name) === false
		|| dwho_file::is_f_r($this->get_option('tpl_path').DWHO_SEP_DIR.$file) === false)
			return(false);

		$this->set_file('bloc_'.$name,$file);

		if($this->get_option('preload_i18n') !== false || (bool) $preload_i18n === true)
			$this->load_i18n_file($file);

		return(true);
	}

	function mk_bloc($name)
	{
		if(($file = $this->get_file('bloc_'.$name)) !== null)
			$this->file_include($file,null,false,null);
	}

	function set_struct($file,$preload_i18n=false,$ext='')
	{
		$file = 'struct'.
			DWHO_SEP_DIR.
			ltrim($file,DWHO_SEP_DIR).
			$this->get_file_extension($ext);

		if(dwho_file::is_f_r($this->get_option('tpl_path').DWHO_SEP_DIR.$file) === false)
			return(false);

		$this->set_file('structure',$file);

		if($this->get_option('preload_i18n') !== false || (bool) $preload_i18n === true)
			$this->load_i18n_file($file);

		return(true);
	}

	function mk_struct()
	{
		if(dwho_file::is_f_r($this->get_option('tpl_path').
				     DWHO_SEP_DIR.
				     $this->get_file('structure')) === false)
			return(false);
		else if($this->get_option('preload_i18n') !== false)
			$this->load_i18n_file($this->get_file('structure'));

		include($this->get_option('tpl_path').DWHO_SEP_DIR.$this->get_file('structure'));
	}

	function display($page,$preload_i18n=false,$ext='')
	{
		if(dwho_has_len($page) === false)
			return(false);

		$ext = $this->get_file_extension($ext);

		if($page{0} === DWHO_SEP_DIR)
			$page .= $ext;
		else
			$page = dwho_file::joinpath(DWHO_SEP_DIR.'struct','page',$page.$ext);

		$path = $this->get_option('tpl_path').$page;

		if((bool) $preload_i18n === true)
			$this->set_option('preload_i18n',true);

		if(dwho_file::is_f_r($path) === false)
			return(false);

		$this->set_file('display',$path);

		$this->load_i18n_file($page);

		unset($page,$path);

		if($this->get_option('preload_i18n') === true)
		{
			ob_start();

			if(dwho_file::is_f_r($this->get_option('tpl_path').DWHO_SEP_DIR.'prepend'.$ext) !== false)
				include($this->get_option('tpl_path').DWHO_SEP_DIR.'prepend'.$ext);

			include($this->get_file('display'));

			if(dwho_file::is_f_r($this->get_option('tpl_path').DWHO_SEP_DIR.'append'.$ext) !== false)
				include($this->get_option('tpl_path').DWHO_SEP_DIR.'append'.$ext);

			ob_end_clean();

			$this->set_option('preload_i18n',false);
		}

		ob_start();

		if(dwho_file::is_f_r($this->get_option('tpl_path').DWHO_SEP_DIR.'prepend'.$ext) !== false)
			include($this->get_option('tpl_path').DWHO_SEP_DIR.'prepend'.$ext);

		include($this->get_file('display'));

		if(dwho_file::is_f_r($this->get_option('tpl_path').DWHO_SEP_DIR.'append'.$ext) !== false)
			include($this->get_option('tpl_path').DWHO_SEP_DIR.'append'.$ext);

		ob_end_flush();
	}

	function file_include($file,$arr=array(),$preload_i18n=false,$ext='')
	{
		$file = ltrim($file,DWHO_SEP_DIR);

		$path = $this->get_option('tpl_path').
			DWHO_SEP_DIR.
			$file.
			$this->get_file_extension($ext);

		if(($path = dwho_file::is_f_r($path)) === false)
			return(false);

		$this->set_file('tpl',$path);
		$this->set_vars($arr);

		if($this->get_option('preload_i18n') !== false || (bool) $preload_i18n === true)
			$this->load_i18n_file($file);

		unset($file,$arr,$path,$ext);

		include($this->get_file('tpl'));
	}

	function file_time($file)
	{
		if(($time = dwho_file::get_time($this->get_option('script_path').
						DWHO_SEP_DIR.
						ltrim($file,DWHO_SEP_DIR))) !== false)
			$file .= '?'.$time;

		return($file);
	}

	function url($url='',$absolute=false,$internal=true)
	{
		$r = '';

		$url = (string) $url;

		if(isset($url{0}) === false)
			return($r);
		else if($url{0} === '#' || (bool) $internal === false)
			return($url);
		else if(isset($this->_url[$url]) === true)
			$url = $this->_url[$url];

		$r = $url{0} !== '/' ? $this->get_option('script_root').'/'.$url : $url;

		if((bool) $absolute === true)
			$r = rtrim(DWHO_URL,'/').'/'.ltrim($url,'/');

		return($r);
	}

	function get_app_path($section,$nb=0,$path='',$ext='')
	{
		$nb = dwho_uint($nb,20);
		$path = (string) $path;

		$search = array('@/+@','@^/@','@/$@','@\.\.@');
		$replace = array(DWHO_SEP_DIR,'','','');

		$section = preg_replace($search,$replace,$section);

		if($section === '')
			return(false);
		else if($path === '' && isset($_SERVER['PATH_INFO']) === true)
			$path = $_SERVER['PATH_INFO'];

		if($path !== '')
		{
			$pathcleaned = preg_replace($search,$replace,$path);

			if(($exp = explode(DWHO_SEP_DIR,$pathcleaned,$nb + 1)) === false)
				return(false);

			$path = '';

			$cnt = count($exp);

			for($i = 0;$i < $cnt && $i < $nb;$i++)
			{
				if(isset($exp[$i]{0}) === false || $exp[$i]{0} === '_' || $exp[$i]{0} === '.')
					return(false);

				$path .= DWHO_SEP_DIR.$exp[$i];
			}

			$path = substr($path,1);
		}

		$this->set_option('api_path_info',$path);
		$this->set_option('api_section_info',$section);

		$filepath = trim($section.DWHO_SEP_DIR.$path,DWHO_SEP_DIR);
		$newpath = $this->_apply_api_misc($filepath);

		if(is_string($newpath) === true)
			$filepath = $newpath;

		return(dwho_file::is_f_r($this->get_option('application_path').
					 DWHO_SEP_DIR.
					 $filepath.
					 $this->get_file_extension($ext)));
	}

	function _apply_api_misc($api_path)
	{
		if(dwho_issa($api_path,$this->_api_misc) === false)
			return(false);

		$list = $this->_api_misc[$api_path];

		if(dwho_issa('define',$list) === true)
		{
			foreach($list['define'] as $key => $value)
			{
				if(defined($key) === false && is_scalar($value) === true)
					define($key,$value);
			}
		}

		if(isset($list['path'],$list['path']{0}) === true)
			return($list['path']);

		return(true);
	}
}

?>
