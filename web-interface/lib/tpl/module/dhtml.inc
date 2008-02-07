<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module.inc');

class xivo_tpl_dhtml extends xivo_tpl_module
{
	var $_css	= array();
	var $_js	= array();
	var $_inc	= array('js' => array('head' => array(),'foot' => array()),'css' => array('head' => array()));
	var $_content	= array('js' => array('head' => array(),'foot' => array()),'css' => array('head' => array()));
	var $_tpl	= null;

	function xivo_tpl_dhtml(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function escape($str)
	{
		$r = xivo_htmlsc(preg_replace('/\s+/',' ',strval($str)),ENT_NOQUOTES);
		$r = str_replace(array('\'','"'),array('\\\'','&quot;'),$r);

		return($r);
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
			if(xivo_haslen($files,$i) === false)
				continue;
			else if($files[$i]{0} === XIVO_SEP_DIR)
				$file = $files[$i];
			else
				$file = XIVO_SEP_DIR.'bloc'.XIVO_SEP_DIR.$files[$i];

			if(xivo_issa($type,$this->_inc) === false
			|| xivo_file::is_f_r($this->get_option('tpl_path').$file) === false
			|| xivo_issa($where,$this->_inc[$type]) === false)
				continue;

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
		|| xivo_issa($type,$this->_content) === false
		|| xivo_issa($where,$this->_content[$type]) === false)
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

			$file = (string) $a[$i];

			$path = $this->get_option('script_path').XIVO_SEP_DIR.ltrim($file,XIVO_SEP_DIR);

			if(xivo_file::is_f_r($path) !== false)
				$this->_css[] = $file;
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

			$file = (string) $a[$i];
			$path = $this->get_option('script_path').XIVO_SEP_DIR.ltrim($file,XIVO_SEP_DIR);

			if(xivo_file::is_f_r($path) !== false)
				$this->_js[] = $file;
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
				$file = $this->get_option('script_root').'/'.$file;

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
				$file = $this->get_option('script_root').'/'.$file;

			$r .= '<link rel="stylesheet" type="text/css" href="'.$file.'">'."\n";
		}

		return($r);
	}

	function load_js($where='head')
	{
		$r = '';

		if(xivo_issa($where,$this->_inc['js']) === false || ($nb = count($this->_inc['js'][$where])) === 0)
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

		echo '<script type="text/javascript">',"\n",'<!--',"\n",(string) $str,"\n",'-->',"\n",'</script>',"\n";
	}

	function write_css($str)
	{
		if(is_array($str) === true)
			$str = implode("\n",$str);

		if(is_scalar($str) === false)
			return(false);

		echo '<style type="text/css">',"\n",'<!--',"\n",(string) $str,"\n",'-->',"\n",'</style>',"\n";
	}

	function load_css()
	{
		$r = '';
		$where = 'head';

		if(xivo_issa($where,$this->_inc['css']) !== false
		&& ($nb = count($this->_inc['css'][$where])) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$r .= $this->_load_file($this->_inc['css'][$where][$i])."\n"; 
		}

		if(xivo_issa($where,$this->_content['css']) !== false
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
		$r = '';

		$file = (string) $file;
		$path = $this->get_option('tpl_path').XIVO_SEP_DIR.ltrim($file,XIVO_SEP_DIR);

		if(xivo_file::is_i($path) === false)
			return($r);

		$this->set_file('dhtml',$path);

		if($this->get_option('overload_i18n') !== false)
			$this->load_i18n_file($file);

		unset($file,$path);

		ob_start();
		include($this->get_file('dhtml'));
		return(ob_get_clean());
	}
}

?>
