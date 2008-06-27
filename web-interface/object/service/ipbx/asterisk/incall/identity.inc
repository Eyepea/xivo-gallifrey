<?php

class xivo_service_asterisk_incall_identity
{
	var $_sre	= null;

	function xivo_service_asterisk_incall_identity(&$sre)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
	}

	function _prepare_identity(&$data)
	{
		$data['exten'] = $this->_sre->display_extension($data['exten']);
		$data['identity'] = $data['exten'].' ('.$data['context'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['exten'],$arr['context']) === false)
			return(false);

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
