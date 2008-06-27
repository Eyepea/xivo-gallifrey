<?php

class xivo_service_asterisk_outcall_identity
{
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
