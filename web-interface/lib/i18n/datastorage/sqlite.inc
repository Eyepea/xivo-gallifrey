<?php

class xivo_i18n_sqlite
{
	var $_param	= array('table'	=> 'i18n_cache');

	var $_dso	= null;

	function xivo_i18n_sqlite(&$dso,$param=array())
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
		$r = false;

		if(is_array($arr) === false)
			return($r);

		if(($arr['dcreate'] = xivo_uint($arr['dcreate'])) === 0)
			$arr['dcreate'] = time();

		if(($arr['dupdate'] = xivo_uint($arr['dupdate'])) === 0)
			$arr['dupdate'] = time();

		$arr['obj'] = serialize($arr['obj']);

		$r = $this->_dso->replace($this->_param['table'],$arr);

		return($r);
	}

	function get_cache($arr)
	{
		$r = false;

		if(is_array($arr) === false)
			return($r);

		$arr['filemtime'] = xivo_uint($arr['filemtime']);
 
		$rs = $this->_dso->query('SELECT dcreate, dupdate, obj FROM '.$this->_param['table'].
			' WHERE locale = \''.$this->_dso->escape_string($arr['locale']).'\''.
			' AND path = \''.$this->_dso->escape_string($arr['path']).'\' LIMIT 1');

		if($rs !== false && ($rw = $this->_dso->fetch_assoc($rs)) !== false)
		{
			$r = array();

			$r['filectime'] = $rw['dcreate'];

			if((int) $rw['dupdate'] === $arr['filemtime'])
				$r['obj'] = xivo_unserialize($rw['obj']);
		}

		return($r);
	}
}

?>
