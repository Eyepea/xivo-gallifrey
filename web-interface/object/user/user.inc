<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user'.XIVO_SEP_DIR.'abstract.inc');

class xivo_user_user extends xivo_user_abstract
{
	var $_param	= array('factory' => true);
	var $_authent	= null;

	function xivo_user_user($factory=true)
	{
		$this->_ini = xivo_user::load_get_init();

		$this->_load_dso();

		if(is_object($this->_dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$factory = (bool) $factory;

		$this->_param['factory'] = $factory;

		if($factory === false)
			$this->load_by_session();

		$this->_load_acl();
	}

	function load_authent($type='')
	{
		$r = false;

		$type = (string) $type;

		if($type === 'internal')
			return(parent::load_authent());

		$ini = xivo_user::load_get_init();

		if(xivo_issa('authent',$ini) === false)
			return($r);

		$authent = &$ini['authent'];

		if(isset($authent['type']) === false)
			trigger_error('Missing authentification type in user ini file',E_USER_ERROR);

		$type = $authent['type'];

		switch($type)
		{
			case 'internal':
				$r = parent::load_authent();
				break;
			case 'service':
				// TODO: Authentification par service
				break;
			default:
				$name = 'xivo_authent_'.$type;
				$dir = XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user'.XIVO_SEP_DIR.'authent';

				if(xivo::load_class($name,$dir,$type,false) === false)
					trigger_error('Failed to load user authentification',E_USER_ERROR);

				if(isset($authent['datastorage']) === true)
				{
					$datastorage = &$authent['datastorage'];

					if(xivo_ak('type',$datastorage) === true && xivo_datastorage::is_valid($datastorage['type']) === true)
						$r = new $name(xivo_datastorage::factory($type,$datastorage),$authent);	
					else
						trigger_error('Invalid user authentification datastorage',E_USER_ERROR);
				}
				else
					$r = new $name($authent);
		}

		return($r);
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
