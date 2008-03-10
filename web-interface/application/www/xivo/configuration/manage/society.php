<?php

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

xivo::load_class('xivo_society',XIVO_PATH_OBJECT,null,false);
$_SCT = new xivo_society();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($result = $_SCT->chk_values($_QR)) === false)
				$result = $_SCT->get_filter_result();
			else if($_SCT->add($result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$_SCT->get_element());
		$_HTML->set_var('territory',xivo_i18n::get_territory_translated_list());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_SCT->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_SCT->chk_values($_QR)) === false)
				$result = $_SCT->get_filter_result();
			else if($_SCT->edit($info['id'],$result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);
		}

		$_HTML->set_var('id',$info['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$_SCT->get_element());
		$_HTML->set_var('territory',xivo_i18n::get_territory_translated_list());
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0)
			$_SCT->delete($id);

		$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('society',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = intval($values[$i])) > 0)
				$_SCT->delete($id);
		}

		$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = xivo_issa_val('society',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_SCT->disable(intval($values[$i]),$disable);

		$_QRY->go($_HTML->url('xivo/configuration/manage/society'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($list = $_SCT->get_all()) !== false)
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
$menu->set_toolbar('toolbar/xivo/configuration/manage/society');

$_HTML->set_bloc('main','xivo/configuration/manage/society/'.$act);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
