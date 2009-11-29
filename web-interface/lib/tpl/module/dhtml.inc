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

dwho::load_class('dwho_uri');

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'tpl','module.inc'));

class dwho_tpl_dhtml extends dwho_tpl_module
{
	var $_css	= array();
	var $_js	= array();
	var $_inc	= array(
				'js'	=> array(
					'head'	=> array(),
					'foot'	=> array()),
				'css'	=> array(
					'head'	=> array()));
	var $_content	= array('js'	=> array(
					'head'	=> array(),
					'foot'	=> array()),
				'css'	=> array(
					'head' => array()));
	var $_tpl	= null;
	var $_query	= null;

	function dwho_tpl_dhtml(&$tpl)
	{
		$this->_tpl	= &$tpl;
		$this->_uri	= new dwho_uri();
	}

	function escape($str,$entquote=ENT_COMPAT)
	{
		$str		= preg_replace('/\s+/',' ',strval($str));

		$search		= array('\'');
		$replace	= array('\\\'');

		if(($specialchars = (($entquote & DWHO_ENT_NOSPECIALCHARS) === 0)))
			$entquote &= ~DWHO_ENT_NOSPECIALCHARS;

		switch($entquote)
		{
			case ENT_NOQUOTES:
				$search[1]	= '"';
				$replace[1]	= '\"';
				break;
			case ENT_QUOTES:
				return(dwho_htmlsc($str,ENT_QUOTES));
			case DWHO_ENT_APOSTROPHE:
				$entquote	= ENT_NOQUOTES;
				$search[1]	= '"';
				$replace	= array('&#039;','\"');
				break;
			default:
				$entquote	= ENT_COMPAT;
		}

		if($specialchars === true)
			$str = dwho_htmlsc($str,$entquote);

		return(str_replace($search,$replace,$str));
	}

	function escape_walk(&$value,$key,$entquote=ENT_COMPAT)
	{
		switch(gettype($value))
		{
			case 'boolean':
			case 'integer':
			case 'double':
				return($value);
			case 'object':
				if(isset($value->scalar) === true)
					return(($value = $value->scalar));
		}

		return(($value = '\''.$this->escape($value,$entquote).'\''));
	}

	function valid_js_function_name($name)
	{
		return((bool) preg_match('/^[a-zA-Z\$_][a-zA-Z0-9\.\$_]*(?<!\.)$/',$name));
	}

	function js_uri($function,$parameters=null,$entquote=ENT_COMPAT)
	{
		if($this->valid_js_function_name($function) === false)
			return(false);

		$entquote = (dwho_uint($entquote) | DWHO_ENT_NOSPECIALCHARS);

		$uri = array('scheme'	=> 'javascript',
			     'path'	=> $function.'();');

		if(is_array($parameters) === false || empty($parameters) === true)
			return($this->_uri->build_uri($uri));

		array_walk($parameters,array(&$this,'escape_walk'),$entquote);

		$uri['path'] = $function.'('.implode(',',$parameters).');';

		return(str_replace('+','%20',$this->_uri->build_uri($uri)));
	}

	function add_css($css)
	{
		return($this->_add_file('css',$css,'head'));
	}

	function add_js($js,$where='head')
	{
		return($this->_add_file('js',$js,$where));
	}

	function _add_file($type,$files,$where='')
	{
		$type = (string) $type;
		$files = (array) $files;
		$where = (string) $where;

		if(($nb = count($files)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if(dwho_has_len($files,$i) === false)
				continue;
			else if($files[$i]{0} === DWHO_SEP_DIR)
				$file = $files[$i];
			else
				$file = DWHO_SEP_DIR.'bloc'.DWHO_SEP_DIR.$files[$i];

			if(dwho_issa($type,$this->_inc) === true
			&& dwho_file::is_f_r($this->get_path().$file) !== false
			&& dwho_issa($where,$this->_inc[$type]) === true)
				$this->_inc[$type][$where][] = $file;
		}

		return(true);
	}

	function content_css($css)
	{
		return($this->_content('css',$css));
	}

	function content_js($js,$where='head')
	{
		return($this->_content('js',$js,$where));
	}

	function _content($type,$content,$where='head')
	{
		if(is_array($content) === true)
			$content = implode("\n",$content);

		if(is_scalar($content) === false
		|| dwho_issa($type,$this->_content) === false
		|| dwho_issa($where,$this->_content[$type]) === false)
			return(false);

		$this->_content[$type][$where][] = $content;

		return(true);
	}

	function set_css($a)
	{
		$a = (array) $a;

		if(($nb = count($a)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($a[$i]) === false)
				continue;

			$path = DWHO_LOCATION_SCRIPT.DWHO_SEP_DIR.ltrim($a[$i],DWHO_SEP_DIR);

			if(dwho_file::is_f_r($path) !== false)
				$this->_css[] = $a[$i];
		}
	}

	function set_js($a)
	{
		$a = (array) $a;

		if(($nb = count($a)) === 0)
			return(null);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($a[$i]) === false)
				continue;

			$path = DWHO_LOCATION_SCRIPT.DWHO_SEP_DIR.ltrim($a[$i],DWHO_SEP_DIR);

			if(dwho_file::is_f_r($path) !== false)
				$this->_js[] = $a[$i];
		}
	}

	function mk_js()
	{
		$r = '';

		if(is_array($this->_js) === false || ($nb = count($this->_js)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
		{
			$file = $this->_js[$i];

			if(isset($file{0}) === false)
				continue;

			$file = $this->file_time($file);

			if($file{0} !== '/')
				$file = DWHO_LOCATION_ROOT_URL.'/'.$file;

			$r .= '<script type="text/javascript" src="'.$file.'"></script>'."\n";
		}

		return($r);
	}

	function mk_css()
	{
		$r = '';

		if(is_array($this->_css) === false || ($nb = count($this->_css)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
		{
			$file = $this->_css[$i];

			if(isset($file{0}) === false)
				continue;

			$file = $this->file_time($file);

			if($file{0} !== '/')
				$file = DWHO_LOCATION_ROOT_URL.'/'.$file;

			$r .= '<link rel="stylesheet" type="text/css" href="'.$file.'">'."\n";
		}

		return($r);
	}

	function load_js($where='head')
	{
		$r = '';

		if(dwho_issa($where,$this->_inc['js']) === false || ($nb = count($this->_inc['js'][$where])) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
			$r .= $this->_load_file($this->_inc['js'][$where][$i])."\n";

		if($r !== '')
			$this->write_js($r);
	}

	function write_js($str)
	{
		if(is_array($str) === true)
			$str = implode("\n",$str);

		if(is_scalar($str) === false)
			return(false);

		echo '<script type="text/javascript">',"\n",'<!--',"\n",$str,"\n",'-->',"\n",'</script>',"\n";
	}

	function write_css($str)
	{
		if(is_array($str) === true)
			$str = implode("\n",$str);

		if(is_scalar($str) === false)
			return(false);

		echo '<style type="text/css">',"\n",'<!--',"\n",$str,"\n",'-->',"\n",'</style>',"\n";
	}

	function load_css()
	{
		$r	= '';
		$where	= 'head';

		if(dwho_issa($where,$this->_inc['css']) !== false
		&& ($nb = count($this->_inc['css'][$where])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$r .= $this->_load_file($this->_inc['css'][$where][$i])."\n";
		}

		if(dwho_issa($where,$this->_content['css']) !== false
		&& ($nb = count($this->_content['css'][$where])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$r .= $this->_content['css'][$where][$i];
		}

		if($r !== '')
			$this->write_css($r);
	}

	function _load_file($file)
	{
		$path = $this->get_path().DWHO_SEP_DIR.ltrim($file,DWHO_SEP_DIR);

		if(($path = dwho_file::is_f_r($path)) === false)
			return('');

		$this->set_file('dhtml',$path);

		if($this->get_option('preload_i18n') !== false)
			$this->load_i18n_file($file);

		unset($file,$path);

		ob_start();
		include($this->get_file('dhtml'));
		return(ob_get_clean());
	}
}

?>
