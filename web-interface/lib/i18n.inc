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

define('DWHO_I18N_NAMESPACE_GLOBAL','global');
define('DWHO_I18N_REGEXP_NAMESPACE','[a-z0-9:\-_]+');
define('DWHO_I18N_ARG_ESCAPE','`');
define('DWHO_I18N_ARG_SEPARATOR','|');

class dwho_i18n
{
	var $_file		= null;
	var $_dso		= null;
	var $_load		= array();
	var $_copy		= false;
	var $_loop		= array();
	var $_path_i18n		= '';
	var $_path_i18n_len	= 0;

	function dwho_i18n()
	{
		if(defined('DWHO_PATH_I18N') === false)
			trigger_error('DWHO i18n path not defined',E_USER_ERROR);

		$this->_path_i18n = rtrim(DWHO_PATH_I18N,DWHO_SEP_DIR);
		$this->_path_i18n_len = mb_strlen($this->_path_i18n);

		if(($dso = &dwho_gct::get('DWHO_DSO')) !== false)
		{
			$name = 'dwho_i18n_'.$dso->get_type();

			if(dwho::load_class($name,'i18n::datastorage',$dso->get_type()) === false)
				trigger_error('Failed to load i18n Datastorage',E_USER_ERROR);

			$this->_dso = new $name($dso);
		}
		else
			$this->_dso = false;

		if(isset($_SESSION['_i18n']) === false)
			$_SESSION['_i18n'] = array();
	}

	function get_language($locale='')
	{
		return(dwho_i18n::mediator('get','language',$locale));
	}

	function attrib_language()
	{
		return(dwho_i18n::mediator('attrib','language'));
	}

	function chk_language($lang)
	{
		return(dwho_i18n::mediator('chk','language',$lang));
	}

	function set_language($lang='')
	{
		return(dwho_i18n::mediator('set','language',$lang));
	}

	function get_language_info($lang='')
	{
		return(dwho_i18n::mediator('info','language',$lang));
	}

	function get_language_list()
	{
		return(dwho_i18n::mediator('list','language'));
	}

	function get_language_translated_list()
	{
		return(dwho_i18n::mediator('translated_list','language'));
	}

	function get_territory($locale='')
	{
		return(dwho_i18n::mediator('get','territory',$locale));
	}

	function attrib_territory()
	{
		return(dwho_i18n::mediator('attrib','territory'));
	}

	function chk_territory($territo)
	{
		return(dwho_i18n::mediator('chk','territory',$territo));
	}

	function set_territory($territo='')
	{
		return(dwho_i18n::mediator('set','territory',$territo));
	}

	function get_territory_info($territo='')
	{
		return(dwho_i18n::mediator('info','territory',$territo));
	}

	function get_territory_list()
	{
		return(dwho_i18n::mediator('list','territory'));
	}

	function get_territory_translated_list()
	{
		return(dwho_i18n::mediator('translated_list','territory'));
	}

	function attrib_locale()
	{
		return(dwho_i18n::mediator('attrib','locale'));
	}

	function chk_locale($locale)
	{
		return(dwho_i18n::mediator('chk','locale',$locale));
	}

	function set_locale($lang=false,$territo=false)
	{
		return(dwho_i18n::mediator('set','locale',array($lang,$territo)));
	}

	function get_locale_info($locale='')
	{
		return(dwho_i18n::mediator('info','locale',$locale));
	}

	function get_locale_list()
	{
		return(dwho_i18n::mediator('list','locale'));
	}

	function get_timezone($locale='')
	{
		return(dwho_i18n::mediator('get','timezone',$locale));
	}

	function attrib_timezone()
	{
		return(dwho_i18n::mediator('attrib','timezone'));
	}

	function chk_timezone($tz)
	{
		return(dwho_i18n::mediator('chk','timezone',$tz));
	}

	function set_timezone($tz='')
	{
		return(dwho_i18n::mediator('set','timezone',$tz));
	}

	function get_timezone_info($tz='')
	{
		return(dwho_i18n::mediator('info','timezone',$tz));
	}

	function get_timezone_list()
	{
		return(dwho_i18n::mediator('list','timezone'));
	}

	function set_babelfish_path()
	{
		return(dwho_i18n::mediator('set_babelfish_path'));
	}

	function vksprintf_l($format,$locale,$value)
	{
		if($locale === null && defined('DWHO_I18N_BABELFISH_REAL_LOCALE') === true)
			$locale = DWHO_I18N_BABELFISH_REAL_LOCALE;

		$loc = setlocale(LC_ALL,null);
		setlocale(LC_ALL,$locale);
		$r = dwho_vksprintf($format,$value);
		setlocale(LC_ALL,$loc);

		return($r);
	}

	function vsprintf_l($format,$locale,$value)
	{
		if($locale === null && defined('DWHO_I18N_BABELFISH_REAL_LOCALE') === true)
			$locale = DWHO_I18N_BABELFISH_REAL_LOCALE;

		$loc = setlocale(LC_ALL,null);
		setlocale(LC_ALL,$locale);
		$r = vsprintf(dwho_printf_escape($format,count($value)),$value);
		setlocale(LC_ALL,$loc);

		return($r);
	}

	function strftime_l($format,$locale,$timestamp=null)
	{
		if($locale === null && defined('DWHO_I18N_BABELFISH_REAL_LOCALE') === true)
			$locale = DWHO_I18N_BABELFISH_REAL_LOCALE;

		$loc = setlocale(LC_TIME,null);
		setlocale(LC_TIME,$locale);
		if($timestamp === null)
			$r = strftime($format);
		else
			$r = strftime($format,$timestamp);
		setlocale(LC_TIME,$loc);

		return($r);
	}

	function gm2localtime_l($format,$time,$locale)
	{
		if($locale === null && defined('DWHO_I18N_BABELFISH_REAL_LOCALE') === true)
			$locale = DWHO_I18N_BABELFISH_REAL_LOCALE;

		$loc = setlocale(LC_TIME,null);
		setlocale(LC_TIME,$locale);
		$r = dwho_gm2localtime($format,$time);
		setlocale(LC_TIME,$loc);

		return($r);
	}

	function mediator($func,$name='',$value='')
	{
		static $mediator, $territory, $language = null;

		if($mediator === null)
		{
			if(dwho::load_class('dwho::i18n::mediator') === false)
				trigger_error('Failed to load i18n mediator',E_USER_ERROR);

			$mediator = new dwho_i18n_mediator();
		}

		$r = false;

		$func = (string) $func;
		$name = (string) $name;

		if($func === 'set_babelfish_path')
			return($mediator->set_babelfish_path());

		if($mediator->chk($name) === false && $func !== 'attrib')
			return($r);

		switch($func)
		{
			case 'attrib':
				$r = $mediator->get_attrib($name);
				break;
			case 'get':
				switch($name)
				{
					case 'language':
						$r = $mediator->get_language($value);
						break;
					case 'territory':
						$r = $mediator->get_territory($value);
						break;
					case 'timezone':
						$r = $mediator->get_timezone($value);
						break;
				}
				break;
			case 'set':
				switch($name)
				{
					case 'language':
						$r = $mediator->set_language($value);
						break;
					case 'territory':
						$r = $mediator->set_territory($value);
						break;
					case 'locale':
						if(is_array($value) === false)
							$value = array(0 => false,1 => false);
						else
						{
							if(isset($value[0]) === false)
								$value[0] = false;

							if(isset($value[1]) === false)
								$value[1] = false;
						}

						$r = $mediator->set_locale($value[0],$value[1]);
						break;
					case 'timezone':
						$r = $mediator->set_timezone($value);
						break;
				}
				break;
			case 'chk':
				switch($name)
				{
					case 'language':
						$r = $mediator->chk_language($value);
						break;
					case 'territory':
						$r = $mediator->chk_territory($value);
						break;
					case 'locale':
						$r = $mediator->chk_locale($value);
						break;
					case 'timezone':
						$r = $mediator->chk_timezone($value);
						break;
				}
				break;
			case 'info':
				switch($name)
				{
					case 'language':
						$r = $mediator->get_language_info($value);
						break;
					case 'territory':
						$r = $mediator->get_territory_info($value);
						break;
					case 'locale':
						$r = $mediator->get_locale_info($value);
						break;
					case 'timezone':
						$r = $mediator->get_timezone_info($value);
						break;
				}
			case 'list':
				switch($name)
				{
					case 'language':
						$r = $mediator->get_language_list();
						break;
					case 'territory':
						$r = $mediator->get_territory_list();
						break;
					case 'locale':
						$r = $mediator->get_locale_list();
						break;
					case 'timezone':
						$r = $mediator->get_timezone_list();
						break;
				}
			case 'translated_list':
				switch($name)
				{
					case 'language':
						if($language !== null)
							return($language);

						$i18n = &dwho_gct::get('dwho_i18n');

						if($i18n->load_file('util'.DWHO_SEP_DIR.'language') === false
						|| ($list = $mediator->get_language_list()) === false
						|| is_array($list) === false
						|| empty($list) === true)
							return(($language = false));

						$language = array();

						reset($list);

						while(list($key) = each($list))
							$language[$key] = dwho_i18n::babelfish('language_'.$key);

						asort($language);
						$r = &$language;
						break;
					case 'territory':
						if($territory !== null)
							return($territory);

						$i18n = &dwho_gct::get('dwho_i18n');

						if($i18n->load_file('util'.DWHO_SEP_DIR.'territory') === false
						|| ($list = $mediator->get_territory_list()) === false
						|| is_array($list) === false
						|| empty($list) === true)
							return(($territory = false));

						$territory = array();

						reset($list);

						while(list($key) = each($list))
							$territory[$key] = dwho_i18n::babelfish('territory_'.$key);

						asort($territory);
						$r = &$territory;
						break;
				}
		}

		return($r);
	}

	function _load_file($file,$namespace='')
	{
		if($this->_get_file_info($file,$namespace) === false)
			return(false);
		else if($this->_get_cache_file() === true)
			return(true);
		else if(dwho_ak('filei18n',$this->_file,true) === false)
		{
			trigger_error('I18N file not found',E_USER_WARNING);
			return(false);
		}
		else if(isset($this->_loop[$this->_file['filei18n']]) === true)
		{
			trigger_error('I18N file already loaded: '.$this->_file['filei18n'],E_USER_WARNING);
			return(false);
		}

		$this->_loop[$this->_file['filei18n']] = 1;

		if(($cache = $this->_mk_cache_file()) !== false)
			return($this->_get_cache_file($cache));

		return(false);
	}

	function load_file($file,$namespace='')
	{
		$this->_copy = false;
		$this->_file = $this->_load = $this->_loop = array();

		return($this->_load_file($file,$namespace));
	}

	function _set_babelfish($namespace,$value)
	{
		return(dwho_i18n::bfish('set',$namespace,$value));
	}

	function get_babelfish($namespace,$name='')
	{
		return(dwho_i18n::bfish('get',$namespace,$name));
	}

	function babelfish($name,$value=array(),$namespace=DWHO_I18N_NAMESPACE_GLOBAL)
	{
		return(dwho_i18n::bfish('translate',$namespace,$value,$name));
	}

	function bfish_build_key($key,$args=null)
	{
		if(is_array($args) === true)
			return($key.'('.implode(DWHO_I18N_ARG_SEPARATOR,$args).')');

		return($key);
	}

	function bfish_argsvar_removed($args)
	{
		if(is_array($args) === false)
			return(false);

		reset($args);

		while(list($key) = each($args))
		{
			if(preg_match('/^\$[a-zA-Z][a-zA-Z0-9_]*$/',$key) === 1)
				unset($args[$key]);
		}

		return($args);
	}

	function bfish_build_args($args)
	{
		if(is_array($args) === false)
			return(false);

		$r = array();

		reset($args);

		while(list($key,$value) = each($args))
		{
			if(is_scalar($value) === false)
				continue;
			else if(preg_match('/^[a-zA-Z][a-zA-Z0-9_]*$/',$key) === 0)
			{
				if(substr($value,0,1) === '$')
					$value = '\\'.$value;

				$r[str_replace(DWHO_I18N_ARG_SEPARATOR,
					       DWHO_I18N_ARG_ESCAPE.DWHO_I18N_ARG_SEPARATOR,
					       $value)] = $value;
			}
			else
				$r['$'.$key] = $value;
		}

		return($r);
	}

	function bfish_parse_args($str)
	{
		if(dwho_is_sgraph($str) === false)
			return(false);

		$regexp = '/('.preg_quote(DWHO_I18N_ARG_ESCAPE).'*'.preg_quote(DWHO_I18N_ARG_SEPARATOR).')/';

		$rs = preg_split($regexp,$str,-1,PREG_SPLIT_DELIM_CAPTURE);

		if(($nb = count($rs)) > 127)
			return(false);

		$arr = array();
		$cur = '';

		for($i = 0,$j = 1;$i < $nb;$i += 2,$j += 2)
		{
			$cur .= $rs[$i];

			if($j >= $nb)
			{
				$arr[] = $cur;
				break;
			}

			$bquote = substr($rs[$j],0,-1);

			if((strlen($bquote) % 2) === 0)
			{
				$arr[] = $cur.$bquote;
				$cur = '';
			}
			else
				$cur .= substr($bquote,0,-1).DWHO_I18N_ARG_SEPARATOR;
		}

		return(implode(DWHO_I18N_ARG_SEPARATOR,$arr));
	}

	function bfish($func,$namespace,$value,$name='')
	{
		static $babelfish	= array();

		$func		= (string) $func;
		$namespace	= (string) $namespace;
		$name		= (string) $name;

		$r = null;

		if($namespace === '')
			return($r);

		switch($func)
		{
			case 'set':
				$value = (array) $value;

				if(isset($babelfish[$namespace]) === false)
					$babelfish[$namespace] = array();

				$babelfish[$namespace] = array_merge($babelfish[$namespace],$value);

				return(true);
			case 'get':
				$value = (string) $value;

				if(isset($babelfish[$namespace]) === false)
					return($r);

				if($value !== '' && isset($namespace[$babelfish][$value]) === true)
					$r = &$babelfish[$namespace][$value];
				else
					$r = &$babelfish[$namespace];

				return($r);
			case 'translate':
				if($name === ''
				|| (isset($babelfish[$namespace]) === false
				   && isset($babelfish[DWHO_I18N_NAMESPACE_GLOBAL]) === false) === true)
					return($name);

				$value = (array) $value;

				if(empty($value) === false)
				{
					$args = dwho_i18n::bfish_build_args($value);
					$name_args = dwho_i18n::bfish_build_key($name,$args);

					if(isset($babelfish[$namespace][$name_args]) === true)
						return(dwho_i18n::vksprintf_l(
								$babelfish[$namespace][$name_args],
								null,
								$value));

					$args_varkeys_arr = array_keys($args);
					$name_args_varkeys = dwho_i18n::bfish_build_key($name,$args_varkeys_arr);

					if(isset($babelfish[$namespace][$name_args_varkeys]) === true)
						return(dwho_i18n::vksprintf_l(
								$babelfish[$namespace][$name_args_varkeys],
								null,
								$value));
					else if(isset($babelfish[DWHO_I18N_NAMESPACE_GLOBAL][$name_args]) === true)
						return(dwho_i18n::vksprintf_l($babelfish[DWHO_I18N_NAMESPACE_GLOBAL][$name_args],
									     null,
									     $value));
					else if(isset($babelfish[DWHO_I18N_NAMESPACE_GLOBAL][$name_args_varkeys]) === true)
						return(dwho_i18n::vksprintf_l(
								$babelfish[DWHO_I18N_NAMESPACE_GLOBAL][$name_args_varkeys],
								null,
								$value));

					$argsvar_removed_arr = dwho_i18n::bfish_argsvar_removed($args);
					$name_args_removed = dwho_i18n::bfish_build_key($name,array_keys($argsvar_removed_arr));

					if(isset($babelfish[$namespace][$name_args_removed]) === true)
						return(dwho_i18n::vksprintf_l(
								$babelfish[$namespace][$name_args_removed],
								null,
								$argsvar_removed_arr));
					else if(isset($babelfish[DWHO_I18N_NAMESPACE_GLOBAL][$name_args_removed]) === true)
						return(dwho_i18n::vksprintf_l(
								$babelfish[DWHO_I18N_NAMESPACE_GLOBAL][$name_args_removed],
								null,
								$argsvar_removed_arr));
				}

				if(isset($babelfish[$namespace][$name]) === false)
				{
					if(isset($babelfish[DWHO_I18N_NAMESPACE_GLOBAL][$name]) === true)
						$namespace = DWHO_I18N_NAMESPACE_GLOBAL;
					else if(isset($name_args) === true)
						return($name_args);
					else
						return($name);
				}

				$r = $babelfish[$namespace][$name];

				if(empty($value) === false)
					$r = dwho_i18n::vksprintf_l($r,null,$value);

				return($r);
		}
	}

	function _chk_namespace($name)
	{
		if(preg_match('/^'.DWHO_I18N_REGEXP_NAMESPACE.'$/',$name) === 1)
			return($name);

		return('');
	}

	function _get_file_info($file,$namespace='')
	{
		$namespace = $this->_chk_namespace($namespace);

		if(dwho_has_len($file) === false)
			return(($this->_file = false));

		$dir = trim(dirname($file),'.'.DWHO_SEP_DIR);
		$file = basename($file);

		if(($pos = strrpos($file,'.')) !== false)
			$file = substr($file,0,$pos);

		$file .= '.i18n';

		$path = ltrim($dir.DWHO_SEP_DIR.$file,DWHO_SEP_DIR);

		if($namespace !== '' && $namespace !== DWHO_I18N_NAMESPACE_GLOBAL)
			$path = trim(str_replace('::',DWHO_SEP_DIR,$namespace).DWHO_SEP_DIR.$path,DWHO_SEP_DIR);
		else
			$namespace = DWHO_I18N_NAMESPACE_GLOBAL;

		$this->_file['namespace'] = $namespace;
		$this->_file['dirname'] = $dir;
		$this->_file['basename'] = $file;
		$this->_file['path'] = $path;
		$this->_file['filename'] = $dir.DWHO_SEP_DIR.$file;

		if(defined('DWHO_I18N_BABELFISH_PATH') === false
		&& $this->set_babelfish_path() === false)
		{
			trigger_error('Invalid I18N Babelfish path',E_USER_ERROR);
			return(false);
		}
		else if(isset($this->_file['locale']) === true
		&& ($lc = $this->chk_locale($this->_file['locale'])) !== DWHO_I18N_BABELFISH_LOCALE)
		{
			trigger_error('Invalid I18N locale',E_USER_ERROR);
			return(false);
		}

		$this->_file['locale'] = DWHO_I18N_BABELFISH_LOCALE;
		$this->_file['language'] = DWHO_I18N_BABELFISH_LANGUAGE;

		$filei18n = dwho_file::joinpath($this->_path_i18n,$this->_file['locale'],$this->_file['path']);

		if(($this->_file['filei18n'] = dwho_file::is_f_r($filei18n)) === false
		|| strncmp($this->_path_i18n,$this->_file['filei18n'],$this->_path_i18n_len) !== 0)
			return(($this->_file = false));

		$this->_file['filemtime'] = dwho_uint(filemtime($this->_file['filei18n']));

		return(true);
	}

	function _mk_cache_file()
	{
		if(is_array($this->_file) === false)
			return(false);

		$cache = array();
		$cache['dcreate']	= isset($this->_file['filectime']) === true ? $this->_file['filectime'] : 0;
		$cache['dupdate']	= $this->_file['filemtime'];
		$cache['locale']	= $this->_file['locale'];

		$cache['obj'] = array();
		$cache['obj']['content']	= $this->_parse_file();
		$cache['obj']['copy']		= $this->_copy;
		$cache['obj']['load']		= $this->_load;

		if($this->chk_locale($cache['locale']) === false
		|| $cache['obj']['content'] === false)
		{
			trigger_error('Wrong locale or unavailable file content',E_USER_WARNING);
			return(false);
		}

		$cache['language']	= $this->get_language($cache['locale']);
		$cache['path']		= $this->_file['path'];

		if(is_object($this->_dso) === true)
			return($this->_dso->save_cache($cache));

		$cache['filectime'] = $cache['dcreate'];

		return($cache);
	}

	function _get_cache_file($cache=null)
	{
		static $cached = array();

		if(is_array($this->_file) === false)
			return(false);

		$namespace = $this->_file['namespace'];

		if(isset($cached[$namespace]) === false)
			$cached[$namespace] = array();

		if(isset($cached[$namespace][$this->_file['locale']]) === false)
			$cached[$namespace][$this->_file['locale']] = array();

		$content = false;

		if(isset($cached[$namespace][$this->_file['locale']][$this->_file['path']]) === false)
		{
			if(is_array($cache) === false
			&& (is_object($this->_dso) === false
			   || ($cache = $this->_dso->get_cache($this->_file)) === false) === true)
				return(false);
			else if($cache['locale'] !== $this->_file['locale']
			|| $cache['path'] !== $this->_file['path']
			|| dwho_issa('obj',$cache) === false)
				return(false);

			$cached[$namespace][$this->_file['locale']][$this->_file['path']] = array(
							'finfo'	=> $this->_file,
							'copy'	=> $cache['obj']['copy'],
							'load'	=> $cache['obj']['load']);

			$this->_file['filectime'] = $cache['filectime'];

			$content = $cache['obj']['content'];
		}

		$ref = &$cached[$namespace][$this->_file['locale']][$this->_file['path']];

		if(dwho_issa('finfo',$ref) === false
		|| (int) $ref['finfo']['filemtime'] !== (int) $this->_file['filemtime'])
			return(false);

		if($ref['copy'] !== false)
		{
			$file = $this->_file;
			$this->_file = $ref['finfo'];
			$this->_file['locale'] = $ref['copy'];
			$this->_load_file($ref['finfo']['filename'],$ref['finfo']['namespace']);
			$this->_file = $file;
		}

		if(is_array($ref['load']) === true
		&& empty($ref['load']) === false)
		{
			$file = $this->_file;
			$this->_file = $ref['finfo'];

			reset($ref['load']);

			while(list($key) = each($ref['load']))
			{
				$exp = explode(';',$key,2);

				if(isset($exp[1]) === true)
					$this->_load_file($exp[1],$exp[0]);
				else
					$this->_load_file($key,$ref['finfo']['namespace']);
			}

			$this->_file = $file;
		}

		if(is_array($content) === true)
			$this->bfish('set',$namespace,$content);

		return(true);
	}

	function utf8_compliant($str)
	{
		if(is_scalar($str) === false)
			return(false);

		$str = (string) $str;

		if(isset($str{0}) === false)
			return(true);

		return(preg_match('/^.{1}/us',$str) === 1);
	}

	function _parse_file()
	{
		if(is_array($this->_file) === false)
			return(false);
		else if(($filei18n = dwho_file::is_f_r($this->_file['filei18n'])) === false)
		{
			trigger_error('I18N file not found',E_USER_WARNING);
			return(false);
		}

		$content = file($filei18n);
		$nb = count($content);
		$priority = 0;
		$txt = false;
		$quote_sep_dir = preg_quote(DWHO_SEP_DIR,'/');
		$key = '';

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$line = trim($content[$i]);

			if(isset($line{0}) === false)
			{
				if($key === ''
				|| $txt === false)
				// allow empty translations
				//|| isset($r[$key]) === false)
				{
					$txt = false;
					continue;
				}
				else
					$line = DWHO_EOL;
			}

			switch($line{0})
			{
				case '#':
					continue;
				case ';':
					if($key !== '' && isset($r[$key]) === true)
						$r[$key] = rtrim($r[$key],DWHO_EOL);

					if(preg_match('/^;[\t ]*copy[\t ]+'.
							'([a-z]{2,3}_[A-Z]{2})$/',$line,$matchcopy) === 1
					&& ($locale = $this->chk_locale($matchcopy[1])) !== false)
					{
						if($priority !== 0)
							continue;

						$priority |= 1;
						$txt = false;

						$file = $this->_file;

						$this->_file['locale'] = $locale;
						if($this->_load_file($file['filename'],$file['namespace']) !== false)
							$this->_copy = $locale;

						$this->_file = $file;
					}
					else if(preg_match('/^;[\t ]*load[\t ]+'.
							    '(?:\[('.DWHO_I18N_REGEXP_NAMESPACE.')\][\t ]*)?'.
							    '(\.{1,2}'.$quote_sep_dir.')*'.
							    '([a-z0-9-_'.$quote_sep_dir.']+)$/i',$line,$matchload) === 1)
					{
						if($priority > 3)
							continue;

						$txt = false;
						$priority |= 2;

						$file = $this->_file;
						$load = $this->_load;
						$copy = $this->_copy;

						if($matchload[1] === '')
							$namespace = $file['namespace'];
						else
							$namespace = $matchload[1];

						if($matchload[2] === '')
							$filename = $matchload[3];
						else
							$filename = $file['dirname'].DWHO_SEP_DIR.$matchload[2].$matchload[3];

						if($this->_load_file($filename,$namespace) !== false)
							$load[$namespace.';'.$this->_file['filename']] = 1;

						$this->_copy = $copy;
						$this->_load = $load;
						$this->_file = $file;
					}
					else if(preg_match('/^;[\t ]*([a-zA-Z0-9-_]+)(\(.*\))?$/',$line,$match) === 1)
					{
						$txt = true;
						$key = $match[1];

						if(isset($match[2]) === true)
						{
							if(($args = $this->bfish_parse_args(
								substr($match[2],1,-1))) === false)
							{
								$txt = false;
								continue;
							}

							$key .= '('.$args.')';
						}

						if(isset($r[$key]) === true)
							$txt = false;
					}
					else if(preg_match('/^;[\t ]*"([a-zA-Z0-9_ -]+)"(\(.*\))?$/',$line,$match) === 1
					&& strpos($match[1],'  ') === false
					&& $match[1]{0} !== ' ' && substr($match[1],-1,1) !== ' ')
					{
						$txt = true;
						$key = $match[1];

						if(isset($match[2]) === true)
						{
							if(($args = $this->bfish_parse_args(
								substr($match[2],1,-1))) === false)
							{
								$txt = false;
								continue;
							}

							$key .= '('.$args.')';
						}

						if(isset($r[$key]) === true)
							$txt = false;
					}
					break;
				default:
					if($this->utf8_compliant($line) === false
					|| $txt === false
					|| $key === '')
					{
						$txt = false;
						continue;
					}
					else if($line{0} === '\\'
					&& isset($line{1}) === true
					&& ($line{1} === '#'
					   || $line{1} === ';') === true)
						$line = substr($line,1);

					if(isset($r[$key]) === false)
					{
						$r[$key] = $line;
						$priority |= 4;
					}
					else if($line === DWHO_EOL)
					{
						$r[$key] .= $line;
						$priority |= 4;
					}
					else if(($line = rtrim($line,DWHO_EOL)) !== '')
					{
						$r[$key] .= DWHO_EOL.$line;
						$priority |= 4;
					}
					else
						$r[$key] = rtrim($r[$key],DWHO_EOL);
			}
		}

		if($key !== '' && isset($r[$key]) === true)
			$r[$key] = rtrim($r[$key],DWHO_EOL);

		return($r);
	}
}

?>
