<?php

require_once(xivo_file::joinpath(XIVO_PATH_OBJECT,'user','abstract.inc'));

class xivo_user_admin extends xivo_user_abstract
{
	var $_param	= array('factory' => true);
	var $_authent	= null;

	function xivo_user_admin($factory=true)
	{
		$this->_ini = xivo_user::load_get_init();

		$this->_load_config(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user');
		$this->_load_dso();

		if(is_object($this->_dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_param['factory'] = (bool) $factory;

		if($this->_param['factory'] === false)
			$this->load_by_session();

		$this->_load_acl();
	}

	function edit($meta,$arr)
	{
		if(is_array($arr) === false
		|| isset($arr['id']) === false
		|| ($arr_filter = $this->_load_filter('edit',$meta)) === false)
			return(false);

		$filter = new xivo_filter($arr_filter);

		if($filter->chk($arr) === false)
			return(false);

		return($this->_dso->edit($arr['id'],$filter->get_result()));
	}
}

?>
