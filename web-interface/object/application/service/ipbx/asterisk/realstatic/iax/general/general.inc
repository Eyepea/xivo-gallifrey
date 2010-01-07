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

class xivo_application_service_asterisk_realstatic_iax_general extends xivo_application_service_asterisk_realstatic_abstract
{
	var $_name		= 'general';
	var $_modulename	= 'staticiax';
	var $_category		= 'general';
	var $_cat_metric	= 0;
	var $_filename		= 'iax.conf';
	var $_contextmember	= false;
	var $_musiconhold	= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_realstatic_iax_general(&$service)
	{
		return($this->_init(&$service));
	}

	function _load_config()
	{
		if(($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'generaliax'))) === false)
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
			case 'regcontext':
				$this->_get_contextmember(null,'regcontext');

				$res = array();
				$res['generaliax-regcontext'] = array();
				$res['generaliax-regcontext']['context'] = $rs['var_val'];
				$res['generaliax-regcontext']['varname'] = 'regcontext';

				if($this->set_contextmember($res) === false
				|| $this->edit_contextmember() === false)
					return(false);
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
		else if(isset($arr['minregexpire']) === false
		&& isset($arr['maxregexpire']) === true)
			$arr['minregexpire'] = $arr['maxregexpire'];
		else if(isset($arr['minregexpire'],$arr['maxregexpire']) === true
		&& $arr['minregexpire'] > $arr['maxregexpire'])
			$arr['minregexpire'] = $arr['maxregexpire'];

		$this->get_musiconhold();

		if(isset($arr['mohinterpret'],$this->_musiconhold[$arr['mohinterpret']]) === false)
			$arr['mohinterpret'] = '';

		if(isset($arr['mohsuggest']) === true
		&& isset($this->_musiconhold[$arr['mohsuggest']]) === false)
			$arr['mohsuggest'] = '';

		return(parent::set_save_all($arr,$disable));
	}
}

?>
