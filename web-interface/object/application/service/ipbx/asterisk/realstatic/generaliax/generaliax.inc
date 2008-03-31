<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_generaliax extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'generaliax';
	var $_modulename	= 'generaliax';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'iax.conf';
	var $_musiconhold	= array();

	function xivo_application_service_asterisk_realstatic_generaliax(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function _chk_pre_save($rs)
	{
		if(is_array($rs) === false
		|| isset($rs['var_name'],$rs['var_val']) === false)
			return(null);

		switch($rs['var_name'])
		{
			case 'allow':
				if(is_array($rs['var_val']) === true)
				{
					$rs['var_val'] = implode(',',$rs['var_val']);
					$this->_set_result($this->_name,$rs);
				}
				break;
		}

		return(true);
	}

	function set_save_all($arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['minregexpire']) === false
		&& isset($arr['maxregexpire']) === true)
			$arr['minregexpire'] = $arr['maxregexpire'];
		else if(isset($arr['minregexpire'],$arr['maxregexpire']) === true
		&& $arr['minregexpire'] > $arr['maxregexpire'])
			$arr['minregexpire'] = $arr['maxregexpire'];

		if(isset($arr['minexcessbuffer']) === false
		&& isset($arr['maxexcessbuffer']) === true)
			$arr['minexcessbuffer'] = $arr['maxexcessbuffer'];
		else if(isset($arr['minexcessbuffer'],$arr['maxexcessbuffer']) === true
		&& $arr['minexcessbuffer'] > $arr['maxexcessbuffer'])
			$arr['minexcessbuffer'] = $arr['maxexcessbuffer'];

		return(parent::set_save_all($arr,$disable));
	}
}

?>
