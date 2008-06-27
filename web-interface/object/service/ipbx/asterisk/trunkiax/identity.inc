<?php

class xivo_service_asterisk_trunkiax_identity
{
	var $_sre	= null;

	function xivo_service_asterisk_trunkiax_identity(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
	}

	function _prepare_identity(&$data)
	{
		if(($channel = $this->_sre->get_channel_by_protocol('iax')) !== false)
			$data['identity'] = $data['name'].' ('.$channel.')';
		else
			$data['identity'] = $data['name'];

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false || isset($arr['name']) === false)
			return(false);

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
