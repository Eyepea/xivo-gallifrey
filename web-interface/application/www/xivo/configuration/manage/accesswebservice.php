<?php

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

xivo::load_class('xivo_accesswebservice',XIVO_PATH_OBJECT,null,false);
$_AWS = new xivo_accesswebservice();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			$add = false;

			if(($result = $_AWS->chk_values($_QR)) === false)
				$result = $_AWS->get_filter_result();
			else if(xivo_haslen($result['login']) === true
			&& xivo_haslen($result['passwd']) === true)
				$add = true;
			else if(xivo_haslen($result['host']) === true)
				$add = true;

			if($add === true && $_AWS->add($result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$_AWS->get_element());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_AWS->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$edit = false;

			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_AWS->chk_values($_QR)) === false)
				$result = $_AWS->get_filter_result();
			else if(xivo_haslen($result['login']) === true
			&& xivo_haslen($result['passwd']) === true)
				$edit = true;
			else if(xivo_haslen($result['host']) === true)
				$edit = true;

			if($edit === true && $_AWS->edit($info['id'],$result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		}

		$_HTML->set_var('id',$info['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$_AWS->get_element());
		break;
	case 'acl':
		if(isset($_QR['id']) === false
		|| ($info = $_AWS->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);

		$webservice_acl = $_AWS->get_acl();

		if(isset($_QR['fm_send']) === true)
		{
			$webservice_acl->edit($_QR);
			$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		}
		else if(($tree = $webservice_acl->get_access_tree($info['id'])) !== false)
		{
			$_HTML->set_var('info',$info);
			$_HTML->set_var('tree',$tree);
		}
		else $_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0)
			$_AWS->delete($id);

		$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('accesswebservice',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = intval($values[$i])) > 0)
				$_AWS->delete($id);
		}

		$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = xivo_issa_val('accesswebservice',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_AWS->disable(intval($values[$i]),$disable);

		$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = 20;

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $_AWS->get_all(null,true,$order,$limit);
		$total = $_AWS->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('xivo/configuration/manage/accesswebservice'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/accesswebservice');

$_HTML->set_bloc('main','xivo/configuration/manage/accesswebservice/'.$act);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
