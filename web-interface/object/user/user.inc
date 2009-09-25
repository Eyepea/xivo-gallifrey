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

class xivo_user_user extends xivo_user_abstract
{
	var $_param	= array('factory' => true);
	var $_authent	= null;

	function xivo_user_user($factory=true)
	{
		$this->_ini = xivo_user::load_get_init();

		$this->_load_config(XIVO_PATH_OBJECT.DWHO_SEP_DIR.'user');
		$this->_load_dso();

		if(is_object($this->_dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_param['factory'] = (bool) $factory;

		if($this->_param['factory'] === false)
			$this->load_by_session();

		$this->_load_acl();
	}

	function load_authent($type='')
	{
		$type = (string) $type;

		if($type === 'internal')
			return(parent::load_authent());

		$ini = xivo_user::load_get_init();

		if(dwho_issa('authent',$ini) === false)
			return(false);

		$authent = &$ini['authent'];

		if(isset($authent['type']) === false)
		{
			trigger_error('Missing authentication type in user init file',E_USER_ERROR);
			return(false);
		}

		$type = $authent['type'];

		$r = false;

		switch($type)
		{
			case 'internal':
				return(parent::load_authent());
			case 'service':
				// TODO: Authentication by service
				break;
			default:
				$name = 'xivo_authent_'.$type;
				$dir = XIVO_PATH_OBJECT.DWHO_SEP_DIR.'user'.DWHO_SEP_DIR.'authent';

				if(xivo::load_class($name,$dir,$type,false) === false)
				{
					trigger_error('Failed to load user authentication',E_USER_ERROR);
					return(false);
				}
				else if(isset($authent['datastorage']) === true)
				{
					$datastorage = &$authent['datastorage'];

					if(dwho_ak('type',$datastorage) === true && dwho_datastorage::is_valid($datastorage['type']) === true)
						return(new $name(dwho_datastorage::factory($type,$datastorage),$authent));
					else
					{
						trigger_error('Invalid user authentication datastorage',E_USER_ERROR);
						return(false);
					}
				}
				else
					return(new $name($authent));
		}

		return(false);
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
