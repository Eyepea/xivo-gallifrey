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
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

$configfiles = &$ipbx->get_module('configfiles');

switch($act)
{
	case 'add':	
		$info = array('configfile' => null);

		if(isset($_QR['fm_send'], $_QR['configfile']['content']) === true)
		{
			if($configfiles->add($_QR['configfile']['filename'],$_QR['configfile']['content']) !== false)
				$_QRY->go($_TPL->url('service/ipbx/system_management/configfiles'),$param);

			$info['configfile'] = $_QR['configfile'];
		}

		$_TPL->set_var('error', $configfiles->get_error());
		$_TPL->set_var('info', $info);
		break;

	case 'import':
		if(isset($_QR['fm_send']) === true && $configfiles->import())
			$_QRY->go($_TPL->url('service/ipbx/system_management/configfiles'), $param);

		$_TPL->set_var('error'	    , $configfiles->get_error());
		$_TPL->set_var('import_file', $configfiles->get_config_import_file());
		break;

	case 'edit':
		if(isset($_QR['id']) === false || ($info = $configfiles->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/configfiles'),$param);

		if(isset($_QR['fm_send'],$_QR['content']) === true)
		{
			if($configfiles->edit($info['name'],$_QRY->get_uqr('content')) !== false)
				$_QRY->go($_TPL->url('service/ipbx/system_management/configfiles'),$param);

			$info['content'] = $_QR['content'];
		}

		$_TPL->set_var('info',$info);
		break;
	case 'delete':
		$param['page'] = $page;

		if($configfiles->delete($_QR['filename']))
			$_QRY->go($_TPL->url('service/ipbx/system_management/configfiles'),$param);

		$_TPL->set_var('error', $configfiles->get_error());
		break;

	case 'deletes':
		$param['page'] = $page;

		foreach(array_values($_QR['configfiles']) as $file)
			$configfiles->delete($file);

		$_QRY->go($_TPL->url('service/ipbx/system_management/configfiles'),$param);
		break;

	default:
		$act = 'list';
		$total = 0;

		if(($files = $configfiles->get_list()) !== false)
		{
			$total = count($files);
			dwho::load_class('dwho_sort');
			$sort = new dwho_sort();
			usort($files,array(&$sort,'strnat_usort'));
		}

		$_TPL->set_var('pager',dwho_calc_page($page,20,$total));
		$_TPL->set_var('list',$files);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/system_management/configfiles');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/configfiles/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
