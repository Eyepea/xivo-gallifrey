<?php

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_voicemail extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'voicemail.conf';

	function _prepare($data)
	{
		if($data['category'] === 'zonemessages')
		{
			$data['name'] = $data['var_name'];

			if(($pos = strpos($data['var_val'],'|')) !== false)
			{
				$data['timezone'] = substr($data['var_val'],0,$pos);
				$data['msg_format'] = substr($data['var_val'],$pos+1);
			}
			else
			{
				$data['timezone'] = $data['var_val'];
				$data['msg_format'] = '';
			}
		}

		return(parent::_prepare($data));
	}
}

?>
