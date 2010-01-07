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

dwho::load_class('dwho_network');

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','webservices','abstract.inc'),true);

class xivo_application_service_asterisk_webservices_phonebook extends xivo_application_service_asterisk_webservices_abstract
{
	var $_dso		= null;
	var $_accessfeatures	= false;
	var $_appxivoserver	= null;
	var $_appldapfilter	= null;
	var $_curl		= null;

	function xivo_application_service_asterisk_webservices_phonebook(&$service,&$dso)
	{
		if(is_object($dso) === false)
		{
			trigger_error('Invalid Datastorage object',E_USER_ERROR);
			return(false);
		}

		$this->_dso = &$dso;

		if($this->_init(&$service) === false
		|| ($this->_accessfeatures = &$this->_service->get_module('accessfeatures')) === false)
			return(false);

		return(true);
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function _load_xivoserver()
	{
		if(is_object($this->_curl) === false)
		{
			if(dwho::load_class('dwho_curl') === false)
				return(false);

			$this->_curl = new dwho_curl();
		}

		if(is_object($this->_appxivoserver) === false
		&& ($this->_appxivoserver = $this->_service->get_application('serverfeatures',
									     array('feature'	=> 'phonebook',
										   'type'	=> 'xivo'))) === false)
			return(false);

		return(true);
	}

	function _load_ldapfilter()
	{
		if(is_object($this->_appldapfilter) === false
		&& ($this->_appldapfilter = $this->_service->get_application('serverfeatures',
									     array('feature'	=> 'phonebook',
										   'type'	=> 'ldap'))) === false)
			return(false);

		return(true);
	}

	function chk_host_access($host)
	{
		if(($rs = $this->_accessfeatures->get_all_where(array('feature' => 'phonebook'),false)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if(dwho_network::ipv4_in_lhost($host,$rs[$i]['host']) === true)
				return($rs[$i]);
		}

		return(false);
	}

	function get_vendor_from_useragent()
	{
		if(isset($_SERVER['HTTP_USER_AGENT']) === false
		|| is_array($this->_conf['vendor']) === false
		|| empty($this->_conf['vendor']) === true)
			return(false);

		$useragent = strtolower($_SERVER['HTTP_USER_AGENT']);

		foreach($this->_conf['vendor'] as $key => $val)
		{
			if(strpos($useragent,$val) !== false)
				return($key);
		}

		return(false);
	}

	function chk_vendor($vendor)
	{
		$r = strval($vendor);

		if(isset($this->_conf['vendor'][$vendor]) === false)
			$r = false;

		return($r);
	}

	function get_phonebook_search($search)
	{
		if(($rs = $this->_dso->search($search)) === false || ($nb = count($rs)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
			$rs[$i]['identity'] = $rs[$i]['name'].' ('.$rs[$i]['type'].')';

		return($rs);
	}

	function get_user_search($search,$disable=null)
	{
		$appuser = &$this->_service->get_application('user',null,false);

		if(($rs = $appuser->get_users_search_number($search,null,null,$disable)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['phone'] = $rs[$i]['number'];

			if(($info['name'] = $rs[$i]['fullname']) === '')
				$info['name'] = $info['phone'];

			$info['type'] = 'local';
			$info['identity'] = $info['name'];

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_phonebook_search_from_xivoserver($query,$disable=null)
	{
		if($this->_load_xivoserver() === false
		|| ($rs = $this->_appxivoserver->get_servers_list($disable)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$option = &$this->_conf['xivoserver']['option'];

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$url = rtrim($rs[$i]['server']['url'],'/').'/'.ltrim($query,'/');

			if(($recv = $this->_parse_xivoserver_recv($this->_curl->load($url,$option))) !== false)
				$r = array_merge($r,$recv);

			$this->_curl->close();
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _parse_xivoserver_recv($recv)
	{
		if(dwho::load_class('dwho::file::csv') === false
		|| dwho_has_len($recv) === false)
			return(false);

		$recv = explode("\n",str_replace("\r",'',$recv));

		$beg = explode(': ',array_shift($recv));
		$end = explode(': ',array_pop($recv));

		if(isset($beg[0],$beg[1],$end[0],$end[1]) === false
		|| $beg[1] !== 'beg-data'
		|| $end[1] !== 'end-data'
		|| ($nb = count($recv)) === 0)
			return(false);

		$filecsv = new dwho_file_csv();

		if(($data = $filecsv->parse($recv,0,'|',null,false)) === false
		|| ($nb = count($data)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(is_array($data[$i]) === false
			|| isset($data[$i][0],$data[$i][1],$data[$i][2]) === false)
				continue;

			$ref = &$data[$i];

			$info['name'] = $ref[0];
			$info['phone'] = $ref[1];
			$info['type'] = $ref[2];
			$info['identity'] = $ref[0];

			if($info['type'] !== 'local')
				$info['identity'] .= ' ('.$ref[2].')';

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_phonebook_search_from_ldapfilter($query,$disable=null)
	{
		if(dwho_has_len($query) === false
		|| $this->_load_ldapfilter() === false
		|| ($rs = $this->_appldapfilter->get_servers_list($disable)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ldapfilter = &$rs[$i]['server']['ldapfilter'];
			$ldapserver = &$rs[$i]['server']['ldapserver'];

			if($ldapserver === false
			|| isset($ldapfilter['attrphonenumber'][0]) === false)
				continue;

			$ldapfilter['attrdisplayname'] = dwho_array_change_value_case($ldapfilter['attrdisplayname'],CASE_LOWER);
			$ldapfilter['attrphonenumber'] = dwho_array_change_value_case($ldapfilter['attrphonenumber'],CASE_LOWER);

			if(dwho_has_len($ldapfilter['filter']) === true)
			{
				$ldapserver['user'] = $ldapfilter['user'];
				$ldapserver['passwd'] = $ldapfilter['passwd'];

				$dso = dwho_datastorage::factory('ldap',$ldapserver);

				if(($filter = dwho_replacef('Q',$dso->escape_string($query),$ldapfilter['filter'])) === false)
				{
					$dso->close();
					continue;
				}
				else if($filter{0} !== '(' || substr($filter,-1) !== ')')
					$filter = '('.$filter.')';

				$search = array();
				$search['from'] = $ldapfilter['basedn'];
				$search['column'] = array_merge($ldapfilter['attrdisplayname'],$ldapfilter['attrphonenumber']);
				$search['where'] = '(&'.$filter.'(|('.implode('=*)(',$ldapfilter['attrdisplayname']).'=*))'.
						   '(|('.implode('=*)(',$ldapfilter['attrphonenumber']).'=*)))';

				if(($res = $dso->select_all($search,true,true)) !== false
				&& ($recv = $this->_prepare_ldapfilter_recv($res,
									    $ldapfilter['attrdisplayname'],
									    $ldapfilter['attrphonenumber'],
									    $ldapfilter['additionaltype'],
									    $ldapfilter['additionaltext'])) !== false)
					$r = array_merge($r,$recv);

				$dso->close();
			}
			else
			{
				$columns = array_merge($ldapfilter['attrdisplayname'],$ldapfilter['attrphonenumber']);

				$ldapserver['user'] = $ldapfilter['user'];
				$ldapserver['passwd'] = $ldapfilter['passwd'];

				$dso = dwho_datastorage::factory('ldap',$ldapserver);

				$dso->new_select($ldapfilter['basedn'],$columns);

				$cnt = count($columns);

				for($j = 0;$j < $cnt;$j++)
					$dso->orwherecontain($columns[$j],$query);

				$dso->aggregate_where();

				$cnt = count($ldapfilter['attrdisplayname']);

				$dso->andwherenotnull($ldapfilter['attrdisplayname'][0]);

				for($j = 1;$j < $cnt;$j++)
					$dso->orwherenotnull($ldapfilter['attrdisplayname'][$j]);

				$dso->aggregate_where();

				$cnt = count($ldapfilter['attrphonenumber']);

				$dso->andwherenotnull($ldapfilter['attrphonenumber'][0]);

				for($j = 1;$j < $cnt;$j++)
					$dso->orwherenotnull($ldapfilter['attrphonenumber'][$j]);

				if(($res = $dso->select_all(null,true,true)) !== false
				&& ($recv = $this->_prepare_ldapfilter_recv($res,
									    $ldapfilter['attrdisplayname'],
									    $ldapfilter['attrphonenumber'],
									    $ldapfilter['additionaltype'],
									    $ldapfilter['additionaltext'])) !== false)
					$r = array_merge($r,$recv);

				$dso->close();
			}
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _prepare_ldapfilter_recv($recv,$attrdisplayname,$attrphonenumber,$additionaltype,$additionaltext)
	{
		if(is_array($recv) === false
		|| is_array($attrdisplayname) === false
		|| is_array($attrphonenumber) === false
		|| ($nb = count($recv)) === 0
		|| ($namecnt = count($attrdisplayname)) === 0
		|| ($phonecnt = count($attrphonenumber)) === 0)
			return(false);

		$r = $info = array();

		if($additionaltype !== 'custom')
			$info['type'] = $additionaltype;
		else
			$info['type'] = $additionaltext;

		$info['additionaltype'] = $additionaltype;
		$info['additionaltext'] = $additionaltext;

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$recv[$i];

			for($j = 0;$j < $namecnt;$j++)
			{
				if(isset($ref[$attrdisplayname[$j]]) === true)
				{
					$info['name'] = $ref[$attrdisplayname[$j]][0];
					break;
				}
			}

			for($j = 0;$j < $phonecnt;$j++)
			{
				if(isset($ref[$attrphonenumber[$j]]) === true)
				{
					$info['phone'] = $ref[$attrphonenumber[$j]][0];
					break;
				}
			}

			if(isset($info['name'],$info['phone']) === false
			|| preg_match_all('/[\+\(\)\d]+/',$info['phone'],$match) === 0)
				continue;

			$info['phone'] = implode('',$match[0]);

			if(preg_match('/^\+(\d+)\(\d+\)(\d+)$/',$info['phone'],$match) === 1)
				$info['phone'] = '00'.$match[1].$match[2];

			if(preg_match_all('/\d+/',$info['phone'],$match) === 0)
				continue;

			$info['phone'] = implode('',$match[0]);

			$info['identity'] = $info['name'];

			if($info['type'] !== '')
				$info['identity'] .= ' ('.$info['type'].')';

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
