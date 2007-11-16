<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$schedule = &$ipbx->get_module('schedule');

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('schedule',$_QR) === false
			|| isset($_QR['schedule']['timebeg'],
				 $_QR['schedule']['timeend']) === false)
				break;

			$result = array();

			$_QR['schedule']['linked'] = true;

			if(($time = $schedule->mk_time($_QR['schedule']['timebeg'],
						       $_QR['schedule']['timeend'])) !== false)
			{
				$timehourbeg = $_QR['schedule']['timebeg']['hour'];
				$timeminbeg = $_QR['schedule']['timebeg']['min'];
				$timehourend = $_QR['schedule']['timeend']['hour'];
				$timeminend = $_QR['schedule']['timeend']['min'];

				$_QR['schedule']['timebeg'] = $time['beg'];
				$_QR['schedule']['timeend'] = $time['end'];
			}
			else
			{
				$add = false;
				$timehourbeg = $timeminbeg = '';
				$timehourend = $timeminend = '';
			}

			if(($result['schedule'] = $schedule->chk_values($_QR['schedule'])) === false
			|| $schedule->is_valid($result['schedule']['typetrue'],
					       $result['schedule']['typevaltrue'],
					       $result['schedule']['applicationvaltrue']) === false
			|| $schedule->is_valid($result['schedule']['typefalse'],
					       $result['schedule']['typevalfalse'],
					       $result['schedule']['applicationvalfalse']) === false)
			{
				$add = false;
				$result['schedule'] = $schedule->get_filter_result();
			}

			if($add !== false)
			{
				if(($date = $schedule->chk_date($result['schedule'])) !== false)
					$result['schedule'] = array_merge($result['schedule'],$date);
				else
					$add = false;
			}

			if($add === false || ($scheduleid = $schedule->add($result['schedule'])) === false)
			{
				$result['schedule']['timehourbeg'] = $timehourbeg;
				$result['schedule']['timeminbeg'] = $timeminbeg;
				$result['schedule']['timehourend'] = $timehourend;
				$result['schedule']['timeminend'] = $timeminend;

				$result['schedule']['endcall'] = null;
				$result['schedule']['user'] = null;
				$result['schedule']['group'] = null;
				$result['schedule']['queue'] = null;
				$result['schedule']['meetme'] = null;
				$result['schedule']['schedule'] = null;
				$result['schedule']['application'] = null;
				$result['schedule']['custom'] = null;

				if(empty($result['schedule']['typetrue']) === false)
				{
					$result['schedule'][$result['schedule']['typetrue']]['true'] = $result['schedule']['typevaltrue'];
					$result['schedule'][$result['schedule']['typetrue']]['false'] = '';
				}

				if(empty($result['schedule']['typefalse']) === false)
				{
					$result['schedule'][$result['schedule']['typefalse']]['true'] = '';
					$result['schedule'][$result['schedule']['typefalse']]['false'] = $result['schedule']['typevalfalse'];
				}
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		}
		while(false);

		$list = array();

		$ufeatures = &$ipbx->get_module('userfeatures');

		if(($list['users'] = $ufeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['users'],array(&$sort,'str_usort'));
		}

		$gfeatures = &$ipbx->get_module('groupfeatures');

		if(($list['groups'] = $gfeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['groups'],array(&$sort,'str_usort'));
		}

		$qfeatures = &$ipbx->get_module('queuefeatures');

		if(($list['queues'] = $qfeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['queues'],array(&$sort,'str_usort'));
		}

		$mfeatures = &$ipbx->get_module('meetmefeatures');

		if(($list['meetme'] = $mfeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['meetme'],array(&$sort,'str_usort'));
		}

		if(($list['schedule'] = $schedule->get_all()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'name'));
			usort($list['schedule'],array(&$sort,'str_usort'));
		}

		$element = array();
		$element['schedule'] = $schedule->get_element();

		$_HTML->set_var('list',$list);
		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/schedule.js');
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info['schedule'] = $schedule->get($_QR['id'])) === false
		|| ($info['schedule']['linked'] === true
		   && (($info['typetrue'] = $schedule->is_valid($info['schedule']['typetrue'],
								$info['schedule']['typevaltrue'],
								$info['schedule']['applicationvaltrue'],
								$info['schedule']['id'])) === false
		      || ($info['typefalse'] = $schedule->is_valid($info['schedule']['typefalse'],
								   $info['schedule']['typevalfalse'],
								   $info['schedule']['applicationvalfalse'],
								   $info['schedule']['id'])) === false) === true) === true)
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);

		$edit = true;
		$return = &$info;

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('schedule',$_QR) === false
			|| isset($_QR['schedule']['timebeg'],
				 $_QR['schedule']['timeend']) === false)
				break;

			$result = array();

			$return = &$result;

			$_QR['schedule']['linked'] = true;
			$_QR['schedule']['commented'] = $info['schedule']['commented'];

			if(($time = $schedule->mk_time($_QR['schedule']['timebeg'],
						       $_QR['schedule']['timeend'])) !== false)
			{
				$timehourbeg = $_QR['schedule']['timebeg']['hour'];
				$timeminbeg = $_QR['schedule']['timebeg']['min'];
				$timehourend = $_QR['schedule']['timeend']['hour'];
				$timeminend = $_QR['schedule']['timeend']['min'];

				$_QR['schedule']['timebeg'] = $time['beg'];
				$_QR['schedule']['timeend'] = $time['end'];
			}
			else
			{
				$edit = false;
				$timehourbeg = $timeminbeg = '';
				$timehourend = $timeminend = '';
			}

			if(($result['schedule'] = $schedule->chk_values($_QR['schedule'])) === false
			|| $schedule->is_valid($result['schedule']['typetrue'],
					       $result['schedule']['typevaltrue'],
					       $result['schedule']['applicationvaltrue'],
					       $info['schedule']['id']) === false
			|| $schedule->is_valid($result['schedule']['typefalse'],
					       $result['schedule']['typevalfalse'],
					       $result['schedule']['applicationvalfalse'],
					       $info['schedule']['id']) === false)
			{
				$edit = false;
				$result['schedule'] = $schedule->get_filter_result();
			}

			if($edit !== false)
			{
				if(($date = $schedule->chk_date($result['schedule'])) !== false)
					$result['schedule'] = array_merge($result['schedule'],$date);
				else
					$edit = false;
			}

			if($edit === false || $schedule->edit($info['schedule']['id'],$result['schedule']) === false)
			{
				$result['schedule']['timehourbeg'] = $timehourbeg;
				$result['schedule']['timeminbeg'] = $timeminbeg;
				$result['schedule']['timehourend'] = $timehourend;
				$result['schedule']['timeminend'] = $timeminend;

				$result['schedule']['endcall'] = null;
				$result['schedule']['user'] = null;
				$result['schedule']['group'] = null;
				$result['schedule']['queue'] = null;
				$result['schedule']['meetme'] = null;
				$result['schedule']['schedule'] = null;
				$result['schedule']['application'] = null;
				$result['schedule']['custom'] = null;
				$result['schedule']['linked'] = $info['schedule']['linked'];

				if(empty($result['schedule']['typetrue']) === false)
				{
					$result['schedule'][$result['schedule']['typetrue']]['true'] = $result['schedule']['typevaltrue'];
					$result['schedule'][$result['schedule']['typetrue']]['false'] = '';
				}

				if(empty($result['schedule']['typefalse']) === false)
				{
					$result['schedule'][$result['schedule']['typefalse']]['true'] = '';
					$result['schedule'][$result['schedule']['typefalse']]['false'] = $result['schedule']['typevalfalse'];
				}

				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		}
		while(false);

		$list = array();

		$ufeatures = &$ipbx->get_module('userfeatures');

		if(($list['users'] = $ufeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['users'],array(&$sort,'str_usort'));
		}

		$gfeatures = &$ipbx->get_module('groupfeatures');

		if(($list['groups'] = $gfeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['groups'],array(&$sort,'str_usort'));
		}

		$qfeatures = &$ipbx->get_module('queuefeatures');

		if(($list['queues'] = $qfeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['queues'],array(&$sort,'str_usort'));
		}

		$mfeatures = &$ipbx->get_module('meetmefeatures');

		if(($list['meetme'] = $mfeatures->get_all_number()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'identity'));
			usort($list['meetme'],array(&$sort,'str_usort'));
		}

		if(($list['schedule'] = $schedule->get_all($info['schedule']['id'])) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'name'));
			usort($list['schedule'],array(&$sort,'str_usort'));
		}

		$element = array();
		$element['schedule'] = $schedule->get_element();

		$_HTML->set_var('id',$info['schedule']['id']);
		$_HTML->set_var('list',$list);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/schedule.js');
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true)
		{
			$id = intval($_QR['id']);

			if($schedule->delete($id) === false)
				continue;

			$schedule->unlinked_where(array('typetrue' => 'schedule',
							'typevaltrue' => $id));
	
			$schedule->unlinked_where(array('typefalse' => 'schedule',
			   				'typevalfalse' => $id));

			$incall = &$ipbx->get_module('incall');

			$incall->unlinked_where(array('type' => 'schedule',
						      'typeval' => $id));
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('schedules',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);

		$incall = &$ipbx->get_module('incall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			$id = intval($values[$i]);

			if($schedule->delete($id) === false)
				continue;

			$schedule->unlinked_where(array('typetrue' => 'schedule',
							'typevaltrue' => $id));
	
			$schedule->unlinked_where(array('typefalse' => 'schedule',
			   				'typevalfalse' => $id));

			$incall->unlinked_where(array('type' => 'schedule',
						      'typeval' => $id));
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;

		if(($values = xivo_issa_val('schedules',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$schedule->disable($values[$i],$disable);

		$_QRY->go($_HTML->url('service/ipbx/call_management/schedule'),$param);	
		break;
	default:
		$total = 0;
		$act = 'list';

		if(($list = $ipbx->get_schedule_list()) !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'schedule','key' => 'name'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
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
