<?php

class xivo_service_asterisk_meetmefeatures_identity
{
	function _prepare_identity(&$data)
	{
		if(xivo_haslen($data['context']) === false)
			$data['context'] = 'default';

		$data['identity'] = trim($data['name']);

		if($data['identity'] === '')
			$data['identity'] = '-';

		if(xivo_haslen($data['number']) === true)
			$data['identity'] .= ' ('.$data['number'].'@'.$data['context'].')';
		else
			$data['identity'] .= ' ('.$data['context'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false || isset($arr['name']) === false)
			return(false);

		if(isset($arr['number']) === false)
			$arr['number'] = '';

		if(isset($arr['context']) === false)
			$arr['context'] = '';

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
