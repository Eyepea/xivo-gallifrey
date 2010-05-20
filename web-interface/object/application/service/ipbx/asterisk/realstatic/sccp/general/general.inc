<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','realstatic','abstract.inc'),true);

class xivo_application_service_asterisk_realstatic_sccp_general extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name			= 'general';
	var $_modulename	= 'staticsccp';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'sccp.conf';
	var $_contextmember	= false;
	var $_musiconhold	= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_realstatic_sccp_general(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		if(($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'generalsccp'))) === false)
			return(false);

		return(parent::_load_config(dirname(__FILE__)));
	}

	function _chk_pre_save($rs)
	{
		if(is_array($rs) === false
		|| isset($rs['var_name'],$rs['var_val']) === false)
			return(null);

		switch($rs['var_name'])
		{
			case 'localnet':
				$result = $this->get_result($this->_name);

				if(is_array($result) === false
				|| isset($result[0],$result[0]['var_name']) === false)
					return(true);
				else if($result[0]['var_name'] !== $rs['var_name'])
					return(false);

				$res = $tmp = array();

				$nb = count($result);

				for($i = 0;$i < $nb;$i++)
				{
					$value = strtolower($result[$i]['var_val']);

					$result[$i]['var_val'] = $value;

					if(isset($tmp[$value]) === false)
						$tmp[$value] = 1;
					else
						unset($result[$i]);
				}

				$this->_set_result($this->_name,array_values($result));
				break;
			case 'allow':
				if(is_array($rs['var_val']) === true)
				{
					$rs['var_val'] = implode(',',$rs['var_val']);
					$this->_set_result($this->_name,$rs);
				}
				break;
		}

		return(true);
	}

	function set_save_all($arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		return(parent::set_save_all($arr,$disable));

	}
}

?>
