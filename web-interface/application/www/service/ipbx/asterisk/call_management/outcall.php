<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$outcall = &$ipbx->get_module('outcall');

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		$extenumbers = &$ipbx->get_module('extenumbers');
		$tfeatures = &$ipbx->get_module('trunkfeatures');
		$trunks_list = $ipbx->get_trunks_list();

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('outcall',$_QR) === false)
				break;

			$result = array();

			if(($result['outcall'] = $outcall->chk_values($_QR['outcall'])) === false
			|| $tfeatures->get_id($result['outcall']['trunkfeaturesid']) === false
			|| ($result['outcall']['mode'] === 'numlen'
			   && (int) $result['outcall']['numlen'] === 0) === true)
			{
				$add = false;
				$result['outcall'] = $outcall->get_filter_result();
			}

			$result['extenumbers'] = null;

			do
			{
				if($result['outcall']['mode'] === 'numlen')
					break;

				if(xivo_issa('extenumbers',$_QR) === false)
				{
					$add = false;
					break;
				}

				$_QR['extenumbers']['context'] = $result['outcall']['context'];
				$_QR['extenumbers']['extenmode'] = $result['outcall']['mode'];

				if(($result['extenumbers'] = $extenumbers->chk_values($_QR['extenumbers'])) !== false
				&& $extenumbers->exists($result['extenumbers']) === false)
					break;

				$add = false;
						
				if($result['extenumbers'] === false)
					$result['extenumbers'] = $extenumbers->get_filter_result();
				else
				{
					$result['extenumbers']['exten'] = '';
					$result['extenumbers']['rangebeg'] = '';
					$result['extenumbers']['rangeend'] = '';
				}
			}
			while(false);

			if($add === false)
				break;

			$extenumid = 0;
				
			if($result['extenumbers'] !== null
			&& ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
				break;

			$result['outcall']['extenid'] = $extenumid;

			if($outcall->add($result['outcall']) === false)
			{
				if($extenumid !== 0)
					$extenumbers->delete($extenumid);
				break;
			}

			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}
		while(false);

		$element['extenumbers'] = $extenumbers->get_element();
		$element['outcall'] = $outcall->get_element();

		$_HTML->assign('info',$result);
		$_HTML->assign('element',$element);
		$_HTML->assign('trunks_list',$trunks_list);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');
		break;
	case 'edit':
		$extenumbers = &$ipbx->get_module('extenumbers');
		$tfeatures = &$ipbx->get_module('trunkfeatures');

		$info['extenumbers'] = null;

		if(isset($_QR['id']) === false
		|| ($info['outcall'] = $outcall->get($_QR['id'])) === false
		|| ($info['tfeatures'] = $tfeatures->get($info['outcall']['trunkfeaturesid'])) === false
		|| ($info['outcall']['mode'] !== 'numlen'
		   && ($info['extenumbers'] = $extenumbers->get($info['outcall']['extenid'])) === false) === true)
			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$edit = true;

		$return = &$info;

		$trunks_list = $ipbx->get_trunks_list();

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('outcall',$_QR) === false)
				break;

			$result = array();

			$return = &$result;

			if(($result['outcall'] = $outcall->chk_values($_QR['outcall'])) === false
			|| $tfeatures->get_id($result['outcall']['trunkfeaturesid']) === false
			|| ($result['outcall']['mode'] === 'numlen'
			   && (int) $result['outcall']['numlen'] === 0) === true)
			{
				$edit = false;
				$result['outcall'] = $outcall->get_filter_result();
			}

			$result['extenumbers'] = null;

			do
			{
				if($result['outcall']['mode'] === 'numlen')
					break;

				if(xivo_issa('extenumbers',$_QR) === false)
				{
					$edit = false;
					break;
				}

				if($info['outcall']['mode'] === 'numlen')
					$extenumid = null;
				else
					$extenumid = $info['outcall']['extenid'];

				$_QR['extenumbers']['context'] = $result['outcall']['context'];
				$_QR['extenumbers']['extenmode'] = $result['outcall']['mode'];

				if(($result['extenumbers'] = $extenumbers->chk_values($_QR['extenumbers'])) !== false
				&& $extenumbers->exists($result['extenumbers'],$extenumid) === false)
					break;

				$edit = false;
						
				if($result['extenumbers'] === false)
					$result['extenumbers'] = $extenumbers->get_filter_result();
				else
				{
					$result['extenumbers']['exten'] = '';
					$result['extenumbers']['rangebeg'] = '';
					$result['extenumbers']['rangeend'] = '';
				}
			}
			while(false);

			if($edit === false)
				break;

			$extenumid = 0;

			if($result['extenumbers'] === null)
			{
				if($info['extenumbers'] !== null
				&& $extenumbers->delete($info['extenumbers']['id']) === false)
					break;
			}
			else if($info['extenumbers'] !== null)
			{
				if($extenumbers->edit($info['extenumbers']['id'],$result['extenumbers']) === false)
					break;

				$extenumid = $info['extenumbers']['id'];
			}
			else if(($extenumid = $extenumbers->add($result['extenumbers'])) === false) 
				break;

			$result['outcall']['extenid'] = $extenumid;

			if($outcall->edit($info['outcall']['id'],$result['outcall']) === false)
			{
				if($result['extenumbers'] === null)
					$extenumbers->add_origin();
				else if($info['extenumbers'] !== null)
					$extenumbers->edit_origin();
				else if($extenumid !== 0)
					$extenumbers->delete($extenumid);

				break;
			}

			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}
		while(false);

		$element['extenumbers'] = $extenumbers->get_element();
		$element['outcall'] = $outcall->get_element();

		$_HTML->assign('id',$info['outcall']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('element',$element);
		$_HTML->assign('trunks_list',$trunks_list);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$extenumbers = &$ipbx->get_module('extenumbers');

		$info['extenumbers'] = false;

		if(isset($_QR['id']) === true
		&& ($info['outcall'] = $outcall->get($_QR['id'])) !== false
		&& ($info['outcall']['mode'] === 'numlen'
		   || ($info['extenumbers'] = $extenumbers->get($info['outcall']['extenid'])) !== false) === true
		&& $outcall->delete($info['outcall']['id']) !== false)
		{
			if($info['extenumbers'] !== false && $extenumbers->delete($info['extenumbers']['id']) === false)
				$outcall->add_origin();
		}

		xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$extenumbers = &$ipbx->get_module('extenumbers');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			$info['extenumbers'] = false;

			if(($info['outcall'] = $outcall->get($values[$i])) === false
			|| ($info['outcall']['mode'] !== 'numlen'
			   && ($info['extenumbers'] = $extenumbers->get($info['outcall']['extenid'])) === false) === true
			|| $outcall->delete($info['outcall']['id']) === false)
			   	continue;

			if($info['extenumbers'] !== false && $extenumbers->delete($info['extenumbers']['id']) === false)
				$outcall->add_origin();
		}

		xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$outcall->disable($values[$i],$disable);

		xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($list = $ipbx->get_outcall_list()) !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'outcall','key' => 'name'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$list);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/call_management/outcall');

$_HTML->assign('act',$act);
$_HTML->assign('bloc','call_management/outcall/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
