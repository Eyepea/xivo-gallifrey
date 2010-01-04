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

class xivo_service_asterisk_meetmefeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'meetmefeatures';
	var $_filter	= false;
	var $_origin	= false;
	var $_identity	= false;

	function xivo_service_asterisk_meetmefeatures(&$sre,&$dso)
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

		$data['admin_announceusercount']	= (bool) $data['admin_announceusercount'];
		$data['admin_moderationmode']		= (bool) $data['admin_moderationmode'];
		$data['admin_initiallymuted']		= (bool) $data['admin_initiallymuted'];
		$data['admin_poundexit']		= (bool) $data['admin_poundexit'];
		$data['admin_quiet']			= (bool) $data['admin_quiet'];
		$data['admin_starmenu']			= (bool) $data['admin_starmenu'];
		$data['admin_closeconflastmarkedexit']	= (bool) $data['admin_closeconflastmarkedexit'];
		$data['admin_enableexitcontext']	= (bool) $data['admin_enableexitcontext'];
		$data['user_announceusercount']		= (bool) $data['user_announceusercount'];
		$data['user_hiddencalls']		= (bool) $data['user_hiddencalls'];
		$data['user_initiallymuted']		= (bool) $data['user_initiallymuted'];
		$data['user_poundexit']			= (bool) $data['user_poundexit'];
		$data['user_quiet']			= (bool) $data['user_quiet'];
		$data['user_starmenu']			= (bool) $data['user_starmenu'];
		$data['user_enableexitcontext']		= (bool) $data['user_enableexitcontext'];
		$data['talkeroptimization']		= (bool) $data['talkeroptimization'];
		$data['record']				= (bool) $data['record'];
		$data['talkerdetection']		= (bool) $data['talkerdetection'];
		$data['noplaymsgfirstenter']		= (bool) $data['noplaymsgfirstenter'];
		$data['closeconfdurationexceeded']	= (bool) $data['closeconfdurationexceeded'];

		if(dwho_has_len($data['startdate']) === true)
			$data['startdate'] = getdate(strtotime($data['startdate']));

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		return(parent::edit($id,$arr));
	}

	function valid_admin_identification($typefrom,$mode,$calleridnum,$pinadmin)
	{
		if($typefrom === 'none')
			return(true);

		switch($mode)
		{
			case 'calleridnum':
				if($typefrom === 'undefined'
				|| dwho_has_len($calleridnum) === false)
					return(false);
				break;
			case 'pin':
				if(dwho_has_len($pinadmin) === false)
					return(false);
				break;
			case 'all':
				if($typefrom === 'undefined'
				|| dwho_has_len($calleridnum) === false
				|| dwho_has_len($pinadmin) === false)
					return(false);
				break;
			default:
				return(false);
		}

		return(true);
	}

	function normalize_admin_typefrom($typefrom,$internalid=null,$externalid=null)
	{
		$r = array('admin_typefrom'	=> 'none',
			   'admin_internalid'	=> null,
			   'admin_externalid'	=> null);

		switch($typefrom)
		{
			case 'internal':
				if(($internalid = dwho_ulongint($internalid)) === 0)
					return(false);

				$r['admin_internalid'] = $internalid;
				break;
			case 'external':
				if(dwho_has_len($externalid) === false)
					return(false);

				$r['admin_externalid'] = $externalid;
				break;
			case 'undefined':
				break;
			case 'none':
			default:
				$typefrom = 'none';
		}

		$r['admin_typefrom'] = $typefrom;

		return($r);
	}

	function get_all_except($id=0,$number=false,$order=null,$limit=null)
	{
		if(($id = dwho_ulongint($id)) === 0 && (bool) $number === false)
			return($this->get_all(null,true,$order,$limit));

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
