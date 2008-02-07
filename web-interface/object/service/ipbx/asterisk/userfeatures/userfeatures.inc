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
		$orig = (bool) $orig;
		$origin = $this->_origin;

		if($orig === false)
			$this->_origin = null;

		if(($r = parent::get($id,$disable)) === false
		|| $this->_sre->chk_protocol($r['protocol']) === false)
		{
			$this->_origin = $origin;
			return(false);
		}
		else if($orig === false)
			$this->_origin = $origin;

		return($this->_prepare($r));
	}

	function get_where($arr,$orig=true,$disable=null,$id=null)
	{
		$orig = (bool) $orig;
		$origin = $this->_origin;

		if($orig === false)
			$this->_origin = null;

		if(($r = parent::get_where($arr,$disable,$id)) === false)
		{
			$this->_origin = $origin;
			return(false);
		}
		else if($orig === false)
			$this->_origin = $origin;

		return($r);
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

		if($r > $min && $r < $max && $this->get_primary(array('provisioningid' => $r)) === false)
			return($r);

		return($this->_mk_provisioning_id($protocol));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['protocol']) === false
		|| ($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false
		|| $this->_sre->chk_protocol($arr['protocol']) === false)
			return(false);
		else if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);

			if((isset($arr['destrna']) === true
			   && $arr['name'] === $arr['destrna']) === true
			|| (isset($arr['destbusy']) === true
			   && $arr['name'] === $arr['destbusy']) === true
			|| (isset($arr['destunc']) === true
			   && $arr['name'] === $arr['destunc']) === true)
				return(false);
		}

		return(parent::add($arr,$id));
	}

	function edit($id,$arr,$provisioning=false,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['name']) === true)
		{
			$arr['name'] = strtolower($arr['name']);

			if((isset($arr['destrna']) === true
			   && $arr['name'] === $arr['destrna']) === true
			|| (isset($arr['destbusy']) === true
			   && $arr['name'] === $arr['destbusy']) === true
			|| (isset($arr['destunc']) === true
			   && $arr['name'] === $arr['destunc']) === true)
				return(false);
		}

		if(isset($arr['protocol']) === true)
		{
			if((bool) $provisioning === true
			&& ($arr['provisioningid'] = $this->_mk_provisioning_id($arr['protocol'])) === false)
				return(false);

			if($this->_sre->chk_protocol($arr['protocol']) === false)
				return(false);
		}

		return(parent::edit($id,$arr,$disable));
	}

	function search($value,$disable=null,$order=null,$limit=null,$number=false)
	{
		if(($list = $this->_dso->search($value,$disable,$order,$limit,$number)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function search_number($value,$disable=null,$order=null,$limit=null)
	{
		return($this->search($value,$disable,$order,$limit,true));
	}

	function get_all_number($id=0,$disable=null)
	{
		$id = xivo_ulongint($id);

		if(($list = $this->get_all($disable)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info = $list[$i];

			if($id === $info['id'] || xivo_haslen($info['number']) === false)
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
