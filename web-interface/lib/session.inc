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

			register_shutdown_function(array(&$this->_dso,'write_close'));
		}

		session_set_cookie_params(XIVO_SESS_TIME,XIVO_SESS_PATH);
		session_name(XIVO_SESS_NAME);

		$this->_mk_cookie();
	}

	function _mk_cookie()
	{
		if(defined('XIVO_SESS_ID') === true)
			return(XIVO_SESS_ID);

		$_QR = &xivo_gat::get('_QR');

		if(xivo_haslen($_COOKIE,XIVO_SESS_NAME) === true)
			$id = substr($_COOKIE[XIVO_SESS_NAME],0,32);
		else if(xivo_haslen($_QR,XIVO_SESS_NAME) === true)
			$id = substr($_QR[XIVO_SESS_NAME],0,32);
		else
			$id = false;

		if(defined('XIVO_SESS_ENABLE') === true && (bool) XIVO_SESS_ENABLE === false)
		{
			$_SESSION = array();
			$_QR[XIVO_SESS_NAME] = md5(uniqid(rand(),true));
		}
		else
		{
			if($id !== false)
				session_id($id);

			session_start();

			if(($_QR[XIVO_SESS_NAME] = session_id()) === '')
			{
				session_regenerate_id();
				$_QR[XIVO_SESS_NAME] = session_id();
			}
		}

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
