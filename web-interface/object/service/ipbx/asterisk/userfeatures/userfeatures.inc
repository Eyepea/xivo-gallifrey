<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_userfeatures extends xivo_service_asterisk_abstract
{
	var $_cnt	= 0;
	var $_dso	= null;
	var $_name	= 'userfeatures';
	var $_filter	= false;
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
		$this->mk_identity($data);

		$data['enableclient'] = (bool) $data['enableclient'];
		$data['enablehint'] = (bool) $data['enablehint'];
		$data['enablevoicemail'] = (bool) $data['enablevoicemail'];
		$data['enablexfer'] = (bool) $data['enablexfer'];
		$data['enableautomon'] = (bool) $data['enableautomon'];
		$data['callrecord'] = (bool) $data['callrecord'];
		$data['callfilter'] = (bool) $data['callfilter'];
		$data['enablednd'] = (bool) $data['enablednd'];
		$data['enablerna'] = (bool) $data['enablerna'];
		$data['enablebusy'] = (bool) $data['enablebusy'];
		$data['enableunc'] = (bool) $data['enableunc'];
		$data['internal'] = (bool) $data['internal'];
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function mk_identity(&$data)
	{
		if($data['context'] === '')
			$data['context'] = 'default';

		$data['fullname'] = trim(trim($data['firstname']).' '.trim($data['lastname']));

		if($data['fullname'] === '')
			$data['fullname'] = '-';

		if(xivo_haslen($data['number']) === true)
			$data['identity'] = $data['fullname'].' ('.$data['number'].'@'.$data['context'].')';
		else
			$data['identity'] = $data['fullname'].' ('.$data['name'].')';

		return($data);
	}

	function get($id,$orig=true,$disable=null)
	{
		if(($orig = (bool) $orig) === true)
			$this->_origin = false;

		if(($id = xivo_uint($id)) === 0
		|| ($r = $this->_dso->get($id,$disable)) === false
		|| $this->_sre->chk_protocol($r['protocol']) === false)
			return(false);

		if($orig === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_where($arr,$orig=true,$disable=null,$id=null)
	{
		if(($orig = (bool) $orig) === true)
			$this->_origin = false;

		if(is_array($arr) === false
		|| ($where = $this->get_element_where($arr)) === false
		|| ($id !== null && ($where['id'] = xivo_uint($id)) === 0) === true
		|| ($r = $this->_dso->get($where)) === false)
			return(false);

		if($orig === true)
			$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function get_by_protocol($id,$protocol,$orig=true,$disable=null)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0
		|| $this->_sre->chk_protocol($protocol) === false
		|| ($r = $this->get_where(array('protocolid' => $id,'protocol' => $protocol),$orig,$disable)) === false)
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
		|| $this->_sre->chk_protocol($arr['protocol']) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);
			$where = array('name' => $arr['name'],'protocol' => $arr['protocol']);

			if($this->get_id($where) !== false)
				return(false);
	
			if((isset($arr['destrna']) === true
			   && $arr['name'] === $arr['destrna']) === true
			|| (isset($arr['destbusy']) === true
			   && $arr['name'] === $arr['destbusy']) === true
			|| (isset($arr['destunc']) === true
			   && $arr['name'] === $arr['destunc']) === true)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function _mk_provisioning_id($protocol)
	{
		if($this->_chk_exists_cfg('provisioning_id',$protocol) === false)
			return(null);

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

	function edit($id,$arr,$provisioning=false,$disable=null)
	{
		$provisioning = (bool) $provisioning;

		if(($id = xivo_uint($id)) === 0
		|| is_array($arr) === false
		|| ($info = $this->get($id,false)) === false)
			return(false);

		$where = array();

		if(isset($arr['name']) === false)
			$where['name'] = $info['name'];
		else
			$arr['name'] = $where['name'] = strtolower($arr['name']);

		if(isset($arr['protocol']) === false)
			$where['protocol'] = $info['protocol'];
		else
			$where['protocol'] = $arr['protocol'];

		if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
			return(false);

		if(isset($arr['destrna']) === false)
			$arr['destrna'] = $info['destrna'];

		if(isset($arr['destbusy']) === false)
			$arr['destbusy'] = $info['destbusy'];

		if(isset($arr['destunc']) === false)
			$arr['destunc'] = $info['destunc'];

		if($arr['destrna'] === $where['name']
		|| $arr['destbusy'] === $where['name']
		|| $arr['destunc'] === $where['name'])
			return(false);

		if(isset($arr['protocol']) === true)
		{
			if($provisioning === true
			&& ($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false)
				return(false);

			if($this->_sre->chk_protocol($arr['protocol']) === false)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable));
	}

	function get_search($value,$disable=null,$order=null,$limit=null,$number=false)
	{
		if(($list = $this->_dso->get_search($value,$disable,$order,$limit,$number)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_search_number($value,$disable=null,$order=null,$limit=null)
	{
		return($this->get_search($value,$disable,$order,$limit,true));
	}

	function get_all_number($id=0,$disable=null)
	{
		$id = xivo_uint($id);

		if(($list = $this->get_all($disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($id === (int) $info['id'] || xivo_haslen($info['number']) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_all_context($disable=null)
	{
		if(($list = $this->_dso->get_all_context($disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			$r[$info['context']] = $info['context'];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
