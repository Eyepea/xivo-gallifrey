<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$idsheetactions = isset($_QR['idsheetactions']) === true ? dwho_uint($_QR['idsheetactions'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$appuser = &$ipbx->get_application('user');
$profileclientlist = $appuser->get_profileclient_list();

$appcontext = &$ipbx->get_application('cticontexts');
$contextlist = $appcontext->get_contexts_list();

$contextavail = array();

foreach($contextlist as $v)
{
	$contextavail[] = $v['cticontexts']['name'];
}

$param = array();
$param['idsheetactions'] = $idsheetactions;
$info = $result = array();

switch($act)
{
	case 'add':
		$app = &$ipbx->get_application('ctisheetactions');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('sheetactions',$_QR) === true)
		{
			$cpt = 10;
			$str = "{";
			foreach($_QR['screencol1'] as $k => $v)
			{
				if((trim($_QR['screencol1'][$k]) != '') || (trim($_QR['screencol2'][$k]) != '') || (trim($_QR['screencol3'][$k]) != '') || (trim($_QR['screencol4'][$k]) != ''))
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['screencol2'][$k]).'",';
					$str .= '"'.trim($_QR['screencol3'][$k]).'",';
					$str .= '"'.trim($_QR['screencol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= '}';
			$_QR['sheetactions']['sheet_info'] = $str;

			$cpt = 10;
			$str = "{";
			foreach($_QR['systrayscol1'] as $k => $v)
			{
				if((trim($_QR['systrayscol1'][$k]) != '') || (trim($_QR['systrayscol2'][$k]) != '') || (trim($_QR['systrayscol3'][$k]) != '') || (trim($_QR['systrayscol4'][$k]) != ''))
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['systrayscol2'][$k]).'",';
					$str .= '"'.trim($_QR['systrayscol3'][$k]).'",';
					$str .= '"'.trim($_QR['systrayscol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= '}';
			$_QR['sheetactions']['systray_info'] = $str;

			$cpt = 10;
			$str = "{";
			foreach($_QR['infoscol1'] as $k => $v)
			{
				if((trim($_QR['infoscol1'][$k]) != '') || (trim($_QR['infoscol2'][$k]) != '') || (trim($_QR['infoscol3'][$k]) != '') || (trim($_QR['infoscol4'][$k]) != ''))
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['infoscol2'][$k]).'",';
					$str .= '"'.trim($_QR['infoscol3'][$k]).'",';
					$str .= '"'.trim($_QR['infoscol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= '}';
			$_QR['sheetactions']['action_info'] = $str;

			$_QR['sheetactions']['deletable'] = 1;

			$str = "[";
			if(isset($_QR['capaids']))
			{
				foreach($_QR['capaids'] as $v)
				{
					$str .= '"' . trim($v) . '",';
				}
				$str = trim($str, ',');
			}
			$str .= "]";
			$_QR['sheetactions']['capaids'] = $str;
			
			$str = "[";
			if(isset($_QR['context']))
			{
				foreach($_QR['context'] as $v)
				{
					$str .= '"' . $contextavail[$v] . '",';
				}
				$str = trim($str, ',');
			}
			$str .= "]";
			$_QR['sheetactions']['context'] = $str;

			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/sheetactions'),$param);
		}

		dwho::load_class('dwho_sort');

		$result['capaids']['list'] = $profileclientlist;
		$result['capaids']['slt'] = null;
		$result['context']['list'] = $contextavail;
		$result['context']['slt'] = null;
		$result['sheetactions'] = null;

		$screens = null;
		$systrays = null;
		$informations = null;
		$_TPL->set_var('contextavail',$contextavail);
		$_TPL->set_var('profileclientlist',$profileclientlist);
		$_TPL->set_var('screens',$screens);
		$_TPL->set_var('systrays',$systrays);
		$_TPL->set_var('informations',$informations);
		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'edit':
		$app = &$ipbx->get_application('ctisheetactions');

		if(isset($_QR['idsheetactions']) === false
		|| ($info = $app->get($_QR['idsheetactions'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/sheetactions'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('sheetactions',$_QR) === true)
		{
			$return = &$result;

			$cpt = 10;
			$str = "{";
			foreach($_QR['screencol1'] as $k => $v)
			{
				if((trim($_QR['screencol1'][$k]) != '') || (trim($_QR['screencol2'][$k]) != '') || (trim($_QR['screencol3'][$k]) != '') || (trim($_QR['screencol4'][$k]) != ''))
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['screencol2'][$k]).'",';
					$str .= '"'.trim($_QR['screencol3'][$k]).'",';
					$str .= '"'.trim($_QR['screencol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= '}';
			$_QR['sheetactions']['sheet_info'] = $str;

			$cpt = 10;
			$str = "{";
			foreach($_QR['systrayscol1'] as $k => $v)
			{
				if((trim($_QR['systrayscol1'][$k]) != '') || (trim($_QR['systrayscol2'][$k]) != '') || (trim($_QR['systrayscol3'][$k]) != '') || (trim($_QR['systrayscol4'][$k]) != ''))
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['systrayscol2'][$k]).'",';
					$str .= '"'.trim($_QR['systrayscol3'][$k]).'",';
					$str .= '"'.trim($_QR['systrayscol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= '}';
			$_QR['sheetactions']['systray_info'] = $str;

			$cpt = 10;
			$str = "{";
			foreach($_QR['infoscol1'] as $k => $v)
			{
				if((trim($_QR['infoscol1'][$k]) != '') || (trim($_QR['infoscol2'][$k]) != '') || (trim($_QR['infoscol3'][$k]) != '') || (trim($_QR['infoscol4'][$k]) != ''))
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['infoscol2'][$k]).'",';
					$str .= '"'.trim($_QR['infoscol3'][$k]).'",';
					$str .= '"'.trim($_QR['infoscol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= '}';
			$_QR['sheetactions']['action_info'] = $str;

			$_QR['sheetactions']['deletable'] = 1;

			$str = "[";
			if(isset($_QR['capaids']))
			{
				foreach($_QR['capaids'] as $v)
				{
					$str .= '"' . trim($v) . '",';
				}
				$str = trim($str, ',');
			}
			$str .= "]";
			$_QR['sheetactions']['capaids'] = $str;
			
			$str = "[";
			if(isset($_QR['context']))
			{
				foreach($_QR['context'] as $v)
				{
					$str .= '"' . $contextavail[$v] . '",';
				}
				$str = trim($str, ',');
			}
			$str .= "]";
			$_QR['sheetactions']['context'] = $str;

			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/sheetactions'),$param);
		}

		dwho::load_class('dwho_sort');
		dwho::load_class('dwho_json');

		$arr = array();
		$data = array();
		foreach($arr as $d)
		{
			$data[] = $d;
		}

		$return['context']['list'] = $contextavail;
		$return['context']['slt'] = array();
		if(isset($return['sheetactions']['context']) && dwho_has_len($return['sheetactions']['context']))
		{
			$sel = dwho_json::decode($return['sheetactions']['context'], true);
			$return['context']['slt'] = array_intersect($sel, $return['context']['list']);
			$return['context']['list'] = array_diff($return['context']['list'], $return['context']['slt']);
		}

		$sel = dwho_json::decode($return['sheetactions']['capaids'], true);
		$arrpf = array();
		foreach($sel as $v)
		{
			$arrpf[$v] = $v;
		}

		$return['capaids']['list'] = $profileclientlist;
		$return['capaids']['slt'] = array();
		if(isset($return['sheetactions']['capaids']) && dwho_has_len($return['sheetactions']['capaids']))
		{
			$return['capaids']['slt'] = dwho_array_intersect_key($arrpf, $return['capaids']['list']);
			$return['capaids']['list'] = dwho_array_diff_key($return['capaids']['list'], $return['capaids']['slt']);
		}

	#	dwho_print_r($info, 'info');
		$screens = null;
        if(isset($info['sheetactions']['sheet_info']) && dwho_has_len($info['sheetactions']['sheet_info']))
        {
            $screens = dwho_json::decode($info['sheetactions']['sheet_info'], true);
        }

		$systrays = null;
        if(isset($info['sheetactions']['systray_info']) && dwho_has_len($info['sheetactions']['systray_info']))
        {
            $systrays = dwho_json::decode($info['sheetactions']['systray_info'], true);
        }

		$informations = null;
        if(isset($info['sheetactions']['action_info']) && dwho_has_len($info['sheetactions']['action_info']))
        {
            $informations = dwho_json::decode($info['sheetactions']['action_info'], true);
        }

		$_TPL->set_var('idsheetactions',$return['sheetactions']['id']);
		$_TPL->set_var('contextavail',$contextavail);
		$_TPL->set_var('profileclientlist',$profileclientlist);
		$_TPL->set_var('screens',$screens);
		$_TPL->set_var('systrays',$systrays);
		$_TPL->set_var('informations',$informations);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('ctisheetactions');

		if(isset($_QR['idsheetactions']) === false
		|| ($info = $app->get($_QR['idsheetactions'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/sheetactions'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('service/ipbx/cti_settings/sheetactions'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctisheetactions',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_sheetactions_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/sheetactions'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/cti_settings/sheetactions');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/cti_settings/sheetactions/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
