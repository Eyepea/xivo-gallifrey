<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$result = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('ldapfilter',$_QR) === true)
		{
			if(xivo_issa('attrdisplayname',$_QR['ldapfilter']) === true)
				$_QR['ldapfilter']['attrdisplayname'] = implode(',',$_QR['ldapfilter']['attrdisplayname']);

			if(xivo_issa('attrphonenumber',$_QR['ldapfilter']) === true)
				$_QR['ldapfilter']['attrphonenumber'] = implode(',',$_QR['ldapfilter']['attrphonenumber']);

			if($appldapfilter->set_add($_QR) === false
			|| $appldapfilter->add() === false)
				$result = $appldapfilter->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);
		}

		if(xivo_issa('ldapfilter',$result) === true)
		{
			if(xivo_ak('attrdisplayname',$result['ldapfilter']) === true
			&& xivo_haslen($result['ldapfilter'],'attrdisplayname') === true)
				$result['ldapfilter']['attrdisplayname'] = explode(',',$result['ldapfilter']['attrdisplayname']);

			if(xivo_ak('attrphonenumber',$result['ldapfilter']) === true
			&& xivo_haslen($result['ldapfilter'],'attrphonenumber') === true)
				$result['ldapfilter']['attrphonenumber'] = explode(',',$result['ldapfilter']['attrphonenumber']);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$appldapfilter->get_elements());
		$_HTML->set_var('ldapservers',$appldapfilter->get_ldapservers_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ldapfilter.js');
		break;
	case 'edit':
		$appldapfilter = &$ipbx->get_application('ldapfilter');

		if(isset($_QR['id']) === false || ($info = $appldapfilter->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);

		$result = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('ldapfilter',$_QR) === true)
		{
			$return = &$result;

			if(xivo_issa('attrdisplayname',$_QR['ldapfilter']) === true)
				$_QR['ldapfilter']['attrdisplayname'] = implode(',',$_QR['ldapfilter']['attrdisplayname']);

			if(xivo_issa('attrphonenumber',$_QR['ldapfilter']) === true)
				$_QR['ldapfilter']['attrphonenumber'] = implode(',',$_QR['ldapfilter']['attrphonenumber']);

			if($appldapfilter->set_edit($_QR) === false
			|| $appldapfilter->edit() === false)
				$result = $appldapfilter->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);
		}

		if(xivo_issa('ldapfilter',$return) === true)
		{
			if(xivo_ak('attrdisplayname',$return['ldapfilter']) === true
			&& xivo_haslen($return['ldapfilter'],'attrdisplayname') === true)
				$return['ldapfilter']['attrdisplayname'] = explode(',',$return['ldapfilter']['attrdisplayname']);

			if(xivo_ak('attrphonenumber',$return['ldapfilter']) === true
			&& xivo_haslen($return['ldapfilter'],'attrphonenumber') === true)
				$return['ldapfilter']['attrphonenumber'] = explode(',',$return['ldapfilter']['attrphonenumber']);
		}

		$_HTML->set_var('id',$info['ldapfilter']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$appldapfilter->get_elements());
		$_HTML->set_var('ldapservers',$appldapfilter->get_ldapservers_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ldapfilter.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		if(isset($_QR['id']) === false || $appldapfilter->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);

		$appldapfilter->delete();

		$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('ldapfilters',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appldapfilter->get($values[$i]) !== false)
				$appldapfilter->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('ldapfilters',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appldapfilter->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appldapfilter->disable();
			else
				$appldapfilter->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);
		break;
	default:
		$act = 'list';
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appldapfilter->get_ldapfilters_list(null,$order,$limit);
		$total = $appldapfilter->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/system_management/ldapfilter'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/system_management/ldapfilter');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/ldapfilter/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
