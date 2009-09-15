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

require_once(xivo_file::joinpath(XIVO_PATH_OBJECT,'abstract','libdatastorage.inc'));

class xivo_entity extends xivo_libdatastorage_abstract
{
	var $_conf		= array();
	var $_dso		= null;
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_entity()
	{
		$this->_load_config(dirname(__FILE__).XIVO_SEP_DIR.'entity');
		$this->_load_dso();
	}

	function _prepare($data)
	{
		$data['disable'] = (bool) $data['disable'];
		$data['identity'] = $data['displayname'].' ('.$data['name'].')';

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

	function _load_dso()
	{
		$dso = &xivo_gct::get('XIVO_DSO');
		$type = $dso->get_type();
		$class = 'xivo_entity_'.$type;

		$dir = xivo_file::joinpath(XIVO_PATH_OBJECT,'entity','datastorage');

		if(xivo::load_class($class,$dir,$type,false) === false)
		{
			trigger_error('Failed to load datastorage '.$type.' for entity',E_USER_ERROR);
			return(false);
		}

		$this->_dso = new $class($dso);
		return(true);
	}

	function get_entities_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_entities('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_entities_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_entities('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_entities($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$order,$limit);
				$this->_cnt = $this->_dso->get_cnt();
				break;
			case 'list':
			default:
				$rs = $this->get_all($disable,true,$order,$limit,$assoc);
		}

		if(empty($rs) === true)
			return(false);

		$_SRE = &xivo_gct::get('xivo_service');
		$ipbx = &$_SRE->get('ipbx');

		$context = &$ipbx->get_module('context');

		$r = array();

		foreach($rs as $key => $val)
		{
			if(($nb_context = $context->get_nb(array('entity' => $val['name']))) !== false)
			{
				$r[$key] = array();
				$r[$key]['entity'] = $val;
				$r[$key]['nb_context'] = $nb_context;
			}
		}

		if(empty($r) === true)
			return(false);

		return($r);
	}
}

?>
