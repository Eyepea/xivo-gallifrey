<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = $result = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		$result = null;

		$allow = array();

		if(isset($_QR['fm_send']) === true && xivo_issa('protocol',$_QR) === true)
		{
			if($apptrunk->set_add($_QR) === false
			|| $apptrunk->add() === false)
			{
				$result = $apptrunk->get_result();

				if(xivo_issa('protocol',$result) === true && isset($result['protocol']['allow']) === true)
					$allow = $result['protocol']['allow'];
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);
		}

		$element = $apptrunk->get_elements();

		if(xivo_issa('allow',$element['protocol']) === true
		&& xivo_issa('value',$element['protocol']['allow']) === true
		&& empty($allow) === false)
		{
			if(is_array($allow) === false)
				$allow = explode(',',$allow);

			$element['protocol']['allow']['value'] = array_diff($element['protocol']['allow']['value'],$allow);
		}

		if(empty($result) === false)
			$result['protocol']['allow'] = $allow;

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/trunksip.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		if(isset($_QR['id']) === false
		|| ($info = $apptrunk->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

		$result = null;
		$return = &$info;

		if(isset($info['protocol']['allow']) === true)
			$allow = $info['protocol']['allow'];
		else
			$allow = array();

		if(isset($_QR['fm_send']) === true && xivo_issa('protocol',$_QR) === true)
		{
			$return = &$result;

			if($apptrunk->set_edit($_QR) === false
			|| $apptrunk->edit() === false)
			{
				$result = $apptrunk->get_result();

				if(xivo_issa('protocol',$result) === true && isset($result['protocol']['allow']) === true)
					$allow = $result['protocol']['allow'];
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);
		}

		$element = $apptrunk->get_elements();

		if(xivo_issa('allow',$element['protocol']) === true
		&& xivo_issa('value',$element['protocol']['allow']) === true
		&& empty($allow) === false)
		{
			if(is_array($allow) === false)
				$allow = explode(',',$allow);

			$element['protocol']['allow']['value'] = array_diff($element['protocol']['allow']['value'],$allow);
		}

		if(empty($return) === false)
			$return['protocol']['allow'] = $allow;

		$_HTML->set_var('id',$info['trunkfeatures']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/trunksip.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		if(isset($_QR['id']) === false || $apptrunk->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

		$apptrunk->delete();

		$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('trunks',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($apptrunk->get($values[$i]) !== false)
				$apptrunk->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('trunks',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($apptrunk->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$apptrunk->disable();
			else
				$apptrunk->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $apptrunk->get_trunks_list(null,$order,$limit);
		$total = $apptrunk->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/trunk_management/sip');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/trunk_management/sip/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
