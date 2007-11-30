<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appoutcall = &$ipbx->get_application('outcall');

		$trunks = array();
		$trunks_list = $ipbx->get_trunks_list(null,false);

		$result = null;
		$rightcall['slt'] = $rightcall = array();

		xivo::load_class('xivo_sort');
		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true,false)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false)
				break;

			if($appoutcall->set_add($_QR) === false
			|| $appoutcall->add() === false)
			{
				$result = $appoutcall->get_result();

				if(xivo_issa('outcall',$result) === true && isset($result['outcall']['trunk']) === true)
					$trunks = $result['outcall']['trunk'];
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}
		while(false);

		if($rightcall['list'] !== false && xivo_ak('rightcall',$result) === true)
		{
			$rightcall['slt'] = xivo_array_intersect_key($result['rightcall'],$rightcall['list'],'rightcallid');

			if($rightcall['slt'] !== false)
			{
				$rightcall['list'] = xivo_array_diff_key($rightcall['list'],$rightcall['slt']);
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');

		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('element',$appoutcall->get_elements());
		$_HTML->set_var('info',$result);
		$_HTML->set_var('trunks_list',$trunks_list);
		break;
	case 'edit':
		$appoutcall = &$ipbx->get_application('outcall');

		if(isset($_QR['id']) === false || ($info = $appoutcall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$trunks = array();
		$trunks_list = $ipbx->get_trunks_list(null,false);

		$result = null;
		$return = &$info;
		$rightcall['slt'] = $rightcall = array();

		xivo::load_class('xivo_sort');
		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false)
				break;

			$return = &$result;

			if($appoutcall->set_edit($_QR) === false
			|| $appoutcall->edit() === false)
			{
				$result = $appoutcall->get_result();

				if(xivo_issa('outcall',$result) === true && isset($result['outcall']['trunk']) === true)
					$trunks = $result['outcall']['trunk'];
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}
		while(false);

		if($rightcall['list'] !== false && xivo_ak('rightcall',$return) === true)
		{
			$rightcall['slt'] = xivo_array_intersect_key($return['rightcall'],$rightcall['list'],'rightcallid');

			if($rightcall['slt'] !== false)
			{
				$rightcall['list'] = xivo_array_diff_key($rightcall['list'],$rightcall['slt']);
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		if(is_array($trunks_list) === true
		&& empty($trunks) === false)
		{
			if(is_array($trunks) === false)
				$trunks = explode(',',$trunks);

			$trunks_list = xivo_array_diff_key($trunks_list,$trunks);
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');

		$_HTML->set_var('id',$info['outcall']['id']);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('element',$appoutcall->get_elements());
		$_HTML->set_var('info',$return);
		$_HTML->set_var('trunks_list',$trunks_list);
		break;
	case 'delete':
		$param['page'] = $page;

		$appoutcall = &$ipbx->get_application('outcall');

		if(isset($_QR['id']) === false || $appoutcall->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$appoutcall->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$appoutcall = &$ipbx->get_application('outcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appoutcall->get($values[$i]) === false)
				continue;

			$appoutcall->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$appoutcall = &$ipbx->get_application('outcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appoutcall->get($values[$i]) === false)
				continue;

			if($act === 'disables')
				$appoutcall->disable();
			else
				$appoutcall->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appoutcall = &$ipbx->get_application('outcall');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		if(($list = $appoutcall->get_outcalls_list(null,$order,$limit)) !== false)
			$total = $appoutcall->get_cnt();

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}


$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/outcall');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/outcall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
