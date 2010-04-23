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

$ctiprofiles = &$ipbx->get_module('ctiprofiles');
$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$idprofiles = isset($_QR['idprofiles']) === true ? dwho_uint($_QR['idprofiles'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

# for ce loading translation file
$_TPL->load_i18n_file('tpl/www/bloc/service/ipbx/asterisk/cti_settings/profiles/list-values.i18n', 'global');

/** list values */
$servicesavail = array(
    'enablevm'      => $_TPL->bbf('enablevm'),
    'callrecord'    => $_TPL->bbf('callrecord'),
    'incallrec'     => $_TPL->bbf('incallrec'),
    'incallfilter'  => $_TPL->bbf('incallfilter'),
    'enablednd'     => $_TPL->bbf('enablednd'),
    'fwdunc'        => $_TPL->bbf('fwdunc'),
    'fwdbusy'       => $_TPL->bbf('fwdbusy'),
    'fwdrna'        => $_TPL->bbf('fwdrna'),
);

$preferencesavail = array(
    'logagent' => $_TPL->bbf('pref-logagent'), 
    'pauseagent' => $_TPL->bbf('pref-pauseagent'), 
    'blinktime' => $_TPL->bbf('pref-blinktime'), 
    'fontsize' => $_TPL->bbf('pref-fontsize'), 
    'fontname' => $_TPL->bbf('pref-fontname'), 
    'iconsize' => $_TPL->bbf('pref-iconsize'), 
    'supervisor' => $_TPL->bbf('pref-supervisor'), 
    'queues-showqueuenames' => $_TPL->bbf('pref-queues-showqueuenames'), 
    'queues-showqueues' => $_TPL->bbf('pref-queues-showqueues'),
    'queues-statscolumns' => $_TPL->bbf('pref-queues-statscolumns'),
    'queues-shortlegends' => $_TPL->bbf('pref-queues-shortlegends'),
    'conference-allowrecord' => $_TPL->bbf('pref-conference-allowrecord'),
    'noqueueaction' => $_TPL->bbf('pref-noqueueaction'),
    'autochangestate' => $_TPL->bbf('pref-autochangestate')
);

$funcsavail = array(
    'agents' => $_TPL->bbf('agents'), 
    'presence' => $_TPL->bbf('presence'), 
    'switchboard' => $_TPL->bbf('switchboard'), 
    'customerinfo' => $_TPL->bbf('customerinfo'), 
    'search' => $_TPL->bbf('search'), 
    'dial' => $_TPL->bbf('dial'), 
    'chitchat' => $_TPL->bbf('chitchat'),
    'conference' => $_TPL->bbf('conference'),
    'directory' => $_TPL->bbf('directory'),
    'fax' => $_TPL->bbf('fax'),
    'features' => $_TPL->bbf('features'),
    'history' => $_TPL->bbf('history'),
    'database' => $_TPL->bbf('database'),
);

$ctixlets = array_keys(dwho_json::decode(file_get_contents('/etc/pf-xivo/ctiservers/allowedxlets.json'), true));

$xletsavail = array();
foreach ($ctixlets as $xlet)
{ $xletsavail[$xlet] = $_TPL->bbf("xlet-$xlet"); }

$xletslocavail = array(
    'dock',
    'grid',
    'tab'
);


$param = array();
$param['act'] = 'list';
$param['idprofiles'] = $idprofiles;

$info = $result = array();

$element = array();
$element['ctiprofiles'] = $ctiprofiles->get_element();

switch($act)
{
	case 'add':
	    var_dump($_QR);
		$app = &$ipbx->get_application('ctiprofiles');
		$apppres = &$ipbx->get_application('ctipresences');

		$pl = $apppres->get_presences_list();
		$preslist = array();
		foreach($pl as $v)
		{
			$p = $v['ctipresences'];
			$preslist[$p['id']] = $p['name'];
		}

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('profiles',$_QR) === true)
		{
			$_QR['profiles']['deletable'] = 1;
			$_QR['profiles']['presence'] = $_QR['presence'];

			if(array_key_exists('xletslist', $_QR))
			{
				$arr = array();
				foreach($_QR['xletslist'] as $k => $v)
				{
					if($v != '')
					{
						$str = "[ \"".$v."\", \"" . $_QR['xletsloc'][$k] . "\", \"";
						if($_QR['xletsf'][$k] == 1)
							$str .= 'f';
						if($_QR['xletsc'][$k] == 1)
							$str .= 'c';
						if($_QR['xletsm'][$k] == 1)
							$str .= 'm';
						if($_QR['xletss'][$k] == 1)
							$str .= 's';
						$str .= "\", \"";
						$str .= $_QR['xletposnum'][$k];
						$str .= "\" ]";
						$arr[] = $str;
					}
				}
				$_QR['profiles']['xlets'] = '[' . implode(',', $arr) . ']';
			}
			else
				$_QR['profiles']['xlets'] = '';

			if(array_key_exists('services', $_QR))
			{
				$arr = array();
				foreach($_QR['services'] as $v)
				{
					$arr[] = $servicesavail[$v];
				}
				$_QR['profiles']['services'] = implode(',', $arr);
			}
			else
				$_QR['profiles']['services'] = '';


			if(array_key_exists('funcs', $_QR))
			{
				$arr = array();
				foreach($_QR['funcs'] as $v)
				{
					$arr[] = $funcsavail[$v];
				}
				$_QR['profiles']['funcs'] = implode(',', $arr);
			}
			else
				$_QR['profiles']['funcs'] = '';

			if(array_key_exists('preferencesargs', $_QR))
			{
				$arr = array();
				foreach($_QR['preferencesargs'] as $k => $v)
				{
					$pref = $_QR['preferenceslist'][$k];
					$arr[] = $pref.'('.$v.')';
				}
				$_QR['profiles']['preferences'] = implode(',', $arr);
			}
			else
				$_QR['profiles']['preferences'] = '';

			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/profiles'),$param);
		}

		dwho::load_class('dwho_sort');

		$info['services']['list'] = $servicesavail;
		$info['services']['slt'] = null;
		$info['funcs']['list'] = $funcsavail;
		$info['funcs']['slt'] = null;
		$info['preferences']['avail'] = $preferencesavail;
		$info['preferences']['slt'] = null;
		$info['xlets']['list']['xlets'] = $xletsavail;
		$info['xlets']['slt'] = null;
		$info['ctiprofiles'] = null;

		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('preslist',$preslist);
		$_TPL->set_var('xletslocavail',$xletslocavail);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'edit':
		$app = &$ipbx->get_application('ctiprofiles');
		$apppres = &$ipbx->get_application('ctipresences');

		$pl = $apppres->get_presences_list();
		$preslist = array();
		foreach($pl as $v)
		{
			$p = $v['ctipresences'];
			$preslist[$p['id']] = $p['name'];
		}

		if(isset($_QR['idprofiles']) === false
		|| ($info = $app->get($_QR['idprofiles'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/profiles'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('profiles',$_QR) === true)
		{
			$_QR['profiles']['deletable'] = 1;
			$_QR['profiles']['presence'] = $_QR['presence'];

			if(array_key_exists('xletslist', $_QR))
			{
				$arr = array();
				foreach($_QR['xletslist'] as $k => $v)
				{
					if($v != '')
					{
						$str = "[ \"".$v."\", \"" . $_QR['xletsloc'][$k] . "\", \"";
						if($_QR['xletsf'][$k] == 1)
							$str .= 'f';
						if($_QR['xletsc'][$k] == 1)
							$str .= 'c';
						if($_QR['xletsm'][$k] == 1)
							$str .= 'm';
						if($_QR['xletss'][$k] == 1)
							$str .= 's';
						$str .= "\", \"";
						$str .= $_QR['xletposnum'][$k];
						$str .= "\" ]";
						$arr[] = $str;
					}
				}
				$_QR['profiles']['xlets'] = '[' . implode(',', $arr) . ']';
			}
			else
				$_QR['profiles']['xlets'] = '';

			if(array_key_exists('services', $_QR))
			{
				$arr = array();
				foreach($_QR['services'] as $v)
				{
					$arr[] = $servicesavail[$v];
				}
				$_QR['profiles']['services'] = implode(',', $arr);
			}
			else
				$_QR['profiles']['services'] = '';


			if(array_key_exists('funcs', $_QR))
			{
				$arr = array();
				foreach($_QR['funcs'] as $v)
				{
					$arr[] = $funcsavail[$v];
				}
				$_QR['profiles']['funcs'] = implode(',', $arr);
			}
			else
				$_QR['profiles']['funcs'] = '';

			if(array_key_exists('preferencesargs', $_QR))
			{
				$arr = array();
				foreach($_QR['preferencesargs'] as $k => $v)
				{
					$pref = $_QR['preferenceslist'][$k];
					$arr[] = $pref.'('.$v.')';
				}
				$_QR['profiles']['preferences'] = implode(',', $arr);
			}
			else
				$_QR['profiles']['preferences'] = '';

			$return = &$result;
			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/profiles'),$param);
		}

		$info['services']['slt'] = array();
        $info['services']['list'] = $servicesavail;

		if(isset($info['ctiprofiles']['services']) && dwho_has_len($info['ctiprofiles']['services']))
		{
			$sel = explode(',', $info['ctiprofiles']['services']);
			$info['services']['slt'] =
				array_intersect(
					$sel,
					$info['services']['list']);
			$info['services']['list'] =
				dwho_array_diff_key(
					$info['services']['list'],
					$info['services']['slt']);
		}

		$info['preferences']['slt'] = array();
		$info['preferences']['avail'] = $preferencesavail;
		
		if(isset($info['ctiprofiles']['preferences']) && dwho_has_len($info['ctiprofiles']['preferences']))
		{
			$info['preferences']['slt'] = explode(',', $info['ctiprofiles']['preferences']);
		}

		$info['funcs']['list'] = $funcsavail;
		$info['funcs']['slt'] = array();
		if(isset($info['ctiprofiles']['funcs']) && dwho_has_len($info['ctiprofiles']['funcs']))
		{
			$sel = explode(',', $info['ctiprofiles']['funcs']);
			$info['funcs']['slt'] =
				array_intersect(
					$sel,
					$info['funcs']['list']);
			$info['funcs']['list'] =
				dwho_array_diff_key(
					$info['funcs']['list'],
					$info['funcs']['slt']);
		}

		$info['xlets']['list']['xlets'] = $xletsavail;
		$info['xlets']['list']['loc'] = $xletslocavail;
		if(isset($info['ctiprofiles']['xlets']) && dwho_has_len($info['ctiprofiles']['xlets']))
		{
			$info['xlets']['slt'] = dwho_json::decode($info['ctiprofiles']['xlets'], true);
		}
		dwho::load_class('dwho_sort');

		$_TPL->set_var('idprofiles',$info['ctiprofiles']['id']);
		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('preslist',$preslist);
		$_TPL->set_var('servicesavail',$servicesavail);
		$_TPL->set_var('funcsavail',$funcsavail);
		$_TPL->set_var('preferencesavail',$preferencesavail);
		$_TPL->set_var('xletsavail',$xletsavail);
		$_TPL->set_var('xletslocavail',$xletslocavail);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('ctiprofiles');

		if(isset($_QR['idprofiles']) === false
		|| ($info = $app->get($_QR['idprofiles'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/profiles'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('service/ipbx/cti_settings/profiles'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctiprofiles',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_profiles_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/profiles'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('idprofiles',$idprofiles);
#$_TPL->set_var('group',$group);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/cti_settings/profiles');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/cti_settings/profiles/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
