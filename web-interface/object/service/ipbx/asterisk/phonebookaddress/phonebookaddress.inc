<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_phonebookaddress extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'phonebookaddress';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_phonebookaddress(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get_all_type_by_phonebook($phonebookid,$prepare=true)
	{
		if(($phonebookid = xivo_ulongint($phonebookid)) === 0)
			return(false);

		$arr = array('phonebookid' => $phonebookid);

		if(($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		if((bool) $prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				if(isset($r[$list[$i]['type']]) === true)
					continue;

				$r[$list[$i]['type']] = $this->_prepare($list[$i]);
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
			{
				if(isset($r[$list[$i]['type']]) === true)
					continue;

				$r[$list[$i]['type']] = $list[$i];
			}
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
