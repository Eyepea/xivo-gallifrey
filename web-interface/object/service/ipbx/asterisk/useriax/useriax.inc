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

class xivo_service_asterisk_useriax extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'useriax';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_useriax(&$sre,&$dso)
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
		$data['initialized'] = false;
		$data['commented'] = (bool) $data['commented'];

		if(($pos = strrpos($data['mailbox'],'@')) !== false)
		{
			$data['voicemailmailbox'] = substr($data['mailbox'],0,$pos);
			$data['voicemailcontext'] = substr($data['mailbox'],$pos + 1);
		}

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		$arr['type'] = 'friend';
		$arr['username'] = null;
		$arr['dbsecret'] = '';
		$arr['fullname'] = null;
		$arr['cid_number'] = null;
		$arr['trunk'] = 0;
		$arr['encryption'] = null;
		$arr['inkeys'] = null;
		$arr['outkey'] = null;
		$arr['transfer'] = null;
		$arr['timezone'] = null;
		$arr['mohinterpret'] = null;
		$arr['defaultip'] = null;
		$arr['sourceaddress'] = null;
		$arr['port'] = null;
		$arr['regexten'] = null;
		$arr['peercontext'] = null;
		$arr['ipaddr'] = '';
		$arr['regseconds'] = 0;
		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
		$arr['category'] = 'user';

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
			$arr['name'] = strtolower($arr['name']);

		if(isset($arr['setvar'],$arr['setvar']{0}) === false)
			unset($arr['setvar']);

		unset($arr['username'],
		      $arr['fullname'],
		      $arr['cid_number'],
		      $arr['encryption'],
		      $arr['inkeys'],
		      $arr['outkey'],
		      $arr['transfer'],
		      $arr['timezone'],
		      $arr['mohinterpret'],
		      $arr['defaultip'],
		      $arr['sourceaddress'],
		      $arr['port'],
		      $arr['regexten'],
		      $arr['peercontext'],
		      $arr['ipaddr'],
		      $arr['regseconds']);

		$arr['type'] = 'friend';
		$arr['dbsecret'] = '';
		$arr['trunk'] = 0;
		$arr['protocol'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
		$arr['category'] = 'user';

		return(parent::edit($id,$arr,$disable));
	}
}

?>
