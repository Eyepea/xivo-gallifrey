<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appserverldap = &$ipbx->get_application('serverldap');

		$result = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('serverldap',$_QR) === true)
		{
			if(xivo_issa('attrdisplayname',$_QR['serverldap']) === true)
				$_QR['serverldap']['attrdisplayname'] = implode(',',$_QR['serverldap']['attrdisplayname']);

			if(xivo_issa('attrphonenumber',$_QR['serverldap']) === true)
				$_QR['serverldap']['attrphonenumber'] = implode(',',$_QR['serverldap']['attrphonenumber']);

			if($appserverldap->set_add($_QR) === false
			|| $appserverldap->add() === false)
				$result = $appserverldap->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);
		}

		if(xivo_issa('serverldap',$result) === true)
		{
			if(xivo_ak('attrdisplayname',$result['serverldap']) === true
			&& xivo_haslen($result['serverldap'],'attrdisplayname') === true)
				$result['serverldap']['attrdisplayname'] = explode(',',$result['serverldap']['attrdisplayname']);

			if(xivo_ak('attrphonenumber',$result['serverldap']) === true
			&& xivo_haslen($result['serverldap'],'attrphonenumber') === true)
				$result['serverldap']['attrphonenumber'] = explode(',',$result['serverldap']['attrphonenumber']);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$appserverldap->get_elements());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/serverldap.js');
		break;
	case 'edit':
		$appserverldap = &$ipbx->get_application('serverldap');

		if(isset($_QR['id']) === false || ($info = $appserverldap->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);

		$result = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('serverldap',$_QR) === true)
		{
			$return = &$result;

			if(xivo_issa('attrdisplayname',$_QR['serverldap']) === true)
				$_QR['serverldap']['attrdisplayname'] = implode(',',$_QR['serverldap']['attrdisplayname']);

			if(xivo_issa('attrphonenumber',$_QR['serverldap']) === true)
				$_QR['serverldap']['attrphonenumber'] = implode(',',$_QR['serverldap']['attrphonenumber']);

			if($appserverldap->set_edit($_QR) === false
			|| $appserverldap->edit() === false)
				$result = $appserverldap->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);
		}

		if(xivo_issa('serverldap',$return) === true)
		{
			if(xivo_ak('attrdisplayname',$return['serverldap']) === true
			&& xivo_haslen($return['serverldap'],'attrdisplayname') === true)
				$return['serverldap']['attrdisplayname'] = explode(',',$return['serverldap']['attrdisplayname']);

			if(xivo_ak('attrphonenumber',$return['serverldap']) === true
			&& xivo_haslen($return['serverldap'],'attrphonenumber') === true)
				$return['serverldap']['attrphonenumber'] = explode(',',$return['serverldap']['attrphonenumber']);
		}

		$_HTML->set_var('id',$info['serverldap']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$appserverldap->get_elements());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/serverldap.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appserverldap = &$ipbx->get_application('serverldap');

		if(isset($_QR['id']) === false || $appserverldap->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);

		$appserverldap->delete();

		$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('serverldaps',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);

		$appserverldap = &$ipbx->get_application('serverldap');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appserverldap->get($values[$i]) !== false)
				$appserverldap->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('serverldaps',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);

		$appserverldap = &$ipbx->get_application('serverldap');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appserverldap->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appserverldap->disable();
			else
				$appserverldap->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);
		break;
	default:
		$act = 'list';
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appserverldap = &$ipbx->get_application('serverldap');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appserverldap->get_serverldaps_list(null,$order,$limit);
		$total = $appserverldap->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/system_management/serverldap'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/system_management/serverldap');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/serverldap/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
