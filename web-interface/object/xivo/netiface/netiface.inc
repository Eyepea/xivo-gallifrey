<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

xivo_object::required(array('abstract','abstract.inc'),true);

class xivo_object_netiface extends xivo_object_abstract
{
	var $_dso		= null;
	var $_name		= 'netiface';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_object_netiface(&$xobj,&$dso)
	{
		$this->xivo_object_abstract(&$xobj);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_dso = &$dso;
	}

	function _prepare($data)
	{
		$data['disable'] = (bool) $data['disable'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(empty($id) === true || isset($arr['dcreate']) === false)
			$arr['dcreate'] = gmmktime();

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr,$disable));
	}

	function exists_virtual($name)
	{
		return($this->_dso->exists_virtual($name));
	}
}

?>
