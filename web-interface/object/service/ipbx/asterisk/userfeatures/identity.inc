<?php

class xivo_service_asterisk_userfeatures_identity
{
	function _prepare_identity(&$data)
	{
		if(xivo_haslen($data['context']) === false)
			$data['context'] = 'default';

		$data['fullname'] = trim(trim($data['firstname']).' '.trim($data['lastname']));

		if($data['fullname'] === '')
			$data['fullname'] = '-';

		if(xivo_haslen($data['number']) === true)
			$data['identity'] = $data['fullname'].' ('.$data['number'].'@'.$data['context'].')';
		else if(xivo_haslen($data['name']) === true)
			$data['identity'] = $data['fullname'].' ('.$data['name'].'@'.$data['context'].')';
		else
			$data['identity'] = $data['fullname'].' ('.$data['context'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['firstname'],$arr['lastname']) === false)
			return(false);

		if(isset($arr['name']) === false)
			$arr['name'] = '';

		if(isset($arr['number']) === false)
			$arr['number'] = '';

		if(isset($arr['context']) === false)
			$arr['context'] = '';

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
