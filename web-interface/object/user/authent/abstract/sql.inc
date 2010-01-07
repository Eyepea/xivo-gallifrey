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

class xivo_authent_abstract_sql
{
	var $_param	= array('table' => 'user');
	var $_attrib	= array();
	var $_dso	= null;

	function _init(&$dso,$param=array())
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = (array) $param;

		if(isset($param['table']) === false)
			$param['table'] = $this->_param['table'];

		$this->_param = (array) $param;
	}

	function set_attrib($name,$value)
	{
		$name = (string) $name;
		$this->_attrib[$name] = $value;
	}

	function get_attrib($name)
	{
		$name = (string) $name;

		if(isset($this->_attrib[$name]) === true)
			return($this->_attrib[$name]);

		return(null);
	}

	function chk($login,$pass,$meta=false)
	{
		$meta = (bool) $meta;

		$column = array('login','passwd');

		if($meta === true)
			$column[] = 'meta';

		$this->_dso->new_select($this->_param['table'],$column);
		$this->_dso->where(array('login' => $login,'valid' => 1));

		if(($r = $this->_dso->select_single()) === false)
			return(false);
		else if($r['login'] !== $login || $r['passwd'] !== $pass)
			$r = false;
		else
			$r = $meta === true ? $r['meta'] : true;

		return($r);
	}
}

?>
