<?php

class xivo_i18n_mysql
{
	var $_param	= array('table'	=> 'i18n_cache');

	var $_dso	= null;

	function xivo_i18n_mysql(&$dso,$param=array())
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

		$this->_dso->lock($this->_param['table']);

		$r = $this->_dso->replace($this->_param['table'],$arr);

		$this->_dso->unlock($this->_param['table']);

		return($r);
	}

	function get_cache($arr)
	{
		$r = false;

		if(is_array($arr) === false)
			return($r);

		$arr['filemtime'] = xivo_uint($arr['filemtime']);
 
		$rs = $this->_dso->query('SELECT dcreate, dupdate,'.
			' IF(dupdate='.$arr['filemtime'].',obj,null) AS obj'.
			' FROM '.$this->_param['table'].
			' WHERE locale = \''.$this->_dso->escape_string($arr['locale']).'\''.
			' AND path = \''.$this->_dso->escape_string($arr['path']).'\' LIMIT 1');

		if($rs !== false)
		{
			if($this->_dso->num_rows($rs) === 1)
			{
				$rw = $this->_dso->fetch_assoc($rs);

				$r = array();

				$r['filectime'] = $rw['dcreate'];

				if($rw['obj'] !== null)
					$r['obj'] = xivo_unserialize($rw['obj']);
			}

			$this->_dso->free_result($rs);
		}

		return($r);
	}
}

?>
