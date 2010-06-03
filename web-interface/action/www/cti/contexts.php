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

$cticontexts = &$ipbx->get_module('cticontexts');
$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$idcontexts = isset($_QR['idcontexts']) === true ? dwho_uint($_QR['idcontexts'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';
$param['idcontexts'] = $idcontexts;

$info = $result = array();

$element = array();
$element['cticontexts'] = $cticontexts->get_element();
$app = &$ipbx->get_application('cticontexts');

switch($act)
{
	case 'add':
		$dirs = &$ipbx->get_application('ctidirectories');
		$disps = &$ipbx->get_application('ctidisplays');
		$pbxctx = &$ipbx->get_application('context');

		$order = array();
		$order['name'] = SORT_ASC;
		$pbxctxlist = $pbxctx->get_contexts_list(null, $order);
		$dirlist = $dirs->get_directories_list();
		$displist = $disps->get_displays_list();
		$dispavail = array();
		foreach($displist as $v)
		{
			$p = $v['ctidisplays'];
			$dispavail[$p['id']] = $p['name'];
		}
		$diravail = array();
		foreach($dirlist as $v)
		{
			$p = $v['ctidirectories'];
			$diravail[] = $p['name'];
		}
		$pbxctxavail = array();
		foreach($pbxctxlist as $v)
		{
			$p = $v['context'];
			$pbxctxavail[] = $p['name'];
		}

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('contexts',$_QR) === true)
		{
			if(array_key_exists('directories', $_QR))
            {
                $arr = array();
                foreach($_QR['directories'] as $v)
                {
                    $arr[] = $diravail[$v];
                }
                $_QR['contexts']['directories'] = implode(',', $arr);
            }
            else
                $_QR['contexts']['directories'] = '';

			$_QR['contexts']['deletable'] = 1;
			$_QR['contexts']['display'] = $_QR['contexts-display'];
			$_QR['contexts']['name'] = $_QR['contexts-name'];

			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/contexts'),$param);
		}

		dwho::load_class('dwho_sort');

		$info['directories']['list'] = $diravail;
		$info['directories']['slt'] = null;
		$info['displays']['list'] = $dispavail;
		$info['displays']['pbxctx'] = $pbxctxavail;
		#$info['displays']['slt'] = null;
		$info['cticontexts'] = null;

		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'edit':
		$dirs = &$ipbx->get_application('ctidirectories');
		$disps = &$ipbx->get_application('ctidisplays');
		$pbxctx = &$ipbx->get_application('context');

		$order = array();
		$order['name'] = SORT_ASC;
		$pbxctxlist = $pbxctx->get_contexts_list(null, $order);
		$dirlist = $dirs->get_directories_list();
		$displist = $disps->get_displays_list();
		$dispavail = array();
		foreach($displist as $v)
		{
			$p = $v['ctidisplays'];
			$dispavail[$p['id']] = $p['name'];
		}
		$diravail = array();
		foreach($dirlist as $v)
		{
			$p = $v['ctidirectories'];
			$diravail[] = $p['name'];
		}
		$pbxctxavail = array();
		foreach($pbxctxlist as $v)
		{
			$p = $v['context'];
			$pbxctxavail[] = $p['name'];
		}

		if(isset($_QR['idcontexts']) === false
		|| ($info = $app->get($_QR['idcontexts'])) === false)
			$_QRY->go($_TPL->url('cti/contexts'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('contexts',$_QR) === true)
		{
			if(array_key_exists('directories', $_QR))
            {
                $arr = array();
                foreach($_QR['directories'] as $v)
                {
                    $arr[] = $diravail[$v];
                }
                $_QR['contexts']['directories'] = implode(',', $arr);
            }
            else
                $_QR['contexts']['directories'] = '';

			$_QR['contexts']['deletable'] = 1;
			$_QR['contexts']['display'] = $_QR['contexts-display'];
			$_QR['contexts']['name'] = $_QR['contexts-name'];

			$return = &$result;
			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/contexts'),$param);
		}

		$info['directories']['slt'] = array();
        $info['directories']['list'] = $diravail;
		$info['displays']['list'] = $dispavail;
		$info['displays']['pbxctx'] = $pbxctxavail;

		if(isset($info['cticontexts']['directories']) && dwho_has_len($info['cticontexts']['directories']))
		{
			$sel = explode(',', $info['cticontexts']['directories']);
			$info['directories']['slt'] =
				array_intersect(
					$sel,
					$info['directories']['list']);
			$info['directories']['list'] =
				array_diff(
					$info['directories']['list'],
					$info['directories']['slt']);
		}

		dwho::load_class('dwho_sort');


		$_TPL->set_var('idcontexts',$info['cticontexts']['id']);
		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('cticontexts');

		if(isset($_QR['idcontexts']) === false
		|| ($info = $app->get($_QR['idcontexts'])) === false)
			$_QRY->go($_TPL->url('cti/contexts'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('cti/contexts'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('cticontexts',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_contexts_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('cti/contexts'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('idcontexts',$idcontexts);
#$_TPL->set_var('group',$group);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/cti/menu');

$menu->set_toolbar('toolbar/cti/contexts');

$_TPL->set_bloc('main','/cti/contexts/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
