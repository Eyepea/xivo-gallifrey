<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'user'.XIVO_SEP_DIR.'abstract.inc');

class xivo_user_admin extends xivo_user_abstract
{
	var $_param	= array('factory' => true);
	var $_authent	= null;

	function xivo_user_admin($factory=true)
	{
		$this->_ini = xivo_user::load_get_init();

		$this->_load_dso();

		if(is_object($this->_dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$factory = (bool) $factory;

		$this->_param['factory'] = $factory;

		if($factory === false)
			$this->load_by_session();

		$this->_load_policy();
	}

	function get_list($arr=array())
	{
		$r = $this->_dso->get_list($arr);

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function edit($meta,$arr)
	{
		$r = false;

		if(is_array($arr) === false || isset($arr['id']) === false)
			return($r);

		if(($arr_filter = $this->_load_filter('edit',$meta)) === false)
			return($r);

		$filter = new xivo_filter($arr_filter);

		if($filter->chk($arr) === false)
			return($r);

		$r = $filter->get_result();
		
		return($this->_dso->edit($arr['id'],$r));
	}
}

?>
