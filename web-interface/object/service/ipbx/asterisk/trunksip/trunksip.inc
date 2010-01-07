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

class xivo_service_asterisk_trunksip extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'trunksip';
	var $_filter	= false;
	var $_origin	= false;
	var $_identity	= false;

	function xivo_service_asterisk_trunksip(&$sre,&$dso)
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
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		$arr['mailbox'] = null;
		$arr['subscribemwi'] = 0;
		$arr['buggymwi'] = null;
		$arr['fullname'] = null;
		$arr['cid_number'] = null;
		$arr['mohinterpret'] = null;
		$arr['mohsuggest'] = null;
		$arr['defaultip'] = null;
		$arr['callgroup'] = null;
		$arr['pickupgroup'] = null;
		$arr['setvar'] = '';
		$arr['regexten'] = null;
		$arr['subscribecontext'] = null;
		$arr['fullcontact'] = null;
		$arr['vmexten'] = null;
		$arr['callingpres'] = null;
		$arr['ipaddr'] = '';
		$arr['regseconds'] = 0;
		$arr['regserver'] = null;
		$arr['lastms'] = '';
		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'trunk';

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		unset($arr['mailbox'],
		      $arr['buggymwi'],
		      $arr['fullname'],
		      $arr['cid_number'],
		      $arr['mohinterpret'],
		      $arr['mohsuggest'],
		      $arr['defaultip'],
		      $arr['callgroup'],
		      $arr['pickupgroup'],
		      $arr['setvar'],
		      $arr['regexten'],
		      $arr['subscribecontext'],
		      $arr['fullcontact'],
		      $arr['vmexten'],
		      $arr['callingpres'],
		      $arr['ipaddr'],
		      $arr['regseconds'],
		      $arr['regserver'],
		      $arr['lastms']);

		$arr['subscribemwi'] = 0;
		$arr['setvar'] = '';
		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
		$arr['category'] = 'trunk';

		return(parent::edit($id,$arr,$disable));
	}
}

?>
