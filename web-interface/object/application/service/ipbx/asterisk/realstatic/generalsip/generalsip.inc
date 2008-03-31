<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_generalsip extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'generalsip';
	var $_modulename	= 'generalsip';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'sip.conf';
	var $_musiconhold	= array();

	function xivo_application_service_asterisk_realstatic_generalsip(&$service)
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
			case 'vmexten':
				if(($appextenfeatures = &$this->_service->get_application('extenfeatures')) === false)
					return(false);

				$voicemsg = array();
				$voicemsg['name'] = 'voicemsg';
				$voicemsg['exten'] = $rs['var_val'];

				if($appextenfeatures->set($voicemsg) === false
				|| $appextenfeatures->save() === false)
					return(false);
				break;
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
		else if(isset($arr['externip']) === false
		|| xivo_haslen($arr['externip']) === false)
			$arr['localnet'] = null;

		$this->get_musiconhold();

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		return(parent::set_save_all($arr,$disable));
	}
}

?>
