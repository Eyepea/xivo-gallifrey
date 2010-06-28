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

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

xivo::load_class('xivo_directories',XIVO_PATH_OBJECT,null,false);
dwho::load_class('dwho_uri');
$uriobject = new dwho_uri();
$_DIR = new xivo_directories();

$param = array();
$param['act'] = 'list';

$result = $fm_save = $error = null;

$prefixes = array('sqlite', 'mysql', 'file', 'phonebook', 'internal');

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			$data = array();
			switch($_QR['type'])
			{
				case 1:
				{
					$uri = "mysql://";
					if($_QR['user'] != '')
						$uri .= $_QR['user'];
					if($_QR['password'] != '')
						$uri .= ":".$_QR['password'];
					if($_QR['user'] != '')
						$uri .= "@";
					$uri .= $_QR['host'] . ":" .$_QR['port'] . "/" .$_QR['dbname'];

					break;
				}
				case 3:
					$uri = $_QR['uri']; break;
				// internal
				case 4:
					$uri = 'internal'; break;
				// phonebook
				case 5:
					$uri = 'phonebook'; break;
				default:
				{
					$uri = $prefixes[$_QR['type']] . "://" . $_QR['uri'];
					break;
				}
			}

			$data['uri']         = $uri;
			$data['eid']         = $_QR['_eid'];
			$data['name']        = $_QR['name'];
			$data['tablename']   = $_QR['tablename'];
			$data['description'] = $_QR['description'];
			$data['dirtype']     = null;

			$result = $_DIR->chk_values($data);
			if(($result = $_DIR->chk_values($data)) === false
			|| $_DIR->add($result)                  === false)
			{
				$fm_save = false;
				$result  = $_DIR->get_filter_result();
				$error   = $_DIR->get_filter_error();
			}
			else 
			{
				$_QRY->go($_TPL->url('xivo/configuration/manage/directories'), $param);
			}
		}

		$info = null;
		$element = $_DIR->get_element();
		$element['type']['default'] = 'sqlite';
		$element['host']['default'] = '';
		$element['port']['default'] = '';
		$element['dbname']['default'] = '';
		$element['user']['default'] = '';
		$element['password']['default'] = '';
		
		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
	
		$_TPL->set_var('info',$info);
		$_TPL->set_var('element',$element);
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_DIR->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/directories'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$data = array();
			switch($_QR['type'])
			{
				case 1:
				{
					$uri = "mysql://";
					if($_QR['user'] != '')
						$uri .= $_QR['user'];
					if($_QR['password'] != '')
						$uri .= ":".$_QR['password'];
					if($_QR['user'] != '')
						$uri .= "@";
					$uri .= $_QR['host'] . ":" .$_QR['port'] . "/" .$_QR['dbname'];

					break;
				}
				case 3:
				{
					$uri = $_QR['uri'];
					break;
				}
				case 4:
					$uri = 'internal'; break;
				// phonebook
				case 5:
					$uri = 'phonebook'; break;
				default:
				{
					$uri = $prefixes[$_QR['type']] . "://" . $_QR['uri'];
					break;
				}
			}

			$data['uri'] = $uri;
			$data['eid'] = $_QR['_eid'];
			$data['name'] = $_QR['name'];
			$data['tablename'] = $_QR['tablename'];
			$data['description'] = $_QR['description'];
			$data['dirtype'] = null;

			if(($result = $_DIR->chk_values($data)) === false
			|| $_DIR->edit($info['id'], $result)    === false)
			{
				$fm_save = false;
				$result = $_DIR->get_filter_result();
				$error = $_DIR->get_filter_error();
			}
			else 
			{
				$_QRY->go($_TPL->url('xivo/configuration/manage/directories'),$param);
			}
		}

		$element = $_DIR->get_element();
		$element['type']['default'] = 'sqlite';
		$element['host']['default'] = '';
		$element['port']['default'] = '';
		$element['dbname']['default'] = '';
		$element['user']['default'] = '';
		$element['password']['default'] = '';

		$return['type'] = $return['host'] = $return['port'] = $return['dbname'] = $return['user'] = $return['password'] = '';

		$parsed = $uriobject->parse_uri($return['uri']);
		$return['type'] = -1;
		if($parsed['path'] == 'internal')
			$return['type'] = 4;
		else if($parsed['path'] == 'phonebook')
			$return['type'] = 5;
		else
		{
			foreach($prefixes as $k => $p)
			{
				if(strcasecmp($parsed['scheme'], $p) == 0)
					$return['type'] = $k;
			}
		}

		# If we can't find type assume it's webservice
		if($return['type'] == -1)
			$return['type'] = 3;

		# Only for MySQL (id 1 in $prefixes)
		if($return['type'] == 1)
		{
			$return['host'] = $parsed['authority']['host'];
			$return['port'] = $parsed['authority']['port'];
			$return['user'] = $parsed['authority']['user'];
			$return['password'] = $parsed['authority']['passwd'];


			$return['dbname'] = $parsed['path'];
			$pattern = '-/-';
			$return['dbname'] = preg_replace($pattern, '', $return['dbname']);
		}

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$_TPL->set_var('id',$info['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('element',$element);
		break;

	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0)
			$_DIR->delete($id);

		$_QRY->go($_TPL->url('xivo/configuration/manage/directories'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = 20;

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $_DIR->get_all(null,true,$order,$limit);
		$total = $_DIR->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('xivo/configuration/manage/directories'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('error',$error);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/directories');

$_TPL->set_bloc('main','xivo/configuration/manage/directories/'.$act);
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
