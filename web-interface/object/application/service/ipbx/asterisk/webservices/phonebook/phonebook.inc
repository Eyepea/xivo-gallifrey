<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','webservices','abstract.inc'),true);

class xivo_application_service_asterisk_webservices_phonebook extends xivo_application_service_asterisk_webservices_abstract
{
	var $_accessfeatures	= null;
	var $_appserverfeatures	= null;
	var $_curl		= null;
	var $_phonebook		= false;
	var $_phonebooknumber	= false;

	function xivo_application_service_asterisk_webservices_phonebook(&$service)
	{
		$where = array();
		$where['type'] = 'phonebook';

		if($this->_init(&$service) === false
		|| ($this->_accessfeatures = &$this->_service->get_module('accessfeatures')) === false
		|| xivo::load_class('xivo::filter') === false
		|| xivo::load_class('xivo::curl') === false
		|| ($this->_appserverfeatures = &$this->_service->get_application('serverfeatures',$where)) === false)
			return(false);

		$this->_curl = new xivo_curl();

		return(true);
	}

	function _load_config()
	{
		return(parent::_load_config(dirname(__FILE__)));
	}

	function chk_host_access($ip)
	{
		if(($rs = $this->_accessfeatures->get_all_where(array('type' => 'phonebook'),false)) === false
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
		|| ($arr = xivo_get_aks($this->_conf['vendor'])) === false)
			return(false);

		$useragent = strtolower($_SERVER['HTTP_USER_AGENT']);
		
		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_conf['vendor'][$key];

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
		if((is_object($this->_phonebook) === false
		   && ($this->_phonebook = &$this->_service->get_module('phonebook')) === false) === true
		|| (is_object($this->_phonebooknumber) === false
		   && ($this->_phonebooknumber = &$this->_service->get_module('phonebooknumber')) === false) === true)
		   	return(false);

		$list = array();

		if(($rsn = $this->_phonebooknumber->get_search($search)) !== false
		&& ($nb = count($rsn)) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
				$list[] = $rsn[$i]['phonebookid'];
		}

		if(($rs = $this->_phonebook->get_search_name($search,$list)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = $info = $where = array();

		for($i = 0;$i < $nb;$i++)
		{
			$phonebook = $rs[$i];
			$where['phonebookid'] = $phonebook['id'];

			if(($phonebooknumber = $this->_phonebooknumber->get_all_where($where)) === false
			|| ($cnt = count($phonebooknumber)) === 0)
				continue;

			for($j = 0;$j < $cnt;$j++)
			{
				if(($info['type'] = $phonebooknumber[$j]['type']) === 'fax')
					continue;

				$info['name'] = $phonebook['displayname'];
				$info['phone'] = $phonebooknumber[$j]['number'];
				$info['identity'] = $info['name'].' ('.$info['type'].')';
				$r[] = $info;
			}
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_user_search($search,$protocols=array(),$disable=null)
	{
		if(($rs = $this->_service->get_users_search_number($search,$protocols,$disable)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['phone'] = $rs[$i]['ufeatures']['number'];

			if(($info['name'] = $rs[$i]['ufeatures']['fullname']) === '')
				$info['name'] = $info['phone'];

			$info['type'] = 'local';
			$info['identity'] = $info['name'];

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_phonebook_search_from_server($query,$disable=null)
	{
		if(($rs = $this->_appserverfeatures->get_servers_list($disable)) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$option = &$this->_conf['phonenumber']['option'];
		
		for($i = 0;$i < $nb;$i++)
		{
			$ref_url = &$rs[$i]['server']['url_http'];
			$url_http = rtrim($ref_url,'/').'/'.ltrim($query,'/');

			if(($recv = $this->_curl->load($url_http,$option)) !== false)
				$recv = $this->_parse_phonenumber_recv($recv);

			$this->_curl->close();
		}

		return($recv);
	}

	function _parse_phonenumber_recv($recv)
	{
		$recv = explode("\n",str_replace("\r",'',$recv));

		if(($nb = count($recv)) < 3)
			return(false);

		$beg = explode(': ',$recv[0]);
		$end = explode(': ',$recv[$nb-1]);

		if(isset($beg[0],$beg[1],$end[0],$end[1]) === false
		|| $beg[1] !== 'beg-data'
		|| $end[1] !== 'end-data')
			return(false);

		$nb--;

		$r = $info = array();

		for($i = 1;$i < $nb;$i++)
		{
			if(($data = xivo_parse_csv($recv[$i],'|')) === false
			|| isset($data[0],$data[1],$data[2]) === false)
				continue;

			$info['name'] = $data[0];
			$info['phone'] = $data[1];
			$info['type'] = $data[2];
			$info['identity'] = $data[0];

			if($info['type'] !== 'local')
				$info['identity'] .= ' ('.$data[2].')';

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
