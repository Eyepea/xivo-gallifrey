<?php

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

xivo::load_class('xivo_ldapserver',XIVO_PATH_OBJECT,null,false);
$_LDAPSVR = new xivo_ldapserver();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($result = $_LDAPSVR->chk_values($_QR)) === false)
				$result = $_LDAPSVR->get_filter_result();
			else if($_LDAPSVR->add($result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$_LDAPSVR->get_element());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_LDAPSVR->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_LDAPSVR->chk_values($_QR)) === false)
				$result = $_LDAPSVR->get_filter_result();
			else if($_LDAPSVR->edit($info['id'],$result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);
		}

		$_HTML->set_var('id',$info['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$_LDAPSVR->get_element());
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0)
			$_LDAPSVR->delete($id);

		$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('ldapserver',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = intval($values[$i])) > 0)
				$_LDAPSVR->delete($id);
		}

		$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = xivo_issa_val('ldapserver',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_LDAPSVR->disable(intval($values[$i]),$disable);

		$_QRY->go($_HTML->url('xivo/configuration/manage/ldapserver'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($list = $_LDAPSVR->get_all()) !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'name'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/ldapserver');

$_HTML->set_bloc('main','xivo/configuration/manage/ldapserver/'.$act);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
