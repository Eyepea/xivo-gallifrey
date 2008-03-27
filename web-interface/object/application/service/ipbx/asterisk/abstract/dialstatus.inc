<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract.inc'),true);

class xivo_application_service_asterisk_dialstatus_abstract extends xivo_application_service_asterisk_abstract
{
	var $_dialstatus	= false;

	function xivo_application_service_asterisk_dialstatus_abstract(&$service)
	{
		$this->xivo_application_service_asterisk_abstract(&$service);
		
		if(($this->_dialstatus = &$this->_service->get_module('dialstatus')) === false)
			return(false);

		return(true);
	}

	function get($type,$id)
	{
		$this->_info = false;
		$this->_origin = false;

		if(is_string($type) === false || ($id = xivo_ulongint($id)) === 0)
			return(false);

		$where = array();
		$where['category'] = $type;
		$where['categoryval'] = $id;

		if(($info = $this->_dialstatus->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$info[$i]['status']]) === true)
				continue;

			$this->_info[$info[$i]['status']] = $info[$i];
		}

		$this->_origin = $this->_dialstatus->get_origin_list();

		return($this->_info);
	}

	function get_sound()
	{
		if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		return($sounds->get_list('playback',true));
	}

	function get_element()
	{
		return($this->_dialstatus->get_element());
	}

	function set($list,$type)
	{
		$this->_reset();

		if(is_array($list) === false
		|| is_string($type) === false)
			return(false);
		else if(empty($list) === true)
			return(null);

		$info = $infotmp = array();
		$info['category'] = $type;
		$info['categoryval'] = 0;
		$info['applicationval'] = '';
		$info['linked'] = true;

		$reslist = array();

		foreach($list as $key => $val)
		{
			if(is_array($val) === false)
				continue;

			$infotmp = array_merge($info,$val);
			$infotmp['status'] = $key;

			if(($rs = $this->_dialstatus->chk_values($infotmp)) !== false
			&& isset($reslist[$rs['status']]) === false)
				$reslist[$rs['status']] = $rs;
		}

		if(empty($reslist) === true)
		{
			$this->_set_result(false);
			return(false);
		}

		$this->_set_result($reslist);

		return(true);
	}

	function get_result_for_display()
	{
		$r = array();

		$r['noanswer'] = $this->_mk_result_for_display('noanswer');
		$r['busy'] = $this->_mk_result_for_display('busy');
		$r['congestion'] = $this->_mk_result_for_display('congestion');
		$r['chanunavail'] = $this->_mk_result_for_display('chanunavail');

		return($r);
	}

	function _mk_result_for_display($status)
	{
		$r = array();
		$r['endcall'] = '';
		$r['user'] = '';
		$r['group'] = '';
		$r['queue'] = '';
		$r['meetme'] = '';
		$r['schedule'] = '';
		$r['application'] = '';
		$r['sound'] = '';
		$r['custom'] = ''; 

		if(xivo_issa($status,$this->_result) === true)
		{
			$r[$this->_result[$status]['type']] = $this->_result[$status]['typeval'];
			$r = array_merge($r,$this->_result[$status]);
		}

		return($r);
	}

	function add($id)
	{
		if(($id = xivo_ulongint($id)) === 0)
			return(false);
		else if(($rs = $this->get_result()) === null)
			return($rs);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return = array();

		foreach($rs as $val)
		{
			$val['categoryval'] = $id;

			if($this->_dialstatus->is_valid($val['category'],
							$val['categoryval'],
							$val['type'],
							$val['typeval'],
							$val['applicationval']) === false
			|| ($this->_return[$val['status']] = $this->_dialstatus->add($val)) === false)
				return(false);
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		return($this->_dialstatus->add_origin_list($this->_origin));
	}

	function edit($status,$type,$id)
	{
		if(($id = xivo_ulongint($id)) === 0)
			return(false);
		else if(empty($this->_info) === true)
			return($this->add($id));

		$this->_return = $this->delete($status,$type,$id);

		if(($rs = $this->get_result()) === null)
			return($this->_return);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return = array();

		$list = is_array($this->_info) === true ? array_keys($this->_info) : false;

		$i = 0;

		foreach($rs as $val)
		{
			if($list !== false && isset($list[$i]) === true)
				$did = $this->_info[$list[$i++]]['id'];
			else
				$did = 0;

			$val['categoryval'] = $id;

			if($this->_dialstatus->is_valid($val['category'],
							$val['categoryval'],
							$val['type'],
							$val['typeval'],
							$val['applicationval']) === false
			|| ($this->_return[$val['status']] = $this->_dialstatus->add($val,$did)) === false)
				return(false);
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function delete($status,$type,$id)
	{
		if(empty($this->_info) === true)
			return(null);
		else if(is_string($type) === false
		|| ($id = xivo_ulongint($id)) === 0)
			return(false);

		$dialstatus_where = array();
		$dialstatus_where['category'] = $type;
		$dialstatus_where['categoryval'] = $id;

		$r = $this->_dialstatus->delete_where($dialstatus_where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		if(is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $val)
		{
			if($val !== false)
				$this->_dialstatus->delete($val);
		}

		return(true);
	}

	function reverse_edit($status,$type,$id)
	{
		$this->delete($status,$type,$id);

		return($this->add_origin_list());
	}

	function unlinked($type,$id)
	{
		if(is_string($type) === false
		|| ($id = xivo_ulongint($id)) === 0)
			return(false);

		$dialstatus_where = array();
		$dialstatus_where['type'] = $type;
		$dialstatus_where['typeval'] = $id;

		$r = $this->_dialstatus->unlinked_where($dialstatus_where);

		return($r);
	}
}

?>
