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
$_TPL->load_i18n_file('tpl/www/bloc/cti/profiles/list-values.i18n', 'global');

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
    'loginwindow.url'                 => $_TPL->bbf('pref-loginwindow-url'),
    'xlet.identity.logagent'          => $_TPL->bbf('pref-xlet-identity-logagent'),
    'xlet.identity.pauseagent'        => $_TPL->bbf('pref-xlet-identity-pauseagent'),
    'xlet.agentsnext.fontname'        => $_TPL->bbf('pref-xlet-agentsnext-fontname'),
    'xlet.agentsnext.fontsize'        => $_TPL->bbf('pref-xlet-agentsnext-fontsize'),
    'xlet.agentsnext.blinktime'       => $_TPL->bbf('pref-xlet-agentsnext-blinktime'),
    'xlet.agentdetails.noqueueaction' => $_TPL->bbf('pref-xlet-agentdetails-noqueueaction'),
    'xlet.agentdetails.hideastid'     => $_TPL->bbf('pref-xlet-agentdetails-hideastid'),
    'xlet.agentdetails.hidecontext'   => $_TPL->bbf('pref-xlet-agentdetails-hidecontext'),
    'xlet.agents.fontname'            => $_TPL->bbf('pref-xlet-agents-fontname'),
    'xlet.agents.fontsize'            => $_TPL->bbf('pref-xlet-agents-fontsize'),
    'xlet.agents.iconsize'            => $_TPL->bbf('pref-xlet-agents-iconsize'),
    'presence.autochangestate'        => $_TPL->bbf('pref-presence-autochangestate')
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
				$_QR['profiles']['services'] = implode(',', $_QR['services']);
			}
			else
				$_QR['profiles']['services'] = '';

			if(array_key_exists('funcs', $_QR))
			{
				$_QR['profiles']['funcs'] = implode(',', $_QR['funcs']);
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
				$_QRY->go($_TPL->url('cti/profiles'),$param);
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
			$_QRY->go($_TPL->url('cti/profiles'),$param);

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
				$_QR['profiles']['services'] = implode(',', $_QR['services']);
			}
			else
				$_QR['profiles']['services'] = '';


			if(array_key_exists('funcs', $_QR))
			{
				$_QR['profiles']['funcs'] = implode(',', $_QR['funcs']);
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

				$info['ctiprofiles'] = $result['profiles'];
			}
			else
				$_QRY->go($_TPL->url('cti/profiles'),$param);
		}



        // we go here ONLY IF:
        //  . 1st time editing the profile
        //  . error after saving changes
		$info['services']['slt'] = array();
        $info['services']['list'] = $servicesavail;

		if(isset($info['ctiprofiles']['services']) && dwho_has_len($info['ctiprofiles']['services']))
		{
			$svcs = explode(',', $info['ctiprofiles']['services']);

			$info['services']['slt'] = array();
			foreach($svcs as $svc)
			{
				$info['services']['slt'][$svc] = $info['services']['list'][$svc];
				unset($info['services']['list'][$svc]);
			}
		}

		$info['funcs']['list'] = $funcsavail;
		$info['funcs']['slt'] = array();
		if(isset($info['ctiprofiles']['funcs']) && dwho_has_len($info['ctiprofiles']['funcs']))
		{
			$fncs = explode(',', $info['ctiprofiles']['funcs']);

			$info['funcs']['slt'] = array();
			foreach($fncs as $fnc)
			{
				$info['funcs']['slt'][$fnc] = $info['funcs']['list'][$fnc];
				unset($info['funcs']['list'][$fnc]);
			}
		}

		$info['preferences']['slt'] = array();
		$info['preferences']['avail'] = $preferencesavail;
		
		if(isset($info['ctiprofiles']['preferences']) && dwho_has_len($info['ctiprofiles']['preferences']))
		{
			$info['preferences']['slt'] = explode(',', $info['ctiprofiles']['preferences']);
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
			$_QRY->go($_TPL->url('cti/profiles'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('cti/profiles'),$param);
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
			$_QRY->go($_TPL->url('cti/profiles'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('idprofiles',$idprofiles);
#$_TPL->set_var('group',$group);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/cti/menu');

$menu->set_toolbar('toolbar/cti/profiles');

$_TPL->set_bloc('main','/cti/profiles/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
