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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'abstract','libdatastorage.inc'));

class xivo_server extends xivo_libdatastorage_abstract
{
	var $_conf		= array();
	var $_dso		= null;
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_server()
	{
		$this->_load_config(dirname(__FILE__).DWHO_SEP_DIR.'server');
		$this->_load_dso();
	}

	function _prepare($data)
	{
		$data['ssl'] = (bool) $data['ssl'];
		$data['disable'] = (bool) $data['disable'];
		$data['identity'] = $data['name'].' ('.$data['host'].')';

		$data['url'] = array();

		if($data['ssl'] === true)
			$data['url']['scheme'] = 'https';
		else
			$data['url']['scheme'] = 'http';

		$data['url']['authority'] = array('host'	=> $data['host'],
						  'port'	=> $data['port']);

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

	function _load_dso()
	{
		$dso = &dwho_gct::get('DWHO_DSO');
		$type = $dso->get_type();
		$class = 'xivo_server_'.$type;
		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,'server','datastorage');

		if(xivo::load_class($class,$dir,$type,false) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for server',E_USER_ERROR);
			return(false);
		}

		$this->_dso = new $class($dso);
		return(true);
	}
}

?>
