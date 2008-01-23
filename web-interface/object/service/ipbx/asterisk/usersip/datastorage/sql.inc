<?php

xivo_service_asterisk::required(array('abstract','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_usersip_sql extends xivo_service_asterisk_abstract_sql
{
	var $_forcecolumn	= array('protocol'	=> XIVO_SRE_IPBX_AST_PROTO_SIP,
					'category'	=> 'user');
	var $_unique		= 'name';

	function get_nb($arr=null,$disable=null,$initialized=null)
	{
		if(is_array($arr) === false)
			$arr = $this->_forcecolumn;
		else
			$arr = array_merge($arr,$this->_forcecolumn);

		if($disable !== null && isset($this->_disable) === true)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		if($initialized !== null)
		{
			if((bool) $initialized === false)
				$this->_dso->andwhere('IFNULL(fullcontact,\'\') = \'\'');
			else
				$this->_dso->andwherenot('IFNULL(fullcontact,\'\') = \'\'');
		}

		return(xivo_ulongint($this->_dso->select_count()));
	}
}

?>
