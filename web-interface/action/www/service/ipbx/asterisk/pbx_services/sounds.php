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

$act	= $_QRY->get('act','');
$dir	= strval($_QRY->get('dir',''));
$page	= dwho_uint($_QRY->get('page'),1);
$search	= strval($_QRY->get('search'));

$param = array();
$param['act'] = 'list';

$sounds = &$ipbx->get_module('sounds');

if(($list_dirs = $sounds->get_list_dirs()) !== false)
{
	dwho::load_class('dwho_sort');
	$sort = new dwho_sort();
	usort($list_dirs,array(&$sort,'strnat_usort'));
}

switch($act)
{
	case 'adddir':
		$param['act'] = 'listdir';

		$info = array();
		$fm_save = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get('dirname'))) !== false
			&& $sounds->add_dir($info['dirname']) === true)
				$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
			else
				$fm_save = false;
		}

		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);
		break;
	case 'editdir':
		$param['act'] = 'listdir';

		if(isset($_QR['id']) === false || ($info = $sounds->get_dir($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);

		$id = $info['dirname'];
		$fm_save = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get('dirname'))) !== false
			&& $sounds->edit_dir($id,$info['dirname']) === true)
				$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
			else
				$fm_save = false;
		}

		$_TPL->set_var('id',$id);
		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);
		break;
	case 'deletedir':
		$param['act'] = 'listdir';
		$param['page'] = $page;

		if(isset($_QR['id']) === false || ($info = $sounds->get_dir($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);

		$sounds->delete_dir($info['dirname']);

		$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
		break;
	case 'add':
		if($list_dirs === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		$param['dir'] = $dir;

		$info = array();
		$info['filename'] = '';
		$info['dirname'] = $dir;

		$fm_save = null;

		if(isset($_QR['fm_send'],$_QR['dirname']) === false
		|| ($info['directory'] = $sounds->get_dir($_QR['dirname'])) === false
		|| ($fileuploaded = $sounds->uploaded_fileinfo('filename')) === false
		|| ($info['dirname'] = $sounds->chk_value('dirname',$info['directory']['dirname'])) === false
		|| ($info['filename'] = $sounds->chk_value('filename',$fileuploaded['name'])) === false)
		{
			if(isset($fileuploaded) === true)
			{
				$fm_save = false;

				if(is_array($fileuploaded) === true)
					dwho_file::rm($fileuploaded['tmp_name']);
			}
		}
		else
		{
			$filename = dwho_file::joinpath($info['dirname'],$fileuploaded['name']);

			if($sounds->add($filename,$fileuploaded['tmp_name']) === true)
			{
				$param['dir'] = $info['dirname'];
				$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
			}
			else
				$fm_save = false;
		}

		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('option',$sounds->get_option());
		break;
	case 'edit':
		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		$dir = $info['dirname'] = $info['directory']['dirname'];

		$param['dir'] = $dir;

		if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['dirname'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);

		$info['filename'] = $info['file']['basename'];
		$id = $info['file']['filename'];

		$fm_save = null;

		if(isset($_QR['fm_send'],$_QR['filename'],$_QR['dirname']) === true
		&& ($info['directory'] = $sounds->get_dir($_QR['dirname'])) !== false)
		{
			$info['dirname'] = $info['directory']['dirname'];
			$info['filename'] = strval($_QR['filename']).'.'.$info['file']['extension'];

			if(($info['dirname'] = $sounds->chk_value('dirname',$info['dirname'])) !== false
			&& ($info['filename'] = $sounds->chk_value('filename',$info['filename'])) !== false)
			{
				$filename = dwho_file::joinpath($info['file']['dirname'],$info['file']['filename']);
				$newfilename = dwho_file::joinpath($info['dirname'],$info['filename']);

				if($sounds->edit($filename,$newfilename) === true)
				{
					$param['dir'] = $info['dirname'];
					$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
				}
				else
					$fm_save = false;

				$info['filename'] = $info['file']['basename'];
			}
			else
				$fm_save = false;
		}

		$_TPL->set_var('id',$id);
		$_TPL->set_var('info',$info);
		$_TPL->set_var('fm_save',$fm_save);
		break;
	case 'delete':
		$param['dir'] = $dir;
		$param['page'] = $page;

		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);

		$file = dwho_file::joinpath($info['directory']['dirname'],$info['file']['filename']);

		$sounds->delete($file);

		$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
		break;
	case 'deletes':
		$param['dir'] = $dir;
		$param['page'] = $page;

		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		if(($values = dwho_issa_val('files',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['file'] = $sounds->get(strval($values[$i]),$info['directory']['dirname'])) === false)
				continue;

			$file = dwho_file::joinpath($info['directory']['dirname'],$info['file']['filename']);

			$sounds->delete($file);
		}

		$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
		break;
	case 'download':
		dwho::load_class('dwho_http');

		$param['dir'] = $dir;
		$param['page'] = $page;

		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		if(isset($_QR['id']) === false
		|| ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false
		|| ($http_response = dwho_http::factory('response')) === false
		|| $http_response->send_file_download($info['file']['path']) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),$param);
		break;
	case 'list':
		$total = 0;

		if($dir === '')
			$dir = false;

		if(($dirs = $sounds->get_dir($dir,true,$search)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_services/sounds'),'act=listdir');

var_dump($dirs);

		if(($dirs = $dirs['files']) !== false)
		{
			$sort_key = 'name';
			if (isset($_QR['sort_key']) === true)
				$sort_key = $_QR['sort_key'];

			$total = count($dirs);
			dwho::load_class('dwho_sort');
			$sort = new dwho_sort(array('key' => $sort_key));
			usort($dirs,array(&$sort,'strnat_usort'));

			$link_sort_order = SORT_ASC;

			if (isset($_QR['sort_order']) === true
			&& $_QR['sort_order'] == SORT_ASC)
			{
				rsort($dirs);
				$link_sort_order = SORT_DESC;
			}
			if (isset($_QR['sort_order']) === true
			&& $_QR['sort_order'] == SORT_DESC)
			{
				sort($dirs);
				$link_sort_order = SORT_ASC;
			}

			$_TPL->set_var('sort_order',$link_sort_order);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,20,$total));
		$_TPL->set_var('list',$dirs);
		$_TPL->set_var('search',$search);
		break;
	case 'listdir':
	default:
		$act = 'listdir';
		$param['act'] = 'listdir';

		$total = 0;

		if(($dirs = $sounds->get_list_dirs_files()) !== false)
		{
			$sort_key = 'dirname';
			if (isset($_QR['sort_key']) === true)
				$sort_key = $_QR['sort_key'];

			$total = count($dirs);
			dwho::load_class('dwho_sort');
			$sort = new dwho_sort(array('key' => $sort_key));
			usort($dirs,array($sort,'strnat_usort'));

			$link_sort_order = SORT_ASC;

			if (isset($_QR['sort_order']) === true
			&& $_QR['sort_order'] == SORT_ASC)
			{
				rsort($dirs);
				$link_sort_order = SORT_DESC;
			}
			if (isset($_QR['sort_order']) === true
			&& $_QR['sort_order'] == SORT_DESC)
			{
				sort($dirs);
				$link_sort_order = SORT_ASC;
			}

			$_TPL->set_var('sort_order',$link_sort_order);

		}

		$_TPL->set_var('pager',dwho_calc_page($page,20,$total));
		$_TPL->set_var('list',$dirs);
		break;
}

$_TPL->set_var('act',$act);
$_TPL->set_var('dir',$dir);
$_TPL->set_var('list_dirs',$list_dirs);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_services/sounds');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/sounds/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
