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

class xivo_service_asterisk_agentgroup extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'agentgroup';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_agentgroup(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		$data['deleted'] = (bool) $data['deleted'];
		$data['commented'] = (bool) $data['commented'];
		$data['deletable'] = (string) XIVO_SRE_IPBX_AST_AGENT_GROUP_DEFAULT !== (string) $data['id'];

		return($data);
	}

	function get($id,$disable=null,$delete=false)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(($id = dwho_ulongint($id)) === 0
		|| ($r = $this->_dso->get($id,$disable,$delete)) === false)
			return(false);
		else if(isset($this->_origin) === true)
			$this->_origin = $r;

		return($this->_prepare($r));
	}

	function get_primary($arr,$disable=null,$delete=false)
	{
		if(is_array($arr) === true
		&& ($arr = $this->get_element_where($arr)) === false)
			return(false);

		return($this->_dso->get_primary($arr,$disable,$delete));
	}

	function get_where($arr,$disable=null,$delete=false)
	{
		if(isset($this->_origin) === true)
			$this->_origin = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get($where,$disable,$delete)) === false)
			return(false);
		else if(isset($this->_origin) === true)
			$this->_origin = $r;

		return($this->_prepare($r));
	}

	function get_nb($arr=null,$disable=null,$delete=false)
	{
		if(($arr !== null && is_array($arr) === false) === true
		|| (is_array($arr) === true
		   && ($arr = $this->get_element_where($arr)) === false) === true)
			return(false);

		return($this->_dso->get_nb($arr,$disable,$delete));
	}

	function is_valid($id,$disable=null)
	{
		if(($id = dwho_ulongint($id)) === 0
		|| $this->get_primary($id,$disable) === false)
			return(false);

		return(true);
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
		$arr['commented'] = false;
		$arr['deleted'] = false;

		if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);

			if($this->get_primary(array('name' => $arr['name'])) !== false)
				return(false);
		}

		if($replace === true)
			return($this->_dso->edit($last,$arr));

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null,$delete=false)
	{
		if(($id = dwho_ulongint($id)) === 0 || is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);

			if(($whereid = $this->get_primary(array('name' => $arr['name']))) !== false
			&& $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable,$delete));
	}

	function edit_where($where,$arr,$disable=null,$delete=false,$id=null,$chkunique=true)
	{
		if(is_array($where) === false
		|| ($where = $this->get_element_where($where)) === false
		|| ($arr = $this->chk_values($arr,true,false,false)) === false)
			return(false);
		else if($id !== null)
		{
			if(($id = $this->_chk_primary_key($id)) === false)
				return(false);

			$where = array_merge($where,$id);
		}

		return($this->_dso->edit($where,$arr,$disable,$delete,$chkunique));
	}

	function delete($id,$disable=null)
	{
		return($this->edit($id,array('deleted' => true),$disable));
	}

	function delete_where($where,$disable=null)
	{
		return($this->edit_where($where,array('deleted' => true),$disable,false,null,false));
	}

	function recover($id,$disable=null)
	{
		return($this->edit($id,array('deleted' => false),$disable,true));
	}

	function recover_where($where,$disable=null)
	{
		return($this->edit_where($where,array('deleted' => true),$disable,true,null,false));
	}

	function get_list($disable=null,$delete=false,$order=null,$limit=null)
	{
		if(($r = $this->_dso->get_list($disable,$delete,$order,$limit)) !== false
		&& isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_list_where($arr,$disable=null,$delete=false,$order=null,$limit=null)
	{
		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get_list_where($where,$disable,$delete,$order,$limit)) === false
		|| isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_all($disable=null,$delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(($list = $this->_dso->get_all($disable,$delete,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);

		return($this->_mk_list_assoc_prepare($list,$assoc,$prepare));
	}

	function get_all_where($arr,$disable=null,$delete=false,$prepare=true,$order=null,$limit=null,$assoc=false)
	{
		$prepare = (bool) $prepare;
		$assoc = (bool) $assoc;

		if(isset($this->_origin_list) === true)
			$this->_origin_list = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable,$delete,$order,$limit)) === false
		|| isset($list[0]) === false)
			return(false);
		else if(isset($this->_origin_list) === true)
			$this->_origin_list = $list;

		if($prepare === false && $assoc === false)
			return($list);

		return($this->_mk_list_assoc_prepare($list,$assoc,$prepare));
	}
}

?>
