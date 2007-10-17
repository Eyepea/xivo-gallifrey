<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','abstract.inc'),true);

class xivo_application_service_asterisk_dialstatus_abstract extends xivo_application_service_asterisk_abstract
{
	function get($type,$id)
	{
		$this->_info = false;
		$this->_origin = false;

		if(is_string($type) === false || ($id = xivo_uint($id)) === 0)
			return(false);

		$where = array();
		$where['category'] = $type;
		$where['categoryval'] = $id;

		if(($dialstatus = &$this->_service->get_module('dialstatus')) === false
		|| ($info = $dialstatus->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$info[$i]['status']]) === true)
				continue;

			$this->_info[$info[$i]['status']] = $info[$i];
		}

		$this->_origin = $dialstatus->get_origin_list();

		return($this->_info);
	}

	function get_destination_list($type,$id)
	{
		$r = array();

		xivo::load_class('xivo_sort');
		$sortidentity = new xivo_sort(array('key' => 'identity'));
		$sortname = new xivo_sort(array('key' => 'name'));

		$userid = $groupid = 0;

		if($type === 'user')
			$userid = xivo_uint($id);
		else
			$groupid = xivo_uint($id);

		if(($ufeatures = &$this->_service->get_module('userfeatures')) === false
		|| ($r['users'] = $ufeatures->get_all_number($userid)) === false)
			$r['users'] = false;
		else
			usort($r['users'],array(&$sortidentity,'str_usort'));

		if(($gfeatures = &$this->_service->get_module('groupfeatures')) === false
		|| ($r['groups'] = $gfeatures->get_all_number($groupid)) === false)
			$r['groups'] = false;
		else
			usort($r['groups'],array(&$sortidentity,'str_usort'));

		if(($qfeatures = &$this->_service->get_module('queuefeatures')) === false
		|| ($r['queues'] = $qfeatures->get_all_number()) === false)
			$r['queues'] = false;
		else
			usort($r['queues'],array(&$sortidentity,'str_usort'));

		if(($mfeatures = &$this->_service->get_module('meetmefeatures')) === false
		|| ($r['meetme'] = $mfeatures->get_all_number()) === false)
			$r['meetme'] = false;
		else
			usort($r['meetme'],array(&$sortidentity,'str_usort'));

		if(($schedule = &$this->_service->get_module('schedule')) === false
		|| ($r['schedule'] = $schedule->get_all()) === false)
			$r['schedule'] = false;
		else
			usort($r['schedule'],array(&$sortname,'str_usort'));

		if(($sounds = &$this->_service->get_module('sounds')) === false)
			$r['sounds'] = false;
		else
			$r['sounds'] = $sounds->get_list('dialstatus',true);

		return($r);
	}

	function set($list,$type)
	{
		if(is_array($list) === false
		|| is_string($type) === false
		|| ($dialstatus = &$this->_service->get_module('dialstatus')) === false
		|| ($arr = xivo_get_aks($list,true)) === false)
			return(false);

		if($arr['cnt'] === 0)
			return(null);

		$info = array();
		$info['category'] = $type;
		$info['categoryval'] = 0;
		$info['linked'] = true;

		$reslist = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$list[$key];

			if(is_array($val) === false)
				continue;

			$info['status'] = $key;
			$info = array_merge($info,$val);

			if(($rs = $dialstatus->chk_values($info)) === false
			|| isset($tmp[$rs['status']]) === true)
				continue;

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

	function add($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		if(($rs = $this->get_result()) === null || $rs === false)
			return($rs);

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$dialstatus = &$this->_service->get_module('dialstatus');

		$this->_return = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$ref = &$rs[$key];
			$ref['categoryval'] = $id;

			if($dialstatus->is_valid($ref['category'],
						 $ref['categoryval'],
						 $ref['type'],
						 $ref['typeval']) === false
			|| ($this->_return[$ref['status']] = $dialstatus->add($ref)) === false)
				return(false);
		}

		if(empty($this->_return) === true)
			$this->_return = false;

		return(true);
	}

	function add_origin_list()
	{
		if(($rightcallmember = &$this->_service->get_module('rightcallmember')) === false)
			return(false);

		return($rightcallmember->add_origin_list($this->_origin));
	}

	function edit($status,$type,$id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		if(empty($this->_info) === true)
			return($this->add($id));

		$this->_return = $this->delete($status,$type,$id);

		if(($rs = $this->get_result()) === null)
			return($this->_return);

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$dialstatus = &$this->_service->get_module('dialstatus');

		$this->_return = array();

		$i = 0;

		if(($list = xivo_get_aks($this->_info)) !== false)
		{
			for($i;$i < $list['cnt'];$i++)
			{
				$key = &$list['keys'][$i];

				if(isset($rs[$key]) === false)
				{
					$i = $arr['cnt'];
					break;
				}

				$ref = &$rs[$key];

				$did = $this->_info[$key]['id'];
				$ref['categoryval'] = $id;

				if($dialstatus->is_valid($ref['category'],
							 $ref['categoryval'],
							 $ref['type'],
							 $ref['typeval']) === false
				|| ($this->_return[$ref['status']] = $dialstatus->add($ref,$did)) === false)
					return(false);
			}
		}

		if($i >= $arr['cnt'])
		{
			if(empty($this->_return) === true)
				$this->_return = false;

			return(true);
		}

		for($i;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$ref = &$rs[$key];

			$ref['categoryval'] = $id;

			if($dialstatus->is_valid($ref['category'],
						 $ref['categoryval'],
						 $ref['type'],
						 $ref['typeval']) === false
			|| ($this->_return[$ref['status']] = $dialstatus->add($ref)) === false)
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

		if(is_string($type) === false
		|| ($id = xivo_uint($id)) === 0
		|| ($dialstatus = &$this->_service->get_module('dialstatus')) === false)
			return(false);

		$dialstatus_where = array();
		$dialstatus_where['category'] = $type;
		$dialstatus_where['categoryval'] = $id;

		$r = $dialstatus->delete_where($dialstatus_where);

		if($status === 'delete')
			$this->_return = $r;

		return($r);
	}

	function reverse_add()
	{
		$dialstatus = &$this->_service->get_module('dialstatus');

		if(is_array($val) === false || ($nb = count($val)) === 0)
			continue;

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_return[$i] !== false)
				$dialstatus->delete($this->_return[$i]);
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
		|| ($id = xivo_uint($id)) === 0
		|| ($dialstatus = &$this->_service->get_module('dialstatus')) === false)
			return(false);

		$dialstatus_where = array();
		$dialstatus_where['type'] = $type;
		$dialstatus_where['typeval'] = $id;

		$r = $dialstatus->unlinked_where($dialstatus_where);

		return($r);
	}
}

?>
