<?php

class xivo_service_asterisk_context_identity
{
	function _prepare_identity(&$data)
	{
		$data['identity'] = $data['displayname'].' ('.$data['name'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['displayname'],$arr['name']) === false)
			return(false);

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
