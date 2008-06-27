<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_incall extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'incall';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_incall(&$sre,&$dso)
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
		$this->_prepare_identity($data);

		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function _prepare_identity(&$data)
	{
		$data['exten'] = $this->_sre->display_extension($data['exten']);
		$data['identity'] = $data['exten'].' ('.$data['context'].')';

		return($data);
	}

	function mk_identity($exten,$context)
	{
		$arr = array();
		$arr['exten'] = $exten;
		$arr['context'] = $context;

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['exten']) === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['exten']) === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return(parent::edit($id,$arr,$disable));
	}

	function search($exten,$disable=null,$order=null,$limit=null)
	{
		if(($list = $this->_dso->search($exten,$disable,$order,$limit)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
