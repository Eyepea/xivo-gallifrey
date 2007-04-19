<?php

require_once(XIVO_PATH_LIB.XIVO_SEP_DIR.'tpl'.XIVO_SEP_DIR.'module.inc');

class xivo_tpl_dhtml extends xivo_tpl_module
{
	var $_css	= array();
	var $_js	= array();
	var $_inc	= array('js' => array('head' => array(),'foot' => array()),'css' => array());
	var $_tpl	= null;

	function xivo_tpl_dhtml(&$tpl)
	{
		$this->_tpl = &$tpl;
	}

	function add_css($css)
	{
		return($this->_add_file('css',$css));
	}

	function add_js($js,$where='head')
	{
		return($this->_add_file('js',$js,$where));
	}

	function _add_file($type,$files,$where='')
	{
		$type = (string) $type;
		$where = (string) $where;
		$files = (array) $files;

		if(($cnt = count($files)) === 0)
			return(null);

		for($i = 0;$i < $cnt;$i++)
		{
			if(isset($files[$i]) === false)
				continue;

			$file = 'bloc'.XIVO_SEP_DIR.ltrim((string) $files[$i],XIVO_SEP_DIR);

			if(xivo_issa($type,$this->_inc) === false
			|| xivo_file::is_f_r($this->get_option('tpl_path').XIVO_SEP_DIR.$file) === false)
				continue;

			if($where !== '')
			{
				if(xivo_issa($where,$this->_inc[$type]) === false)
					continue;

				$this->_inc[$type][$where][] = $file;
				continue;
			}

			$this->_inc[$type][] = $file;
		}

		return(true);
	}

	function set_css($a)
	{
		$a = (array) $a;

		if(($cnt = count($a)) === 0)
			return(null);
			
		for($i = 0;$i < $cnt;$i++)
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

		if(($cnt = count($a)) === 0)
			return(null);

		for($i = 0;$i < $cnt;$i++)
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

		if(is_array($this->_js) === true && ($nb = count($this->_js)) > 0)
		{
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
		}
	
		return($r);
	}

	function mk_css()
	{
		$r = '';

		if(is_array($this->_css) === true && ($nb = count($this->_css)) > 0)
		{
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
		}

		return($r);
	}

	function load_js($where='head')
	{
		$r = '';

		if(xivo_issa($where,$this->_inc['js']) === true && ($nb = count($this->_inc['js'][$where])) > 0)
		{
			for($i = 0;$i < $nb;$i++)
				$r .= $this->_load_file($this->_inc['js'][$where][$i])."\n"; 

			if($r !== '')
				$this->write_js($r);
		}
	}

	function write_js($str)
	{
		$str = (string) $str;

		echo '<script type="text/javascript">',"\n",'<!--',"\n",$str,"\n",'-->',"\n",'</script>',"\n";
	}

	function write_css($str)
	{
		$str = (string) $str;

		echo '<style type="text/css">',"\n",'<!--',"\n",$str,"\n",'-->',"\n",'</style>',"\n";
	}

	function load_css()
	{
		$r = '';

		if(is_array($this->_inc['css']) === true && ($nb = count($this->_inc['css'])) > 0)
		{
			for($i = 0;$i < $nb;$i++)
				$r .= $this->_load_file($this->_inc['css'][$i]);

			if($r !== '')
				$this->write_css($r);
		}
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
		$r = ob_get_contents();
		ob_end_clean();

		return($r);
	}
}

?>
