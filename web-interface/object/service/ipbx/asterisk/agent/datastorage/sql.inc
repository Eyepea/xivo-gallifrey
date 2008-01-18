<?php

xivo_service_asterisk::required(array('staticconf','datastorage','abstract','sql.inc'),true);

class xivo_service_asterisk_agent_sql extends xivo_service_asterisk_staticconf_abstract_sql
{
	var $_forcecolumn	= array('filename' => 'agents.conf');

	function get_all_by_var_metric($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$this->_dso->new_select($this->_table);

		$arr = $this->_forcecolumn;
		$arr['var_metric'] = $id;

		if($disable !== null)
			$arr[$this->_disable] = intval((bool) $disable);

		$this->_dso->where($arr);

		return($this->_dso->select_all());
	}

	function get_last_var_metric($min,$max)
	{
		$min = xivo_uint($min);
		$max = xivo_uint($max);

		$column = array();
		$column['cnt'] = (object) 'COUNT(*)';
		$column['var_metric'] = (object) 'MAX(var_metric)';

		$this->_dso->new_select($this->_table,$column);
		$this->_dso->where($this->_forcecolumn);
		$this->_dso->andwherebetween('var_metric',$min,$max);

		if(($r = $this->_dso->select_single()) !== false)
			$r = $r['cnt'] > 0 ? $r['var_metric'] : $min;

		return($r);
	}

	function get_free_var_metric($min,$max)
	{
		$this->_dso->new_select($this->_table,array('id','var_metric'));

		$arr = $this->_forcecolumn;
		$arr['var_name'] = 'deleted';
		$arr[$this->_disable] = 1;

		$this->_dso->where($arr);
		$this->_dso->andwherebetween('var_metric',xivo_uint($min),xivo_uint($max));

		return($this->_dso->select_single());
	}
}

?>
