<?php

class xivo_service_asterisk_voicemenu_identity
{
	function _prepare_identity(&$data)
	{
		if(xivo_haslen($data['number']) === true)
			$data['identity'] = $data['name'].' ('.$data['number'].'@'.$data['context'].')';
		else
			$data['identity'] = $data['name'].' ('.$data['context'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['name'],$arr['context']) === false)
			return(false);
		
		if(isset($arr['number']) === false)
			$arr['number'] = '';

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
