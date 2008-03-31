<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_voicemail_general extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'generalvoicemail';
	var $_modulename	= 'voicemail';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'voicemail.conf';

	function xivo_application_service_asterisk_realstatic_voicemail_general(&$service)
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
			case 'format':
				if(is_array($rs['var_val']) === true)
				{
					$rs['var_val'] = implode('|',$rs['var_val']);
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
		else if(isset($arr['minmessage']) === false
		&& isset($arr['maxmessage']) === true)
			$arr['minmessage'] = $arr['maxmessage'];
		else if(isset($arr['minmessage'],$arr['maxmessage']) === true
		&& $arr['minmessage'] > $arr['maxmessage'])
			$arr['minmessage'] = $arr['maxmessage'];

		if(isset($arr['format'],$arr['attachformat']) === true
		&& is_array($arr['format']) === true
		&& xivo_haslen($arr['attachformat']) === true
		&& ($format = (int) array_search($arr['attachformat'],$arr['format'],true)) !== 0)
		{
			unset($arr['format'][$format]);
			array_unshift($arr['format'],$arr['attachformat']);
		}
		else
			unset($arr['attachformat']);

		return(parent::set_save_all($arr,$disable));
	}
}

?>
