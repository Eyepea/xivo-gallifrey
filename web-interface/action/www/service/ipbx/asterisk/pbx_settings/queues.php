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
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

$info = $result = array();

switch($act)
{
	case 'add':
		$appqueue = &$ipbx->get_application('queue');

		$result = $fm_save = null;

		$pannounce = array();
		$pannounce['list'] = $appqueue->get_announce();
		$pannounce['slt'] = array();

		$user = $agentgroup = $agent = array();
		$user['slt'] = $agentgroup['slt'] = $agent['slt'] = array();

		$userorder = array();
		$userorder['firstname'] = SORT_ASC;
		$userorder['lastname'] = SORT_ASC;
		$userorder['number'] = SORT_ASC;
		$userorder['context'] = SORT_ASC;
		$userorder['name'] = SORT_ASC;

		$appuser = &$ipbx->get_application('user',null,false);
		$user['list'] = $appuser->get_users_list(null,null,$userorder,null,true);

		$appagentgroup = &$ipbx->get_application('agentgroup',null,false);
		$agentgroup['list'] = $appagentgroup->get_agentgroups_list(null,
									   array('name'	=> SORT_ASC),
									   null,
									   true);

		$appagent = &$ipbx->get_application('agent',null,false);
		$agent['list'] = $appagent->get_agents_list(null,
							    array('firstname'	=> SORT_ASC,
								  'lastname'	=> SORT_ASC,
								  'number'	=> SORT_ASC,
								  'context'	=> SORT_ASC),
							    null,
							    true);

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('queuefeatures',$_QR) === true
		&& dwho_issa('queue',$_QR) === true)
		{
			if($appqueue->set_add($_QR) === false
			|| $appqueue->add() === false)
			{
				$fm_save = false;
				$result = $appqueue->get_result();
				$result['dialaction'] = $appqueue->get_dialaction_result();
			}
			else
			{
				$ipbx->discuss('xivo[queuelist,update]');
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);
			}
		}

		if($pannounce['list'] !== false
		&& dwho_issa('queue',$result) === true
		&& dwho_ak('periodic-announce',$result['queue']) === true
		&& empty($result['queue']['periodic-announce']) === false)
		{
			if(is_array($result['queue']['periodic-announce']) === false)
				$pannounce['slt'] = explode('|',$result['queue']['periodic-announce']);
			else
				$pannounce['slt'] = $result['queue']['periodic-announce'];

			$pannounce['slt'] = dwho_array_intersect_key(array_flip($pannounce['slt']),
								     $pannounce['list']);

			if(empty($pannounce['slt']) === false)
				$pannounce['list'] = dwho_array_diff_key($pannounce['list'],
									 $pannounce['slt']);
		}

		dwho::load_class('dwho_sort');

		if($user['list'] !== false && dwho_ak('user',$result) === true)
		{
			$user['slt'] = dwho_array_intersect_key($result['user'],
								$user['list'],
								'userid');

			if($user['slt'] !== false)
			{
				$user['list'] = dwho_array_diff_key($user['list'],$user['slt']);

				$usersort = new dwho_sort(array('key' => 'identity'));
				uasort($user['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($agentgroup['list'] !== false && dwho_ak('agentgroup',$result) === true)
		{
			$agentgroup['slt'] = dwho_array_intersect_key($result['agentgroup'],
								      $agentgroup['list'],
								      'userid');

			if($agentgroup['slt'] !== false)
			{
				$agentgroupsort = new dwho_sort(array('browse'	=> 'agentgroup',
								      'key'	=> 'name'));

				$agentgroup['list'] = dwho_array_diff_key($agentgroup['list'],$agentgroup['slt']);
				uasort($agentgroup['slt'],array(&$agentgroupsort,'str_usort'));
			}
		}

		if($agent['list'] !== false && dwho_ak('agent',$result) === true)
		{
			$agent['slt'] = dwho_array_intersect_key($result['agent'],
								 $agent['list'],
								 'userid');

			if($agent['slt'] !== false)
			{
				$agentsort = new dwho_sort(array('browse'	=> 'agentfeatures',
								 'key'		=> 'identity'));

				$agent['list'] = dwho_array_diff_key($agent['list'],$agent['slt']);
				uasort($agent['slt'],array(&$agentsort,'str_usort'));
			}
		}

		if(empty($result) === false)
		{
			if(dwho_issa('dialaction',$result) === false || empty($result['dialaction']) === true)
				$result['dialaction'] = null;

			if(dwho_issa('callerid',$result) === false || empty($result['callerid']) === true)
				$result['callerid'] = null;
		}

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/queues.js');
		$dhtml->set_js('js/dwho/submenu.js');

		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('dialaction',$result['dialaction']);
		$_TPL->set_var('dialaction_from','queue');
		$_TPL->set_var('element',$appqueue->get_elements());
		$_TPL->set_var('user',$user);
		$_TPL->set_var('agentgroup',$agentgroup);
		$_TPL->set_var('agent',$agent);
		$_TPL->set_var('pannounce',$pannounce);
		$_TPL->set_var('destination_list',$appqueue->get_dialaction_destination_list());
		$_TPL->set_var('moh_list',$appqueue->get_musiconhold());
		$_TPL->set_var('announce_list',$appqueue->get_announce());
		$_TPL->set_var('context_list',$appqueue->get_context_list());
		break;
	case 'edit':
		$appqueue = &$ipbx->get_application('queue');

		if(isset($_QR['id']) === false || ($info = $appqueue->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);

		$result = $fm_save = null;
		$return = &$info;

		$pannounce = array();
		$pannounce['list'] = $appqueue->get_announce();
		$pannounce['slt'] = array();

		$user = $agentgroup = $agent = array();
		$user['slt'] = $agentgroup['slt'] = $agent['slt'] = array();

		$userorder = array();
		$userorder['firstname'] = SORT_ASC;
		$userorder['lastname'] = SORT_ASC;
		$userorder['number'] = SORT_ASC;
		$userorder['context'] = SORT_ASC;
		$userorder['name'] = SORT_ASC;

		$appuser = &$ipbx->get_application('user',null,false);
		$user['list'] = $appuser->get_users_list(null,null,$userorder,null,true);

		$appagentgroup = &$ipbx->get_application('agentgroup',null,false);
		$agentgroup['list'] = $appagentgroup->get_agentgroups_list(null,
									   array('name'	=> SORT_ASC),
									   null,
									   true);

		$appagent = &$ipbx->get_application('agent',null,false);
		$agent['list'] = $appagent->get_agents_list(null,
							    array('firstname'	=> SORT_ASC,
								  'lastname'	=> SORT_ASC,
								  'number'	=> SORT_ASC,
								  'context'	=> SORT_ASC),
							    null,
							    true);

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('queuefeatures',$_QR) === true
		&& dwho_issa('queue',$_QR) === true)
		{
			$return = &$result;

			if($appqueue->set_edit($_QR) === false
			|| $appqueue->edit() === false)
			{
				$fm_save = false;
				$result = $appqueue->get_result();
				$result['dialaction'] = $appqueue->get_dialaction_result();
			}
			else
			{
				$ipbx->discuss('xivo[queuelist,update]');
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);
			}
		}

		if($pannounce['list'] !== false
		&& dwho_issa('queue',$return) === true
		&& dwho_ak('periodic-announce',$return['queue']) === true
		&& empty($return['queue']['periodic-announce']) === false)
		{
			if(is_array($return['queue']['periodic-announce']) === false)
				$pannounce['slt'] = explode('|',$return['queue']['periodic-announce']);
			else
				$pannounce['slt'] = $return['queue']['periodic-announce'];

			$pannounce['slt'] = dwho_array_intersect_key(array_flip($pannounce['slt']),
								     $pannounce['list']);

			if(empty($pannounce['slt']) === false)
				$pannounce['list'] = dwho_array_diff_key($pannounce['list'],
									 $pannounce['slt']);
		}

		dwho::load_class('dwho_sort');

		if($user['list'] !== false && dwho_ak('user',$return) === true)
		{
			$user['slt'] = dwho_array_intersect_key($return['user'],
								$user['list'],
								'userid');

			if($user['slt'] !== false)
			{
				$user['list'] = dwho_array_diff_key($user['list'],$user['slt']);

				$usersort = new dwho_sort(array('key' => 'identity'));
				uasort($user['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($agentgroup['list'] !== false && dwho_ak('agentgroup',$return) === true)
		{
			$agentgroup['slt'] = dwho_array_intersect_key($return['agentgroup'],
								      $agentgroup['list'],
								      'userid');

			if($agentgroup['slt'] !== false)
			{
				$agentgroupsort = new dwho_sort(array('browse'	=> 'agentgroup',
								      'key'	=> 'name'));

				$agentgroup['list'] = dwho_array_diff_key($agentgroup['list'],$agentgroup['slt']);
				uasort($agentgroup['slt'],array(&$agentgroupsort,'str_usort'));
			}
		}

		if($agent['list'] !== false && dwho_ak('agent',$return) === true)
		{
			$agent['slt'] = dwho_array_intersect_key($return['agent'],
								 $agent['list'],
								 'userid');

			if($agent['slt'] !== false)
			{
				$agentsort = new dwho_sort(array('browse'	=> 'agentfeatures',
								 'key'		=> 'identity'));

				$agent['list'] = dwho_array_diff_key($agent['list'],$agent['slt']);
				uasort($agent['slt'],array(&$agentsort,'str_usort'));
			}
		}

		if(empty($return) === false)
		{
			if(dwho_issa('dialaction',$return) === false || empty($return['dialaction']) === true)
				$return['dialaction'] = null;

			if(dwho_issa('callerid',$return) === false || empty($return['callerid']) === true)
				$return['callerid'] = null;
		}

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/queues.js');
		$dhtml->set_js('js/dwho/submenu.js');

		$_TPL->set_var('id',$info['queuefeatures']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('dialaction',$return['dialaction']);
		$_TPL->set_var('dialaction_from','queue');
		$_TPL->set_var('element',$appqueue->get_elements());
		$_TPL->set_var('user',$user);
		$_TPL->set_var('agentgroup',$agentgroup);
		$_TPL->set_var('agent',$agent);
		$_TPL->set_var('pannounce',$pannounce);
		$_TPL->set_var('destination_list',$appqueue->get_dialaction_destination_list());
		$_TPL->set_var('moh_list',$appqueue->get_musiconhold());
		$_TPL->set_var('announce_list',$appqueue->get_announce());
		$_TPL->set_var('context_list',$appqueue->get_context_list());
		break;
	case 'delete':
		$param['page'] = $page;

		$appqueue = &$ipbx->get_application('queue');

		if(isset($_QR['id']) === false || $appqueue->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);

		$appqueue->delete();

		$ipbx->discuss('xivo[queuelist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('queues',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);

		$appqueue = &$ipbx->get_application('queue');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appqueue->get($values[$i]) !== false)
				$appqueue->delete();
		}

		$ipbx->discuss('xivo[queuelist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables';
		$invdisable = $disable === false;

		if(($values = dwho_issa_val('queues',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);

		$queuefeatures = &$ipbx->get_module('queuefeatures');
		$queue = &$ipbx->get_module('queue');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $queuefeatures->get($values[$i])) !== false)
				$queue->disable($info['name'],$disable);
		}

		$ipbx->discuss('xivo[queuelist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appqueue = &$ipbx->get_application('queue',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appqueue->get_queues_list(null,$order,$limit);
		$total = $appqueue->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/queues'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/queues');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/queues/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
