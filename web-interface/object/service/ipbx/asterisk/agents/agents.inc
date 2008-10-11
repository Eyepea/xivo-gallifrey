<?php

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_agents extends xivo_service_asterisk_realstatic
{
	var $_filename	=		'agents.conf';
	var $_var_metric_range =	XIVO_SRE_IPBX_AST_AGENT_VAR_METRIC_RANGE;

	function _prepare($data)
	{
		if($data['category'] === 'agents'
		&& $data['var_name'] === 'agent')
		{
			$val = explode(',',$data['var_val'],3);

			$data['number'] = xivo_haslen($val,0) === true ? $val[0] : '';
			$data['passwd'] = xivo_haslen($val,1) === true ? $val[1] : '';
			$data['fullname'] = xivo_haslen($val,2) === true ? $val[2] : '';
		}

		return(parent::_prepare($data));
	}

	function get_last_var_metric($group)
	{
		if(($group = xivo_uint($group)) === 0)
			return(false);

		$min = $group * $this->_var_metric_range;
		$max = $min + $this->_var_metric_range - 1;

		return($this->_dso->get_last_var_metric($min,$max));
	}

	function get_free_var_metric($group)
	{
		if(($group = xivo_uint($group)) === 0)
			return(false);

		$var_metric_range = $this->_var_metric_range;

		$min = $group * $this->_var_metric_range;
		$max = $min + $this->_var_metric_range - 1;

		return($this->_dso->get_free_var_metric($min,$max));
	}

	function _chk_free_var_metric($var_metric)
	{
		if(($var_metric = xivo_uint($var_metric)) === 0
		|| $var_metric < $this->_var_metric_range)
			return(false);

		$arr = array();
		$arr['var_metric'] = $var_metric;
		$arr['var_name'] = 'deleted';
		$arr['commented'] = true;

		return($this->get_where($arr));
	}
}

?>
