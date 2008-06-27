<?php

class xivo_service_asterisk_callfilter_identity
{
	var $_sre	= null;

	function xivo_service_asterisk_callfilter_identity(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
	}

	function _prepare_identity(&$data)
	{
		$data['identity'] = $data['name'].' ('.$data['context'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['name'],$arr['context']) === false)
			return(false);

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
