<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_agent extends xivo_application_asterisk_abstract
{
	var $_agentfeatures	= false;
	var $_agentgroup	= false;
	var $_appagents		= false;
	var $_agent		= false;
	var $_agentoptions	= false;
	var $_contextmember	= false;
	var $_userfeatures	= null;
	var $_queuemember	= null;
	var $_contextlist	= array();
	var $_musiconhold	= array();

	function xivo_application_service_asterisk_agent(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_agentfeatures = &$this->_service->get_module('agentfeatures')) === false
		|| ($this->_agentgroup = &$this->_service->get_module('agentgroup')) === false
		|| ($this->_appagents = &$this->_service->get_apprealstatic('agents')) === false
		|| ($this->_agent = &$this->_appagents->get_module('agent')) === false
		|| ($this->_agentoptions = &$this->_appagents->get_module('agentoptions')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_contextmember = $this->get_abstract('contextmember',
								array('type' => 'agent'))) === false)
			return(false);

		$this->_userfeatures = &$this->_service->get_module('userfeatures');
		$this->_queuemember = &$this->_service->get_module('queuemember');

		return(true);
	}

	function get($id,$disable=null,$nocomponents=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['agentfeatures'] = $this->_agentfeatures->get($id,$disable)) === false
		|| ($this->_info['agent'] = $this->_agent->get_agent(
							$this->_info['agentfeatures']['agentid'])) === false
		|| ($this->_info['agentgroup'] = $this->_agentgroup->get(
							$this->_info['agentfeatures']['numgroup'])) === false
		|| ($this->_info['agentoptions'] = $this->_agentoptions->get_agentoptions(
							($this->_info['agent']['var_metric']-1))) === false)
			return(false);

		$this->_origin['agentfeatures'] = $this->_agentfeatures->get_origin();
		$this->_origin['agent'] = $this->_agent->get_origin();
		$this->_origin['agentoptions'] = $this->_agentoptions->get_origin();

		if($this->_components === false)
			return($this->_info);

		if(dwho_ak('contextmember',$nocomponents,true) === false)
			$this->_get_contextmember($this->_info['agentfeatures']['id']);

		if(dwho_ak('usermember',$nocomponents,true) === false)
			$this->_get_usermember();

		if(dwho_ak('queuemember',$nocomponents,true) === false)
			$this->_get_queuemember();

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_agentfeatures->get_nb($arr,$disable));
	}

	function _get_usermember()
	{
		$this->_info['usermember'] = false;
		$this->_origin['usermember'] = false;

		$where = array();
		$where['agentid'] = $this->_info['agentfeatures']['id'];

		if(is_object($this->_userfeatures) === false
		|| ($this->_info['usermember'] = $this->_userfeatures->get_all_where($where)) === false
		|| isset($this->_info['usermember'][0]) === false)
			return(false);

		$this->_origin['usermember'] = $this->_userfeatures->get_origin_list();

		return($this->_info['usermember']);
	}

	function _get_queuemember()
	{
		$this->_info['queuemember'] = false;
		$this->_origin['queuemember'] = false;

		$where = array();
		$where['usertype'] = 'agent';
		$where['category'] = 'queue';
		$where['userid'] = $this->_info['agentfeatures']['id'];

		if(is_object($this->_queuemember) === false
		|| ($queuefeatures = &$this->_service->get_module('queuefeatures')) === false
		|| ($this->_info['queuemember'] = $this->_queuemember->get_all_where($where)) === false
		|| ($nb = count($this->_info['queuemember'])) === 0)
		{
			$this->_info['queuemember'] = false;
			return(false);
		}

		$ref = &$this->_info['queuemember'];

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = $queuefeatures->get_primary(array('name' => $ref[$i]['queue_name']))) !== false)
				$ref[$i]['queuefeaturesid'] = $id;
		}

		$this->_origin['queuemember'] = $this->_queuemember->get_origin_list();

		return($this->_info['queuemember']);
	}

	function get_elements()
	{
		$r = array();

		$r['agent'] = $this->_agent->get_element();
		$r['agentfeatures'] = $this->_agentfeatures->get_element();
		$r['agentoptions'] = $this->_agentoptions->get_element();

		if(is_object($this->_queuemember) === true)
			$r['qmember'] = $this->_queuemember->get_element();
		else
			$r['qmember'] = false;

		return($r);
	}

	function get_beep()
	{
		return($this->_agentoptions->get_beep());
	}

	function get_goodbye()
	{
		return($this->_agentoptions->get_goodbye());
	}

	function get_agents_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_agents('search',$search,null,$disable,$order,$limit,$assoc));
	}

	function get_agents_group($group,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_agents('group',null,$group,$disable,$order,$limit,$assoc));
	}

	function get_agents_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_agents('list',null,null,$disable,$order,$limit,$assoc));
	}

	function _get_agents($action,$search='',$group=null,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$group = strval($group);
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_agentfeatures->search($search,$disable,$order,$limit);
				break;
			case 'group':
				$where = array();
				$where['numgroup'] = $group;

				$rs = $this->_agentfeatures->get_all_where($where,$disable,true,$order,$limit);
				break;
			case 'list':
			default:
				$rs = $this->_agentfeatures->get_all($disable,true,$order,$limit);
		}

		if($rs === false || ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_agentfeatures->get_cnt();
			return(false);
		}

		$this->_cnt = $this->_agentfeatures->get_cnt();

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['agentfeatures'] = &$rs[$i];

			if(($info['agent'] = $this->_agent->get_agent($info['agentfeatures']['agentid'],
								      $disable)) !== false
			&& ($info['agentoptions'] = $this->_agentoptions->get_agentoptions(
							($info['agent']['var_metric']-1))) !== false)
				$r[$info['agentfeatures']['id']] = $info;
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
		|| dwho_issa('agentfeatures',$arr) === false
		|| dwho_issa('agentoptions',$arr) === false)
			return(false);

		$arr['agentfeatures']['agentid'] = 0;
		$this->set_agentfeatures($arr['agentfeatures']);

		$arr['agent']['numgroup'] = (string) $this->get_result_var('agentfeatures','numgroup');
		$arr['agent']['number'] = (string) $this->get_result_var('agentfeatures','number');
		$arr['agent']['passwd'] = (string) $this->get_result_var('agentfeatures','passwd');
		$arr['agent']['firstname'] = (string) $this->get_result_var('agentfeatures','firstname');
		$arr['agent']['lastname'] = (string) $this->get_result_var('agentfeatures','lastname');

		$this->set_agent($arr['agent']);
		$this->set_agentoptions($arr['agentoptions']);
		$this->set_contextmember();

		if(dwho_issa('user-select',$arr) === true)
			$this->set_usermember($arr['user-select']);

		if(dwho_issa('queue',$arr) === true && dwho_issa('queue-select',$arr) === true)
			$this->set_queuemember($arr['queue'],$arr['queue-select'],$arr['agent']['number']);

		return(($this->get_errnb() < 1));
	}

	function set_agentfeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('agentfeatures',null);
			$this->_set_error('agentfeatures','invalid data');
			return(false);
		}
		else if(($rs = $this->_agentfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('agentfeatures',$this->_agentfeatures->get_filter_result());
			$this->_set_error('agentfeatures',$this->_agentfeatures->get_filter_error());
			return(false);
		}
		else if($this->_agentgroup->is_valid($rs['numgroup']) === false)
		{
			$this->_set_result('agentfeatures',$this->_agentfeatures->get_filter_result());
			$this->_set_error('agentfeatures','invalid group');
			return(false);
		}

		$this->_set_result('agentfeatures',$rs);

		return(true);
	}

	function set_agent($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('agent',null);
			$this->_set_error('agent','invalid data');
			return(false);
		}
		else if(($r = $this->_agent->set_agent($arr)) === false)
			$this->_set_error('agent',$this->_agent->get_error('agent'));

		$this->_set_result('agent',$this->_agent->get_result('agent'));

		return($r);
	}

	function set_agentoptions($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('agentoptions',null);
			$this->_set_error('agentoptions','invalid data');
			return(false);
		}
		else if(($r = $this->_agentoptions->set_agentoptions($arr)) === false)
			$this->_set_error('agentoptions',$this->_agentoptions->get_error('agentoptions'));

		$this->_set_result('agentoptions',$this->_agentoptions->get_result('agentoptions'));

		return($r);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['agent-context']['context'] = (string) $this->get_result_var('agentfeatures','context');
		$rs['agent-context']['typeval'] = 0;
		$rs['agent-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
	}

	function set_usermember($select)
	{
		if(is_array($select) === false
		|| is_object($this->_userfeatures) === false)
			return(false);

		$slt = array_values($select);

		if(($nb = count($slt)) === 0)
			return(null);

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($uinfo = $this->_userfeatures->get($slt[$i])) === false
			|| isset($tmp[$uinfo['id']]) === true)
				continue;

			$tmp[$uinfo['id']] = array();
			$tmp[$uinfo['id']]['id'] = $uinfo['id'];
			$tmp[$uinfo['id']]['agentid'] = $uinfo['agentid'];

			$reslist[] = $tmp[$uinfo['id']];
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('usermember',false);
			$this->_set_error('usermember','empty');
			return(false);
		}

		$this->_set_result('usermember',$reslist);

		return(true);
	}

	function set_queuemember($list,$select,$number)
	{
		if(is_array($list) === false
		|| is_array($select) === false
		|| ($queuefeatures = &$this->_service->get_module('queuefeatures')) === false
		|| is_object($this->_queuemember) === false)
			return(false);

		$slt = array_values($select);

		$error = false;

		if(($nb = count($slt)) === 0)
			return(null);
		else if(dwho_has_len($number) === false)
		{
			$error = true;
			$number = 0;
			$this->_set_error('queuemember','wrong agent number');
		}

		$info = array();
		$info['usertype'] = 'agent';
		$info['userid'] = 0;
		$info['category'] = 'queue';
		$info['call-limit'] = 0;

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$slt[$i];

			if(isset($tmp[$qname]) === true
			|| isset($list[$qname]) === false
			|| ($interface = $this->mk_agent_interface($number)) === false
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

			if($error === false)
				$this->_set_error('queuemember','empty');

			return(false);
		}

		$this->_set_result('queuemember',$reslist);

		return(($error === false));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$agentres = null;

		if($this->get_errnb() > 0
		|| ($agentres = $this->add_agent()) === false
		|| ($agentfeaturesid = $this->add_agentfeatures($agentres['id'])) === false
		|| $this->add_agentoptions($agentres['var_metric']) === false
		|| $this->add_contextmember($agentfeaturesid) === false
		|| $this->add_usermember($agentfeaturesid) === false
		|| $this->add_queuemember($agentfeaturesid) === false)
		{
			$r = false;
			if($agentres !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_agent()
	{
		if(($rs = $this->_agent->add_agent()) === null
		|| $rs === false)
			return(false);

		return(($this->_return['agent'] = $this->_agent->get_return()));
	}

	function add_agentfeatures($agentid)
	{
		if(($agentid = dwho_ulongint($agentid)) === 0
		|| ($rs = $this->get_result('agentfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['agentid'] = $agentid;

		return(($this->_return['agentfeatures'] = $this->_agentfeatures->add($rs)));
	}

	function add_agentoptions($var_metric)
	{
		if(($var_metric = dwho_uint($var_metric)) === 0
		|| ($rs = $this->_agentoptions->add_agentoptions(--$var_metric)) === null
		|| $rs === false)
			return(false);

		return(($this->_return['agentoptions'] = $this->_agentoptions->get_return()));
	}

	function add_usermember($agentfeaturesid)
	{
		$rs = false;

		if(($agentfeaturesid = dwho_ulongint($agentfeaturesid)) === 0
		|| ($rs = $this->get_result('usermember')) === null
		|| $rs === false)
			return($rs);

		$this->_return['usermember'] = array();

		$arr = array();
		$arr['agentid'] = $agentfeaturesid;

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_userfeatures->edit($rs[$i]['id'],$arr) === false)
				return(false);

			$this->_return['usermember'][] = $rs[$i];
		}

		if(isset($this->_return['usermember'][0]) === false)
			$this->_return['usermember'] = false;

		return(true);
	}

	function add_queuemember($agentfeaturesid)
	{
		$rs = false;

		if(($agentfeaturesid = dwho_ulongint($agentfeaturesid)) === 0
		|| ($rs = $this->get_result('queuemember')) === null
		|| $rs === false)
			return($rs);

		$ret = $this->_return['queuemember'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $agentfeaturesid;

			unset($rs[$i]['queuefeaturesid']);

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
				case 'agent':
					if(is_array($val) === true && isset($val['id']) === true)
						$this->_agent->delete_agent($this->_status,$val['id']);
					break;
				case 'agentfeatures':
					$this->_agentfeatures->delete($val);
					break;
				case 'agentoptions':
					$this->_agentoptions->delete_agentoptions($this->_status,$val);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_add();
					break;
				case 'usermember':
					if(is_array($val) === false)
						continue;

					foreach($val as $umemberval)
						$this->_userfeatures->edit($umemberval['id'],
									   array('agentid' => $umemberval['agentid']));
					break;
				case 'queuemember':
					if(is_array($val) === false)
						continue;

					foreach($val as $qmemberval)
						$this->_queuemember->delete($qmemberval);
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
		$agent_var_metric = null;

		if($this->get_errnb() > 0
		|| ($agent_var_metric = $this->edit_agent()) === false
		|| $this->edit_agentfeatures() === false
		|| $this->edit_agentoptions($agent_var_metric) === false
		|| $this->edit_contextmember() === false
		|| $this->edit_usermember() === false
		|| $this->edit_queuemember() === false)
		{
			$r = false;
			if($agent_var_metric !== null)
				$this->_reverse_edit();
		}
		else
		{
			$numgroup_info = (string) $this->_info['agentfeatures']['numgroup'];
			$numgroup_result = (string) $this->get_result_var('agentfeatures','numgroup');

			if($numgroup_info !== $numgroup_result)
				$this->_agent->add_deleted_agent($this->_info['agent']['var_metric']);
		}

		$this->_status = '';

		return($r);
	}

	function edit_agent()
	{
		if(dwho_issa('agent',$this->_info) === false
		|| dwho_issa('agentfeatures',$this->_info) === false
		|| ($rs = $this->_agent->edit_agent($this->_info['agent']['id'],
						    $this->_info['agentfeatures']['numgroup'],
						    $this->_info['agent']['var_metric'])) === null
		|| $rs === false)
			return(false);

		return(($this->_return['agent'] = $this->_agent->get_return()));
	}

	function edit_agentfeatures()
	{
		if(dwho_issa('agentfeatures',$this->_info) === false
		|| ($rs = $this->get_result('agentfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['agentid'] = $this->_info['agentfeatures']['agentid'];

		return(($this->_return['agentfeatures'] = $this->_agentfeatures->edit($this->_info['agentfeatures']['id'],$rs)));
	}

	function edit_agentoptions($var_metric)
	{
		if(dwho_issa('agentoptions',$this->_origin) === false
		|| isset($this->_origin['agentoptions'][0]) === false
		|| ($var_metric = dwho_uint($var_metric)) === 0
		|| ($rs = $this->_agentoptions->edit_agentoptions($this->_status,
								  --$var_metric,
								  $this->_origin['agentoptions'][0]['var_metric'])) === null
		|| $rs === false)
			return(false);

		return(($this->_return['agentoptions'] = $this->_agentoptions->get_return()));
	}

	function edit_contextmember()
	{
		if(dwho_issa('agentfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['agentfeatures']['id']));
	}

	function edit_usermember()
	{
		if(dwho_issa('agentfeatures',$this->_info) === false)
			return(false);
		else if(dwho_issa('usermember',$this->_info) === false)
			return($this->add_usermember($this->_info['agentfeatures']['id']));

		$this->_return['usermember'] = $this->delete_usermember();

		if(($rs = $this->get_result('usermember')) === null)
			return($this->_return['usermember']);
		else if($rs === false)
			return($rs);

		$this->_return['usermember'] = array();

		$arr = array();
		$arr['agentid'] = $this->_info['agentfeatures']['id'];

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_userfeatures->edit($rs[$i]['id'],$arr) === false)
				return(false);

			$this->_return['usermember'][] = $rs[$i];
		}

		if(isset($this->_return['usermember'][0]) === false)
			$this->_return['usermember'] = false;

		return(true);
	}

	function edit_queuemember()
	{
		if(dwho_issa('agentfeatures',$this->_info) === false)
			return(false);
		else if(dwho_issa('queuemember',$this->_info) === false)
			return($this->add_queuemember($this->_info['agentfeatures']['id']));

		$this->_return['queuemember'] = $this->delete_queuemember();

		if(($rs = $this->get_result('queuemember')) === null)
			return($this->_return['queuemember']);
		else if($rs === false)
			return($rs);

		$ret = $this->_return['queuemember'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $this->_info['agentfeatures']['id'];

			unset($rs[$i]['queuefeaturesid']);

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
				case 'agent':
					if(isset($this->_origin['agent']) === false
					|| $this->_origin['agent'] === false)
						$this->delete_agent();
					else
						$this->_agent->edit_origin($this->_origin['agent']);
					break;
				case 'agentfeatures':
					if(isset($this->_origin['agentfeatures']) === false
					|| $this->_origin['agentfeatures'] === false)
						$this->delete_agentfeatures();
					else
						$this->_agentfeatures->edit_origin($this->_origin['agentfeatures']);
					break;
				case 'agentoptions':
					$this->_agentoptions->delete_agentoptions($this->_status,$val);

					if(isset($this->_origin['agentoptions']) === true
					&& $this->_origin['agentoptions'] !== false)
						$this->_agentoptions->add_origin_list($this->_origin['agentoptions']);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_edit($this->_status);
					break;
				case 'usermember':
					if(is_array($val) === false)
						continue;

					foreach($val as $umemberval)
						$this->_userfeatures->edit($umemberval['id'],
									   array('agentid' => $umemberval['agentid']));
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

		if($this->delete_agent() === false
		|| $this->delete_agentfeatures() === false
		|| $this->delete_agentoptions() === false
		|| $this->delete_contextmember() === false
		|| $this->delete_usermember() === false
		|| $this->delete_queuemember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_agent()
	{
		if(dwho_issa('agent',$this->_info) === false
		|| ($r = $this->_agent->delete_agent($this->_status,
						     $this->_info['agent']['id'])) === null
		|| $r === false)
			return(false);
		else if($this->_status === 'delete')
			$this->_return['agent'] = $r;

		return($r);
	}

	function delete_agentfeatures()
	{
		if(dwho_issa('agentfeatures',$this->_info) === false)
			return(false);

		$r = $this->_agentfeatures->delete($this->_info['agentfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['agentfeatures'] = $r;

		return($r);
	}

	function delete_agentoptions()
	{
		if(dwho_issa('agentoptions',$this->_origin) === false
		|| isset($this->_origin['agentoptions'][0]['var_metric']) === false
		|| ($r = $this->_agentoptions->delete_agentoptions($this->_status,
								   $this->_origin['agentoptions'][0]['var_metric'])) === null
		|| $r === false)
			return(false);
		else if($this->_status === 'delete')
			$this->_return['agentoptions'] = $r;

		return($r);
	}

	function delete_usermember()
	{
		if(dwho_issa('usermember',$this->_info) === false)
			return(null);
		else if(dwho_issa('agentfeatures',$this->_info) === false
		|| is_object($this->_userfeatures) === false)
			return(false);

		$where = array();
		$where['agentid'] = $this->_info['agentfeatures']['id'];

		$value = array();
		$value['agentid'] = 0;

		$r = $this->_userfeatures->edit_where($where,$value);

		if($this->_status === 'delete')
			$this->_return['usermember'] = $r;

		return($r);
	}

	function delete_queuemember()
	{
		if(dwho_issa('queuemember',$this->_info) === false)
			return(null);
		else if(dwho_issa('agentfeatures',$this->_info) === false
		|| is_object($this->_queuemember) === false)
			return(false);

		$where = array();
		$where['usertype'] = 'agent';
		$where['category'] = 'queue';
		$where['userid'] = $this->_info['agentfeatures']['id'];

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
				case 'agent':
					if(isset($this->_origin['agent']) === true
					&& $this->_origin['agent'] !== false)
						$this->_agent->add_origin($this->_origin['agent']);
					break;
				case 'agentfeatures':
					if(isset($this->_origin['agentfeatures']) === true
					&& $this->_origin['agentfeatures'] !== false)
						$this->_agentfeatures->add_origin($this->_origin['agentfeatures']);
					break;
				case 'agentoptions':
					if(isset($this->_origin['agentoptions']) === true
					&& $this->_origin['agentoptions'] !== false)
						$this->_agentoptions->add_origin_list($this->_origin['agentoptions']);
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
				case 'usermember':
					if(isset($this->_origin['usermember']) === true
					&& $this->_origin['usermember'] !== false)
						$this->_userfeatures->add_origin_list($this->_origin['usermember']);
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

		if($this->enable_agent() === false
		|| $this->enable_agentfeatures() === false)
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

		if($this->disable_agent() === false
		|| $this->disable_agentfeatures() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return(true);
	}

	function enable_agent()
	{
		return($this->_enable_disable_agent(false));
	}

	function disable_agent()
	{
		return($this->_enable_disable_agent(true));
	}

	function _enable_disable_agent($disable=false)
	{
		if(dwho_issa('agent',$this->_info) === false)
			return(false);
		else if((bool) $disable === true)
			$r = $this->_agent->disable($this->_info['agent']['id']);
		else if($this->_info['agentgroup']['commented'] === false)
			$r = $this->_agent->enable($this->_info['agent']['id']);
		else
			return(null);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['agent'] = $r;

		return($r);
	}

	function enable_agentfeatures()
	{
		return($this->_enable_disable_agentfeatures(false));
	}

	function disable_agentfeatures()
	{
		return($this->_enable_disable_agentfeatures(true));
	}

	function _enable_disable_agentfeatures($disable=false)
	{
		if(dwho_issa('agentfeatures',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_agentfeatures->enable($this->_info['agentfeatures']['id']);
		else
			$r = $this->_agentfeatures->disable($this->_info['agentfeatures']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['agentfeatures'] = $r;

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
				case 'agent':
					$this->_enable_disable_agent($disable);
					break;
				case 'agentfeatures':
					$this->_enable_disable_agentfeatures($disable);
					break;
			}
		}

		return(true);
	}

	function add_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_add($data) === false
		|| $this->add() === false)
			return(false);

		return(true);
	}

	function edit_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_edit($data) === false
		|| $this->edit() === false)
			return(false);

		return(true);
	}
}

?>
