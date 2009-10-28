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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_groupfeatures extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'groupfeatures';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;
	var $_identity		= false;

	function xivo_service_asterisk_groupfeatures(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
		$this->_identity = &$this->_sre->get_identity($this->_name);
	}

	function _prepare($data)
	{
		$this->_identity->_prepare_identity($data);

		$data['deleted'] = (bool) $data['deleted'];

		return($data);
	}

	function get($id,$delete=false)
	{
		return(parent::get($id,$delete));
	}

	function get_primary($arr,$delete=false)
	{
		return(parent::get_primary($arr,$delete));
	}

	function get_where($arr,$delete=false)
	{
		return(parent::get_where($arr,$delete));
	}

	function add($arr)
	{
		if(is_array($arr) === false || ($last = $this->_dso->get_last()) === false)
			return(false);

		$replace = false;

		$last++;

		if($last > 63)
		{
			if(($last = $this->_dso->get_free()) === false)
				return(false);

			$replace = true;
		}

		$arr['id'] = $last;
		$arr['deleted'] = false;

		if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);
			$where = array('name' => $arr['name']);

			if($this->get_primary($where) !== false)
				return(false);
		}

		if($replace === false)
			return($this->_dso->add($arr));
		else if($this->_dso->edit($last,$arr) !== false)
			return($last);

		return(false);
	}

	function edit($id,$arr,$delete=false)
	{
		if(($id = dwho_ulongint($id)) === 0 || is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);
			$where = array('name' => $arr['name']);

			if(($whereid = $this->get_primary($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$delete));
	}

	function edit_where($where,$arr,$delete=false,$id=null,$chkunique=true)
	{
		return(parent::edit_where($where,$arr,$delete,$id,$chkunique));
	}

	function delete($id)
	{
		return($this->edit($id,array('deleted' => true)));
	}

	function delete_where($where)
	{
		return(parent::edit_where($where,array('deleted' => true),null,null,false));
	}

	function recover($id)
	{
		return($this->edit($id,array('deleted' => false)));
	}

	function recover_where($where)
	{
		return(parent::edit_where($where,array('deleted' => false),null,null,false));
	}

	function get_nb($arr=null,$delete=false)
	{
		return(parent::get_nb($arr,$delete));
	}

	function get_list($delete=false,$order=null,$limit=null)
	{
		return(parent::get_list($delete,$order,$limit));
	}

	function get_list_where($arr,$delete=false,$order=null,$limit=null)
	{
		return(parent::get_list_where($arr,$delete,$order,$limit));
	}

	function get_all($delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		return(parent::get_all($delete,$prepare,$order,$limit,$assoc));
	}

	function get_all_where($arr,$delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		return(parent::get_all_where($arr,$delete,$prepare,$order,$limit,$assoc));
	}

	function get_all_except($id=0,$number=false,$order=null,$limit=null)
	{
		if(($id = dwho_ulongint($id)) === 0 && (bool) $number === false)
			return($this->get_all(false,true,$order,$limit));

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(($list = $this->_dso->get_all_except($id,$number,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		return($this->_mk_list_assoc_prepare($list,false,true));
	}
}

?>
