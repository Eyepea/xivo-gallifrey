<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_userfeatures extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'userfeatures';
	var $_filter	= null;
	var $_origin	= null;

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

	function get($id)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id)) === false
		|| ($r['protocol'] = $this->_sre->get_protocol_by_protocol_name($r['protocol'])) === false)
			return(false);

		$this->_origin = $r;

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

	function edit_where($arr,$values)
	{
		$arr = (array) $arr;

		if(is_array($values) === false
		|| ($where = $this->get_elements_where($arr)) === false
		|| ($values = $this->chk_set_values($values,true,true)) === false)
			return(false);

		return($this->_dso->edit_where($where,$values));
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
