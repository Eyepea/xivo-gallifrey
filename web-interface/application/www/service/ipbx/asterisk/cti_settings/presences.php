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

$ctistatus = &$ipbx->get_module('ctistatus');
$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$idpresences = isset($_QR['idpresences']) === true ? dwho_uint($_QR['idpresences'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';
$param['idpresences'] = $idpresences;

$info = $result = array();
$info['access_status'] = array();
$info['access_status']['info'] = array();
$info['access_status']['slt'] = array();

$element = array();
$element['ctistatus'] = $ctistatus->get_element();

switch($act)
{
	case 'add':
		$app = &$ipbx->get_application('ctipresences');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('presences',$_QR) === true)
		{
			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		}

		dwho::load_class('dwho_sort');

		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'edit':
		$app = &$ipbx->get_application('ctipresences');

		if(isset($_QR['idpresences']) === false
		|| ($info = $app->get($_QR['idpresences'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('presences',$_QR) === true)
		{
			$return = &$result;
			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		}

		dwho::load_class('dwho_sort');

		$_TPL->set_var('idpresences',$info['presences']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('ctipresences');

		if(isset($_QR['idpresences']) === false
		|| ($info = $app->get($_QR['idpresences'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		break;

	case 'liststatus':
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctistatus',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_status_list($idpresences);
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
		break;

	case 'editstatus':
		$param['act'] = 'liststatus';
		$idstatus = $param['idstatus'] = $_QR['idstatus'];
		$info = array();

		$app = &$ipbx->get_application('ctistatus');
		$raw_status_list = $app->get_status_list();
		#$info['access_status']['info'] = $app->get();

		$status_list = array();
		foreach($raw_status_list as $k => $sl)
		{
			$sl_key = $sl['ctistatus']['id'];
			$status_list[$sl_key] = $sl['ctistatus'];
		}
		
		$actionslist = explode(',', $status_list[$idstatus]['actions']);
		if($actionslist[0] == '')
			$actionslist = null;

		if(isset($_QR['idstatus']) === false
		|| ($info = $app->getstatus($_QR['idstatus'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);

		$result = $fm_save = null;
		$return = &$info;

////////////////////////// FM_SEND ///////////////////////////////
		if(isset($_QR['fm_send']) === true
		&& dwho_issa('status',$_QR) === true)
		{
			$return = &$result;
			if(array_key_exists('access_status', $_QR) === true)
			{
				$as_str = array();
				foreach($_QR['access_status'] as $k => $as)
				{
					$as_str[] = $as;
				}
				$_QR['status']['access_status'] = implode(',', $as_str);
			}
			else
				$_QR['status']['access_status'] = '';

			if(array_key_exists('actionsargs', $_QR) === true)
			{
				$actarr = array();
				foreach($_QR['actionsargs'] as $k => $as)
				{
					$actarr[] = $_QR['actionslist'][$k] . '(' . $as . ')';
				}
				$_QR['status']['actions'] = implode(',', $actarr);
			}
			else
				$_QR['status']['actions'] = '';

			$_QR['status']['presence_id'] = $idpresences;
			$_QR['status']['deletable'] = 1;

			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		}
////////////////////////////////////////////////////////////////

		$info['access_status']['slt'] = array();
		if(($info['access_status']['list'] = $app->get_status_list(null, null, null, true)) !== false)
		{
			if(isset($info['status']['access_status']) && dwho_has_len($info['status']['access_status']))
			{
				$sel = explode(',', $info['status']['access_status']);
				$selected = array();
				foreach($sel as $s => $k)
				{
					$selected[] = array('id' => $k);
				}
				$info['access_status']['slt'] = 
					dwho_array_intersect_key(
						$selected,
						$info['access_status']['list'],
						'id');
				$info['access_status']['list'] =
					dwho_array_diff_key(
						$info['access_status']['list'],
						$info['access_status']['slt']);
			}
		}

		dwho::load_class('dwho_sort');

		$_TPL->set_var('idstatus',$info['status']['id']);
		$_TPL->set_var('idpresences',$idpresences);
		$_TPL->set_var('info',$info);
		$_TPL->set_var('element',$element);
		$_TPL->set_var('actionslist',$actionslist);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'addstatus':
		$param['act'] = 'liststatus';

		$app = &$ipbx->get_application('ctistatus');
		$raw_status_list = $app->get_status_list();
		#$info['access_status']['info'] = $app->get();

		$result = $fm_save = null;
		$return = &$info;

////////////////////////// FM_SEND ///////////////////////////////
		if(isset($_QR['fm_send']) === true
		&& dwho_issa('status',$_QR) === true)
		{
			$return = &$result;
			if(array_key_exists('access_status', $_QR) === true)
			{
				$as_str = array();
				foreach($_QR['access_status'] as $k => $as)
				{
					$as_str[] = $as;
				}
				$_QR['status']['access_status'] = implode(',', $as_str);
			}
			else
				$_QR['status']['access_status'] = '';

			if(array_key_exists('actionsargs', $_QR) === true)
			{
				$actarr = array();
				foreach($_QR['actionsargs'] as $k => $as)
				{
					$actarr[] = $_QR['actionslist'][$k] . '(' . $as . ')';
				}
				$_QR['status']['actions'] = implode(',', $actarr);
			}
			else
				$_QR['status']['actions'] = '';

			$_QR['status']['presence_id'] = $idpresences;
			$_QR['status']['deletable'] = 1;

			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		}
////////////////////////////////////////////////////////////////

		$info['access_status']['slt'] = array();
		if(($info['access_status']['list'] = $app->get_status_list(null, null, null, true)) !== false)
		{
			if(isset($info['status']['access_status']) && dwho_has_len($info['status']['access_status']))
			{
				$sel = explode(',', $info['status']['access_status']);
				$selected = array();
				foreach($sel as $s => $k)
				{
					$selected[] = array('id' => $k);
				}
				$info['access_status']['slt'] = array(); 
			}
		}

		dwho::load_class('dwho_sort');

		$info['status'] = null;
		$_TPL->set_var('info',$info);
		$_TPL->set_var('element',$element);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'deletestatus':
		$param['page'] = $page;
		$param['act'] = 'liststatus';

		$app = &$ipbx->get_application('ctistatus');

		if(isset($_QR['idstatus']) === false
		|| ($info = $app->get($_QR['idstatus'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctipresences',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_presences_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/presences'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('idpresences',$idpresences);
#$_TPL->set_var('group',$group);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/cti_settings/presences');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/cti_settings/presences/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
