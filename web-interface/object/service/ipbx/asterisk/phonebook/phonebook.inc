<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_phonebook extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'phonebook';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_phonebook(&$sre,&$dso)
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
		$data['fullname'] = trim(trim($data['firstname']).' '.trim($data['lastname']));

		if($data['fullname'] === '')
		{
			if($data['displayname'] === '')
				$data['fullname'] = '-';
			else
				$data['fullname'] = $data['displayname'];
		}

		return($data);
	}

	function get_search($value,$list=null,$order=null,$limit=null,$assoc=false,$name=false)
	{
		$assoc = (bool) $assoc;

		if(($list = $this->_dso->get_search($value,$list,$order,$limit,$name)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$list[$i]['id']] = $this->_prepare($list[$i]);
		
		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_search_name($value,$list=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->get_search($value,$list,$order,$limit,$assoc,true));
	}
}

?>
