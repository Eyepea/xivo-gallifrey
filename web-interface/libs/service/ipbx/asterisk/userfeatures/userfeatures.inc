<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_userfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'userfeatures';
	var $_filter	= null;
	var $_origin	= null;
	var $_autoprov	= null;
	var $_sockprov	= null;

	function xivo_service_asterisk_userfeatures(&$sre,&$dso)
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

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id)) === false
		|| ($r['protocol'] = $this->_sre->get_protocol_by_protocol_name($r['protocol'])) === false)
			return(false);

		$this->_origin = $r;

		return($r);
	}

	function _chk_autoprov()
	{
		if(xivo_issa($this->_name,$this->_sre->_ini) === false
		|| xivo_issa('autoprov',$this->_sre->_ini[$this->_name]) === false
		|| isset($this->_sre->_ini[$this->_name]['autoprov']['host'],$this->_sre->_ini[$this->_name]['autoprov']['port']) === false)
			$this->_autoprov = false;
		else
		{
			$this->_autoprov = $this->_conf['autoprov'];
			$this->_autoprov['prov']['address'] = $this->_sre->_ini[$this->_name]['autoprov']['host'];
			$this->_autoprov['prov']['port'] = $this->_sre->_ini[$this->_name]['autoprov']['port'];
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

		if(isset($recv['status'],$recv['data'][0]) === false || preg_match('/^(http\/1\.(1|0)) 200 /i',$recv['status']) !== 1)
			return(false);

		return($recv['data']);
	}

	function _mk_sockprov_header($type)
	{
		if(xivo_ak($type,$this->_autoprov) === false)
			return(false);

		$r  = strtoupper($this->_autoprov[$type]['method']);
		$r .= ' '.$this->_autoprov[$type]['url'].' HTTP/1.1'."\r\n\r\n";

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

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,$this->_origin['id']);
		else
			$r = $this->edit($this->_origin['id'],$this->_origin);

		return($r);
	}

	function get_by_provisioning($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($id = $this->_dso->get_by_provisioning($id)) === false)
			return(false);

		return($this->get($id));
	}

	function get_by_protocol($id,$protocol)
	{
		if(($id = xivo_uint($id)) === 0
		|| ($protocol = $this->_sre->get_protocol_name($protocol)) === false
		|| ($id = $this->_dso->get_by_protocol($id,$protocol)) === false)
			return(false);

		return($this->get($id));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if(($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false
		|| ($arr['protocol'] = $this->_sre->get_protocol_name($arr['protocol'])) === false)
			return(false);

		return($this->_dso->add($arr));
	}

	function _mk_provisioning_id($protocol)
	{
		if($this->_chk_exists_cfg('provisioning_id',$protocol) === false)
			return(false);

		$ref = &$this->_conf['provisioning_id'][$protocol];

		$min = $ref['prefix'] * $ref['range'];
		$max = $min + $ref['range'];
		$len = strlen($max)-strlen($ref['prefix']);

		$r = $ref['prefix'].substr(crc32(uniqid(rand(),true)),1,$len);

		if($r > $min && $r < $max && $this->_dso->get_by_provisioning($r) === false)
			return($r);
		else
			$r = $this->_mk_provisioning_id($protocol);

		return($r);
	}

	function edit($id,$arr,$provisioning=false)
	{
		$provisioning = (bool) $provisioning;

		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['protocol']) === true)
		{
			if($provisioning === true)
				$arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol']);

			if(($arr['protocol'] = $this->_sre->get_protocol_name($arr['protocol'])) === false)
				return(false);
		}

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}

	function get_search($value)
	{
		if(($r = $this->_dso->get_search($value)) !== false && isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_number($disable=null)
	{
		$r = false;

		if(($list = $this->get_list()) === false)
			return($r);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $this->get($list[$i],$disable)) === false || xivo_empty($info['number']) === true)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
