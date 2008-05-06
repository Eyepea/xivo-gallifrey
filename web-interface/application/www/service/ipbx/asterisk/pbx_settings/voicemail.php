<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appvoicemail = &$ipbx->get_application('voicemail');

		$result = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('voicemail',$_QR) === true)
		{
			if($appvoicemail->set_add($_QR) === false
			|| $appvoicemail->add() === false)
				$result = $appvoicemail->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$appvoicemail->get_elements());
		$_HTML->set_var('tz_list',$appvoicemail->get_timezones());
		$_HTML->set_var('context_list',$appvoicemail->get_context_list());
		break;
	case 'edit':
		$appvoicemail = &$ipbx->get_application('voicemail');

		if(isset($_QR['id']) === false
		|| ($info = $appvoicemail->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);

		$result = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('voicemail',$_QR) === true)
		{
			$return = &$result;

			if($appvoicemail->set_edit($_QR) === false
			|| $appvoicemail->edit() === false)
				$result = $appvoicemail->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_HTML->set_var('id',$info['voicemail']['uniqueid']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$appvoicemail->get_elements());
		$_HTML->set_var('tz_list',$appvoicemail->get_timezones());
		$_HTML->set_var('context_list',$appvoicemail->get_context_list());
		break;
	case 'delete':
		$param['page'] = $page;

		$appvoicemail = &$ipbx->get_application('voicemail');

		if(isset($_QR['id']) === false || $appvoicemail->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);

		$appvoicemail->delete();

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('voicemail',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);

		$appvoicemail = &$ipbx->get_application('voicemail');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appvoicemail->get($values[$i]) !== false)
				$appvoicemail->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('voicemail',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);

		$appvoicemail = &$ipbx->get_application('voicemail');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appvoicemail->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appvoicemail->disable();
			else
				$appvoicemail->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appvoicemail = &$ipbx->get_application('voicemail');

		$order = array();
		$order['fullname'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appvoicemail->get_voicemail_list(null,$order,$limit);
		$total = $appvoicemail->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/voicemail'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/voicemail');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/voicemail/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
