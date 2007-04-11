<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

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
			return(true);

		xivo::load_class('socket');
		$this->_sockprov[$type] = new xivo_socket($this->_autoprov[$type]);

		return(true);
	}

	function get_autoprov_list()
	{
		if($this->_load_sockprov('list') === false || $this->_sockprov['list']->is_open() !== true
		|| ($write = $this->_mk_sockprov_header('list')) === false
		|| $this->_sockprov['list']->write($write) === false
		|| ($read = $this->_sockprov['list']->read_normal()) === false
		|| ($recv = $this->_parse_sockprov_recv($read)) === false)
			return(false);

		if(isset($recv['status']) === false
		|| ($nb = count($recv['data'])) === 0
		|| preg_match('/^HTTP\/1\.(0|1) 200 /i',$recv['status']) !== 1)
			return(false);

		$data = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$recv['data'][$i];

			if(preg_match('/^([0-9a-z\-_]+)="([0-9a-zA-Z\-_\.]+)"$/',$ref,$match) === 1
			&& isset($data[$match[1]]) === false)
			{
				$data[$match[1]] = array();
				$data[$match[1]]['name'] = $match[2];
				$data[$match[1]]['model'] = array();
			}
			else if(preg_match('/^([0-9a-z\-_]+)\.([0-9a-z\-_]+)="([0-9a-zA-Z\-_\.]+)"$/',$ref,$match) === 1
			&& isset($data[$match[1]]) === true)
			{
				$data[$match[1]]['model'][$match[2]] = array();
				$data[$match[1]]['model'][$match[2]]['path'] = $match[1].'.'.$match[2];
				$data[$match[1]]['model'][$match[2]]['meta'] = $match[2];
				$data[$match[1]]['model'][$match[2]]['label'] = $match[3];
			}
		}

		if(empty($data) === true)
			$data = false;

		return($data);
	}

	function _mk_sockprov_header($type)
	{
		if(xivo_ak($type,$this->_autoprov) === false)
			return(false);

		$r  = strtoupper($this->_autoprov[$type]['method']);
		$r .= ' '.$this->_autoprov[$type]['url'].' HTTP/1.1'."\r\n";
		$r .= 'Host: '.$this->_autoprov[$type]['address']."\r\n\r\n";

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
			$header = explode(': ',$recv[$i]);

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

	function informative($arr)
	{
		if(($aks = xivo_get_aks($arr)) === false)
			return(false);

		$data = 'mode=informative'."\n";
		$data .= 'actions=no'."\n";

		for($i = 0;$i < $aks['cnt'];$i++)
		{
			$key = &$aks['keys'][$i];

			$data .= $key.'='.$arr[$key]."\n";
		}

		print $data;
		die();
	}

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id)) === false)
			return(false);

		$this->_origin = $r;

		return($r);
	}

	function get_by_iduserfeatures($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$arr = array();

		$arr['idfeatures'] = $id;

		return($this->get_where($arr));
	}

	function get_where($arr)
	{
		$arr = (array) $arr;

		$where = array();

		if(isset($arr['macaddr']) === true)
			$where['macaddr'] = $arr['macaddr'];

		if(isset($arr['vendor']) === true)
			$where['vendor'] = $arr['vendor'];

		if(isset($arr['model']) === true)
			$where['model'] = $arr['model'];

		if(isset($arr['proto']) === true)
			$where['proto'] = $arr['proto'];

		if(isset($arr['idfeatures']) === true)
			$where['idfeatures'] = $arr['idfeatures'];

		if(($id = $this->_dso->get_where($where)) === false)
			return(false);

		return($this->get($id));
	}
}

?>
