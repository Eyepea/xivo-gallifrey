<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

class dwho_session
{
	var $_dso	= null;

	function dwho_session()
	{
		if(defined('DWHO_SESS_HANDLER') === false)
		{
			define('DWHO_SESS_HANDLER',true);

			if(($dso = &dwho_gct::get('DWHO_DSO')) !== false)
			{
				$name = 'dwho_session_'.$dso->get_type();

				if(dwho::load_class('dwho::session::'.$dso->get_type()) === false)
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
			else
				session_module_name('files');
		}

		session_set_cookie_params(DWHO_SESS_TIME,DWHO_SESS_PATH);
		session_name(DWHO_SESS_NAME);

		$this->_mk_cookie();
	}

	function _mk_cookie()
	{
		if(defined('DWHO_SESS_ID') === true)
			return(DWHO_SESS_ID);

		$_QR = &dwho_gat::get('_QR');

		if(dwho_has_len($_COOKIE,DWHO_SESS_NAME) === true)
			$id = substr($_COOKIE[DWHO_SESS_NAME],0,32);
		else if(dwho_has_len($_QR,DWHO_SESS_NAME) === true)
			$id = substr($_QR[DWHO_SESS_NAME],0,32);
		else
			$id = false;

		if(defined('DWHO_SESS_ENABLE') === true && (bool) DWHO_SESS_ENABLE === false)
		{
			$_SESSION = array();
			$_QR[DWHO_SESS_NAME] = md5(uniqid(rand(),true));
		}
		else
		{
			if($id !== false)
				session_id($id);

			session_start();

			if(($_QR[DWHO_SESS_NAME] = session_id()) === '')
			{
				session_regenerate_id();
				$_QR[DWHO_SESS_NAME] = session_id();
			}
		}

		define('DWHO_SESS_ID',$_QR[DWHO_SESS_NAME]);
		define('DWHO_SESS_STR',DWHO_SESS_NAME.'='.DWHO_SESS_ID);
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
