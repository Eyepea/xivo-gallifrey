<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$act 		= isset($_QR['act']) === true ? $_QR['act'] : '';
$page 		= isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;
$search 	= isset($_QR['search']) === true ? strval($_QR['search']) : '';
$context 	= isset($_QR['context']) === true ? strval($_QR['context']) : '';

$param = array();
// default view mode == list
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;
else if($context !== '')
	$param['context'] = $context;

$contexts = false;
$error = false;
$param['act'] = 'list';

$tagmod = &$ipbx->get_module('campaign_tag');
$fm_save = $error = null;

switch($act)
{
	case 'add':
		$fm_save = true;

		if(isset($_QR['fm_send']) 	 === true 
		&& dwho_issa('tag', $_QR) === true)
		{	
			// save item
			if(($values = $tagmod->chk_values($_QR['tag'])) === false
			|| $tagmod->add($values) === false)
			{
				$error = $values===false?$tagmod->get_filter_error():array('name' => 'already_set');

				$info = array('tag' => $_QR['tag']);
				$fm_save = false;
			}
			else
			{
				$_QRY->go($_TPL->url('callcenter/tags'), $param);
			}
		}
 
		$_TPL->set_var('fm_save', $fm_save);
		break;

	case 'edit':
		$fm_save  = true;

		if(isset($_QR['fm_send']) 		=== true
		&& dwho_issa('tag', $_QR) 	=== true)
		{
			if(($values = $tagmod->chk_values($_QR['tag'])) !== false
			&& $tagmod->edit($_QR['id'], $values) !== false)
				$_QRY->go($_TPL->url('callcenter/tags'), $param);

			$fm_save = false;

			// on update error
			$error = $values===false?$tagmod->get_filter_error():array('name' => 'already_set');
			$info['tag'] = $_QR['tag'];
		}
		else
		{
			$info = array('tag' => $tagmod->get($_QR['id']));
		}
			
		$_TPL->set_var('fm_save' , $fm_save);
		break;

	case 'delete':
		if(isset($_QR['id']) && $_QR['id'] != 'notag')
			$tagmod->delete($_QR['id']);

		// must reload configuration files
		$_QRY->go($_TPL->url('callcenter/tags'),$param);
		break;

	case 'deletes':
		// delete multiple items
		$param['page'] = $page;

		if(($values = dwho_issa_val('tags',$_QR)) === false)
			$_QRY->go($_TPL->url('callcenter/tags'),$param);

		$nb = count($values);
		for($i = 0; $i < $nb; $i++)
		{
			if($values[$i] == 'notag')
				continue;

			$tagmod->delete($values[$i]);
		}

		$_QRY->go($_TPL->url('callcenter/tags'), $param);
		break;

	case 'list':
	default:
		// list mode :: view all queueskills (modulo the filter)

		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list	  	= $tagmod->get_all(null,true,$search, $limit);
		$total		= $tagmod->get_cnt($search);

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('callcenter/tags'),$param);
		}

		$_TPL->set_var('pager'	, dwho_calc_page($page, $nbbypage, $total));
		$_TPL->set_var('list'		, $list);
		$_TPL->set_var('search'	, $search);
}


$element = $tagmod->get_element();

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');

$_TPL->set_var('act',$act);
$_TPL->set_var('fm_save', $fm_save);
$_TPL->set_var('info'   , $info);
$_TPL->set_var('error',$error);
$_TPL->set_var('element', $element);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/callcenter/menu');
$menu->set_toolbar('toolbar/callcenter/tags');

$_TPL->set_bloc('main','callcenter/tags/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
