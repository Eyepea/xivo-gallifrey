<?php

class xivo_service_asterisk_voicemail_identity
{
	function _prepare_identity(&$data)
	{
		$data['fullmailbox'] = $data['mailbox'].'@'.$data['context'];
		$data['identity'] = $data['fullname'].' ('.$data['fullmailbox'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['mailbox'],$arr['fullname'],$arr['context']) === false)
			return(false);

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
