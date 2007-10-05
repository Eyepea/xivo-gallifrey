<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_userfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'userfeatures';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_userfeatures(&$sre,&$dso)
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
		if($data['context'] === '')
			$data['context'] = 'default';

		$data['fullname'] = trim(trim($data['firstname']).' '.trim($data['lastname']));

		if($data['fullname'] === '')
			$data['fullname'] = '-';

		if(xivo_empty($data['number']) === true)
			$data['identity'] = $data['fullname'].' ('.$data['context'].')';
		else
			$data['identity'] = $data['fullname'].' ('.$data['number'].'@'.$data['context'].')';

		return($data);
	}

	function get($id,$orig=true)
	{
		$orig = (bool) $orig;

		if(($id = xivo_uint($id)) === 0
		|| ($r = $this->_dso->get($id)) === false
		|| ($r['protocol'] = $this->_sre->get_protocol_by_protocol_name($r['protocol'])) === false)
			return(false);

		if($orig === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_where($arr,$orig=true)
	{
		$orig = (bool) $orig;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($r = $this->_dso->get($where)) === false)
			return(false);

		if($orig === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_by_protocol($id,$protocol)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0
		|| ($protocol = $this->_sre->get_protocol_name($protocol)) === false
		|| ($r = $this->get_where(array('protocolid' => $id,'protocol' => $protocol))) === false)
			return($r);

		return($r);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if(isset($arr['protocol']) === false
		|| ($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false
		|| ($arr['protocol'] = $this->_sre->get_protocol_name($arr['protocol'])) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$where = array('name' => $arr['name'],'protocol' => $arr['protocol']);

			if($this->get_id($where) !== false)
				return(false);
		}

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

		if($r > $min && $r < $max && $this->get_id(array('provisioningid' => $r)) === false)
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

		if(isset($arr['name'],$arr['protocol']) === true)
			$where = array('name' => $arr['name'],'protocol' => $arr['protocol']);
		else
		{
			$where = array();

			if(($info = $this->get($id,false)) === false)
				return(false);

			if(isset($arr['name']) === false)
				$where['name'] = $info['name'];

			if(isset($arr['protocol']) === false)
				$where['protocol'] = $info['protocol'];
		}

		if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
			return(false);

		if(isset($arr['protocol']) === true)
		{
			if($provisioning === true)
			{
				if(($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false)
					return(false);
			}

			if(($arr['protocol'] = $this->_sre->get_protocol_name($arr['protocol'])) === false)
				return(false);
		}

		return($this->_dso->edit($id,$arr));
	}

	function get_search($value)
	{
		if(($list = $this->_dso->get_search($value)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_context()
	{
		if(($list = $this->get_all()) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($info['context'] === '' || $info['context'] === 'local-extensions')
				$info['context'] = '#main';

			$r[$info['context']] = $info['context'];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_all_number($id=0)
	{
		$id = xivo_uint($id);

		if(($list = $this->get_all()) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($id === (int) $info['id'] || xivo_empty($info['number']) === true)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
