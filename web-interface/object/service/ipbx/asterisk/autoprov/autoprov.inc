<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_autoprov extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'autoprov';
	var $_filter	= null;
	var $_autoprov	= null;
	var $_sockprov	= null;

	function xivo_service_asterisk_autoprov(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
		$this->_chk_autoprov();
	}

	function get($macaddr)
	{
		if(($r = $this->_dso->get($macaddr)) === false)
			return(false);

		$this->_origin = $r;

		return($r);
	}

	function _chk_autoprov()
	{
		if(xivo_issa($this->_name,$this->_sre->_ini) === false
		|| isset($this->_sre->_ini[$this->_name]['host'],$this->_sre->_ini[$this->_name]['port']) === false)
			$this->_autoprov = false;
		else
		{
			$this->_autoprov = $this->_conf['autoprov'];
			$this->_autoprov['prov']['address'] = $this->_sre->_ini[$this->_name]['host'];
			$this->_autoprov['prov']['port'] = $this->_sre->_ini[$this->_name]['port'];
			$this->_autoprov['list']['address'] = $this->_autoprov['prov']['address'];
			$this->_autoprov['list']['port'] = $this->_autoprov['prov']['port'];
		}
	}

	function _load_sockprov($type)
	{
		if(($type !== 'list' && $type !== 'prov') === true || xivo_ak($type,$this->_autoprov) === false)
			return(false);

		if(xivo_ak($type,$this->_sockprov) === true)
			return($this->_sockprov[$type]->open());

		xivo::load_class('xivo_socket');
		$this->_sockprov[$type] = new xivo_socket($this->_autoprov[$type]);

		if(xivo_issa('sndtimeo',$this->_autoprov[$type]) === true)
		{
			$this->_sockprov[$type]->set_option(null,
							    'sndtimeo',
							    $this->_autoprov[$type]['sndtimeo']);
		}

		if(xivo_issa('rcvtimeo',$this->_autoprov[$type]) === true)
		{
			$this->_sockprov[$type]->set_option(null,
							    'rcvtimeo',
							    $this->_autoprov[$type]['rcvtimeo']);
		}

		return(true);
	}

	function get_autoprov_list()
	{
		$r = false;

		if($this->_load_sockprov('list') === false
		|| $this->_sockprov['list']->is_open() === false
		|| ($write = $this->_mk_sockprov_http('list')) === false)
			return($r);
	
		if(($write = $this->_mk_sockprov_http('list')) === false
		|| $this->_sockprov['list']->write($write) === false
		|| ($read = $this->_sockprov['list']->read_binary()) === false
		|| ($recv = $this->_parse_sockprov_recv($read)) === false)
		{
			$this->_sockprov['list']->close();
			return($r);
		}

		if(isset($recv['status']) === false
		|| ($nb = count($recv['data'])) === 0
		|| preg_match('/^HTTP\/1\.(0|1) 200 /i',$recv['status']) !== 1)
		{
			$this->_sockprov['list']->close();
			return($r);
		}

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$recv['data'][$i];

			if(preg_match('/^([0-9a-z\-_]+)="([0-9a-zA-Z\-_\.]+)"$/',$ref,$match) === 1
			&& isset($r[$match[1]]) === false)
			{
				$r[$match[1]] = array();
				$r[$match[1]]['name'] = $match[2];
				$r[$match[1]]['model'] = array();
			}
			else if(preg_match('/^([0-9a-z\-_]+)\.([0-9a-z\-_]+)="([0-9a-zA-Z\-_\.]+)"$/',$ref,$match) === 1
			&& isset($r[$match[1]]) === true)
			{
				$r[$match[1]]['model'][$match[2]] = array();
				$r[$match[1]]['model'][$match[2]]['path'] = $match[1].'.'.$match[2];
				$r[$match[1]]['model'][$match[2]]['meta'] = $match[2];
				$r[$match[1]]['model'][$match[2]]['label'] = $match[3];
			}
		}

		if(empty($r) === true)
			$r = false;
			
		return($r);
	}

	function _mk_sockprov_http($type,$data='')
	{
		$data = (string) $data;

		if(xivo_ak($type,$this->_autoprov) === false)
			return(false);

		$r  = strtoupper($this->_autoprov[$type]['method']);
		$r .= ' '.$this->_autoprov[$type]['url'].' HTTP/1.1'."\r\n";
		$r .= 'Host: '.$this->_autoprov[$type]['address']."\r\n";

		if(($len = strlen($data)) !== 0)
		{
			$r .= 'Content-Length: '.$len."\r\n\r\n";
			$r .= $data;
		}
		else $r .= "\r\n";

		return($r);
	}

	function _parse_sockprov_recv($recv)
	{
		$recv = (string) $recv;

		$recv = str_replace("\r",'',$recv);
		$recv = explode("\n",$recv);

		if(($nb = count($recv)) === 0)
			return(false);

		$r = array();
		$r['data'] = array();
		$data = false;

		for($i = 0;$i < $nb;$i++)
		{
			$header = explode(': ',$recv[$i],2);

			if($i === 0 && $header[0] !== '' && isset($header[1]) === false)
				$r['status'] = $header[0];
			else if($data === false && $recv[$i] === '')
				$data = true;
			else if($data === false && isset($header[0],$header[1]) === true)
				$r[$header[0]] = $header[1];
			else if($i === $nb-1 && $recv[$i] === '')
				continue;
			else
				$r['data'][] = $recv[$i];
		}

		return($r);
	}

	function _send_sockprov_mode($mode,$arr,$modact='')
	{
		$r = false;

		$mode = (string) $mode;
		$modact = (string) $modact;

		if(is_array($arr) === false)
			return($r);

		unset($arr['modact']);

		if($modact === 'guest')
			$arr['iduserfeatures'] = 0;

		$arr['mode'] = $mode;

		if(($aks = xivo_get_aks($arr)) === false)
			return($r);

		$data = '';

		for($i = 0;$i < $aks['cnt'];$i++)
		{
			$key = &$aks['keys'][$i];
			$val = &$arr[$key];

			if(xivo_empty($val) === true)
				continue;

			$data .= $key.'='.$val."\r\n";
		}

		if($this->_load_sockprov('prov') === false
		|| ($write = $this->_mk_sockprov_http('prov',$data)) === false
		|| $this->_sockprov['prov']->is_open() === false)
			return($r);

		if($this->_sockprov['prov']->write($write) !== false)
			$r = true;

		$this->_sockprov['prov']->close();

		return($r);
	}

	function notification($arr,$mod)
	{
		if(is_array($arr) === false)
			return(false);

		unset($arr['vendor'],$arr['model']);

		$arr['actions'] = 'yes';

		if(($mod !== 'guest' && $mod !== 'prov') === true
		|| $this->_send_sockprov_mode('notification',$arr,$mod) === false)
			return(false);

		return(true);
	}

	function authoritative($arr,$mod)
	{
		if(is_array($arr) === false)
			return(false);

		$arr['actions'] = 'no';
		$arr['isinalan'] = 0;

		if(($mod !== 'guest' && $mod !== 'prov') === true
		|| $this->_send_sockprov_mode('authoritative',$arr,$mod) === false)
			return(false);

		return(true);
	}

	function userdeleted($iduserfeatures)
	{
		if(($id = xivo_uint($iduserfeatures)) === 0)
			return(false);

		$r = array();
		$r['actions'] = 'no';
		$r['iduserfeatures'] = $id;

		if($this->_send_sockprov_mode('userdeleted',$r) === false)
			return(false);

		return(true);
	}
}

?>
