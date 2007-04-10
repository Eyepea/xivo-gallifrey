<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$dfeatures = &$ipbx->get_module('didfeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');

$info = $result = array();

$tyfeatures = false;

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('extenumbers',$_QR) === false || xivo_issa('dfeatures',$_QR) === false)
				break;

			$result = array();

			$_QR['extenumbers']['context'] = 'did-extensions';

			if(($result['extenumbers'] = $extenumbers->chk_values($_QR['extenumbers'],true,true)) === false
			|| $extenumbers->get_where($result['extenumbers']) !== false)
			{
				$add = false;
				$result['extenumbers'] = $extenumbers->get_filter_result();
			}
			else
			{
				$didexten = array();
				$didexten['exten'] = $result['extenumbers']['number'];
				$didexten['context'] = $result['extenumbers']['context'];
				$didexten['priority'] = 1;
				$didexten['app'] = 'Macro';
				$didexten['appdata'] = 'superdid';

				if(($result['did'] = $extensions->chk_values($didexten,true,true)) === false)
				{
					$add = false;
					$result['did'] = $extensions->get_filter_result();
				}
				else
					$_QR['dfeatures']['number'] = $result['extenumbers']['number'];
			}

			$_QR['dfeatures']['extenid'] = 0;

			if(xivo_ak('type',$_QR['dfeatures'],true) === 'custom')
				unset($_QR['dfeatures']['typeid'],$_QR['dfeatures']['number']);
			else
				unset($_QR['dfeatures']['custom']);

			if(($result['dfeatures'] = $dfeatures->chk_values($_QR['dfeatures'],true,true)) === false
			|| ($result['dfeatures']['type'] !== 'custom'
			   && (($tyfeatures = &$ipbx->get_module($result['dfeatures']['type'].'features')) === false
			   || ($result['tyfeatures'] = $tyfeatures->get($result['dfeatures']['typeid'],false)) === false
			   || xivo_empty($result['tyfeatures']['number']) === true) === true) === true)
			{
				$add = false;
				$result['dfeatures'] = $dfeatures->get_filter_result();
			}

			if($add === false || ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
				break;

			if(($result['dfeatures']['extenid'] = $extensions->add($result['did'])) === false)
			{
				$extenumbers->delete($extenumid);
				break;
			}

			if($dfeatures->add($result['dfeatures']) === false)
			{
				$extenumbers->delete($extenumid);
				$extensions->delete($result['dfeatures']['extenid']);
				break;
			}

			xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');
		}
		while(false);

		$element = array();
		$element['dfeatures'] = $dfeatures->get_element();
		$element['extenumbers'] = $extenumbers->get_element();

		$ufeatures = &$ipbx->get_module('userfeatures');
		$gfeatures = &$ipbx->get_module('groupfeatures');
		$mfeatures = &$ipbx->get_module('meetmefeatures');

		if(($list['users'] = $ufeatures->get_all_number()) !== false)
		{
			$total = count($list['users']);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'number'));
			usort($list['users'],array(&$sort,'num_usort'));
		}

		if(($list['groups'] = $gfeatures->get_all_number(false)) !== false)
		{
			$total = count($list['groups']);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'number'));
			usort($list['groups'],array(&$sort,'num_usort'));
		}

		if(($list['meetme'] = $mfeatures->get_all()) !== false)
		{
			$total = count($list['meetme']);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'number'));
			usort($list['meetme'],array(&$sort,'num_usort'));
		}

		$_HTML->assign('list',$list);
		$_HTML->assign('info',$result);
		$_HTML->assign('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/did.js');
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info['dfeatures'] = $dfeatures->get($_QR['id'])) === false
		|| ($info['did'] = $extensions->get($info['dfeatures']['extenid'])) === false
		|| ($info['extenumbers'] = $extenumbers->get_by_number_context($info['did']['exten'],$info['did']['context'])) === false
		|| ($info['dfeatures']['type'] !== 'custom' && $info['dfeatures']['disable'] === false
		   && (($tyfeatures = &$ipbx->get_module($info['dfeatures']['type'].'features')) === false
		   || ($info['tyfeatures'] = $tyfeatures->get($info['dfeatures']['typeid'])) === false
		   || xivo_empty($info['tyfeatures']['number']) === true) === true) === true)
			xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');

		if($info['dfeatures']['disable'] === true)
		{
			$info['dfeatures']['typeid'] = '';
			$info['dfeatures']['type'] = '';
			$info['dfeatures']['custom'] = '';
			$info['dfeatures']['number'] = '';
		}

		$edit = true;

		$return = &$info;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('extenumbers',$_QR) === false || xivo_issa('dfeatures',$_QR) === false)
				break;

			$return = &$result;

			$_QR['extenumbers']['context'] = $info['extenumbers']['context'];

			if(($result['extenumbers'] = $extenumbers->chk_values($_QR['extenumbers'],true,true)) === false
			|| (($extenum = $extenumbers->get_where($result['extenumbers'])) !== false 
			   && (int) $extenum['id'] !== (int) $info['extenumbers']['id']) === true)
			{
				$edit = false;
				$result['extenumbers'] = array_merge($info['extenumbers'],$extenumbers->get_filter_result());
			}
			else
			{
				$didexten = $info['did'];
				$didexten['exten'] = $result['extenumbers']['number'];
				$didexten['context'] = $result['extenumbers']['context'];

				if(($result['did'] = $extensions->chk_values($didexten,true,true)) === false)
				{
					$edit = false;
					$result['did'] = $extensions->get_filter_result();
				}
				else
					$_QR['dfeatures']['number'] = $result['extenumbers']['number'];
			}

			$_QR['dfeatures']['extenid'] = $info['dfeatures']['extenid'];

			if(xivo_ak('type',$_QR['dfeatures'],true) === 'custom')
				unset($_QR['dfeatures']['typeid'],$_QR['dfeatures']['number']);
			else
				unset($_QR['dfeatures']['custom']);

			if(($result['dfeatures'] = $dfeatures->chk_values($_QR['dfeatures'],true,true)) === false
			|| ($result['dfeatures']['type'] !== 'custom'
				&& (($tyfeatures = &$ipbx->get_module($result['dfeatures']['type'].'features')) === false
				|| ($result['tyfeatures'] = $tyfeatures->get($result['dfeatures']['typeid'],false)) === false
				|| xivo_empty($result['tyfeatures']['number']) === true) === true) === true)
			{
				$edit = false;
				$result['dfeatures'] = array_merge($info['dfeatures'],$dfeatures->get_filter_result());
			}

			if($edit === false || $extenumbers->edit($info['extenumbers']['id'],$result['extenumbers']) === false)
				break;

			if($extensions->edit($info['did']['id'],$result['did']) === false)
			{
				$extenumbers->edit_origin();
				break;
			}

			if($dfeatures->edit($info['dfeatures']['id'],$result['dfeatures']) === false)
			{
				$extenumbers->edit_origin();
				$extensions->edit_origin();
				break;
			}

			xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');
		}
		while(false);

		$element = array();
		$element['dfeatures'] = $dfeatures->get_element();
		$element['extenumbers'] = $extenumbers->get_element();

		$ufeatures = &$ipbx->get_module('userfeatures');
		$gfeatures = &$ipbx->get_module('groupfeatures');
		$mfeatures = &$ipbx->get_module('meetmefeatures');

		if(($list['users'] = $ufeatures->get_all_number()) !== false)
		{
			$total = count($list['users']);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'number'));
			usort($list['users'],array(&$sort,'num_usort'));
		}

		if(($list['groups'] = $gfeatures->get_all_number(false)) !== false)
		{
			$total = count($list['groups']);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'number'));
			usort($list['groups'],array(&$sort,'num_usort'));
		}

		if(($list['meetme'] = $mfeatures->get_all()) !== false)
		{
			$total = count($list['meetme']);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'number'));
			usort($list['meetme'],array(&$sort,'num_usort'));
		}

		$_HTML->assign('id',$info['dfeatures']['id']);
		$_HTML->assign('list',$list);
		$_HTML->assign('info',$return);
		$_HTML->assign('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/did.js');
		break;
	case 'delete':
		if(isset($_QR['id']) === false
		|| ($info['dfeatures'] = $dfeatures->get($_QR['id'])) === false
		|| ($info['did'] = $extensions->get($info['dfeatures']['extenid'])) === false
		|| ($info['extenumbers'] = $extenumbers->get_by_number_context($info['did']['exten'],$info['did']['context'])) === false)
			xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');

		do
		{
			if($dfeatures->delete($info['dfeatures']['id']) === false)
				break;

			if($extensions->delete($info['did']['id']) === false)
			{
				$dfeatures->add_origin();
				break;
			}

			if($extenumbers->delete($info['extenumbers']['id']) === false)
			{
				$dfeatures->add_origin();
				$extensions->add_origin();
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');
		break;
	case 'disables':
	case 'enables':
		$disable = $act === 'disables' ? true : false;

		if(xivo_issa('dids',$_QR) === false)
			xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');

		$values = array_values($_QR['dids']);

		if(($nb = count($values)) === 0)
			xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['dfeatures'] = $dfeatures->get($_QR['dids'][$i])) === false
			|| ($info['did'] = $extensions->get($info['dfeatures']['extenid'])) === false)
				continue;

			$extensions->disable($info['did']['id'],$disable);
		}

		xivo_go($_HTML->url('service/ipbx/call_management/did'),'act=list');
		break;
	default:
		$act = 'list';
		$total = 0;
		$page = 1;

		if(($did = $ipbx->get_did_list()) !== false)
		{
			$total = count($did);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'did','key' => 'exten'));
			usort($did,array(&$sort,'num_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$did);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/call_management/did');

$_HTML->assign('act',$act);
$_HTML->assign('bloc','call_management/did/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index',true);

?>
