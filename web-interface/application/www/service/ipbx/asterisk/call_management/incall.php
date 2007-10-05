<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$search = isset($_QR['search']) === true ? strval($_QR['search']) : '';

$incall = &$ipbx->get_module('incall');

$info = array();

$param = array();
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		$extenumbers = &$ipbx->get_module('extenumbers');
		$extensions = &$ipbx->get_module('extensions');

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('incall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			$result = array();

			$_QR['incall']['extenumid'] = 0;
			$_QR['incall']['linked'] = true;

			if(($result['incall'] = $incall->chk_values($_QR['incall'])) === false)
			{
				$add = false;
				$result['incall'] = $incall->get_filter_result();
			}

			$_QR['extenumbers']['context'] = 'incall-extensions';

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
						       array('appdata' => 'superincall'),
						       $result['extenumbers']['exten'],
						       $result['extenumbers']['context'])) === false)
					$add = false;
			}

			if($add === false || ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
			{
				$result['incall']['endcall'] = null;
				$result['incall']['user'] = null;
				$result['incall']['group'] = null;
				$result['incall']['queue'] = null;
				$result['incall']['meetme'] = null;
				$result['incall']['schedule'] = null;
				$result['incall']['application'] = null;
				$result['incall']['custom'] = null;

				if(empty($result['incall']['type']) === false)
					$result['incall'][$result['incall']['type']] = $result['incall']['typeval'];
				break;
			}

			$result['incall']['extenumid'] = $extenumid;

			if(($incallid = $incall->add($result['incall'])) === false)
			{
				$extenumbers->delete($extenumid);
				break;
			}

			if($extensions->add_exten($extensid) === false)
			{
				$extenumbers->delete($extenumid);
				$incall->delete($incallid);
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		}
		while(false);

		$element = array();
		$element['incall'] = $incall->get_element();
		$element['extenumbers'] = $extenumbers->get_element();

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

		$schedule = &$ipbx->get_module('schedule');

		if(($list['schedule'] = $schedule->get_all()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'name'));
			usort($list['schedule'],array(&$sort,'str_usort'));
		}

		$_HTML->assign('list',$list);
		$_HTML->assign('info',$result);
		$_HTML->assign('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');
		break;
	case 'edit':
		$extenumbers = &$ipbx->get_module('extenumbers');

		if(isset($_QR['id']) === false
		|| ($info['incall'] = $incall->get($_QR['id'])) === false
		|| ($info['extenumbers'] = $extenumbers->get($info['incall']['extenumid'])) === false
		|| ($info['incall']['linked'] === true
		   && $incall->is_valid($info['incall']['type'],
		   			   $info['incall']['typeval']) === false) === true)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$edit = true;

		$return = &$info;

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('incall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			$result = array();

			$return = &$result;

			$_QR['incall']['extenumid'] = $info['extenumbers']['id'];
			$_QR['incall']['linked'] = true;
			$_QR['incall']['commented'] = $info['incall']['commented'];

			if(($result['incall'] = $incall->chk_values($_QR['incall'])) === false
			|| $incall->is_valid($result['incall']['type'],$result['incall']['typeval']) === false)
			{
				$edit = false;
				$result['incall'] = $incall->get_filter_result();
			}

			$_QR['extenumbers']['context'] = $info['extenumbers']['context'];

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
			{
				$result['incall']['endcall'] = null;
				$result['incall']['user'] = null;
				$result['incall']['group'] = null;
				$result['incall']['queue'] = null;
				$result['incall']['meetme'] = null;
				$result['incall']['schedule'] = null;
				$result['incall']['application'] = null;
				$result['incall']['custom'] = null;
				$result['incall']['linked'] = $info['incall']['linked'];

				if(empty($result['incall']['type']) === false)
					$result['incall'][$result['incall']['type']] = $result['incall']['typeval'];
				break;
			}

			if($incall->edit($info['incall']['id'],$result['incall']) === false)
			{
				$extenumbers->edit_origin();
				break;
			}

			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['extenumbers']['context'];

			$exten_edit = array();
			$exten_edit['exten'] = $result['extenumbers']['exten'];

			$extensions = &$ipbx->get_module('extensions');

			if($extensions->edit_where($exten_where,$exten_edit) === false)
			{
				$extenumbers->edit_origin();
				$incall->edit_origin();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		}
		while(false);

		$element = array();
		$element['incall'] = $incall->get_element();
		$element['extenumbers'] = $extenumbers->get_element();

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

		$schedule = &$ipbx->get_module('schedule');

		if(($list['schedule'] = $schedule->get_all()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'name'));
			usort($list['schedule'],array(&$sort,'str_usort'));
		}
		
		$_HTML->assign('id',$info['incall']['id']);
		$_HTML->assign('list',$list);
		$_HTML->assign('info',$return);
		$_HTML->assign('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$extenumbers = &$ipbx->get_module('extenumbers');
		$extensions = &$ipbx->get_module('extensions');

		if(isset($_QR['id']) === true
		&& ($info['incall'] = $incall->get($_QR['id'])) !== false
		&& ($info['extenumbers'] = $extenumbers->get($info['incall']['extenumid'])) !== false
		&& $incall->delete($info['incall']['id']) !== false)
		{
			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['extenumbers']['context'];

			if($extenumbers->delete($info['extenumbers']['id']) === false)
				$incall->add_origin();
			else if($extensions->delete_where($exten_where) === false)
			{
				$incall->add_origin();
				$extenumbers->add_origin();
			}
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('incalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$extenumbers = &$ipbx->get_module('extenumbers');
		$extensions = &$ipbx->get_module('extensions');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['incall'] = $incall->get($values[$i])) === false
			|| ($info['extenumbers'] = $extenumbers->get($info['incall']['extenumid'])) === false
			|| $incall->delete($info['incall']['id']) === false)
				continue;

			if($extenumbers->delete($info['extenumbers']['id']) === false)
			{
				$incall->add_origin();
				continue;
			}

			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['extenumbers']['context'];

			if($extensions->delete_where($exten_where) === true)
				continue;

			$incall->add_origin();
			$extenumbers->add_origin();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;
		$invdisable = $disable === true ? false : true;

		if(($values = xivo_issa_val('incalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$extenumbers = &$ipbx->get_module('extenumbers');
		$extensions = &$ipbx->get_module('extensions');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['incall'] = $incall->get($values[$i])) === false
			|| ($info['extenumbers'] = $extenumbers->get($info['incall']['extenumid'])) === false
			|| $incall->disable($info['incall']['id'],$disable) === false)
				continue;

			$exten_where = array();
			$exten_where['exten'] = $info['extenumbers']['exten'];
			$exten_where['context'] = $info['extenumbers']['context'];

			if($extensions->disable_where($exten_where,$disable) === false)
				$incall->disable($info['incall']['id'],$invdisable);
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if($search !== '')
			$list = $ipbx->get_incall_search($search);
		else
			$list = $ipbx->get_incall_list();

		if($list !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'extenumbers','key' => 'exten'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$list);
		$_HTML->assign('search',$search);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/incall');

$_HTML->assign('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/incall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
