<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appvoicemenu = &$ipbx->get_application('voicemenu');

		$result = $fm_save = $error = null;

		if(isset($_QR['fm_send']) === true && dwho_issa('voicemenu',$_QR) === true)
		{
			if($appvoicemenu->set_add($_QR) === false
			|| $appvoicemenu->add() === false)
			{
				$fm_save = false;
				$result = $appvoicemenu->get_result();
				$error = $appvoicemenu->get_error();
			}
			else
			{
				$ipbx->discuss('dialplan reload');
				$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);
			}
		}

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction-application.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ipbxapplication.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/voicemenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		if(dwho_issa('voicemenu',$result) === false)
			$result['voicemenu'] = null;

		if(dwho_issa('voicemenuflow-data',$result) === false)
			$result['voicemenuflow-data'] = null;

		if(dwho_issa('voicemenuevent-data',$result) === false)
			$result['voicemenuevent-data'] = null;

		$_TPL->set_var('info',$result);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('voicemenuevent',&$result['voicemenuevent-data']);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appvoicemenu->get_elements());
		$_TPL->set_var('voicemail_list',$appvoicemenu->get_voicemail_list());
		$_TPL->set_var('destination_list',$appvoicemenu->get_destination_list());
		$_TPL->set_var('sound_list',$appvoicemenu->get_sound());
		$_TPL->set_var('moh_list',$appvoicemenu->get_musiconhold());
		$_TPL->set_var('context_list',$appvoicemenu->get_context_list());
		$_TPL->set_var('ipbxapplications',$appvoicemenu->get_ipbxapplications());
		break;
	case 'edit':
		$appvoicemenu = &$ipbx->get_application('voicemenu');

		if(isset($_QR['id']) === false
		|| ($info = $appvoicemenu->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);

		$result = $fm_save = $error = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && dwho_issa('voicemenu',$_QR) === true)
		{
			$return = &$result;

			if($appvoicemenu->set_edit($_QR) === false
			|| $appvoicemenu->edit() === false)
			{
				$fm_save = false;
				$result = $appvoicemenu->get_result();
				$error = $appvoicemenu->get_error();
			}
			else
			{
				$ipbx->discuss('dialplan reload');
				$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);
			}
		}

		if(dwho_issa('voicemenuflow-data',$return) === false)
			$return['voicemenuflow-data'] = null;

		if(dwho_issa('voicemenuevent-data',$return) === false)
			$return['voicemenuevent-data'] = null;

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction-application.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ipbxapplication.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/voicemenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_TPL->set_var('id',$info['voicemenu']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('voicemenuevent',&$return['voicemenuevent-data']);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appvoicemenu->get_elements());
		$_TPL->set_var('voicemail_list',$appvoicemenu->get_voicemail_list());
		$_TPL->set_var('destination_list',$appvoicemenu->get_destination_list());
		$_TPL->set_var('moh_list',$appvoicemenu->get_musiconhold());
		$_TPL->set_var('context_list',$appvoicemenu->get_context_list());
		$_TPL->set_var('ipbxapplications',$appvoicemenu->get_ipbxapplications());
		break;
	case 'delete':
		$param['page'] = $page;

		$appvoicemenu = &$ipbx->get_application('voicemenu');

		if(isset($_QR['id']) === false || $appvoicemenu->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);

		$appvoicemenu->delete();

		$ipbx->discuss('dialplan reload');
		$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('voicemenus',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);

		$appvoicemenu = &$ipbx->get_application('voicemenu');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appvoicemenu->get($values[$i]) !== false)
				$appvoicemenu->delete();
		}

		$ipbx->discuss('dialplan reload');
		$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('voicemenus',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);

		$appvoicemenu = &$ipbx->get_application('voicemenu');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appvoicemenu->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appvoicemenu->disable();
			else
				$appvoicemenu->enable();
		}

		$ipbx->discuss('dialplan reload');
		$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appvoicemenu = &$ipbx->get_application('voicemenu');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appvoicemenu->get_voicemenu_list(null,$order,$limit);
		$total = $appvoicemenu->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/call_management/voicemenu'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/voicemenu');

$_TPL->set_var('act',$act);
$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/voicemenu/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
