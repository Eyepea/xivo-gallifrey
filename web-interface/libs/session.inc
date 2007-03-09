<?php

class xivo_session
{
	var $_dso	= null;

	function xivo_session()
	{
		if(defined('XIVO_SESS_HANDLER') === false)
		{
			define('XIVO_SESS_HANDLER',true);
			$dso = &xivo_gct::get('XIVO_DSO');
			$name = 'xivo_session_'.$dso->get_type();

			if(xivo::load_class('xivo::session::'.$dso->get_type()) === false)
				trigger_error('Failed to load Session Datastorage',E_USER_ERROR);

			$this->_dso = new $name($dso);

			session_module_name('user');
			session_set_save_handler(
				array(&$this->_dso,'open'),
				array(&$this->_dso,'close'),
				array(&$this->_dso,'read'),
				array(&$this->_dso,'write'),
				array(&$this->_dso,'destroy'),
				array(&$this->_dso,'gc'));
		}

		register_shutdown_function('session_write_close');
		session_set_cookie_params(XIVO_SESS_TIME,XIVO_SESS_PATH);
		session_name(XIVO_SESS_NAME);

		$this->_mk_cookie();
	}

	function _mk_cookie()
	{
		$_QR = &xivo_gat::get('_QR');

		if(isset($_COOKIE[XIVO_SESS_NAME]) === true)
		{
			if(strlen($_COOKIE[XIVO_SESS_NAME]) > 32)
				$_COOKIE[XIVO_SESS_NAME] = substr($_COOKIE[XIVO_SESS_NAME],0,32);
			session_id($_COOKIE[XIVO_SESS_NAME]);
		}
		else if(isset($_QR[XIVO_SESS_NAME]) === true && $_QR[XIVO_SESS_NAME] !== '-')
		{
			if(strlen($_QR[XIVO_SESS_NAME]) > 32)
				$_QR[XIVO_SESS_NAME] = substr($_QR[XIVO_SESS_NAME],0,32);
			session_id($_QR[XIVO_SESS_NAME]);
		}

		session_start();

		$_QR[XIVO_SESS_NAME] = session_id();

		define('XIVO_SESS_ID',$_QR[XIVO_SESS_NAME]);
		define('XIVO_SESS_STR',XIVO_SESS_NAME.'='.XIVO_SESS_ID);
	}

	function set_dso_attrib($name,$value)
	{
		$this->_dso->set_attrib($name,$value);
	}

	function get_dso_attrib($name)
	{
		return($this->_dso->get_attrib($name));
	}
}
	
?>
