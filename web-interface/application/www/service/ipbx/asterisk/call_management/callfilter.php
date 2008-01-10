<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		$result = $callfiltermember = null;

		$secretary['slt'] = $secretary = array();
		$secretary['list'] = $appcallfilter->get_secretary_users(null,true);

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('callfilter',$_QR) === false)
				break;

			if($appcallfilter->set_add($_QR) === false
			|| $appcallfilter->add() === false)
			{
				$result = $appcallfilter->get_result();

				if(xivo_issa('callfiltermember',$result) === true)
					$callfiltermember = &$result['callfiltermember'];

				$result['dialstatus'] = $appcallfilter->get_dialstatus_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		}
		while(false);

		if($secretary['list'] !== false && xivo_issa('secretary',$callfiltermember) === true)
		{
			xivo::load_class('xivo_sort');
			$secretarysort = new xivo_sort(array('key' => 'priority'));
			usort($callfiltermember['secretary'],array(&$secretarysort,'num_usort'));

			$secretary['slt'] = xivo_array_intersect_key($callfiltermember['secretary'],$secretary['list'],'typeval');

			if($secretary['slt'] !== false)
				$secretary['list'] = xivo_array_diff_key($secretary['list'],$secretary['slt']);
		}

		if(empty($result) === false
		&& (xivo_issa('dialstatus',$result) === false || empty($result['dialstatus']) === true) === true)
			$result['dialstatus'] = null;

		$_HTML->set_var('info',$result);
		$_HTML->set_var('dialstatus',$result['dialstatus']);
		$_HTML->set_var('element',$appcallfilter->get_elements());
		$_HTML->set_var('dialstatus_list',$appcallfilter->get_dialstatus_destination_list());
		$_HTML->set_var('bosslist',$appcallfilter->get_free_boss_users());
		$_HTML->set_var('secretary',$secretary);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callfilter.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialstatus.js');
		break;
	case 'edit':
		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		if(isset($_QR['id']) === false || ($info = $appcallfilter->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$result = $callfiltermember = null;
		$return = &$info;

		$secretary['slt'] = $secretary = array();
		$secretary['list'] = $appcallfilter->get_secretary_users(null,true);

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('callfilter',$_QR) === false)
				break;

			$return = &$result;

			if($appcallfilter->set_edit($_QR) === false
			|| $appcallfilter->edit() === false)
			{
				$result = $appcallfilter->get_result();
				$result['dialstatus'] = $appcallfilter->get_dialstatus_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		}
		while(false);

		if(xivo_issa('callfiltermember',$return) === true)
			$callfiltermember = &$return['callfiltermember'];
		
		if($secretary['list'] !== false && xivo_issa('secretary',$callfiltermember) === true)
		{
			xivo::load_class('xivo_sort');
			$secretarysort = new xivo_sort(array('key' => 'priority'));
			usort($callfiltermember['secretary'],array(&$secretarysort,'num_usort'));

			$secretary['slt'] = xivo_array_intersect_key($callfiltermember['secretary'],$secretary['list'],'typeval');

			if($secretary['slt'] !== false)
				$secretary['list'] = xivo_array_diff_key($secretary['list'],$secretary['slt']);
		}

		if(empty($return) === false
		&& (xivo_issa('dialstatus',$return) === false || empty($return['dialstatus']) === true) === true)
			$return['dialstatus'] = null;

		$_HTML->set_var('id',$info['callfilter']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('dialstatus',$return['dialstatus']);
		$_HTML->set_var('element',$appcallfilter->get_elements());
		$_HTML->set_var('dialstatus_list',$appcallfilter->get_dialstatus_destination_list());
		$_HTML->set_var('bosslist',$appcallfilter->get_boss_users());
		$_HTML->set_var('secretary',$secretary);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callfilter.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialstatus.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		if(isset($_QR['id']) === false || $appcallfilter->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$appcallfilter->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('callfilters',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcallfilter->get($values[$i]) === false)
				continue;

			$appcallfilter->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('callfilters',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcallfilter->get($values[$i]) === false)
				continue;

			if($act === 'disables')
				$appcallfilter->disable();
			else
				$appcallfilter->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		break;
	default:
		$act = 'list';
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appcallfilter->get_callfilters_list(null,$order,$limit);
		$total = $appcallfilter->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/callfilter');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/callfilter/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

