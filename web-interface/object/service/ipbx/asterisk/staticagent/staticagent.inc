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

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_staticagent extends xivo_service_asterisk_realstatic
{
	var $_filename	=		'agents.conf';
	var $_var_metric_range =	XIVO_SRE_IPBX_AST_AGENT_VAR_METRIC_RANGE;

	function _prepare($data)
	{
		if($data['category'] === 'agents'
		&& $data['var_name'] === 'agent')
		{
			$val = explode(',',$data['var_val'],3);

			$data['number'] = dwho_has_len($val,0) === true ? $val[0] : '';
			$data['passwd'] = dwho_has_len($val,1) === true ? $val[1] : '';
			$data['fullname'] = dwho_has_len($val,2) === true ? $val[2] : '';
		}

		return(parent::_prepare($data));
	}

	function get_last_var_metric($group)
	{
		if(($group = dwho_uint($group)) === 0)
			return(false);

		$min = $group * $this->_var_metric_range;
		$max = $min + $this->_var_metric_range - 1;

		return($this->_dso->get_last_var_metric($min,$max));
	}

	function get_free_var_metric($group)
	{
		if(($group = dwho_uint($group)) === 0)
			return(false);

		$var_metric_range = $this->_var_metric_range;

		$min = $group * $this->_var_metric_range;
		$max = $min + $this->_var_metric_range - 1;

		return($this->_dso->get_free_var_metric($min,$max));
	}

	function _chk_free_var_metric($var_metric)
	{
		if(($var_metric = dwho_uint($var_metric)) === 0
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
