<?php

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_meetme extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'meetme.conf';

	function _prepare($data)
	{
		$val = explode(',',$data['var_val']);

		$data['number'] = xivo_haslen($val,0) === true ? $val[0] : '';
		$data['pin'] = xivo_haslen($val,1) === true ? $val[1] : '';
		$data['admin-pin'] = xivo_haslen($val,2) === true ? $val[2] : '';

		$data = parent::_prepare($data);

		return($data);
	}
}

?>
