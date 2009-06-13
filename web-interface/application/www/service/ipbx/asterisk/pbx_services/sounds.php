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
$dir = isset($_QR['dir']) === true ? strval($_QR['dir']) : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$search = isset($_QR['search']) === true ? strval($_QR['search']) : '';

$param = array();
$param['act'] = 'list';

$sounds = &$ipbx->get_module('sounds');

if(($list_dirs = $sounds->get_list_dirs()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort();
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
			if(($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get_qr('dirname'))) !== false
			&& $sounds->add_dir($info['dirname']) === true)
				$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
			else
				$fm_save = false;
		}

		$_HTML->set_var('info',$info);
		$_HTML->set_var('fm_save',$fm_save);
		break;
	case 'editdir':
		$param['act'] = 'listdir';

		if(isset($_QR['id']) === false || ($info = $sounds->get_dir($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

		$id = $info['dirname'];
		$fm_save = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($info['dirname'] = $sounds->chk_value('dirname',$_QRY->get_qr('dirname'))) !== false
			&& $sounds->edit_dir($id,$info['dirname']) === true)
				$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
			else
				$fm_save = false;
		}

		$_HTML->set_var('id',$id);
		$_HTML->set_var('info',$info);
		$_HTML->set_var('fm_save',$fm_save);
		break;
	case 'deletedir':
		$param['act'] = 'listdir';
		$param['page'] = $page;

		if(isset($_QR['id']) === false || ($info = $sounds->get_dir($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

		$sounds->delete_dir($info['dirname']);

		$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
		break;
	case 'add':
		if($list_dirs === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		$param['dir'] = $dir;

		$info = array();
		$info['filename'] = '';
		$info['dirname'] = $dir;

		$fm_save = null;

		if(isset($_QR['fm_send'],$_QR['dirname']) === false
		|| ($info['directory'] = $sounds->get_dir($_QR['dirname'])) === false
		|| ($fileuploaded = $sounds->get_upload('filename')) === false
		|| ($info['dirname'] = $sounds->chk_value('dirname',$info['directory']['dirname'])) === false
		|| ($info['filename'] = $sounds->chk_value('filename',$fileuploaded['name'])) === false)
		{
			if(isset($fileuploaded) === true)
			{
				$fm_save = false;

				if(is_array($fileuploaded) === true)
					xivo_file::rm($fileuploaded['tmp_name']);
			}
		}
		else
		{
			$filename = xivo_file::joinpath($info['dirname'],$fileuploaded['name']);

			if($sounds->add($filename,$fileuploaded['tmp_name']) === true)
			{
				$param['dir'] = $info['dirname'];
				$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
			}
			else
				$fm_save = false;
		}

		$_HTML->set_var('info',$info);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('option',$sounds->get_option());
		break;
	case 'edit':
		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		$dir = $info['dirname'] = $info['directory']['dirname'];

		$param['dir'] = $dir;

		if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['dirname'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

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
				$filename = xivo_file::joinpath($info['file']['dirname'],$info['file']['filename']);
				$newfilename = xivo_file::joinpath($info['dirname'],$info['filename']);

				if($sounds->edit($filename,$newfilename) === true)
				{
					$param['dir'] = $info['dirname'];
					$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
				}
				else
					$fm_save = false;

				$info['filename'] = $info['file']['basename'];
			}
			else
				$fm_save = false;
		}

		$_HTML->set_var('id',$id);
		$_HTML->set_var('info',$info);
		$_HTML->set_var('fm_save',$fm_save);
		break;
	case 'delete':
		$param['dir'] = $dir;
		$param['page'] = $page;

		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

		$file = xivo_file::joinpath($info['directory']['dirname'],$info['file']['filename']);

		$sounds->delete($file);

		$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
		break;
	case 'deletes':
		$param['dir'] = $dir;
		$param['page'] = $page;

		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		if(($values = xivo_issa_val('files',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['file'] = $sounds->get(strval($values[$i]),$info['directory']['dirname'])) === false)
				continue;

			$file = xivo_file::joinpath($info['directory']['dirname'],$info['file']['filename']);

			$sounds->delete($file);
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);
		break;
	case 'download':
		$param['dir'] = $dir;
		$param['page'] = $page;

		$info = array();

		if(($info['directory'] = $sounds->get_dir($dir)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		if(isset($_QR['id']) === false || ($info['file'] = $sounds->get($_QR['id'],$info['directory']['dirname'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

		$file = new xivo_file();

		if(($file->download($info['file']['path'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),$param);

		die();
		break;
	case 'list':
		$total = 0;

		if($dir === '')
			$dir = false;

		if(($dirs = $sounds->get_dir($dir,true,$search)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_services/sounds'),'act=listdir');

		if(($dirs = $dirs['files']) !== false)
		{
			$total = count($dirs);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'name'));
			usort($dirs,array(&$sort,'strnat_usort'));
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$dirs);
		$_HTML->set_var('search',$search);
		break;
	case 'listdir':
	default:
		$act = 'listdir';
		$param['act'] = 'listdir';

		$total = 0;

		if(($dirs = $sounds->get_list_dirs_files()) !== false)
		{
			$total = count($dirs);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('key' => 'dirname'));
			usort($dirs,array(&$sort,'strnat_usort'));
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$dirs);
		break;
}

$_HTML->set_var('act',$act);
$_HTML->set_var('dir',$dir);
$_HTML->set_var('list_dirs',$list_dirs);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_services/sounds');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/sounds/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
