<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_agentgroup extends xivo_application_asterisk_abstract
{
	var $_agentgroup	= false;
	var $_appagents		= false;
	var $_staticagentgroup	= false;
	var $_agentmember	= null;
	var $_queuemember	= null;
	var $_contextlist	= array();
	var $_musiconhold	= array();

	function xivo_application_service_asterisk_agentgroup(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_agentgroup = &$this->_service->get_module('agentgroup')) === false
		|| ($this->_appagents = &$this->_service->get_apprealstatic('agents')) === false
		|| ($this->_staticagentgroup = &$this->_appagents->get_module('group')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_agentmember = &$this->get_abstract('agentmember')) === false
		|| ($this->_queuemember = &$this->_service->get_module('queuemember')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['agentgroup'] = $this->_agentgroup->get($id)) === false
		|| ($this->_info['staticagentgroup'] = $this->_staticagentgroup->get_group(
							$this->_info['agentgroup']['groupid'])) === false)
			return(false);

		$this->_origin['agentgroup'] = $this->_agentgroup->get_origin();
		$this->_origin['staticagentgroup'] = $this->_staticagentgroup->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_agentmember();
		$this->_get_queuemember();

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_agent->get_nb($arr,$disable));
	}

	function _get_agentmember()
	{
		$this->_info['agentmember'] = false;
		$this->_origin['agentmember'] = false;

		if(is_object($this->_agentmember) === true
		&& xivo_issa('agentgroup',$this->_info) === true
		&& ($this->_info['agentmember'] = $this->_agentmember->get($this->_info['agentgroup']['id'])) !== false)
			$this->_origin['agentmember'] = $this->_agentmember->get_origin();

		return($this->_info['agentmember']);
	}

	function _get_queuemember()
	{
		$this->_info['queuemember'] = false;
		$this->_origin['queuemember'] = false;

		$where = array();
		$where['usertype'] = 'agent';
		$where['category'] = 'group';
		$where['userid'] = $this->_info['agentgroup']['id'];

		if(is_object($this->_queuemember) === false
		|| ($queuefeatures = &$this->_service->get_module('queuefeatures')) === false
		|| ($this->_info['queuemember'] = $this->_queuemember->get_all_where($where)) === false
		|| ($nb = count($this->_info['queuemember'])) === 0)
		{
			$this->_info['queuemember'] = false;
			return(false);
		}

		$ref = &$this->_info['queuemember'];

		$where = array();

		for($i = 0;$i < $nb;$i++)
		{
			$where['name'] = $ref[$i]['queue_name'];

			if(($id = $queuefeatures->get_primary($where)) !== false)
				$ref[$i]['queuefeaturesid'] = $id;
		}

		$this->_origin['queuemember'] = $this->_queuemember->get_origin_list();

		return($this->_info['queuemember']);
	}

	function get_elements()
	{
		$r = array();

		$r['agentgroup'] = $this->_agentgroup->get_element();

		if(is_object($this->_queuemember) === true)
			$r['qmember'] = $this->_queuemember->get_element();
		else
			$r['qmember'] = false;

		return($r);
	}

	function get_agentgroups_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_agentgroup->get_all(null,false,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_agentgroup->get_cnt();
			return(false);
		}

		$this->_cnt = $this->_agentgroup->get_cnt();

		$agentfeatures = &$this->_service->get_module('agentfeatures');

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['agentgroup'] = $rs[$i];

			if(($info['staticagentgroup'] = $this->_staticagentgroup->get_group(
									$info['agentgroup']['groupid'],
									$disable)) !== false
			&& ($info['nb_amember'] = $agentfeatures->get_nb(
							array('numgroup' => $info['agentgroup']['id']),
							$disable)) !== false)
				$r[$info['agentgroup']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('agentgroup',$arr) === false)
			return(false);

		$arr['agentgroup']['groupid'] = 0;
		$this->set_agentgroup($arr['agentgroup']);
		$this->set_staticagentgroup();

		if(xivo_issa('agent-select',$arr) === true)
			$this->set_agentmember($arr['agent-select']);

		if(xivo_issa('queue',$arr) === true && xivo_issa('queue-select',$arr) === true)
			$this->set_queuemember($arr['queue'],$arr['queue-select'],0);

		return(($this->get_errnb() < 1));
	}

	function set_agentgroup($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('agentgroup',null);
			$this->_set_error('agentgroup','invalid data');
			return(false);
		}
		else if(($rs = $this->_agentgroup->chk_values($arr)) === false)
		{
			$this->_set_result('agentgroup',$this->_agentgroup->get_filter_result());
			$this->_set_error('agentgroup',$this->_agentgroup->get_filter_error());
			return(false);
		}

		$this->_set_result('agentgroup',$rs);

		return(true);
	}

	function set_staticagentgroup()
	{
		$arr = array();
		$arr['group'] = 1;

		if(($r = $this->_staticagentgroup->set_group($arr)) === false)
			$this->_set_error('staticagentgroup',$this->_staticagentgroup->get_error('group'));

		$this->_set_result('staticagentgroup',$this->_staticagentgroup->get_result('group'));

		return($r);
	}

	function set_agentmember($list)
	{
		if(is_object($this->_agentmember) === false)
			return(false);
		else if(($r = $this->_agentmember->set($list)) === false)
			$this->_set_error('agentmember',$this->_agentmember->get_error());

		$this->_set_result('agentmember',$this->_agentmember->get_result());

		return($r);
	}

	function set_queuemember($list,$select)
	{
		if(is_array($list) === false
		|| is_array($select) === false
		|| ($queuefeatures = &$this->_service->get_module('queuefeatures')) === false
		|| is_object($this->_queuemember) === false)
			return(false);

		$slt = array_values($select);

		if(($nb = count($slt)) === 0)
			return(null);

		$info = array();
		$info['usertype'] = 'agent';
		$info['userid'] = 0;
		$info['category'] = 'group';
		$info['call-limit'] = 0;

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$slt[$i];

			if(isset($tmp[$qname]) === true
			|| isset($list[$qname]) === false
			|| ($interface = $this->mk_agent_interface(0,true)) === false
			|| ($typeid = $queuefeatures->get_primary(array('name' => $qname))) === false)
				continue;

			$info['queue_name'] = $qname;
			$info['interface'] = $interface['interface'];
			$info['channel'] = $interface['channel'];

			$values = array_merge($list[$qname],$info);

			if(($rs = $this->_queuemember->chk_values($values)) === false)
				continue;

			$rs['queuefeaturesid'] = $typeid;
			$tmp[$qname] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('queuemember',false);
			$this->_set_error('queuemember','empty');
			return(false);
		}

		$this->_set_result('queuemember',$reslist);
		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$staticagentgroupid = null;

		if($this->get_errnb() > 0
		|| ($agentgroupid = $this->add_agentgroup()) === false
		|| ($staticagentgroupid = $this->add_staticagentgroup($agentgroupid)) === false
		|| $this->update_agentgroup($agentgroupid,$staticagentgroupid) === false
		|| $this->add_agentmember($agentgroupid) === false
		|| $this->add_queuemember($agentgroupid) === false)
		{
			$r = false;
			if($agentgroupid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_agentgroup()
	{
		if(($rs = $this->get_result('agentgroup')) === null
		|| $rs === false)
			return(false);

		$rs['groupid'] = 0;

		return(($this->_return['agentgroup'] = $this->_agentgroup->add($rs)));
	}

	function add_staticagentgroup($agentgroupid)
	{
		if(($agentgroupid = xivo_ulongint($agentgroupid)) === 0
		|| ($rs = $this->_staticagentgroup->add_group($agentgroupid)) === null
		|| $rs === false)
			return(false);

		return(($this->_return['staticagentgroup'] = $this->_staticagentgroup->get_return()));
	}

	function update_agentgroup($agentgroupid,$staticagentgroupid)
	{
		if(($agentgroupid = xivo_ulongint($agentgroupid)) === 0
		|| ($staticagentgroupid = xivo_ulongint($staticagentgroupid)) === 0)
			return(false);

		return($this->_agentgroup->edit($agentgroupid,
						array('groupid'	=> $staticagentgroupid)));
	}

	function add_agentmember($agentgroupid)
	{
		if(is_object($this->_agentmember) === false)
			return(false);

		$r = $this->_agentmember->save($agentgroupid);

		$this->_return['agentmember'] = $this->_agentmember->get_return();

		return($r);
	}

	function add_queuemember($agentgroupid)
	{
		$rs = false;

		if(($agentgroupid = xivo_ulongint($agentgroupid)) === 0
		|| ($rs = $this->get_result('queuemember')) === null
		|| $rs === false)
			return($rs);

		$ret = $this->_return['queuemember'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $agentgroupid;

			unset($rs[$i]['queuefeaturesid']);

			if(($interface = $this->mk_agent_interface($agentgroupid,true)) === false)
				return(false);

			$rs[$i]['interface'] = $interface['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return['queuemember'][] = $ret;
		}

		if(isset($this->_return['queuemember'][0]) === false)
			$this->_return['queuemember'] = false;

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'agentgroup':
					$this->_agentgroup->delete($val);
					break;
				case 'staticagentgroup':
					$this->_staticagentgroup->delete_group($this->_status,$val);
					break;
				case 'agentmember':
					$this->_agentmember->reverse_save($this->_status);
					break;
				case 'queuemember':
					if(is_array($val) === false)
						continue;

					foreach($val as $memberval)
						$this->_queuemember->delete($memberval);
					break;
			}
		}

		return(true);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_agentgroup() === false
		|| $this->edit_staticagentgroup() === false
		|| $this->edit_agentmember() === false
		|| $this->edit_queuemember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_agentgroup()
	{
		if(xivo_issa('agentgroup',$this->_info) === false
		|| ($rs = $this->get_result('agentgroup')) === null
		|| $rs === false)
			return(false);

		$rs['groupid'] = $this->_info['agentgroup']['groupid'];

		return(($this->_return['agentgroup'] = $this->_agentgroup->edit($this->_info['agentgroup']['id'],$rs)));
	}

	function edit_staticagentgroup()
	{
		if(xivo_issa('staticagentgroup',$this->_info) === false
		|| xivo_issa('agentgroup',$this->_info) === false
		|| ($rs = $this->_staticagentgroup->edit_group($this->_info['staticagentgroup']['id'],
							       $this->_info['agentgroup']['id'])) === null
		|| $rs === false)
			return(false);

		return(($this->_return['staticagentgroup'] = $this->_staticagentgroup->get_return()));
	}

	function edit_agentmember()
	{
		if(is_object($this->_agentmember) === false
		|| xivo_issa('agentgroup',$this->_info) === false)
			return(false);

		$r = $this->_agentmember->save($this->_info['agentgroup']['id']);

		$this->_return['agentmember'] = $this->_agentmember->get_return();

		return($r);
	}

	function edit_queuemember()
	{
		if(xivo_issa('agentgroup',$this->_info) === false)
			return(false);
		else if(xivo_issa('queuemember',$this->_info) === false)
			return($this->add_queuemember($this->_info['agentgroup']['id']));

		$agentgroupid = $this->_info['agentgroup']['id'];

		$this->_return['queuemember'] = $this->delete_queuemember();

		if(($rs = $this->get_result('queuemember')) === null)
			return($this->_return['queuemember']);
		else if($rs === false)
			return($rs);

		$ret = $this->_return['queuemember'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $agentgroupid;

			unset($rs[$i]['queuefeaturesid']);

			if(($interface = $this->mk_agent_interface($agentgroupid,true)) === false)
				return(false);

			$rs[$i]['interface'] = $interface['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return['queuemember'][] = $ret;
		}

		if(isset($this->_return['queuemember'][0]) === false)
			$this->_return['queuemember'] = false;

		return(true);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'agentgroup':
					if(isset($this->_origin['agentgroup']) === false
					|| $this->_origin['agentgroup'] === false)
						$this->delete_agentgroup();
					else
						$this->_agentgroup->edit_origin($this->_origin['agentgroup']);
					break;
				case 'staticagentgroup':
					if(isset($this->_origin['staticagentgroup']) === false
					|| $this->_origin['staticagentgroup'] === false)
						$this->delete_staticagentgroup();
					else
						$this->_staticagentgroup->edit_origin($this->_origin['staticagentgroup']);
					break;
				case 'agentmember':
					$this->_agentmember->reverse_save($this->_status);
					break;
				case 'queuemember':
					$this->delete_queuemember();

					if(isset($this->_origin['queuemember']) === true
					&& $this->_origin['queuemember'] !== false)
						$this->_queuemember->add_origin_list($this->_origin['queuemember']);
					break;
			}
		}

		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->delete_agentgroup() === false
		|| $this->delete_staticagentgroup() === false
		|| $this->delete_agentmember() === false
		|| $this->delete_queuemember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_agentgroup()
	{
		if(xivo_issa('agentgroup',$this->_info) === false
		|| (string) $this->_info['agentgroup']['id'] === (string) XIVO_SRE_IPBX_AST_AGENT_GROUP_DEFAULT)
			return(false);

		$r = $this->_agentgroup->delete($this->_info['agentgroup']['id']);

		if($this->_status === 'delete')
			$this->_return['agentgroup'] = $r;

		return($r);
	}

	function delete_staticagentgroup()
	{
		if(xivo_issa('staticagentgroup',$this->_info) === false
		|| ($r = $this->_staticagentgroup->delete_group($this->_status,
								$this->_info['staticagentgroup']['id'])) === null
		|| $r === false)
			return(false);
		else if($this->_status === 'delete')
			$this->_return['staticagentgroup'] = $r;

		return($r);
	}

	function delete_agentmember()
	{
		if(xivo_issa('agentmember',$this->_info) === false)
			return(null);
		else if(is_object($this->_agentmember) === false)
			return(false);
		else if(($r = $this->_agentmember->delete()) === null)
			return(null);
		else if($this->_status === 'delete')
			$this->_return['agentmember'] = $r;

		return($r);
	}

	function delete_queuemember()
	{
		if(xivo_issa('queuemember',$this->_info) === false)
			return(null);
		else if(xivo_issa('agentgroup',$this->_info) === false
		|| is_object($this->_queuemember) === false)
			return(false);

		$where = array();
		$where['usertype'] = 'agent';
		$where['category'] = 'group';
		$where['userid'] = $this->_info['agentgroup']['id'];

		$r = $this->_queuemember->delete_where($where);

		if($this->_status === 'delete')
			$this->_return['queuemember'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'agentgroup':
					if(xivo_issa('agentgroup',$this->_origin) === true)
						$this->_agentgroup->recover($this->_origin['agentgroup']['id']);
					break;
				case 'staticagentgroup':
					if(isset($this->_origin['staticagentgroup']) === true
					&& $this->_origin['staticagentgroup'] !== false)
						$this->_staticagentgroup->add_origin($this->_origin['staticagentgroup']);
					break;
				case 'agentmember':
					$this->_agentmember->reverse_delete($this->_status);
					break;
				case 'queuemember':
					if(isset($this->_origin['queuemember']) === true
					&& $this->_origin['queuemember'] !== false)
						$this->_queuemember->add_origin_list($this->_origin['queuemember']);
					break;
			}
		}

		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_agentgroup() === false
		|| $this->enable_staticagentgroup() === false
		|| $this->enable_agentmember() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_agentgroup() === false
		|| $this->disable_staticagentgroup() === false
		|| $this->disable_agentmember() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return(true);
	}

	function enable_agentgroup()
	{
		return($this->_enable_disable_agentgroup(false));
	}

	function disable_agentgroup()
	{
		return($this->_enable_disable_agentgroup(true));
	}

	function _enable_disable_agentgroup($disable=false)
	{
		if(xivo_issa('agentgroup',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_agentgroup->enable($this->_info['agentgroup']['id']);
		else
			$r = $this->_agentgroup->disable($this->_info['agentgroup']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['agentgroup'] = $r;

		return($r);
	}

	function enable_staticagentgroup()
	{
		return($this->_enable_disable_staticagentgroup(false));
	}

	function disable_staticagentgroup()
	{
		return($this->_enable_disable_staticagentgroup(true));
	}

	function _enable_disable_staticagentgroup($disable=false)
	{
		if(xivo_issa('staticagentgroup',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_staticagentgroup->enable($this->_info['staticagentgroup']['id']);
		else
			$r = $this->_staticagentgroup->disable($this->_info['staticagentgroup']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['staticagentgroup'] = $r;

		return($r);
	}

	function enable_agentmember()
	{
		return($this->_enable_disable_agentmember(false));
	}

	function disable_agentmember()
	{
		return($this->_enable_disable_agentmember(true));
	}

	function _enable_disable_agentmember($disable=false)
	{
		if(xivo_issa('agentmember',$this->_origin) === false)
			return(false);

		$disable = (bool) $disable;

		$rs = array();

		$r = true;

		foreach($this->_origin['agentmember'] as $val)
		{
			if($disable === false)
			{
				if(($r = $this->_agentmember->enable_agent($val['agentid'])) === false)
					break;
			}
			else if(($r = $this->_agentmember->disable_agent($val['agentid'])) === false)
				break;
		}

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['agentmember'] = $rs;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_status !== 'enable' && $this->_status !== 'disable') === true
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_status === 'enable';

		$this->_status = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'agentgroup':
					$this->_enable_disable_agentgroup($disable);
					break;
				case 'staticagentgroup':
					$this->_enable_disable_staticagentgroup($disable);
					break;
				case 'agentmember':
					$this->_enable_disable_agentmember($disable);
					break;
			}
		}

		return(true);
	}
}

?>
