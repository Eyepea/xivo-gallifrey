<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

class xivo_object_resolvconf extends xivo_object_abstract
{
	var $_dso		= null;
	var $_name		= 'resolvconf';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_object_resolvconf(&$xobj,&$dso)
	{
		$this->xivo_object_abstract(&$xobj);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_dso = &$dso;
	}

	function _prepare($data)
	{
		if(dwho_has_len($data,'search') === true)
			$data['search'] = explode(' ',$data['search']);
		else
			$data['search'] = null;

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(empty($id) === false)
			$id = 1;

		$arr['id'] = 1;

		if(isset($arr['hostname']) === true)
			$arr['hostname'] = strtolower($arr['hostname']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		$id = 1;

		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['id']) === true)
			$arr['id'] = 1;

		if(isset($arr['hostname']) === true)
			$arr['hostname'] = strtolower($arr['hostname']);

		return(parent::edit($id,$arr));
	}

	function save($arr)
	{
		if($this->exists(null,null,1) === false)
			return($this->add($arr));

		return($this->edit(1,$arr));
	}
}

?>
