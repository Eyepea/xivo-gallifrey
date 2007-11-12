<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

$info = $result = array();

switch($act)
{
	case 'add':
		$appqueue = &$ipbx->get_application('queue');

		$result = null;

		$user = $agentgroup = $agent = array();
		$user['slt'] = $agentgroup['slt'] = $agent['slt'] = array();

		xivo::load_class('xivo_sort');
		$usersort = new xivo_sort(array('browse' => 'ufeatures','key' => 'identity'));

		if(($user['list'] = $ipbx->get_users_list(null,null,null,null,true)) !== false)
			uasort($user['list'],array(&$usersort,'str_usort'));

		$agentgroupsort = new xivo_sort(array('browse' => 'agroup','key' => 'name'));

		if(($agentgroup['list'] = $ipbx->get_agent_groups_list(null,true)) !== false)
			uasort($agentgroup['list'],array(&$agentgroupsort,'str_usort'));

		$agentsort = new xivo_sort(array('browse' => 'afeatures','key' => 'identity'));

		if(($agent['list'] = $ipbx->get_agents_list(null,null,true)) !== false)
			uasort($agent['list'],array(&$agentsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('qfeatures',$_QR) === false
			|| xivo_issa('queue',$_QR) === false)
				break;

			if($appqueue->set_add($_QR) === false
			|| $appqueue->add() === false)
			{
				$result = $appqueue->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		}
		while(false);

		if($user['list'] !== false && xivo_ak('user',$result) === true)
		{
			$user['slt'] = xivo_array_intersect_key($result['user'],$user['list'],'userid');
			
			if($user['slt'] !== false)
			{
				$user['list'] = xivo_array_diff_key($user['list'],$user['slt']);
				uasort($user['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($agentgroup['list'] !== false && xivo_ak('agentgroup',$result) === true)
		{
			$agentgroup['slt'] = xivo_array_intersect_key($result['agentgroup'],$agentgroup['list'],'agentgroupid');
			
			if($agentgroup['slt'] !== false)
			{
				$agentgroup['list'] = xivo_array_diff_key($agentgroup['list'],$agentgroup['slt']);
				uasort($agentgroup['slt'],array(&$agentgroupsort,'str_usort'));
			}
		}

		if($agent['list'] !== false && xivo_ak('agent',$result) === true)
		{
			$agent['slt'] = xivo_array_intersect_key($result['agent'],$agent['list'],'agentid');
			
			if($agent['slt'] !== false)
			{
				$agent['list'] = xivo_array_diff_key($agent['list'],$agent['slt']);
				uasort($agent['slt'],array(&$agentsort,'str_usort'));
			}
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$appqueue->get_elements());
		$_HTML->set_var('user',$user);
		$_HTML->set_var('agentgroup',$agentgroup);
		$_HTML->set_var('agent',$agent);
		$_HTML->set_var('moh_list',$appqueue->get_musiconhold());
		$_HTML->set_var('announce_list',$appqueue->get_announce());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$appqueue = &$ipbx->get_application('queue');

		if(isset($_QR['id']) === false || ($info = $appqueue->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		$result = null;
		$return = &$info;

		$user = $agentgroup = $agent = array();
		$user['slt'] = $agentgroup['slt'] = $agent['slt'] = array();

		xivo::load_class('xivo_sort');
		$usersort = new xivo_sort(array('browse' => 'ufeatures','key' => 'identity'));

		if(($user['list'] = $ipbx->get_users_list(null,null,null,null,true)) !== false)
			uasort($user['list'],array(&$usersort,'str_usort'));

		$agentgroupsort = new xivo_sort(array('browse' => 'agroup','key' => 'name'));

		if(($agentgroup['list'] = $ipbx->get_agent_groups_list(null,true)) !== false)
			uasort($agentgroup['list'],array(&$agentgroupsort,'str_usort'));

		$agentsort = new xivo_sort(array('browse' => 'afeatures','key' => 'identity'));

		if(($agent['list'] = $ipbx->get_agents_list(null,null,true)) !== false)
			uasort($agent['list'],array(&$agentsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('qfeatures',$_QR) === false
			|| xivo_issa('queue',$_QR) === false)
					break;

			$return = &$result;

			if($appqueue->set_edit($_QR) === false
			|| $appqueue->edit() === false)
			{
				$result = $appqueue->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		}
		while(false);

		if($user['list'] !== false && xivo_ak('user',$return) === true)
		{
			$user['slt'] = xivo_array_intersect_key($return['user'],$user['list'],'userid');
			
			if($user['slt'] !== false)
			{
				$user['list'] = xivo_array_diff_key($user['list'],$user['slt']);
				uasort($user['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($agentgroup['list'] !== false && xivo_ak('agentgroup',$return) === true)
		{
			$agentgroup['slt'] = xivo_array_intersect_key($return['agentgroup'],$agentgroup['list'],'userid');
			
			if($agentgroup['slt'] !== false)
			{
				$agentgroup['list'] = xivo_array_diff_key($agentgroup['list'],$agentgroup['slt']);
				uasort($agentgroup['slt'],array(&$agentgroupsort,'str_usort'));
			}
		}

		if($agent['list'] !== false && xivo_ak('agent',$return) === true)
		{
			$agent['slt'] = xivo_array_intersect_key($return['agent'],$agent['list'],'userid');
			
			if($agent['slt'] !== false)
			{
				$agent['list'] = xivo_array_diff_key($agent['list'],$agent['slt']);
				uasort($agent['slt'],array(&$agentsort,'str_usort'));
			}
		}

		$_HTML->set_var('id',$info['qfeatures']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('user',$user);
		$_HTML->set_var('agentgroup',$agentgroup);
		$_HTML->set_var('agent',$agent);
		$_HTML->set_var('element',$appqueue->get_elements());
		$_HTML->set_var('moh_list',$appqueue->get_musiconhold());
		$_HTML->set_var('announce_list',$appqueue->get_announce());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appqueue = &$ipbx->get_application('queue');

		if(isset($_QR['id']) === false || $appqueue->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		$appqueue->delete();

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('queues',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		$appqueue = &$ipbx->get_application('queue');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appqueue->get($values[$i]) === false)
				continue;

			$appqueue->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;
		$invdisable = $disable === true ? false : true;

		if(($values = xivo_issa_val('queues',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		$qfeatures = &$ipbx->get_module('queuefeatures');
		$queue = &$ipbx->get_module('queue');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $qfeatures->get($values[$i])) === false)
				continue;

			$queue->disable($info['name'],$disable);
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($queues = $ipbx->get_queues_list()) !== false)
		{
			$total = count($queues);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'qfeatures','key' => 'name'));
			usort($queues,array(&$sort,'str_usort'));
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$queues);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/queues');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/queues/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
