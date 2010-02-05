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

/********************************************************** WARNING ********************************************************/
/*
	In this code, we assume that only
	'field_phone', 'field_fullname', 'field_firstname', 'field_lastname', 'field_company' and 'field_mail'
	are used. These correspond to 
	'{db-phone}', '{db-fullname}', '{db-firstname}', '{db-lastname}', '{db-company}' and '{db-mail}'
	in "displays" form.
	If you want to add fields you need to
	1/ Add fields in asterisk database (table ctidirectories)
	2/ Add support for these fields below in add and edit sections
	3/ Add corresponding entries and filters in the config file :
		/usr/share/pf-xivo-web-interface/object/service/ipbx/asterisk/ctidirectories/config.inc
	4/ Add corresponding widgets in template file :
		/usr/share/pf-xivo-web-interface/tpl/www/bloc/service/ipbx/asterisk/cti_settings/directories/form.php
	5/ Add JSON generation for these fields in 
		/usr/share/pf-xivo-web-interface/application/www/service/ipbx/asterisk/web_services/ctiserver/configuration.php
*/
/***************************************************************************************************************************/

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$iddirectories = isset($_QR['iddirectories']) === true ? dwho_uint($_QR['iddirectories'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;
$urilist = array();

xivo::load_class('xivo_directories',XIVO_PATH_OBJECT,null,false);
$dir = new xivo_directories();
$dirlist = $dir->get_all(null,true);

$appldapfilter = &$ipbx->get_application('ldapfilter');
$ldapfilterlist = $appldapfilter->get_ldapfilters_list();
$ldapservers = $appldapfilter->get_ldapservers_list();

foreach($dirlist as $v)
{
	$urilist[] = $v['uri'];
}

foreach($ldapfilterlist as $v)
{
	$urilist[] = "ldapfilter://".$v['ldapfilter']['name'];
}

foreach($ldapservers as $v)
{
	$urilist[] = "ldap://".$v['name'];
}

$param = array();
$param['act'] = 'list';
$param['iddirectories'] = $iddirectories;
$info = $result = array();

switch($act)
{
	case 'add':
		$app = &$ipbx->get_application('ctidirectories');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('directories',$_QR) === true)
		{
			foreach(array('match_direct', 'match_reverse', 'field_phone', 'field_firstname', 'field_lastname', 'field_fullname', 'field_company', 'field_mail', 'display_reverse') as $v)
			{
				if($_QR['directories'][$v] != '')
				{
					$str = "[";
					foreach(explode(',', $_QR['directories'][$v]) as $w)
					{
						$str .= '"' . trim($w) . '",';
					}
					$str = trim($str, ',');
					$str .= "]";
					$_QR['directories'][$v] = $str;
				}
			}

			$_QR['directories']['uri'] = $_QR['directories-uri'];
			$_QR['directories']['deletable'] = 1;
			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/directories'),$param);
		}

		dwho::load_class('dwho_sort');

		$_TPL->set_var('urilist',$urilist);
		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'edit':
		$app = &$ipbx->get_application('ctidirectories');

		if(isset($_QR['iddirectories']) === false
		|| ($info = $app->get($_QR['iddirectories'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/directories'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('directories',$_QR) === true)
		{
			$return = &$result;

			foreach(array('match_direct', 'match_reverse', 'field_phone', 'field_firstname', 'field_lastname', 'field_fullname', 'field_company', 'field_mail', 'display_reverse') as $v)
			{
				if($_QR['directories'][$v] != '')
				{
					$str = "[";
					foreach(explode(',', $_QR['directories'][$v]) as $w)
					{
						$str .= '"' . trim($w) . '",';
					}
					$str = trim($str, ',');
					$str .= "]";
					$_QR['directories'][$v] = $str;
				}
			}
			$_QR['directories']['deletable'] = 1;
			$_QR['directories']['uri'] = $_QR['directories-uri'];
			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/cti_settings/directories'),$param);
		}

		dwho::load_class('dwho_sort');
		dwho::load_class('dwho_json');

		$arr = array();

		foreach(array('match_direct', 'match_reverse', 'field_phone', 'field_firstname', 'field_lastname', 'field_fullname', 'field_company', 'field_mail', 'display_reverse') as $v)
		{
			if($return['directories'][$v] != '')
			{
				$arr = dwho_json::decode($return['directories'][$v], true);
				$return['directories'][$v] = implode(',', $arr);
			}
		}
		
		$_TPL->set_var('urilist',$urilist);
		$_TPL->set_var('iddirectories',$info['directories']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('ctidirectories');

		if(isset($_QR['iddirectories']) === false
		|| ($info = $app->get($_QR['iddirectories'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/directories'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('service/ipbx/cti_settings/directories'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctidirectories',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_directories_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/cti_settings/directories'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
#$_TPL->set_var('group',$group);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/cti_settings/directories');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/cti_settings/directories/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
