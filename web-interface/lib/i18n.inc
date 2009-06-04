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

class xivo_i18n
{
	var $_file	= null;
	var $_dso	= null;
	var $_load	= array();
	var $_copy	= false;
	var $_loop	= array();

	function xivo_i18n()
	{
		if(defined('XIVO_PATH_I18N') === false)
			define('XIVO_PATH_I18N',XIVO_PATH_ROOT.XIVO_SEP_DIR.'i18n');

		$dso = &xivo_gct::get('XIVO_DSO');
		$name = 'xivo_i18n_'.$dso->get_type();

		if(xivo::load_class($name,'i18n::datastorage',$dso->get_type()) === false)
			trigger_error('Failed to load i18n Datastorage',E_USER_ERROR);

		$this->_dso = new $name($dso);

		if(isset($_SESSION['_i18n']) === false)
			$_SESSION['_i18n'] = array('babelfish' => array());
	}

	function get_language($locale='')
	{
		return(xivo_i18n::mediator('get','language',$locale));
	}

	function attrib_language()
	{
		return(xivo_i18n::mediator('attrib','language'));
	}

	function chk_language($lang)
	{
		return(xivo_i18n::mediator('chk','language',$lang));
	}

	function set_language($lang='')
	{
		return(xivo_i18n::mediator('set','language',$lang));
	}

	function get_language_info($lang='')
	{
		return(xivo_i18n::mediator('info','language',$lang));
	}

	function get_language_list()
	{
		return(xivo_i18n::mediator('list','language'));
	}

	function get_language_translated_list()
	{
		return(xivo_i18n::mediator('translated_list','language'));
	}

	function get_territory($locale='')
	{
		return(xivo_i18n::mediator('get','territory',$locale));
	}

	function attrib_territory()
	{
		return(xivo_i18n::mediator('attrib','territory'));
	}

	function chk_territory($territo)
	{
		return(xivo_i18n::mediator('chk','territory',$territo));
	}

	function set_territory($territo='')
	{
		return(xivo_i18n::mediator('set','territory',$territo));
	}

	function get_territory_info($territo='')
	{
		return(xivo_i18n::mediator('info','territory',$territo));
	}

	function get_territory_list()
	{
		return(xivo_i18n::mediator('list','territory'));
	}

	function get_territory_translated_list()
	{
		return(xivo_i18n::mediator('translated_list','territory'));
	}

	function attrib_locale()
	{
		return(xivo_i18n::mediator('attrib','locale'));
	}

	function chk_locale($locale)
	{
		return(xivo_i18n::mediator('chk','locale',$locale));
	}

	function set_locale($lang=false,$territo=false)
	{
		return(xivo_i18n::mediator('set','locale',array($lang,$territo)));
	}

	function get_locale_info($locale='')
	{
		return(xivo_i18n::mediator('info','locale',$locale));
	}

	function get_locale_list()
	{
		return(xivo_i18n::mediator('list','locale'));
	}

	function get_timezone($locale='')
	{
		return(xivo_i18n::mediator('get','timezone',$locale));
	}

	function attrib_timezone()
	{
		return(xivo_i18n::mediator('attrib','timezone'));
	}

	function chk_timezone($tz)
	{
		return(xivo_i18n::mediator('chk','timezone',$tz));
	}

	function set_timezone($tz='')
	{
		return(xivo_i18n::mediator('set','timezone',$tz));
	}

	function get_timezone_info($tz='')
	{
		return(xivo_i18n::mediator('info','timezone',$tz));
	}

	function get_timezone_list()
	{
		return(xivo_i18n::mediator('list','timezone'));
	}

	function set_babelfish_path()
	{
		return(xivo_i18n::mediator('set_babelfish_path'));
	}

	function vsprintf_l($format,$locale,$value)
	{
		if($locale === null && defined('XIVO_I18N_BABELFISH_REAL_LOCALE') === true)
			$locale = XIVO_I18N_BABELFISH_REAL_LOCALE;

		$loc = setlocale(LC_ALL,null);
		setlocale(LC_ALL,$locale);
		$r = vsprintf(xivo_printf_escape($format,count($value)),$value);
		setlocale(LC_ALL,$loc);

		return($r);
	}

	function strftime_l($format,$locale,$timestamp=null)
	{
		if($locale === null && defined('XIVO_I18N_BABELFISH_REAL_LOCALE') === true)
			$locale = XIVO_I18N_BABELFISH_REAL_LOCALE;

		$loc = setlocale(LC_TIME,null);
		setlocale(LC_TIME,$locale);
		if($timestamp === null)
			$r = strftime($format);
		else
			$r = strftime($format,$timestamp);
		setlocale(LC_TIME,$loc);

		return($r);
	}

	function gmlcstrftime_l($format,$time,$locale)
	{
		if($locale === null && defined('XIVO_I18N_BABELFISH_REAL_LOCALE') === true)
			$locale = XIVO_I18N_BABELFISH_REAL_LOCALE;

		$loc = setlocale(LC_TIME,null);
		setlocale(LC_TIME,$locale);
		$r = xivo_gmlcstrftime($format,$time);
		setlocale(LC_TIME,$loc);

		return($r);
	}

	function mediator($func,$name='',$value='')
	{
		static $mediator, $territory, $language = null;

		if($mediator === null)
		{
			if(xivo::load_class('xivo::i18n::mediator') === false)
				trigger_error('Failed to load i18n mediator',E_USER_ERROR);

			$mediator = new xivo_i18n_mediator();
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

						$i18n = &xivo_gct::get('xivo_i18n');

						if($i18n->load_file('util'.XIVO_SEP_DIR.'language') === false
						|| ($list = $mediator->get_language_list()) === false
						|| is_array($list) === false
						|| empty($list) === true)
							return(($language = false));

						$language = array();

						reset($list);

						while(list($key) = each($list))
							$language[$key] = xivo_i18n::babelfish('language_'.$key);

						asort($language);
						$r = &$language;
						break;
					case 'territory':
						if($territory !== null)
							return($territory);

						$i18n = &xivo_gct::get('xivo_i18n');

						if($i18n->load_file('util'.XIVO_SEP_DIR.'territory') === false
						|| ($list = $mediator->get_territory_list()) === false
						|| is_array($list) === false
						|| empty($list) === true)
							return(($territory = false));

						$territory = array();

						reset($list);

						while(list($key) = each($list))
							$territory[$key] = xivo_i18n::babelfish('territory_'.$key);

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
		else if(xivo_ak('filei18n',$this->_file,true) === false)
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

		if($this->_mk_cache_file() !== false)
			return($this->_get_cache_file());

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
		return(xivo_i18n::bfish('set',$namespace,$value));
	}

	function get_babelfish($namespace,$name='')
	{
		return(xivo_i18n::bfish('get',$namespace,$name));
	}

	function babelfish($name,$value=array(),$namespace='_empty_')
	{
		return(xivo_i18n::bfish('translate',$namespace,$value,$name));
	}

	function bfish($func,$namespace,$value,$name='')
	{
		static $babelfish = array();

		$func = (string) $func;
		$namespace = (string) $namespace;
		$name = (string) $name;

		$r = null;

		if($namespace === '')
			return($r);

		switch($func)
		{
			case 'set':
				$value = (array) $value;

				if(xivo_issa($namespace,$babelfish) === true)
					$babelfish[$namespace] = array_merge($babelfish[$namespace],$value);
				else
					$babelfish[$namespace] = $value;

				$r = true;
				break;
			case 'get':
				$value = (string) $value;

				if(xivo_issa($namespace,$babelfish) === false)
					return($r);

				if($value !== '' && isset($namespace[$babelfish][$value]) === true)
					$r = &$babelfish[$namespace][$value];
				else
					$r = &$babelfish[$namespace];
				break;
			case 'translate':
				if($name === ''
				|| (xivo_issa($namespace,$babelfish) === false
				   && xivo_issa('_empty_',$babelfish) === false) === true)
					return($name);

				$value = (array) $value;

				if(empty($value) === false)
				{
					$value = array_map('strval',array_filter($value,'is_scalar'));
					$name_args = $name.'('.implode('|',$value).')';

					if(isset($babelfish[$namespace][$name_args]) === true)
						return(xivo_i18n::vsprintf_l($babelfish[$namespace][$name_args],
									     null,
									     $value));
					else if(isset($babelfish['_empty_'][$name_args]) === true)
						return(xivo_i18n::vsprintf_l($babelfish['_empty_'][$name_args],
									     null,
									     $value));
				}

				if(isset($babelfish[$namespace][$name]) === false)
				{
					if(isset($babelfish['_empty_'][$name]) === true)
						$namespace = '_empty_';
					else
						return($name);
				}

				$r = (string) $babelfish[$namespace][$name];

				if(empty($value) === false)
					$r = xivo_i18n::vsprintf_l($r,null,$value);
				break;
		}

		return($r);
	}

	function _chk_namespace($name)
	{
		$r = '';

		$name = (string) $name;

		if($name === '' || preg_match('/^[a-z0-9:\-_]+$/',$name) !== 1)
			return($r);
		else
			return($name);
	}

	function _get_file_info($file,$namespace='')
	{
		$file = (string) $file;

		$namespace = $this->_chk_namespace($namespace);

		if(empty($file) === true)
		{
			$this->_file = false;
			return(false);
		}
		// TODO: trigger_error();

		$dir = trim(dirname($file),'.'.XIVO_SEP_DIR);
		$file = basename($file);

		if(($pos = strrpos($file,'.')) !== false)
			$file = substr($file,0,$pos);

		$file .= '.i18n';

		$path = ltrim($dir.XIVO_SEP_DIR.$file,XIVO_SEP_DIR);

		if($namespace !== '' && $namespace !== '_empty_')
			$path = trim(str_replace('::',XIVO_SEP_DIR,$namespace).XIVO_SEP_DIR.$path,XIVO_SEP_DIR);
		else
			$namespace = '_empty_';

		$this->_file['namespace'] = $namespace;
		$this->_file['dirname'] = $dir;
		$this->_file['basename'] = $file;
		$this->_file['path'] = $path;
		$this->_file['filename'] = $dir.XIVO_SEP_DIR.$file;

		if(defined('XIVO_I18N_BABELFISH_PATH') === false && $this->set_babelfish_path() === false)
		{
			trigger_error('Invalid I18N Babelfish path',E_USER_ERROR);
			return(false);
		}

		if(isset($this->_file['locale']) === true && ($lc = $this->chk_locale($this->_file['locale'])) !== false)
		{
			if($lc !== XIVO_I18N_BABELFISH_LOCALE)
			{
				trigger_error('Invalid I18N locale',E_USER_ERROR);
				return(false);
			}

			$this->_file['locale'] = XIVO_I18N_BABELFISH_LOCALE;
			$this->_file['language'] = XIVO_I18N_BABELFISH_LANGUAGE;
		}
		else
		{
			$this->_file['locale'] = XIVO_I18N_BABELFISH_LOCALE;
			$this->_file['language'] = XIVO_I18N_BABELFISH_LANGUAGE;
		}

		$filei18n = xivo_file::joinpath(XIVO_PATH_I18N,$this->_file['locale'],$this->_file['path']);

		if(($this->_file['filei18n'] = xivo_file::is_f_r($filei18n)) === false)
			return(($this->_file = false));

		$this->_file['filemtime'] = xivo_uint(filemtime($this->_file['filei18n']));

		return(true);
	}

	function _mk_cache_file()
	{
		if(is_array($this->_file) === false)
			return(false);

		$cache = array('obj' => array());

		$cache['dcreate'] = isset($this->_file['filectime']) === true ? $this->_file['filectime'] : 0;
		$cache['dupdate'] = $this->_file['filemtime'];

		$cache['locale'] = $this->chk_locale($this->_file['locale']);

		$cache['obj']['content'] = $this->_parse_file();
		$cache['obj']['copy'] = $this->_copy;
		$cache['obj']['load'] = $this->_load;

		if($cache['locale'] === false || $cache['obj']['content'] === false)
		{
			trigger_error('Wrong locale or unavaivable file content',E_USER_WARNING);
			return(false);
		}

		$cache['language'] = $this->get_language($cache['locale']);
		$cache['path'] = $this->_file['path'];

		return($this->_dso->save_cache($cache));
	}

	function _get_cache_file()
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
			if(($cache = $this->_dso->get_cache($this->_file)) === false
			|| xivo_issa('obj',$cache) === false)
				return(false);

			$cached[$namespace][$this->_file['locale']][$this->_file['path']] = array(
							'finfo'	=> $this->_file,
							'copy'	=> $cache['obj']['copy'],
							'load'	=> $cache['obj']['load']);

			$this->_file['filectime'] = $cache['filectime'];

			$content = $cache['obj']['content'];
		}

		$ref = &$cached[$namespace][$this->_file['locale']][$this->_file['path']];

		if(xivo_issa('finfo',$ref) === false
		|| (int) $ref['finfo']['filemtime'] !== (int) $this->_file['filemtime'])
			return(false);

		if($ref['copy'] !== false)
		{
			$_file = $this->_file;
			$this->_file = $ref['finfo'];
			$this->_file['locale'] = $ref['copy'];
			$this->_load_file($ref['finfo']['filename'],$ref['finfo']['namespace']);
			$this->_file = $_file;
		}

		if(is_array($ref['load']) === true
		&& empty($ref['load']) === false)
		{
			$_file = $this->_file;
			$this->_file = $ref['finfo'];

			reset($ref['load']);

			while(list($key) = each($ref['load']))
				$this->_load_file($key,$ref['finfo']['namespace']);

			$this->_file = $_file;
		}

		if($content !== false)
			$this->_set_babelfish($namespace,$content);

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
		else if(($file = xivo_file::is_f_r($this->_file['filei18n'])) === false)
		{
			trigger_error('I18N file not found',E_USER_WARNING);
			return(false);
		}

		$content = file($file);
		$nb = count($content);
		$priority = 0;
		$txt = $copy = $load = $data = false;
		$quote_sep_dir = preg_quote(XIVO_SEP_DIR);
		$key = '';

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$line = trim($content[$i]);

			if(isset($line{0}) === false)
			{
				$txt = false;
				continue;
			}

			switch($line{0})
			{
				case '#':
					$txt = false;
					break;
				case ';':
					if(preg_match('/^;[\t ]*copy[\t ]+([a-z]{2,3}_[A-Z]{2})$/',$line,$matchcopy) === 1
					&& ($locale = $this->chk_locale($matchcopy[1])) !== false)
					{
						if($priority !== 0)
							continue;

						$priority |= 1;
						$txt = false;
						$_file = $this->_file;

						$this->_file['locale'] = $locale;
						if($this->_load_file($this->_file['filename'],$this->_file['namespace']) !== false);
							$this->_copy = $locale;

						$this->_file = $_file;
					}
					else if(preg_match('#^;[\t ]*load[\t ]+'.
							   '(\.{1,2}'.$quote_sep_dir.')?'.
							   '([a-z0-9-_'.$quote_sep_dir.']+)$#i',$line,$matchload) === 1)
					{
						if($priority > 3)
							continue;

						$txt = false;
						$priority |= 2;

						$_file = $this->_file;
						$_load = $this->_load;
						$_copy = $this->_copy;

						$filename = $matchload[2];

						if($matchload[1] !== '')
							$filename = $this->_file['dirname'].XIVO_SEP_DIR.$matchload[1].$filename;

						if($this->_load_file($filename,$this->_file['namespace']) !== false)
							$_load[$this->_file['filename']] = 1;

						$this->_copy = $_copy;
						$this->_load = $_load;
						$this->_file = $_file;
					}
					else if(preg_match('/^;[\t ]*([a-zA-Z0-9-_]+)(\(.*\))?$/',$line,$match) === 1)
					{
						$txt = true;
						$key = $match[1];

						if(isset($match[2]) === true)
						{
							if(xivo_is_sgraph($match[2]) === true)
								$key .= $match[2];
							else
							{
								$txt = false;
								continue;
							}
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
							if(xivo_is_sgraph($match[2]) === true)
								$key .= $match[2];
							else
							{
								$txt = false;
								continue;
							}
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
					else if(strlen(($line = trim($line))) > 0)
					{
						$r[$key] .= XIVO_EOL.$line;
						$priority |= 4;
					}
			}
		}

		return($r);
	}
}

?>
