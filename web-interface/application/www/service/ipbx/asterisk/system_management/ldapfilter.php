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
		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true && dwho_issa('ldapfilter',$_QR) === true)
		{
			if($appldapfilter->set_add($_QR) === false
			|| $appldapfilter->add() === false)
			{
				$fm_save = false;
				$result = $appldapfilter->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);
		}

		if(dwho_issa('ldapfilter',$result) === true)
		{
			if(dwho_ak('attrdisplayname',$result['ldapfilter']) === true
			&& dwho_has_len($result['ldapfilter'],'attrdisplayname') === true)
				$result['ldapfilter']['attrdisplayname'] = explode(',',$result['ldapfilter']['attrdisplayname']);

			if(dwho_ak('attrphonenumber',$result['ldapfilter']) === true
			&& dwho_has_len($result['ldapfilter'],'attrphonenumber') === true)
				$result['ldapfilter']['attrphonenumber'] = explode(',',$result['ldapfilter']['attrphonenumber']);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appldapfilter->get_elements());
		$_TPL->set_var('ldapservers',$appldapfilter->get_ldapservers_list(null,array('name' => SORT_ASC)));

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ldapfilter.js');
		break;
	case 'edit':
		$appldapfilter = &$ipbx->get_application('ldapfilter');

		if(isset($_QR['id']) === false || ($info = $appldapfilter->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && dwho_issa('ldapfilter',$_QR) === true)
		{
			$return = &$result;

			if($appldapfilter->set_edit($_QR) === false
			|| $appldapfilter->edit() === false)
			{
				$fm_save = false;
				$result = $appldapfilter->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);
		}

		if(dwho_issa('ldapfilter',$return) === true)
		{
			if(dwho_ak('attrdisplayname',$return['ldapfilter']) === true
			&& dwho_has_len($return['ldapfilter'],'attrdisplayname') === true)
				$return['ldapfilter']['attrdisplayname'] = explode(',',$return['ldapfilter']['attrdisplayname']);

			if(dwho_ak('attrphonenumber',$return['ldapfilter']) === true
			&& dwho_has_len($return['ldapfilter'],'attrphonenumber') === true)
				$return['ldapfilter']['attrphonenumber'] = explode(',',$return['ldapfilter']['attrphonenumber']);
		}

		$_TPL->set_var('id',$info['ldapfilter']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appldapfilter->get_elements());
		$_TPL->set_var('ldapservers',$appldapfilter->get_ldapservers_list(null,array('name' => SORT_ASC)));

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/ldapfilter.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		if(isset($_QR['id']) === false || $appldapfilter->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);

		$appldapfilter->delete();

		$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('ldapfilters',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appldapfilter->get($values[$i]) !== false)
				$appldapfilter->delete();
		}

		$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('ldapfilters',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appldapfilter->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appldapfilter->disable();
			else
				$appldapfilter->enable();
		}

		$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appldapfilter = &$ipbx->get_application('ldapfilter');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appldapfilter->get_ldapfilters_list(null,$order,$limit);
		$total = $appldapfilter->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/system_management/ldapfilter'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/system_management/ldapfilter');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/ldapfilter/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
