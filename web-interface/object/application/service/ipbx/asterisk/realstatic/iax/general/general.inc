<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_iax_general extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'general';
	var $_modulename	= 'iax';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'iax.conf';
	var $_contextmember	= false;
	var $_musiconhold	= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_realstatic_iax_general(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		if(($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'generaliax'))) === false)
			return(false);

		return(parent::_load_config(dirname(__FILE__)));
	}

	function _chk_pre_save($rs)
	{
		if(is_array($rs) === false
		|| isset($rs['var_name'],$rs['var_val']) === false)
			return(null);

		switch($rs['var_name'])
		{
			case 'regcontext':
				$this->_get_contextmember(null,'regcontext');

				$res = array();
				$res['generaliax-regcontext'] = array();
				$res['generaliax-regcontext']['context'] = $rs['var_val'];
				$res['generaliax-regcontext']['varname'] = 'regcontext';

				if($this->set_contextmember($res) === false
				|| $this->edit_contextmember() === false)
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
