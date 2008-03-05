<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appldapserver = &$ipbx->get_application('ldapserver');

		$result = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('ldapserver',$_QR) === true)
		{
			if(xivo_issa('attrdisplayname',$_QR['ldapserver']) === true)
				$_QR['ldapserver']['attrdisplayname'] = implode(',',$_QR['ldapserver']['attrdisplayname']);

			if(xivo_issa('attrphonenumber',$_QR['ldapserver']) === true)
				$_QR['ldapserver']['attrphonenumber'] = implode(',',$_QR['ldapserver']['attrphonenumber']);

			if($appldapserver->set_add($_QR) === false
			|| $appldapserver->add() === false)
				$result = $appldapserver->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);
		}

		if(xivo_issa('ldapserver',$result) === true)
		{
			if(xivo_ak('attrdisplayname',$result['ldapserver']) === true
			&& xivo_haslen($result['ldapserver'],'attrdisplayname') === true)
				$result['ldapserver']['attrdisplayname'] = explode(',',$result['ldapserver']['attrdisplayname']);

			if(xivo_ak('attrphonenumber',$result['ldapserver']) === true
			&& xivo_haslen($result['ldapserver'],'attrphonenumber') === true)
				$result['ldapserver']['attrphonenumber'] = explode(',',$result['ldapserver']['attrphonenumber']);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$appldapserver->get_elements());
		$_HTML->set_var('xldapservers',$appldapserver->get_xldapservers_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ldapserver.js');
		break;
	case 'edit':
		$appldapserver = &$ipbx->get_application('ldapserver');

		if(isset($_QR['id']) === false || ($info = $appldapserver->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);

		$result = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('ldapserver',$_QR) === true)
		{
			$return = &$result;

			if(xivo_issa('attrdisplayname',$_QR['ldapserver']) === true)
				$_QR['ldapserver']['attrdisplayname'] = implode(',',$_QR['ldapserver']['attrdisplayname']);

			if(xivo_issa('attrphonenumber',$_QR['ldapserver']) === true)
				$_QR['ldapserver']['attrphonenumber'] = implode(',',$_QR['ldapserver']['attrphonenumber']);

			if($appldapserver->set_edit($_QR) === false
			|| $appldapserver->edit() === false)
				$result = $appldapserver->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);
		}

		if(xivo_issa('ldapserver',$return) === true)
		{
			if(xivo_ak('attrdisplayname',$return['ldapserver']) === true
			&& xivo_haslen($return['ldapserver'],'attrdisplayname') === true)
				$return['ldapserver']['attrdisplayname'] = explode(',',$return['ldapserver']['attrdisplayname']);

			if(xivo_ak('attrphonenumber',$return['ldapserver']) === true
			&& xivo_haslen($return['ldapserver'],'attrphonenumber') === true)
				$return['ldapserver']['attrphonenumber'] = explode(',',$return['ldapserver']['attrphonenumber']);
		}

		$_HTML->set_var('id',$info['ldapserver']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$appldapserver->get_elements());
		$_HTML->set_var('xldapservers',$appldapserver->get_xldapservers_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ldapserver.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appldapserver = &$ipbx->get_application('ldapserver');

		if(isset($_QR['id']) === false || $appldapserver->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);

		$appldapserver->delete();

		$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('ldapservers',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);

		$appldapserver = &$ipbx->get_application('ldapserver');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appldapserver->get($values[$i]) !== false)
				$appldapserver->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('ldapservers',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);

		$appldapserver = &$ipbx->get_application('ldapserver');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appldapserver->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appldapserver->disable();
			else
				$appldapserver->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);
		break;
	default:
		$act = 'list';
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appldapserver = &$ipbx->get_application('ldapserver');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appldapserver->get_ldapservers_list(null,$order,$limit);
		$total = $appldapserver->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapserver'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/system_management/ldapserver');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/ldapserver/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
