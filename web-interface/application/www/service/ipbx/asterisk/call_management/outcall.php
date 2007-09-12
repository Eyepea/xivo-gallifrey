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
		$extensions = &$ipbx->get_module('extensions');
		$trunks_list = $ipbx->get_trunks_list();

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			$result = array();

			$_QR['outcall']['extenumid'] = 0;

			if(($result['outcall'] = $outcall->chk_values($_QR['outcall'])) === false
			|| $tfeatures->get_id($result['outcall']['trunkfeaturesid']) === false)
			{
				$add = false;
				$result['outcall'] = $outcall->get_filter_result();
			}

			$_QR['extenumbers']['context'] = $result['outcall']['context'];

			if(($result['extenumbers'] = $extenumbers->chk_values($_QR['extenumbers'])) === false
			|| $extenumbers->exists($result['extenumbers']) !== false)
			{
				$add = false;
						
				if($result['extenumbers'] === false)
					$result['extenumbers'] = $extenumbers->get_filter_result();
				else
					$result['extenumbers']['exten'] = '';
			}

			if($add === true)
			{
				if(($extensid = $extensions->new_exten('macro',
						       array('appdata' => 'superoutcall'),
						       $result['extenumbers']['exten'],
						       $result['outcall']['context'])) === false
				|| $extensions->set_exten('hangup',$extensid) === false)
					$add = false;
			}

			if($add === false || ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
				break;

			$result['outcall']['extenumid'] = $extenumid;

			if(($outcallid = $outcall->add($result['outcall'])) === false)
			{
				$extenumbers->delete($extenumid);
				break;
			}

			if($extensions->add_exten($extensid) === false)
			{
				$extenumbers->delete($extenumid);
				$outcall->delete($outcallid);
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
		|| ($info['extenumbers'] = $extenumbers->get($info['outcall']['extenumid'])) === false)
			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$edit = true;

		$return = &$info;

		$trunks_list = $ipbx->get_trunks_list();

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			$result = array();

			$return = &$result;

			$_QR['outcall']['extenumid'] = $info['extenumbers']['id'];
			$_QR['outcall']['commented'] = $info['outcall']['commented'];

			if(($result['outcall'] = $outcall->chk_values($_QR['outcall'])) === false
			|| $tfeatures->get_id($result['outcall']['trunkfeaturesid']) === false)
			{
				$edit = false;
				$result['outcall'] = $outcall->get_filter_result();
			}

			$_QR['extenumbers']['context'] = $result['outcall']['context'];

			if(($result['extenumbers'] = $extenumbers->chk_values($_QR['extenumbers'])) === false
			|| $extenumbers->exists($result['extenumbers'],$info['extenumbers']['id']) !== false)
			{
				$edit = false;

				if($result['extenumbers'] === false)
					$result['extenumbers'] = $extenumbers->get_filter_result();
				else
					$result['extenumbers']['exten'] = '';
			}

			if($edit === false || $extenumbers->edit($info['extenumbers']['id'],$result['extenumbers']) === false)
				break;

			if($outcall->edit($info['outcall']['id'],$result['outcall']) === false)
			{
				$extenumbers->edit_origin();
				break;
			}

			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['outcall']['context'];

			$exten_edit = array();
			$exten_edit['exten'] = $result['extenumbers']['exten'];
			$exten_edit['context'] = $result['outcall']['context'];

			$extensions = &$ipbx->get_module('extensions');

			if($extensions->edit_where($exten_where,$exten_edit) === false)
			{
				$extenumbers->edit_origin();
				$outcall->edit_origin();
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
		$extensions = &$ipbx->get_module('extensions');

		if(isset($_QR['id']) === true
		&& ($info['outcall'] = $outcall->get($_QR['id'])) !== false
		&& ($info['extenumbers'] = $extenumbers->get($info['outcall']['extenumid'])) !== false
		&& $outcall->delete($info['outcall']['id']) !== false)
		{
			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['outcall']['context'];

			if($extenumbers->delete($info['extenumbers']['id']) === false)
				$outcall->add_origin();
			else if($extensions->delete_where($exten_where) === false)
			{
				$outcall->add_origin();
				$extenumbers->add_origin();
			}
		}

		xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$extenumbers = &$ipbx->get_module('extenumbers');
		$extensions = &$ipbx->get_module('extensions');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['outcall'] = $outcall->get($values[$i])) === false
			|| ($info['extenumbers'] = $extenumbers->get($info['outcall']['extenumid'])) === false
			|| $outcall->delete($info['outcall']['id']) === false)
				continue;

			if($extenumbers->delete($info['extenumbers']['id']) === false)
			{
				$outcall->add_origin();
				continue;
			}

			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['outcall']['context'];

			if($extensions->delete_where($exten_where) === true)
				continue;

			$outcall->add_origin();
			$extenumbers->add_origin();
		}

		xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;
		$invdisable = $disable === true ? false : true;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			xivo_go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$extenumbers = &$ipbx->get_module('extenumbers');
		$extensions = &$ipbx->get_module('extensions');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['outcall'] = $outcall->get($values[$i])) === false
			|| ($info['extenumbers'] = $extenumbers->get($info['outcall']['extenumid'])) === false
			|| $outcall->disable($info['outcall']['id'],$disable) === false)
				continue;

			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['outcall']['context'];

			if($extensions->disable_where($exten_where,$disable) === false)
				$outcall->disable($info['outcall']['id'],$invdisable);
		}

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
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/outcall');

$_HTML->assign('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/outcall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
