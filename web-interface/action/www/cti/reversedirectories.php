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
dwho::load_class('dwho_json');

$ctireversedirectories = &$ipbx->get_module('ctireversedirectories');
$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$idrdid = isset($_QR['idrdid']) === true ? dwho_uint($_QR['idrdid'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';
$param['idrdid'] = $idrdid;

$info = $result = array();

$element = array();
$element['ctireversedirectories'] = $ctireversedirectories->get_element();
$app = &$ipbx->get_application('ctireversedirectories');
$dir = &$ipbx->get_application('ctidirectories');
$appincall = &$ipbx->get_application('incall');

$incalllist = $appincall->get_incalls_list();
$dirlist = $dir->get_directories_list();

$_TPL->load_i18n_file('tpl/www/bloc/cti/reversedirectories/form.i18n', 'global');

$diravail = array();
foreach($dirlist as $v)
{
	$p = $v['ctidirectories'];
	$diravail[] = $p['name'];
}


$appcontext = &$ipbx->get_application('context');
$contexts   = array('*' => $_TPL->bbf('incallavailable-default'));
foreach($appcontext->get_contexts_list($disable=false) as $context)
	$contexts[$context['context']['name']] = $context['context']['displayname'];

$appincall  = &$ipbx->get_application('incall');
$incalls    = array();
foreach($appincall->get_incalls_list($disable=false) as $incall)
{
	if(!array_key_exists($incall['context'], $incalls))
		$incalls[$incall['context']] = array();

	$incalls[$incall['context']][] = array($incall['id'], $incall['exten']);
}


switch($act)
{
	case 'add':
		$result = $fm_save = null;

		$extens = array('*' => $_TPL->bbf('incallavailable-default'));
		foreach($incalls[$info['reversedirectories']['context']] as $exten)
			$extens[$exten[1]] = $exten[1];

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('reversedirectories',$_QR) === true)
		{
			$str = '[';
			foreach($_QR['directories'] as $v)
			{
				$str .= '"'.trim($diravail[$v]).'",';
			}
			$str = trim($str, ',');
			$str .= ']';
			$_QR['reversedirectories']['directories'] = $str;
			$_QR['reversedirectories']['deletable'] = 1;

			if(!array_key_exists('extensions', $_QR['reversedirectories']))
				$_QR['reversedirectories']['extensions'] = array();
			$_QR['reversedirectories']['extensions'] = trim(implode(',', $_QR['reversedirectories']['extensions']));

			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/reversedirectories'),$param);
		}

		dwho::load_class('dwho_sort');

		$info['directories']['list'] = $diravail;
		$info['directories']['slt'] = null;
		$info['reversedirectories'] = null;

		$_TPL->set_var('info',$info);
		$_TPL->set_var('extens',$extens);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'edit':
		if(isset($_QR['idrdid']) === false
		|| ($info = $app->get($_QR['idrdid'])) === false)
			$_QRY->go($_TPL->url('cti/reversedirectories'),$param);

		if(strlen($info['reversedirectories']['extensions']) == 0)
			$info['reversedirectories']['extensions'] = array();
		else
			$info['reversedirectories']['extensions'] = explode(',', $info['reversedirectories']['extensions']);

		$extens = array();
		if(($idx = array_search('*', $info['reversedirectories']['extensions'])) !== false)
			$info['reversedirectories']['extensions'][$idx] = array('*' => $_TPL->bbf('incallavailable-default'));
		else
			$extens['*'] = $_TPL->bbf('incallavailable-default');

		foreach($incalls[$info['reversedirectories']['context']] as $exten)
		{
			if(($idx = array_search($exten[1], $info['reversedirectories']['extensions'])) !== false)
			{
				$info['reversedirectories']['extensions'][$idx] = array($exten[1] => $exten[1]);
				continue;
			}
			$extens[$exten[1]] = $exten[1];
		}

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('reversedirectories',$_QR) === true)
		{
			$return = &$result;
			$str = '[';
			foreach($_QR['directories'] as $v)
			{
				$str .= '"'.trim($diravail[$v]).'",';
			}
			$str = trim($str, ',');
			$str .= ']';
			$_QR['reversedirectories']['directories'] = $str;
			$_QR['reversedirectories']['deletable'] = 1;

			if(!array_key_exists('extensions', $_QR['reversedirectories']))
				$_QR['reversedirectories']['extensions'] = array();
			$_QR['reversedirectories']['extensions'] = trim(implode(',', $_QR['reversedirectories']['extensions']));

			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result  = $app->get_result();
				$error  = $app->get_error();
			}
			else
				$_QRY->go($_TPL->url('cti/reversedirectories'),$param);
		}

		$info['directories']['slt'] = array();
        $info['directories']['list'] = $diravail;

		if(isset($info['reversedirectories']['directories']) && dwho_has_len($info['reversedirectories']['directories']))
		{
			$sel = dwho_json::decode($return['reversedirectories']['directories'], true);
			$info['directories']['slt'] =
				array_intersect(
					$sel,
					$return['directories']['list']);
			$info['directories']['list'] =
				array_diff(
					$return['directories']['list'],
					$return['directories']['slt']);
		}

		dwho::load_class('dwho_sort');


		$_TPL->set_var('idcontexts', $info['reversedirectories']['id']);
		$_TPL->set_var('info'      , $return);
		$_TPL->set_var('extens'    , $extens);
		$_TPL->set_var('fm_save'   , $fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('ctireversedirectories');

		if(isset($_QR['idrdid']) === false
		|| ($info = $app->get($_QR['idrdid'])) === false)
			$_QRY->go($_TPL->url('cti/reversedirectories'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('cti/reversedirectories'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctireversedirectories',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_reversedirectories_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('cti/reversedirectories'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act'     , $act);
$_TPL->set_var('idrdid'  , $idrdid);
$_TPL->set_var('contexts', $contexts);
$_TPL->set_var('incalls' , $incalls);
	

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/cti/reversedirectories.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/cti/menu');

$menu->set_toolbar('toolbar/cti/reversedirectories');

$_TPL->set_bloc('main','/cti/reversedirectories/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
