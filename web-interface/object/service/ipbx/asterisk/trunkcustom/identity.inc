<?php

class xivo_service_asterisk_trunkcustom_identity
{
	function _prepare_identity(&$data)
	{
		if(xivo_haslen($data['intfsuffix']) === true)
			$data['identity'] = $data['name'].' ('.$data['interface'].'|'.$data['intfsuffix'].')';
		else
			$data['identity'] = $data['name'].' ('.$data['interface'].')';

		return($data);
	}

	function mk_identity($arr)
	{
		if(is_array($arr) === false
		|| isset($arr['name'],$arr['interface']) === false)
			return(false);

		if(isset($arr['intfsuffix']) === false)
			$arr['intfsuffix'] = '';

		$this->_prepare_identity($arr);

		return($arr['identity']);
	}
}

?>
