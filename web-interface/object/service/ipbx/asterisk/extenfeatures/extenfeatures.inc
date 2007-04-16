<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'extensions'.XIVO_SEP_DIR.'extensions.inc');

class xivo_service_asterisk_extenfeatures extends xivo_service_asterisk_extensions
{
	var $_name		= 'extenfeatures';
	var $_extensions	= array();
	var $_exfeatures	= array();
	var $_context		= 'features';

	function xivo_service_asterisk_extenfeatures(&$sre,&$dso)
	{
		$this->xivo_service_asterisk_extensions(&$sre,&$dso);

		$this->_extensions['conf'] = $this->_conf;
		$this->_conf = array();
		$this->_load_exfeatures_config();
		$this->_conf = &$this->_exfeatures['conf'];
	}

	function _load_exfeatures_config()
	{
		if(xivo_issa('conf',$this->_exfeatures) === true && empty($this->_exfeatures['conf']) === false)
			return(true);

		$dir = dirname(__FILE__).XIVO_SEP_DIR.'config';

		if(($this->_exfeatures['conf'] = xivo_gat::get_file($dir)) !== false)
			return(true);

		$this->_exfeatures['conf'] = array();

		return(false);
	}

	function _chk_exfeatures_exists($name,$cmd='')
	{
		$name = (string) $name;
		$cmd = (string) $cmd;

		if(xivo_issa('exten',$this->_exfeatures['conf']) === false
		|| xivo_issa($name,$this->_exfeatures['conf']['exten']) === false)
			return(false);

		if($cmd !== '' && isset($this->_exfeatures['conf']['exten'][$name][$cmd]) === false)
			return(false);

		return(true);
	}

	function _load_filter()
	{
		if(xivo::load_class('xivo_filter') === false || xivo_issa('filter',$this->_exfeatures['conf']) === false)
			return(false);

		$this->_exfeatures['filter'] = new xivo_filter($this->_exfeatures['conf']['filter']);
		$this->_filter = &$this->_exfeatures['filter'];

		return(true);
	}

	function _chk_extensions_values($arr)
	{
		$this->_conf = &$this->_extensions['conf'];

		if(isset($this->_extensions['filter']) === false && $this->_load_extensions_filter() === false)
			return(false);

		$this->_filter = &$this->_extensions['filter'];

		$r = $this->chk_values($arr);

		$this->_conf = &$this->_exfeatures['conf'];
		$this->_filter = &$this->_exfeatures['filter'];

		return($r);
	}

	function _load_extensions_filter()
	{
		if(xivo::load_class('xivo_filter') === false || xivo_issa('filter',$this->_extensions['conf']) === false)
			return(false);

		$this->_extensions['filter'] = new xivo_filter($this->_extensions['conf']['filter']);

		return(true);
	}

	function _mk_exten_appdata_vars($arr)
	{
		if(xivo_ak('appdata',$arr) === false)
			return($arr);

		$vars = array('$<EXTNUM>' => '','$<EXTNUMLEN>' => 0);

		if(isset($arr['exten']) === true)
		{
			if(preg_match('/^[_]?([0-9\*#]+)/',$arr['exten'],$extmatch) === 1)
			{
				$extnum = $extmatch[1];
				$vars['$<EXTNUM>'] = $extnum;
				$vars['$<EXTNUMLEN>'] = strlen($extnum);
			}
		}

		$arr['appdata'] = strtr($arr['appdata'],$vars);

		return($arr);
	}

	function _mk_exten($name,$val,$disable=null)
	{
		$r = false;

		$name = (string) $name;
		$val = (string) $val;
		$disable = $disable === null ? $disable : intval((bool) $disable);

		if($this->_chk_exfeatures_exists($name) === false)
			return($r);

		$arr = $this->_exfeatures['conf']['exten'][$name];

		if(isset($arr['_line']) === false)
		{
			$r = array();

			$arr['context'] = $this->_context;
			$arr['exten'] = $val;
			$arr['name'] = $name;

			if(($r[0] = $this->_chk_extensions_values($arr)) === false)
				return(false);

			$r[0] = $this->_mk_exten_appdata_vars($r[0]);

			return($r);
		}

		unset($arr['_line']);

		$ref = &$this->_exfeatures['conf']['exten'];

		if(is_array($ref[$name]) === false || ($nb = count($ref[$name]['_line'])) === 0)
			return($r);

		$lines = array_values($ref[$name]['_line']);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{	
			$line = array_merge($arr,$lines[$i]);

			$line['name'] = $name;
			$line['exten'] = $val;
			$line['context'] = $this->_context;

			if(($line = $this->_chk_extensions_values($line)) === false)
				return(false);

			$r[] = $this->_mk_exten_appdata_vars($line);
		}

		return($r);
	}

	function replace_exten_by_name($name,$exten,$disable=null)
	{
		if($disable === null)
			$disable = $exten !== '' && $exten !== null ? 0 : 1;	

		if(($arr = $this->_mk_exten($name,$exten,$disable)) === false)
			return(false);

		$nb = count($arr);

		$r = array();

		if(($list = $this->get_all_where(array('name' => $name,'context' => $this->_context))) === false)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$arr[$i]['context'] = $this->_context;

				if(($ret = $this->add($arr[$i])) === false)
					break;

				$r[$i] = $ret;
			}

			if(isset($r[0]) === false)
				return(false);

			if($ret !== false)
				return($r);

			for($j = 0;$j < $i;$j++)
				$this->delete($r[$j]);

			return(false);
		}

		$cnt = count($list);

		for($i = 0;$i < $cnt && $i < $nb;$i++)
		{
			$arr[$i]['context'] = $this->_context;

			if(($ret = $this->edit($list[$i]['id'],$arr[$i])) === false)
				break;

			$r[] = $list[$i];
		}

		if(isset($r[0]) === false)
			return(false);

		if($ret === false)
		{
			$nb = count($r);

			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$r[$i]['id'];
				$this->edit($ref['id'],$ref);
			}

			return(false);
		}

		if($cnt > $nb)
		{
			for($i;$i < $cnt;$i++)
				$this->delete($list[$i]['id']);

			return($r);
		}

		for($i;$i < $nb;$i++)
		{
			if(($ret = $this->add($arr[$i])) === false)
				break;

			$r[$i] = $ret;
		}

		if($ret !== false)
			return($r);

		for($j = 0;$j < $i;$j++)
			$this->delete($r[$j]);

		return(false);
	}

	function set_chk_value($key,$value,$default=null)
	{
		if(isset($value{0}) === true && $value{0} !== '_'
		&& preg_match('/[0-9]/',$value) !== 0
		&& preg_match('/[X\.\*#]/',$value) !== 0)
			$value = '_'.$value;

		return(parent::set_chk_value($key,$value,$default));
	}

	function get_name_exten($arr=array(),$disable=null)
	{
		$arr = (array) $arr;
		$arr['context'] = $this->_context;

		return(parent::get_name_exten($arr,$disable));
	}

	function get_name_exten_for_display($arr=array(),$disable=null)
	{
		if(($r = $this->get_name_exten($arr,$disable)) === false
		|| ($arr = xivo_get_aks($r)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if(isset($r[$key]{0}) === true && $r[$key]{0} === '_')
				$r[$key] = substr($r[$key],1);
		}

		return($r);
	}
}

?>
