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

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$group = isset($_QR['group']) === true ? dwho_uint($_QR['group'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';
$param['group'] = $group;

$info = $result = array();

switch($act)
{
	case 'add':
		$amember = $qmember = array();
		$amember['list'] = $qmember['list'] = false;
		$qmember['info'] = false;
		$amember['slt'] = $qmember['slt'] = array();

		$appagent = &$ipbx->get_application('agent',null,false);

		$amember['list'] = $appagent->get_agents_list(null,
							      array('firstname'	=> SORT_ASC,
								    'lastname'	=> SORT_ASC,
								    'number'	=> SORT_ASC,
								    'context'	=> SORT_ASC),
							      null,
							      true);

		$appqueue = &$ipbx->get_application('queue',null,false);

		if(($queues = $appqueue->get_queues_list(null,
							 array('name'		=> SORT_ASC),
							 null,
							 true)) !== false)
			$qmember['list'] = $queues;

		$appagentgroup = &$ipbx->get_application('agentgroup');

		$result = $fm_save = $error = null;

		if(isset($_QR['fm_send'])              === true
		&& dwho_issa('agentgroup',$_QR)        === true)
		{
			if($appagentgroup->set_add($_QR) === false
			|| $appagentgroup->add() === false)
			{
				$fm_save = false;
				$error = $appagentgroup->get_error();
				$result = $appagentgroup->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		}

		dwho::load_class('dwho_sort');

		if($amember['list'] !== false && dwho_ak('agentmember',$result) === true)
		{
			$amember['slt'] = dwho_array_intersect_key($result['agentmember'],
								   $amember['list'],
								   'id');

			if($amember['slt'] !== false)
			{
				$amember['list'] = dwho_array_diff_key($amember['list'],$amember['slt']);

				$agentsort = new dwho_sort(array('browse'	=> 'agentfeatures',
								 'key'		=> 'identity'));

				uasort($amember['slt'],array(&$agentsort,'str_usort'));
			}
		}

		if($qmember['list'] !== false && dwho_ak('queuemember',$result) === true)
		{
			$qmember['slt'] = dwho_array_intersect_key($result['queuemember'],
								   $qmember['list'],
								   'queuefeaturesid');

			if($qmember['slt'] !== false)
			{
				$qmember['info'] = dwho_array_copy_intersect_key($result['queuemember'],
										 $qmember['slt'],
										 'queuefeaturesid');

				$qmember['list'] = dwho_array_diff_key($qmember['list'],$qmember['slt']);

				$queuesort = new dwho_sort(array('key'	=> 'name'));

				uasort($qmember['slt'],array(&$queuesort,'str_usort'));
			}
		}

		$agentgroup_list = $appagentgroup->get_agentgroups_list(null,
									array('name'	=> SORT_ASC));

		$_TPL->set_var('info',$result);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appagentgroup->get_elements());
		$_TPL->set_var('amember',$amember);
		$_TPL->set_var('queues',$queues);
		$_TPL->set_var('qmember',$qmember);
		$_TPL->set_var('agentgroup_list',$agentgroup_list);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'edit':
		$appagentgroup = &$ipbx->get_application('agentgroup');

		if(isset($_QR['group']) === false
		|| ($info = $appagentgroup->get($_QR['group'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$amember = $qmember = array();
		$amember['list'] = $qmember['list'] = false;
		$qmember['info'] = false;
		$amember['slt'] = $qmember['slt'] = array();

		$appagent = &$ipbx->get_application('agent',null,false);

		$amember['list'] = $appagent->get_agents_list(null,
							      array('firstname'	=> SORT_ASC,
								    'lastname'	=> SORT_ASC,
								    'number'	=> SORT_ASC,
								    'context'	=> SORT_ASC),
							      null,
							      true);
		$appqueue = &$ipbx->get_application('queue',null,false);

		if(($queues = $appqueue->get_queues_list(null,
							 array('name'		=> SORT_ASC),
							 null,
							 true)) !== false)
			$qmember['list'] = $queues;

		$result = $fm_save = $error = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('agentgroup',$_QR) === true)
		{
			$return = &$result;

			if($appagentgroup->set_edit($_QR) === false
			|| $appagentgroup->edit() === false)
			{
				$fm_save = false;
				$error = $appagentgroup->get_error();
				$result = $appagentgroup->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		}

		dwho::load_class('dwho_sort');

		if($amember['list'] !== false && dwho_ak('agentmember',$return) === true)
		{
			$amember['slt'] = dwho_array_intersect_key($return['agentmember'],
								   $amember['list'],
								   'id');

			if($amember['slt'] !== false)
			{
				$amember['list'] = dwho_array_diff_key($amember['list'],$amember['slt']);

				$agentsort = new dwho_sort(array('browse'	=> 'agentfeatures',
								 'key'		=> 'identity'));

				uasort($amember['slt'],array(&$agentsort,'str_usort'));
			}
		}

		if($qmember['list'] !== false && dwho_ak('queuemember',$return) === true)
		{
			$qmember['slt'] = dwho_array_intersect_key($return['queuemember'],
								   $qmember['list'],
								   'queuefeaturesid');

			if($qmember['slt'] !== false)
			{
				$qmember['info'] = dwho_array_copy_intersect_key($return['queuemember'],
										 $qmember['slt'],
										 'queuefeaturesid');
				$qmember['list'] = dwho_array_diff_key($qmember['list'],$qmember['slt']);

				$queuesort = new dwho_sort(array('key'	=> 'name'));

				uasort($qmember['slt'],array(&$queuesort,'str_usort'));
			}
		}

		$agentgroup_list = $appagentgroup->get_agentgroups_list(null,
									array('name'	=> SORT_ASC));

		$_TPL->set_var('id',$info['agentgroup']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appagentgroup->get_elements());
		$_TPL->set_var('amember',$amember);
		$_TPL->set_var('queues',$queues);
		$_TPL->set_var('qmember',$qmember);
		$_TPL->set_var('agentgroup_list',$agentgroup_list);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appagentgroup = &$ipbx->get_application('agentgroup');

		if(isset($_QR['group']) === false
		|| ($info = $appagentgroup->get($_QR['group'])) === false
		|| (string) $info['agentgroup']['id'] === (string) XIVO_SRE_IPBX_AST_AGENT_GROUP_DEFAULT)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$appagentgroup->delete();

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('agentgroups',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$appagentgroup = &$ipbx->get_application('agentgroup');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $appagentgroup->get($values[$i])) !== false
			&& (string) $info['agentgroup']['id'] !== (string) XIVO_SRE_IPBX_AST_AGENT_GROUP_DEFAULT)
				$appagentgroup->delete();
		}

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('agentgroups',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$appagentgroup = &$ipbx->get_application('agentgroup');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appagentgroup->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appagentgroup->disable();
			else
				$appagentgroup->enable();
		}

		$ipbx->discuss('module reload chan_agent.so');
		$ipbx->discuss('xivo[agentlist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'addagent':
		$appagentgroup = &$ipbx->get_application('agentgroup',null,false);

		if(($agentgroup_list = $appagentgroup->get_agentgroups_list(null,
									    array('name' => SORT_ASC))) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$param['act'] = 'listagent';

		$umember = $qmember = array();
		$umember['list'] = $qmember['list'] = false;
		$umember['info'] = $qmember['info'] = false;
		$umember['slt'] = $qmember['slt'] = array();

		$appuser = &$ipbx->get_application('user',null,false);
		$umember['list'] = $appuser->get_users_list(null,
							    null,
							    array('firstname'	=> SORT_ASC,
								  'lastname'	=> SORT_ASC,
								  'number'	=> SORT_ASC,
								  'context'	=> SORT_ASC,
								  'name'	=> SORT_ASC),
							    null,
							    true);

		$appqueue = &$ipbx->get_application('queue',null);

		if(($queues = $appqueue->get_queues_list(null,
							 array('name'	=> SORT_ASC),
							 null,
							 true)) !== false)
			$qmember['list'] = $queues;

		$appagent = &$ipbx->get_application('agent');

		$result = $fm_save = $error = null;
		$queueskills = array();

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('agentfeatures',$_QR) === true
		&& dwho_issa('agentoptions',$_QR) === true
		&& dwho_issa('queueskill-skill',$_QR)  === true
		&& dwho_issa('queueskill-weight',$_QR) === true)
		{
			// skipping the last one (empty entry)
			$count = count($_QR['queueskill-skill']) - 1;
			for($i = 0; $i < $count; $i++)
			{
				$queueskills[] = array(
					'id'    	=> $_QR['queueskill-skill'][$i],
					'weight'	=> $_QR['queueskill-weight'][$i],
				);
			}
			$_QR['queueskills'] = $queueskills;

			if($appagent->set_add($_QR) === false
			|| $appagent->add() === false)
			{
				$fm_save = false;
				$result = $appagent->get_result();
				$queueskills = $result['queueskills'];

				$error = $appagent->get_error();
			}
			else
			{
				$ipbx->discuss('module reload chan_agent.so');
				$ipbx->discuss('xivo[agentlist,update]');

				$param['group'] = $appagent->get_result_var('agentfeatures','numgroup');
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
			}
		}

		dwho::load_class('dwho_sort');

		if($umember['list'] !== false && dwho_ak('usermember',$result) === true)
		{
			$umember['slt'] = dwho_array_intersect_key($result['usermember'],
								   $umember['list'],
								   'id');

			if($umember['slt'] !== false)
			{
				$umember['list'] = dwho_array_diff_key($umember['list'],$umember['slt']);

				$usersort = new dwho_sort(array('key'	=> 'identity'));

				uasort($umember['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($qmember['list'] !== false && dwho_ak('queuemember',$result) === true)
		{
			$qmember['slt'] = dwho_array_intersect_key($result['queuemember'],
								   $qmember['list'],
								   'queuefeaturesid');

			if($qmember['slt'] !== false)
			{
				$qmember['info'] = dwho_array_copy_intersect_key($result['queuemember'],
										 $qmember['slt'],
										 'queuefeaturesid');

				$qmember['list'] = dwho_array_diff_key($qmember['list'],$qmember['slt']);

				$queuesort = new dwho_sort(array('key'	=> 'name'));

				uasort($qmember['slt'],array(&$queuesort,'str_usort'));
			}
		}

		$element = $appagent->get_elements();
		$element['queueskills'] = $appqueue->skills_gettree();
		$_TPL->set_var('element', $element);

		$_TPL->set_var('queueskills', $queueskills);

		$_TPL->set_var('info',$result);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('umember',$umember);
		$_TPL->set_var('queues',$queues);
		$_TPL->set_var('qmember',$qmember);
		$_TPL->set_var('moh_list',$appagent->get_musiconhold());
		$_TPL->set_var('beep_list',$appagent->get_beep());
		$_TPL->set_var('goodbye_list',$appagent->get_goodbye());
		$_TPL->set_var('context_list',$appagent->get_context_list());
		$_TPL->set_var('agentgroup_list',$agentgroup_list);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'editagent':
		$appagent = &$ipbx->get_application('agent');

		if(isset($_QR['id']) === false
		|| ($info = $appagent->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$param['act'] = 'listagent';
		$param['group'] = $info['agentgroup']['id'];

		$umember = $qmember = array();
		$umember['list'] = $qmember['list'] = false;
		$umember['info'] = $qmember['info'] = false;
		$umember['slt'] = $qmember['slt'] = array();

		$appuser = &$ipbx->get_application('user',null,false);
		$umember['list'] = $appuser->get_users_list(null,
							    null,
							    array('firstname'	=> SORT_ASC,
								  'lastname'	=> SORT_ASC,
								  'number'	=> SORT_ASC,
								  'context'	=> SORT_ASC,
								  'name'	=> SORT_ASC),
							    null,
							    true);

		$appqueue = &$ipbx->get_application('queue',null);

		if(($queues = $appqueue->get_queues_list(null,
							 array('name'	=> SORT_ASC),
							 null,
							 true)) !== false)
			$qmember['list'] = $queues;

		$_TPL->set_var('queueskills', $appqueue->agentskills_get($_QR['id']));

		$result = $fm_save = $error = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('agentfeatures',$_QR) === true
		&& dwho_issa('agentoptions',$_QR) === true)
		{
			$return = &$result;
			$queueskills = array();

			// skipping the last one (empty entry)
			$count = count($_QR['queueskill-skill']) - 1;
			for($i = 0; $i < $count; $i++)
			{
				$queueskills[] = array(
					'agentid'	=> $_QR['id'],
					'skillid'	=> $_QR['queueskill-skill'][$i],
					'weight'	=> $_QR['queueskill-weight'][$i],
				);
			}
			$skillerr = $appqueue->agentskills_setedit($queueskills);

			if($appagent->set_edit($_QR) === false
			|| $appagent->edit() === false)
			{
				$fm_save = false;
				$result = $appagent->get_result();
				$error = $appagent->get_error();
				$error['queueskills'] = $appqueue->agentskills_get_error();

				$_TPL->set_var('queueskills', $queueskills);
			}
			else
			{
				// updating skills
				$appqueue->agentskills_edit($_QR['id'], $queueskills);

				$ipbx->discuss('module reload chan_agent.so');
				$ipbx->discuss('xivo[agentlist,update]');

				$param['group'] = $appagent->get_result_var('agentfeatures','numgroup');
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
			}
		}

		dwho::load_class('dwho_sort');

		if($umember['list'] !== false && dwho_ak('usermember',$return) === true)
		{
			$umember['slt'] = dwho_array_intersect_key($return['usermember'],
								   $umember['list'],
								   'id');

			if($umember['slt'] !== false)
			{
				$umember['list'] = dwho_array_diff_key($umember['list'],$umember['slt']);

				$usersort = new dwho_sort(array('key'	=> 'identity'));

				uasort($umember['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($qmember['list'] !== false && dwho_ak('queuemember',$return) === true)
		{
			$qmember['slt'] = dwho_array_intersect_key($return['queuemember'],
								   $qmember['list'],
								   'queuefeaturesid');

			if($qmember['slt'] !== false)
			{
				$qmember['info'] = dwho_array_copy_intersect_key($return['queuemember'],
										 $qmember['slt'],
										 'queuefeaturesid');

				$qmember['list'] = dwho_array_diff_key($qmember['list'],$qmember['slt']);

				$queuesort = new dwho_sort(array('key'	=> 'name'));

				uasort($qmember['slt'],array(&$queuesort,'str_usort'));
			}
		}

		$appagentgroup = &$ipbx->get_application('agentgroup',null,false);

		$agentgroup_list = $appagentgroup->get_agentgroups_list(null,
									array('name' => SORT_ASC));

		$element = $appagent->get_elements();
		$element['queueskills'] =  $appqueue->skills_gettree();
		$_TPL->set_var('element', $element);

		$_TPL->set_var('id',$info['agentfeatures']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('umember',$umember);
		$_TPL->set_var('queues',$queues);
		$_TPL->set_var('qmember',$qmember);
		$_TPL->set_var('moh_list',$appagent->get_musiconhold());
		$_TPL->set_var('beep_list',$appagent->get_beep());
		$_TPL->set_var('goodbye_list',$appagent->get_goodbye());
		$_TPL->set_var('context_list',$appagent->get_context_list());
		$_TPL->set_var('agentgroup_list',$agentgroup_list);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'deleteagent':
		$appagent = &$ipbx->get_application('agent');

		if(isset($_QR['id']) === false || $appagent->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$param['act'] = 'listagent';
		$param['numgroup'] = $appagent->get_info_var('agentfeatures','numgroup');
		$param['page'] = $page;

		$appagent->delete();

		$ipbx->discuss('module reload chan_agent.so');
		$ipbx->discuss('xivo[agentlist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'deleteagents':
		$param['act'] = 'listagent';
		$param['page'] = $page;

		if(($values = dwho_issa_val('agents',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$appagent = &$ipbx->get_application('agent');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appagent->get($values[$i]) !== false)
				$appagent->delete();
		}

		$ipbx->discuss('module reload chan_agent.so');
		$ipbx->discuss('xivo[agentlist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'enableagents':
	case 'disableagents':
		$param['act'] = 'listagent';
		$param['page'] = $page;

		if(($values = dwho_issa_val('agents',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);

		$appagent = &$ipbx->get_application('agent',null,false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appagent->get($values[$i]) === false)
				continue;
			else if($act === 'disableagents')
				$appagent->disable();
			else
				$appagent->enable();
		}

		$ipbx->discuss('module reload chan_agent.so');
		$ipbx->discuss('xivo[agentlist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'listagent':
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appagent = &$ipbx->get_application('agent',null,false);

		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appagent->get_agents_group($group,null,$order,$limit);
		$total = $appagent->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		}

		$appagentgroup = &$ipbx->get_application('agentgroup',null,false);

		$agentgroup_list = $appagentgroup->get_agentgroups_list(null,
									array('name' => SORT_ASC));

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
		$_TPL->set_var('agentgroup_list',$agentgroup_list);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appagentgroup = &$ipbx->get_application('agentgroup',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appagentgroup->get_agentgroups_list(null,$order,$limit);
		$total = $appagentgroup->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/agents'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
		$_TPL->set_var('agentgroup_list',$appagentgroup->get_agentgroups_list(null,$order));
}

$_TPL->set_var('act',$act);
$_TPL->set_var('group',$group);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
