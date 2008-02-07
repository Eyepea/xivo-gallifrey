<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$search = isset($_QR['search']) === true ? strval($_QR['search']) : '';

$param = array();
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;

$appphonebook = &$ipbx->get_application('phonebook');

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('phonebook',$_QR) === true)
		{
			if($appphonebook->set_add($_QR) === false
			|| $appphonebook->add() === false)
				$result = $appphonebook->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);
		}

		if(xivo_issa('phonebook',$result) === false || empty($result['phonebook']) === true)
			$result['phonebook'] = null;

		if(xivo_issa('phonebookaddress',$result) === false || empty($result['phonebookaddress']) === true)
			$result['phonebookaddress'] = null;

		if(xivo_issa('phonebooknumber',$result) === false || empty($result['phonebooknumber']) === true)
			$result['phonebooknumber'] = null;

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_HTML->set_var('element',$appphonebook->get_elements());
		$_HTML->set_var('territory',xivo_i18n::get_territory_translated_list());
		$_HTML->set_var('info',$result);
		break;
	case 'edit':
		$appphonebook = &$ipbx->get_application('phonebook');

		if(isset($_QR['id']) === false || ($info = $appphonebook->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);

		$result = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('phonebook',$_QR) === true)
		{
			$return = &$result;

			if($appphonebook->set_edit($_QR) === false
			|| $appphonebook->edit() === false)
				$result = $appphonebook->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);
		}

		if(xivo_issa('phonebook',$return) === false || empty($return['phonebook']) === true)
			$return['phonebook'] = null;

		if(xivo_issa('phonebookaddress',$return) === false || empty($return['phonebookaddress']) === true)
			$return['phonebookaddress'] = null;

		if(xivo_issa('phonebooknumber',$return) === false || empty($return['phonebooknumber']) === true)
			$return['phonebooknumber'] = null;

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_HTML->set_var('id',$info['phonebook']['id']);
		$_HTML->set_var('element',$appphonebook->get_elements());
		$_HTML->set_var('territory',xivo_i18n::get_territory_translated_list());
		$_HTML->set_var('info',$return);
		$_HTML->set_var('phonebookaddress',$return['phonebookaddress']);
		$_HTML->set_var('phonebooknumber',$return['phonebooknumber']);
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === false || $appphonebook->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);

		$appphonebook->delete();

		$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('phonebook',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appphonebook->get($values[$i]) !== false)
				$appphonebook->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);
		break;
	case 'import':
		if(isset($_QR['fm_send']) === true)
		{
			$appphonebook->import_csv();
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);
		}

		$_HTML->set_var('import_file',$appphonebook->get_config_import_file());
		break;
	case 'list':
	default:
		$act = 'list';
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$order = array();
		$order['displayname'] = SORT_ASC;
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		if($search !== '')
			$list = $appphonebook->get_phonebook_search($search,$order,$limit);
		else
			$list = $appphonebook->get_phonebook_list($order,$limit);

		$total = $appphonebook->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/phonebook'),$param);
		}

		$_HTML->set_var('total',$total);
		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
		$_HTML->set_var('search',$search);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_services/phonebook');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/phonebook/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
