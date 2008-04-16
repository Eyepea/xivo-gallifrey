<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_queuefeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queuefeatures';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_queuefeatures(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		if($data['context'] === '')
			$data['context'] = 'default';

		$data['identity'] = trim($data['name']);

		if($data['identity'] === '')
			$data['identity'] = '-';

		if(xivo_haslen($data['number']) === false)
			$data['identity'] .= ' ('.$data['context'].')';
		else
			$data['identity'] .= ' ('.$data['number'].'@'.$data['context'].')';

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr));
	}

	function get_all_number($id=0,$disable=null)
	{
		$id = xivo_ulongint($id);

		if(($list = $this->get_all($disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($id === $info['id'] || xivo_haslen($info['number']) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
