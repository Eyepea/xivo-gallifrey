<?php

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

xivo::load_class('xivo_server',XIVO_PATH_OBJECT,null,false);
$_SVR = new xivo_server();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($result = $_SVR->chk_values($_QR)) === false)
				$result = $_SVR->get_filter_result();
			else if($_SVR->add($result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$_SVR->get_element());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_SVR->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_SVR->chk_values($_QR)) === false)
				$result = $_SVR->get_filter_result();
			else if($_SVR->edit($info['id'],$result) !== false)
				$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);
		}

		$_HTML->set_var('id',$info['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$_SVR->get_element());
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0
		&& $_SVR->delete($id) !== false)
		{
			$ipbx = &$_SRE->get('ipbx');
			$serverfeatures = &$ipbx->get_module('serverfeatures');

			$where_serverfeatures = array();
			$where_serverfeatures['serverid'] = $id;
			$serverfeatures->delete_where($where_serverfeatures);
		}

		$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('server',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);

		$ipbx = &$_SRE->get('ipbx');
		$serverfeatures = &$ipbx->get_module('serverfeatures');

		$where_serverfeatures = array();

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = intval($values[$i])) > 0
			&& $_SVR->delete($id) !== false)
			{
				$where_serverfeatures['serverid'] = $id;
				$serverfeatures->delete_where($where_serverfeatures);
			}
		}

		$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = xivo_issa_val('server',$_QR)) === false)
			$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_SVR->disable(intval($values[$i]),$disable);

		$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);
		break;
	default:
		$act = 'list';
		$nbbypage = 20;

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $_SVR->get_all(null,true,$order,$limit);
		$total = $_SVR->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('xivo/configuration/manage/server'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/server');

$_HTML->set_bloc('main','xivo/configuration/manage/server/'.$act);
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
