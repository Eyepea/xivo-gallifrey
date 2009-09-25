<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'user','abstract.inc'));

class xivo_user_visitor extends xivo_user_abstract
{
	var $_param	= array('factory' => true);
	var $_authent	= null;
	var $_dso	= null;
	var $_ini	= array();

	function xivo_user_visitor($factory=true)
	{
		$factory = false;

		$this->_ini = xivo_user::load_get_init();

		$this->_load_config(XIVO_PATH_OBJECT.DWHO_SEP_DIR.'user');
		$this->_load_dso();

		if(is_object($this->_dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_param['factory'] = (bool) $factory;

		if($this->_param['factory'] === false)
			$this->load_by_session();
	}

	function call_auth($func,$args=null)
	{
		if($this->_param['factory'] === true)
			trigger_error('Unable to call authentication with factory',E_USER_ERROR);

		if(is_object($this->_authent) === false)
		{
			$user = &xivo_user::factory('user');

			if(($this->_authent = $user->load_authent()) === false)
				trigger_error('Failed to load user authentication in '.__CLASS__,E_USER_ERROR);
		}

		return(parent::call_auth($func,$args));
	}

	function auth_chk($login,$pass)
	{
		if($this->_ini['authent']['type'] !== 'internal' && $this->_authent->chk($login,$pass) === true)
			return(XIVO_USER__USER);

		$this->_authent = $this->load_authent('internal');

		if(($type = xivo_user::meta_to_type($this->_authent->chk($login,$pass,true))) > 0)
			return($type);

		return(false);
	}
}

?>
