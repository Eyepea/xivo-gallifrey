<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$gfeatures = &$ipbx->get_module('groupfeatures');
$queue = &$ipbx->get_module('queue');

$result = $info = array();

switch($act)
{
	case 'add':
		if(isset($_QR['fm_send']) === true)
		{
			do
			{
				if(xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
					break;

				if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'],true,true)) === false
				|| ($gid = $gfeatures->add($result['gfeatures'])) === false)
				{
					$info['gfeatures'] = $gfeatures->get_filter_result();
					break;
				}

				$_QR['queue']['name'] = $result['gfeatures']['name'];

				if($queue->add($_QR['queue']) !== false)
					break;

				$gfeatures->delete($gid);
			}
			while(false);

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');
		}

		$_HTML->assign('queue_elt',$queue->get_element());
		$_HTML->assign('gfeatures_elt',$gfeatures->get_element());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');

		if(isset($_QR['fm_send']) === true)
		{
			do
			{
				if(xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
					break;

				if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'],true,true)) === false
				|| $gfeatures->edit($info['gfeatures']['id'],$result['gfeatures']) === false)
				{
					$info['mfeatures'] = array_merge($info['gfeatures'],$gfeatures->get_filter_result());
					break;
				}

				$_QR['queue']['name'] = $_QR['gfeatures']['name'];

				if($queue->edit($info['queue']['name'],$_QR['queue']) === false)
				{
					$gfeatures->edit_origin();
					break;
				}

				if($info['queue']['name'] === $_QR['queue']['name'])
					break;

				$qmember = &$ipbx->get_module('queuemember');
				if($qmember->update_by_name($info['queue']['name'],array('queue_name' => $_QR['queue']['name'])) === false)
				{
					$gfeatures->edit_origin();
					$queue->edit_origin();
					break;
				}
			}
			while(false);

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');
		}

		$_HTML->assign('info',$info);
		$_HTML->assign('queue_elt',$queue->get_element());
		$_HTML->assign('gfeatures_elt',$gfeatures->get_element());
		break;
	case 'delete':
		$qmember = &$ipbx->get_module('queuemember');

		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false
		|| $qmember->get_nb_by_name($info['queue']['name']) !== 0)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');

		do
		{
			if($gfeatures->disable($info['gfeatures']['id']) === false)
				break;

			if($queue->delete($info['queue']['name']) === false)
			{
				$gfeatures->enable($info['gfeatures']['id']);
				break;
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),'act=list');
		break;
	default:
		$act = 'list';
		$total = 0;
		$page = 1;

		if(($groups = $ipbx->get_groups_list(false)) !== false)
		{
			$total = count($groups);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
			usort($groups,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$groups);

}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/groups');

$_HTML->assign('bloc','pbx_settings/groups/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index',true);

?>
