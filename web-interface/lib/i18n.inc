<?php

class xivo_i18n
{
	var $_file	= null;
	var $_dso	= null;
	var $_load	= array();
	var $_copy	= false;
	var $_loop	= array();

	function xivo_i18n()
	{
		$this->set_locale(XIVO_LC_LANGUAGE,XIVO_LC_TERRITORY);

		$dso = &xivo_gct::get('XIVO_DSO');
		$name = 'xivo_i18n_'.$dso->get_type();

		if(xivo::load_class($name,'i18n::datastorage',$dso->get_type()) === false)
			trigger_error('Failed to load i18n Datastorage',E_USER_ERROR);

		$this->_dso = new $name($dso);

		if(isset($_SESSION['_i18n']) === false)
			$_SESSION['_i18n'] = array('babelfish' => array());

		$this->load_file('xivo');
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

	function mediator($func,$name,$value='')
	{
		static $mediator = null;
		static $territory = null;

		if($mediator === null)
		{
			if(xivo::load_class('xivo::i18n::mediator') === false)
				trigger_error('Failed to load i18n mediator',E_USER_ERROR);

			$mediator = new xivo_i18n_mediator();
		}

		$r = false;

		$func = (string) $func;
		$name = (string) $name;

		if($mediator->chk($name) === false)
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
					case 'territory':
						if($territory !== null)
							return($territory);

						$i18n = &xivo_gct::get('xivo_i18n');

						if($i18n->load_file('util/territory') === false
						|| ($list = $mediator->get_territory_list()) === false
						|| ($arr = xivo_get_aks($list)) === false)
							return(($territory = false));

						$territory = array();

						for($i = 0;$i < $arr['cnt'];$i++)
						{
							$key = &$arr['keys'][$i];
							$territory[$key] = xivo_i18n::babelfish('territory_'.$key);
						}

						asort($territory);
						$r = &$territory;
						break;
				}
		}

		return($r);
	}

	function _load_file($file,$namespace='')
	{
		$r = false;

		if($this->_get_file_info($file,$namespace) === false)
			return($r);

		if($this->_get_cache_file() === true)
			return(true);

		if(xivo_ak('filei18n',$this->_file,true) === false)
		{
			trigger_error('I18N file not found',E_USER_WARNING);
			return($r);
		}

		if(isset($this->_loop[$this->_file['filei18n']]) === true)
		{
			trigger_error('I18N file already loaded: '.$this->_file['filei18n'],E_USER_WARNING);
			return($r);
		}

		$this->_loop[$this->_file['filei18n']] = 1;

		if($this->_mk_cache_file() === true)
			$r = $this->_get_cache_file();

		return($r);
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
				$r = '';

				if($name === '')
					return($r);

				$value = (array) $value;

				if(xivo_issa($namespace,$babelfish) === false || isset($babelfish[$namespace][$name]) === false)
				{
					if(xivo_issa('_empty_',$babelfish) === false || isset($babelfish['_empty_'][$name]) === false)
					{
						//trigger_error('Undefined translation: '.$name,E_USER_NOTICE);
						return($name);
					}
					else
						$namespace = '_empty_';
				}

				$r = (string) $babelfish[$namespace][$name];

				if(empty($value) === false)
					$r = vsprintf(xivo_printf_strip($r,count($value)),$value);
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
		$r = false;

		$file = (string) $file;

		$namespace = $this->_chk_namespace($namespace);

		if(empty($file) === true)
		{
			$this->_file = false;
			return($r);
		}
		// TODO: trigger_error();

		$this->_file['filename'] = $file;

		$dir = ltrim(dirname($file),'.');
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

		if(isset($this->_file['locale']) === true && ($lc = $this->chk_locale($this->_file['locale'])) !== false)
		{
			$this->_file['locale'] = $lc;
			$this->_file['language'] = $this->get_language($lc);
		}
		else
		{
			$this->_file['locale'] = $this->attrib_locale();
			$this->_file['language'] = $this->attrib_language();
		}

		$this->_file['filei18n'] = realpath(XIVO_PATH_I18N.XIVO_SEP_DIR.$this->_file['locale'].XIVO_SEP_DIR.$this->_file['path']);

		if($this->_file['filei18n'] !== false && xivo_file::is_f_r($this->_file['filei18n']) !== false)
		{
			$r = true;
			$this->_file['filemtime'] = xivo_uint(filemtime($this->_file['filei18n']));
		}
		else
			$this->_file = false;

		return($r);
	}

	function _mk_cache_file()
	{
		$r = false;

		if(is_array($this->_file) === false)
			return($r);

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
			return($r);
		}

		$cache['language'] = $this->get_language($cache['locale']);
		$cache['path'] = $this->_file['path'];

		$r = $this->_dso->save_cache($cache);

		return($r);
	}

	function _get_cache_file()
	{
		$r = false;

		if(is_array($this->_file) === false)
			return($r);

		if($this->_mk_babelfish() === true)
			return(true);

		if(($cache = $this->_dso->get_cache($this->_file)) === false || xivo_issa('obj',$cache) === false)
			return($r);

		$this->_file['filectime'] = $cache['filectime'];

		if(isset($cache['obj']['content']) === true)
		{
			$r = true;

			$_SESSION['_i18n']['babelfish'][$this->_file['namespace']][$this->_file['locale']][$this->_file['path']] = array(
						'finfo'		=> $this->_file,
						'copy'		=> $cache['obj']['copy'],
						'load'		=> $cache['obj']['load'],
						'content'	=> $cache['obj']['content']);

			$this->_mk_babelfish();
		}

		return($r);
	}

	function _mk_babelfish()
	{
		$r = false;

		if(is_array($this->_file) === false)
			return($r);

		$ref_sess = &$_SESSION['_i18n']['babelfish'];

		$namespace = $this->_file['namespace'];

		if(isset($ref_sess[$namespace]) === false)
			$ref_sess[$namespace] = array();

		if(isset($ref_sess[$namespace][$this->_file['locale']]) === false)
			$ref_sess[$namespace][$this->_file['locale']] = array();

		if(isset($ref_sess[$namespace][$this->_file['locale']][$this->_file['path']]) === false)
			return($r);

		$ref_file = &$ref_sess[$namespace][$this->_file['locale']][$this->_file['path']];

		if(xivo_issa('finfo',$ref_file) === true && $ref_file['finfo']['filemtime'] === $this->_file['filemtime'])
		{
			$r = true;

			if($ref_file['copy'] !== false)
			{
				$_file = $this->_file;
				$this->_file = $ref_file['finfo'];
				$this->_file['locale'] = $ref_file['copy'];
				$this->_load_file($ref_file['finfo']['filename'],$ref_file['finfo']['namespace']);
				$this->_file = $_file;
			}

			if(($arr = xivo_get_aks($ref_file['load'])) !== false)
			{
				$_file = $this->_file;
				$this->_file = $ref_file['finfo'];

				for($i = 0;$i < $arr['cnt'];$i++)
					$this->_load_file($arr['keys'][$i],$ref_file['finfo']['namespace']);
				
				$this->_file = $_file;
			}

			$this->_set_babelfish($namespace,$ref_file['content']);
		}

		return($r);
	}

	function _parse_file()
	{
		$r = false;

		if(is_array($this->_file) === false)
			return($r);

		if(($file = xivo_file::is_f_r($this->_file['filei18n'])) === false)
		{
			trigger_error('I18N file not found',E_USER_WARNING);
			return($r);
		}

		$content = file($file);
		$nb = count($content);
		$priority = 0;
		$txt = $copy = $load = $data = false;

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
					if(preg_match('/^;(?:\t| )*copy(\t| )+([a-z]{2,3}_[A-Z]{2})$/',$line,$matchcopy) === 1
					&& ($locale = $this->chk_locale($matchcopy[2])) !== false)
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
					else if(preg_match('#^;(?:\t| )*load(\t| )+'.
							   '(\.{1,2}'.preg_quote(XIVO_SEP_DIR).')?'.
							   '([a-z0-9-_'.preg_quote(XIVO_SEP_DIR).']+)$#i',$line,$matchload) === 1)
					{
						if($priority > 3)
							continue;

						$txt = false;
						$priority |= 2;

						$_file = $this->_file;
						$_load = $this->_load;
						$_copy = $this->_copy;

						$filename = $matchload[3];

						if($matchload[2] !== '')
							$filename = $this->_file['dirname'].XIVO_SEP_DIR.$matchload[2].$filename;

						if($this->_load_file($filename,$this->_file['namespace']) !== false)
							$_load[$filename] = 1;

						$this->_copy = $_copy;
						$this->_load = $_load; 
						$this->_file = $_file;
					}
					else if(preg_match('/^;(?:\t| )*([a-zA-Z0-9-_]+)$/',$line,$match) === 1)
					{
						$txt = true;
						$key = $match[1];

						if(isset($r[$key]) === true)
							$txt = false;
					}
					else if(preg_match('/^;(?:\t| )*"([a-zA-Z0-9_ -]+)"$/',$line,$match) === 1
					&& strpos($match[1],'  ') === false
					&& $match[1]{0} !== ' ' && substr($match[1],-1,1) !== ' ')
					{
						$txt = true;
						$key = $match[1];

						if(isset($r[$key]) === true)
							$txt = false;
					}
					break;
				default:
					if($line{0} === '\\' && isset($line{1}) === true && ($line{1} === '#' || $line{1} === ';') === true)
						$line = substr($line,1);

					if($txt === true && $key !== '')
					{
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
					else $txt = false;
			}
		}

		return($r);
	}
}

?>
