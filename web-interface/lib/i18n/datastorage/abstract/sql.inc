<?php

class xivo_i18n_abstract_sql
{
	var $_param	= array('table'	=> 'i18ncache');
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === true)
			$this->_param['table'] = (string) $param['table'];
	}

	function save_cache($arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(($arr['dcreate'] = xivo_uint($arr['dcreate'])) === 0)
			$arr['dcreate'] = time();

		if(($arr['dupdate'] = xivo_uint($arr['dupdate'])) === 0)
			$arr['dupdate'] = time();

		$arr['obj'] = serialize($arr['obj']);

		$r = $this->_dso->replace($this->_param['table'],$arr);

		return($r);
	}

	function get_cache($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$this->_dso->new_select($this->_param['table'],array('dcreate','dupdate','obj'));
		$this->_dso->where(array('locale' => $arr['locale'],
					 'path'	=> $arr['path']));

 		if(($r = $this->_dso->select_single()) !== false)
		{
			$r['filectime'] = $r['dcreate'];

			if((int) $r['dupdate'] === (int) $arr['filemtime'])
				$r['obj'] = xivo_unserialize($r['obj']);
		}

		return($r);
	}
}

?>
