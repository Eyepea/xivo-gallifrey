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

class xivo_object_netiface extends xivo_object_abstract
{
	var $_dso		= null;
	var $_sysnet		= null;
	var $_name		= 'netiface';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;
	var $_identity		= false;

	function xivo_object_netiface(&$xobj,&$dso)
	{
		$this->xivo_object_abstract(&$xobj);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_dso = &$dso;

		$sysinfo = new dwho_sysinfo();
		$this->_sysnet = $sysinfo->net();

		$this->_identity = &$this->_xobj->get_identity($this->_name);
	}

	function _prepare($data)
	{
		$this->_identity->_prepare_identity($data);

		$data['disable'] = (bool) $data['disable'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(empty($id) === true || isset($arr['dcreate']) === false)
			$arr['dcreate'] = gmstrftime('%s');

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		if(isset($arr['ifname']) === true)
			$arr['ifname'] = strtolower($arr['ifname']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		if(isset($arr['ifname']) === true)
			$arr['ifname'] = strtolower($arr['ifname']);

		return(parent::edit($id,$arr,$disable));
	}

	function exists_link($ifname)
	{
		return($this->_dso->exists_link($ifname));
	}
	
	function get_voip_iface()
	{ 
	    return($this->_dso->get_voip_iface());
	}
	
	function save($arr)
	{
		if($this->exists(null,null,1) === false)
			return($this->add($arr));

		return($this->edit(1,$arr));
	}
}

?>
