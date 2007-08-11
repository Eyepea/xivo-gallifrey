<?php

xivo_service_asterisk::required(array('staticconf','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agent_abstract_sql extends xivo_service_asterisk_staticconf_abstract_sql
{
	var $_param	= array('table' => 'agent');
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		parent::_init(&$dso,$param);
	}

	function get_all_by_var_metric($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$where = 'var_metric = '.$id.
			 ' AND filename = \''.$this->_dso->escape_string($this->_param['filename']).'\'';

		if($disable !== null)
			$where = ' AND commented = '.intval((bool) $disable);

		$r = $this->_dso->select_all($this->_param['table'],null,$where);

		return($r);
	}

	function get_last_var_metric($min,$max)
	{
		$min = xivo_uint($min);
		$max = xivo_uint($max);

		$r = $this->_dso->select_one($this->_param['table'],
					     'COUNT(*) AS cnt, MAX(var_metric) AS var_metric',
					     'filename = \''.$this->_dso->escape_string($this->_param['filename']).'\''.
					     ' AND var_metric BETWEEN '.(int) $min.' AND '.(int) $max);

		if($r !== false)
			$r = $r['cnt'] > 0 ? $r['var_metric'] : $min;

		return($r);
	}

	function get_free_var_metric($min,$max)
	{
		$min = xivo_uint($min);
		$max = xivo_uint($max);

		$r = $this->_dso->select_one($this->_param['table'],'id, var_metric',
					     'filename = \''.$this->_dso->escape_string($this->_param['filename']).'\''.
					     ' AND var_metric BETWEEN '.(int) $min.' AND '.(int) $max.
					     ' AND var_name = \'deleted\' AND commented = 1');

		return($r);
	}
}

?>
