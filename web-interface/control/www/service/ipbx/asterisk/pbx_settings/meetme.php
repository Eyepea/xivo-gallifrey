<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$meetme = &$ipbx->get_module('meetme');
$mfeatures = &$ipbx->get_module('meetmefeatures');

$result = $info = array();

switch($act)
{
	case 'add':
		if(isset($_QR['fm_send']) === true)
		{
			do
			{
				if(xivo_issa('meetme',$_QR) === false || xivo_issa('mfeatures',$_QR) === false)
					break;

				if(($mid = $meetme->add($_QR['meetme'])) === false)
					break;

				$_QR['mfeatures']['number'] = $_QR['meetme']['number'];
				$_QR['mfeatures']['meetmeid'] = $mid;

				if(($result['mfeatures'] = $mfeatures->chk_values($_QR['mfeatures'],true,true)) !== false
				&& $mfeatures->add($result['mfeatures']) !== false)
					break;

				$info['mfeatures'] = $mfeatures->get_filter_result();
				$meetme->delete($mid);
			}
			while(false);

			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list');
		}

		$_HTML->assign('info',$info);
		$_HTML->assign('mfeatures_elt',$mfeatures->get_element());
		break;
	case 'edit':
		$info = array();

		if(isset($_QR['id']) === false
		|| ($info['meetme'] = $meetme->get($_QR['id'])) === false
		|| ($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list');

		if(isset($_QR['fm_send']) === true)
		{
			do
			{
				if(xivo_issa('meetme',$_QR) === false || xivo_issa('mfeatures',$_QR) === false)
					break;

				if($meetme->edit($info['meetme']['id'],$_QR['meetme']) === false)
					break;

				$_QR['mfeatures']['number'] = $_QR['meetme']['number'];
				$_QR['mfeatures']['meetmeid'] = $info['mfeatures']['meetmeid'];

				if(($result['mfeatures'] = $mfeatures->chk_values($_QR['mfeatures'],true,true)) !== false
				&& $mfeatures->edit($info['mfeatures']['id'],$result['mfeatures']) !== false)
					break;

				$info['mfeatures'] = array_merge($info['mfeatures'],$mfeatures->get_filter_result());
				$meetme->edit_origin();
			}
			while(false);

			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list');
		}

		$_HTML->assign('info',$info);
		$_HTML->assign('mfeatures_elt',$mfeatures->get_element());
		break;
	case 'delete':
		$info = array();

		if(isset($_QR['id']) === false
		|| ($info['meetme'] = $meetme->get($_QR['id'])) === false
		|| ($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);

		do
		{
			if($meetme->delete($info['meetme']['id']) === false)
				break;

			if($mfeatures->delete($info['mfeatures']['id']) === true)
				break;

			$meetme->add_origin();
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);
		break;
	case 'enables':
	case 'disables':
		$disable = $act === 'disables' ? true : false;

		do
		{
			if(xivo_issa('meetme',$_QR) === false)
				break;

			$val = array_values($_QR['meetme']);

			if(($nb = count($val)) === false)
				break;

			for($i = 0;$i < $nb;$i++)
			{
				if($disable === true)
					$meetme->disable($val[$i]);
				else
					$meetme->enable($val[$i]);
			}
		}
		while(false);
		
		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);
		break;
	case 'deletes':
		$info = array();

		do
		{
			if(xivo_issa('meetme',$_QR) === false)
				break;

			$val = array_values($_QR['meetme']);

			if(($nb = count($val)) === false)
				break;

			for($i = 0;$i < $nb;$i++)
			{
				if(($info['meetme'] = $meetme->get($val[$i])) === false
				|| ($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
					continue;

				if($meetme->delete($info['meetme']['id']) === false)
					continue;

				if($mfeatures->delete($info['mfeatures']['id']) === true)
					continue;

				$meetme->add_origin();
			}
		}
		while(false);
		
		xivo_go($_HTML->url('service/ipbx/pbx_settings/meetme'),'act=list&page='.$page);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($meets = $ipbx->get_meetme_list()) !== false)
		{
			$total = count($meets);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'mfeatures','key' => 'name'));
			usort($meets,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$meets);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/meetme');

$_HTML->assign('act',$act);
$_HTML->assign('bloc','pbx_settings/meetme/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
