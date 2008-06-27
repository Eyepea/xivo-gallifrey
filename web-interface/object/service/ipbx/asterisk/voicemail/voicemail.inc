<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_voicemail extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'voicemail';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_voicemail(&$sre,&$dso)
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
		$data['fullmailbox'] = $data['mailbox'].'@'.$data['context'];
		$data['identity'] = $data['fullname'].' ('.$data['fullmailbox'].')';

		return($data);
	}

	function mk_identity($mailbox,$fullname,$context)
	{
		$arr = array();
		$arr['mailbox'] = $mailbox;
		$arr['fullname'] = $fullname;
		$arr['context'] = $context;

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}

	function get_all_by_mailbox_context($arr=null,$disable=null,$prepare=true,$order=null,$limit=null)
	{
		$prepare = (bool) $prepare;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(is_array($arr) === false || empty($arr) === true)
			$list = $this->_dso->get_all($disable,$order,$limit);
		else if(($where = $this->get_element_where($arr)) !== false)
			$list = $this->_dso->get_all_where($where,$disable,$order,$limit);
		else
			return(false);

		if($list === false || isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		$r = array();

		if(is_array($list) === false || ($nb = count($list)) === 0)
			return(false);
		else if($prepare === true)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$list[$i];

				$key = $ref['mailbox'].'@'.$ref['context'];

				$r[$key] = $this->_prepare($list[$i]);
			}
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$list[$i];

				$key = $ref['mailbox'].'@'.$ref['context'];

				$r[$key] = $list[$i];
			}
		}

		return($r);
	}
}

?>
