<?php

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

xivo::load_class('xivo_entity',XIVO_PATH_OBJECT,null,false);
$_ETT = new xivo_entity();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($result = $_ETT->chk_values($_QR)) === false)
				$result = $_ETT->get_filter_result();
			else if($_ETT->add($result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$_ETT->get_element());
		$_HTML->set_var('territory',xivo_i18n::get_territory_translated_list());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_ETT->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_ETT->chk_values($_QR)) === false)
				$result = $_ETT->get_filter_result();
			else if($_ETT->edit($info['id'],$result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);
		}

		$_HTML->set_var('id',$info['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$_ETT->get_element());
		$_HTML->set_var('territory',xivo_i18n::get_territory_translated_list());
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0)
			$_ETT->delete($id);

		$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('entity',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = intval($values[$i])) > 0)
				$_ETT->delete($id);
		}

		$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = xivo_issa_val('entity',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_ETT->disable(intval($values[$i]),$disable);

		$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);
		break;
	default:
		$act = 'list';
		$nbbypage = 20;

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $_ETT->get_all(null,true,$order,$limit);
		$total = $_ETT->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('xivo/configuration/manage/entity'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/entity');

$_HTML->set_bloc('main','xivo/configuration/manage/entity/'.$act);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
