<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appcontext = &$ipbx->get_application('context');

		$result = $error = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('context',$_QR) === true)
		{
			if(xivo_issa('contextentity',$_QR) === true
			&& isset($_QR['context']['entity']) === true
			&& xivo_haslen($_QR['context']['entity']) === true)
			{
				if(xivo_issa('user',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['user'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['user'])) === false)
					unset($_QR['contextentity']['user']);

				if(xivo_issa('group',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['group'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['group'])) === false)
					unset($_QR['contextentity']['group']);

				if(xivo_issa('queue',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['queue'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['queue'])) === false)
					unset($_QR['contextentity']['queue']);

				if(xivo_issa('meetme',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['meetme'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['meetme'])) === false)
					unset($_QR['contextentity']['meetme']);

				if(xivo_issa('incall',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['incall'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['incall'])) === false)
					unset($_QR['contextentity']['incall']);
			}
			else
				unset($_QR['contextentity']);
				
			if(($toto = $appcontext->set_add($_QR)) === false
			|| ($tutu = $appcontext->add()) === false)
			{
				$result = $appcontext->get_result();
				$error = $appcontext->get_error();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('error',$error);
		$_HTML->set_var('element',$appcontext->get_elements());
		$_HTML->set_var('entities',$appcontext->get_entities_list(null,array('displayname' => SORT_ASC)));

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/context.js');
		break;
	case 'edit':
		$appcontext = &$ipbx->get_application('context');

		if(isset($_QR['id']) === false || ($info = $appcontext->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);

		$result = $error = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('context',$_QR) === true)
		{
			$return = &$result;

			if(xivo_issa('contextentity',$_QR) === true
			&& isset($_QR['context']['entity']) === true
			&& xivo_haslen($_QR['context']['entity']) === true)
			{
				if(xivo_issa('user',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['user'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['user'])) === false)
					unset($_QR['contextentity']['user']);

				if(xivo_issa('group',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['group'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['group'])) === false)
					unset($_QR['contextentity']['group']);

				if(xivo_issa('queue',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['queue'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['queue'])) === false)
					unset($_QR['contextentity']['queue']);

				if(xivo_issa('meetme',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['meetme'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['meetme'])) === false)
					unset($_QR['contextentity']['meetme']);

				if(xivo_issa('incall',$_QR['contextentity']) === true
				&& ($_QR['contextentity']['incall'] = xivo_group_array('typevalbeg',
										       $_QR['contextentity']['incall'])) === false)
					unset($_QR['contextentity']['incall']);
			}
			else
				unset($_QR['contextentity']);

			if($appcontext->set_edit($_QR) === false
			|| $appcontext->edit() === false)
			{
				$result = $appcontext->get_result();
				$error = $appcontext->get_error();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);
		}

		$_HTML->set_var('id',$info['context']['name']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('error',$error);
		$_HTML->set_var('element',$appcontext->get_elements());
		$_HTML->set_var('entities',$appcontext->get_entities_list(null,array('displayname' => SORT_ASC)));

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/context.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appcontext = &$ipbx->get_application('context');

		if(isset($_QR['id']) === false || $appcontext->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);

		$appcontext->delete();

		$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('contexts',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);

		$appcontext = &$ipbx->get_application('context');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcontext->get($values[$i]) !== false)
				$appcontext->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('contexts',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);

		$appcontext = &$ipbx->get_application('context');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcontext->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appcontext->disable();
			else
				$appcontext->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appcontext = &$ipbx->get_application('context');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appcontext->get_contexts_list(null,$order,$limit);
		$total = $appcontext->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/system_management/context'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/system_management/context');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/context/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
