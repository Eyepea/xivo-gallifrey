<?php

class xivo_service_asterisk_agentfeatures_identity
{
	function _prepare_identity(&$data)
	{
		$data['fullname'] = trim(trim($data['firstname']).' '.trim($data['lastname']));

		if($data['fullname'] === '')
			$data['fullname'] = '-';

		$data['identity'] = $data['fullname'].' ('.$data['number'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['firstname'],$arr['lastname'],$arr['number']) === false)
			return(false);
		
		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
