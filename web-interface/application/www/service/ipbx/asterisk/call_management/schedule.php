<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appschedule = &$ipbx->get_application('schedule');

		$result = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('schedule',$_QR) === true)
		{
			if($appschedule->set_add($_QR) === false
			|| $appschedule->add() === false)
				$result = $appschedule->get_result_for_display();
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/schedule.js');

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$appschedule->get_elements());
		$_HTML->set_var('context_list',$appschedule->get_context_list());
		$_HTML->set_var('list',$appschedule->get_destination_list());
		break;
	case 'edit':
		$appschedule = &$ipbx->get_application('schedule');

		if(isset($_QR['id']) === false
		|| ($info = $appschedule->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);

		$result = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('schedule',$_QR) === true)
		{
			$return = &$result;

			if($appschedule->set_edit($_QR) === false
			|| $appschedule->edit() === false)
				$result = $appschedule->get_result_for_display();
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/schedule.js');

		$_HTML->set_var('id',$info['schedule']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('context_list',$appschedule->get_context_list());
		$_HTML->set_var('element',$appschedule->get_elements());
		$_HTML->set_var('list',$appschedule->get_destination_list());
		break;
	case 'delete':
		$param['page'] = $page;

		$appschedule = &$ipbx->get_application('schedule');

		if(isset($_QR['id']) === false || $appschedule->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);

		$appschedule->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('schedules',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);

		$appschedule = &$ipbx->get_application('schedule');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appschedule->get($values[$i]) !== false)
				$appschedule->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('schedules',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);

		$appschedule = &$ipbx->get_application('schedule');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appschedule->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appschedule->disable();
			else
				$appschedule->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appschedule = &$ipbx->get_application('schedule');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appschedule->get_schedules_list(null,$order,$limit);
		$total = $appschedule->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/schedule');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/schedule/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
