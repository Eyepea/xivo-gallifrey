<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_extensions extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'extensions';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_extensions(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _load_config()
	{
		if(empty($this->_conf) === false)
			return(null);

		$dir = dirname(__FILE__).XIVO_SEP_DIR.'config';

		if(($this->_conf = xivo_gat::get_file($dir)) === false)
			$this->_conf = array();
	}

	function _prepare($data)
	{
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(($id = xivo_uint($id)) !== 0)
			$r['id'] = $id;

		$r['context'] = $arr['context'];
		$r['exten'] = $arr['exten'];
		$r['priority'] = $arr['priority'];
		$r['app'] = $arr['app'];
		$r['appdata'] = $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === false)
			$r['commented'] = false;

		return($this->_dso->add($r));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();

		if(isset($arr['context']) === true)
			$r['context'] = (string) $arr['context'];

		if(isset($arr['exten']) === true)
			$r['exten'] = (string) $arr['exten'];

		if(isset($arr['priority']) === true)
			$r['priority'] = (string) $arr['priority'];

		if(isset($arr['app']) === true)
			$r['app'] = (string) $arr['app'];

		if(isset($arr['appdata']) === true)
			$r['appdata'] = (string) $arr['appdata'];

		if(isset($arr['name']) === true)
			$r['name'] = (string) $arr['name'];

		if(isset($arr['commented']) === false)
			$arr['commented'] = false;

		$r['commented'] = intval((bool) $arr['commented']);

		return($this->_dso->edit($id,$r,$disable));
	}

	function get_name_exten($arr=array(),$disable=null)
	{
		$r = false;

		$arr = (array) $arr;

		if(($list = $this->get_all_where($arr,$disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$list[$i]['name']] = $list[$i]['exten'];

		if(empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
