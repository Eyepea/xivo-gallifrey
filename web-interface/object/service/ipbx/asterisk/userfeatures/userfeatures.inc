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

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_userfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'userfeatures';
	var $_filter	= false;
	var $_origin	= false;
	var $_identity	= false;

	function xivo_service_asterisk_userfeatures(&$sre,&$dso)
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

		$data['enableclient'] = (bool) $data['enableclient'];
		$data['enablehint'] = (bool) $data['enablehint'];
		$data['enablevoicemail'] = (bool) $data['enablevoicemail'];
		$data['enablexfer'] = (bool) $data['enablexfer'];
		$data['enableautomon'] = (bool) $data['enableautomon'];
		$data['callrecord'] = (bool) $data['callrecord'];
		$data['incallfilter'] = (bool) $data['incallfilter'];
		$data['enablednd'] = (bool) $data['enablednd'];
		$data['enablerna'] = (bool) $data['enablerna'];
		$data['enablebusy'] = (bool) $data['enablebusy'];
		$data['enableunc'] = (bool) $data['enableunc'];
		$data['internal'] = (bool) $data['internal'];
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function get($id,$orig=true,$disable=null)
	{
		$orig = (bool) $orig;
		$origin = $this->_origin;

		if($orig === false)
			$this->_origin = null;

		if(($r = parent::get($id,$disable)) === false
		|| $this->_sre->chk_protocol($r['protocol']) === false)
		{
			$this->_origin = $origin;
			return(false);
		}
		else if($orig === false)
			$this->_origin = $origin;

		return($this->_prepare($r));
	}

	function get_where($arr,$orig=true,$disable=null,$id=null)
	{
		$orig = (bool) $orig;
		$origin = $this->_origin;

		if($orig === false)
			$this->_origin = null;

		if(($r = parent::get_where($arr,$disable,$id)) === false)
		{
			$this->_origin = $origin;
			return(false);
		}
		else if($orig === false)
			$this->_origin = $origin;

		return($r);
	}

	function _mk_provisioning_id($protocol)
	{
		if($this->_chk_exists_cfg('provisioning_id',$protocol) === false)
			return(null);

		$ref = &$this->_conf['provisioning_id'][$protocol];

		$min = $ref['prefix'] * $ref['range'];
		$max = $min + $ref['range'];
		$len = strlen($max)-strlen($ref['prefix']);

		$r = $ref['prefix'].substr(crc32(uniqid(rand(),true)),1,$len);

		if($r > $min && $r < $max && $this->get_primary(array('provisioningid' => $r)) === false)
			return($r);

		return($this->_mk_provisioning_id($protocol));
	}

	function loginclient_exists($loginclient,$context,$id=null)
	{
		if(dwho_has_len($loginclient) === false
		|| dwho_has_len($context) === false)
			return(null);

		$arr = array();
		$arr['loginclient'] = strtolower($loginclient);
		$arr['context'] = $context;

		if(($primary = $this->get_primary($arr)) === false
		|| (empty($id) === false
		   && dwho_ulongint($primary) === dwho_ulongint($id)) === true)
			return(false);

		return(true);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false
		|| isset($arr['protocol']) === false
		|| ($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false
		|| $this->_sre->chk_protocol($arr['protocol']) === false)
			return(false);
		else if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);

			if((isset($arr['destrna']) === true
			   && $arr['name'] === $arr['destrna']) === true
			|| (isset($arr['destbusy']) === true
			   && $arr['name'] === $arr['destbusy']) === true
			|| (isset($arr['destunc']) === true
			   && $arr['name'] === $arr['destunc']) === true)
				return(false);
		}

		if(isset($arr['loginclient'],$arr['context']) === true)
		{
			if($this->loginclient_exists($arr['loginclient'],
						     $arr['context'],
						     $id) === true)
				return(false);

			$arr['loginclient'] = strtolower($arr['loginclient']);
		}

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$provisioning=false,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);

			if((isset($arr['destrna']) === true
			   && $arr['name'] === $arr['destrna']) === true
			|| (isset($arr['destbusy']) === true
			   && $arr['name'] === $arr['destbusy']) === true
			|| (isset($arr['destunc']) === true
			   && $arr['name'] === $arr['destunc']) === true)
				return(false);
		}

		if(isset($arr['loginclient'],$arr['context']) === true)
		{
			if($this->loginclient_exists($arr['loginclient'],
						     $arr['context'],
						     $id) === true)
				return(false);

			$arr['loginclient'] = strtolower($arr['loginclient']);
		}

		if(isset($arr['protocol']) === true)
		{
			if((bool) $provisioning === true
			&& ($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false)
				return(false);

			if($this->_sre->chk_protocol($arr['protocol']) === false)
				return(false);
		}

		return(parent::edit($id,$arr,$disable));
	}

	function search($value,$disable=null,$order=null,$limit=null,$number=false)
	{
		if(($list = $this->_dso->search($value,$disable,$order,$limit,$number)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function search_number($value,$disable=null,$order=null,$limit=null)
	{
		return($this->search($value,$disable,$order,$limit,true));
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

	function get_all_context($disable=null,$order=null,$limit=null)
	{
		if(($list = $this->_dso->get_all_context($disable,$order,$limit)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			$r[$info['context']] = $info['contextdisplayname'];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
