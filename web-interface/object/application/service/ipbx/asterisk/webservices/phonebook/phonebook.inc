<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','webservices','abstract.inc'),true);

class xivo_application_service_asterisk_webservices_phonebook extends xivo_application_service_asterisk_webservices_abstract
{
	var $_dso		= null;
	var $_accessfeatures	= null;
	var $_appxivoserver	= false;
	var $_appldapserver	= false;
	var $_curl		= false;
	var $_phonebook		= false;
	var $_phonebooknumber	= false;

	function xivo_application_service_asterisk_webservices_phonebook(&$service,&$dso)
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if($this->_init(&$service) === false
		|| ($this->_accessfeatures = &$this->_service->get_module('accessfeatures')) === false
		|| xivo::load_class('xivo_filter') === false)
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
			if(xivo::load_class('xivo_curl') === false)
				return(false);

			$this->_curl = new xivo_curl();
		}

		if(is_object($this->_appxivoserver) === false
		&& ($this->_appxivoserver = $this->_service->get_application('serverfeatures',
									    array('feature'	=> 'phonebook',
									    	  'type'	=> 'xivo'))) === false)
			return(false);

		return(true);
	}

	function _load_ldapserver()
	{
		if(is_object($this->_appldapserver) === false
		&& ($this->_appldapserver = $this->_service->get_application('serverfeatures',
									    array('feature'	=> 'phonebook',
									    	  'type'	=> 'ldap'))) === false)
			return(false);

		return(true);
	}

	function chk_host_access($ip)
	{
		if(($rs = $this->_accessfeatures->get_all_where(array('feature' => 'phonebook'),false)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if(xivo_filter::chk_ipv4_in_lhost($ip,$rs[$i]['host']) === true)
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

		if(($rs = $appuser->get_users_search_number($search,$disable)) === false
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
		if(xivo::load_class('xivo::file::csv') === false
		|| xivo_haslen($recv) === false)
			return(false);

		$recv = explode("\n",str_replace("\r",'',$recv));

		$beg = explode(': ',array_shift($recv));
		$end = explode(': ',array_pop($recv));

		if(isset($beg[0],$beg[1],$end[0],$end[1]) === false
		|| $beg[1] !== 'beg-data'
		|| $end[1] !== 'end-data'
		|| ($nb = count($recv)) === 0)
			return(false);

		$filecsv = new xivo_file_csv();

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

	function get_phonebook_search_from_ldapserver($query,$disable=null)
	{
		if(xivo_haslen($query) === false
		|| $this->_load_ldapserver() === false
		|| ($rs = $this->_appldapserver->get_servers_list($disable)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$option = &$this->_conf['ldapserver']['option'];
		
		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ldapserver = &$rs[$i]['server']['ldapserver'];
			$xldapserver = &$rs[$i]['server']['xldapserver'];

			if($xldapserver === false
			|| isset($ldapserver['attrphonenumber'][0]) === false)
				continue;
			else if(xivo_haslen($ldapserver['filter']) === true)
			{
				$xldapserver['user'] = $ldapserver['user'];
				$xldapserver['passwd'] = $ldapserver['passwd'];

				$dso = xivo_datastorage::factory('ldap',$xldapserver);

				if(($filter = xivo_replacef('Q',$dso->escape_string($query),$ldapserver['filter'])) === false)
				{
					$dso->close();
					continue;
				}
				else if($filter{0} !== '(' || substr($filter,-1) !== ')')
					$filter = '('.$filter.')';

				$search = array();
				$search['from'] = $ldapserver['basedn'];
				$search['column'] = array_merge($ldapserver['attrdisplayname'],$ldapserver['attrphonenumber']);
				$search['where'] = '(&'.$filter.'(|('.implode('=*)(',$ldapserver['attrdisplayname']).'=*))'.
						   '(|('.implode('=*)(',$ldapserver['attrphonenumber']).'=*)))';

				if(($res = $dso->select_all($search,true,true)) !== false
				&& ($recv = $this->_prepare_ldapserver_recv($res,
									    $ldapserver['attrdisplayname'],
									    $ldapserver['attrphonenumber'],
									    $ldapserver['additionaltype'],
									    $ldapserver['additionaltext'])) !== false)
					$r = array_merge($r,$recv);

				$dso->close();
			}
			else
			{
				$columns = array_merge($ldapserver['attrdisplayname'],$ldapserver['attrphonenumber']);

				$xldapserver['user'] = $ldapserver['user'];
				$xldapserver['passwd'] = $ldapserver['passwd'];

				$dso = xivo_datastorage::factory('ldap',$xldapserver);

				$dso->new_select($ldapserver['basedn'],$columns);

				$cnt = count($columns);

				for($j = 0;$j < $cnt;$j++)
					$dso->orwherecontain($columns[$j],$query);

				$dso->aggregate_where();

				$cnt = count($ldapserver['attrdisplayname']);

				$dso->andwherenotnull($ldapserver['attrdisplayname'][0]);
			
				for($j = 1;$j < $cnt;$j++)
					$dso->orwherenotnull($ldapserver['attrdisplayname'][$j]);

				$dso->aggregate_where();

				$cnt = count($ldapserver['attrphonenumber']);

				$dso->andwherenotnull($ldapserver['attrphonenumber'][0]);
			
				for($j = 1;$j < $cnt;$j++)
					$dso->orwherenotnull($ldapserver['attrphonenumber'][$j]);

				if(($res = $dso->select_all(null,true,true)) !== false
				&& ($recv = $this->_prepare_ldapserver_recv($res,
									    $ldapserver['attrdisplayname'],
									    $ldapserver['attrphonenumber'],
									    $ldapserver['additionaltype'],
									    $ldapserver['additionaltext'])) !== false)
					$r = array_merge($r,$recv);

				$dso->close();
			}
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function _prepare_ldapserver_recv($recv,$attrdisplayname,$attrphonenumber,$additionaltype,$additionaltext)
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

			if(isset($info['name'],$info['phone']) === false)
				continue;

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
