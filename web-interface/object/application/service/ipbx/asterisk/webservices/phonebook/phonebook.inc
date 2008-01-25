<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','webservices','abstract.inc'),true);

class xivo_application_service_asterisk_webservices_phonebook extends xivo_application_service_asterisk_webservices_abstract
{
	var $_dso		= null;
	var $_accessfeatures	= null;
	var $_appserverfeatures	= null;
	var $_curl		= null;
	var $_phonebook		= false;
	var $_phonebooknumber	= false;

	function xivo_application_service_asterisk_webservices_phonebook(&$service,&$dso)
	{
		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if($this->_init(&$service) === false
		|| ($this->_accessfeatures = &$this->_service->get_module('accessfeatures')) === false
		|| xivo::load_class('xivo_filter') === false
		|| xivo::load_class('xivo_curl') === false
		|| ($this->_appserverfeatures = &$this->_service->get_application('serverfeatures',
										  array('type' => 'phonebook'))) === false)
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
		if(xivo::load_class('xivo::file::csv') === false)
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
}

?>
