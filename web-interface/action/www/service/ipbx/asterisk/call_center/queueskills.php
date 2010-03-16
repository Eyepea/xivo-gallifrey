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

switch($act)
{
	case 'add':
		// add a new item (either display add form (fm_send not set)
		// 	OR save new entered item
		$appqueue = &$ipbx->get_application('queue');
		$fm_save = false;

		// we must save new item ?
		if(isset($_QR['fm_send']) 	 === true 
		&& dwho_issa('queueskill', $_QR) === true)
		{	
			$queueskills = array();

			// reformating skill values array
			// count() -1 because we skip last line (is empty & used to add new empty lines)
			$count = count($_QR['queueskill']['values']['id']) - 1;
			for($i = 0; $i < $count; $i++)
			{
				$queueskills[] = array(
					'id'		=> $_QR['queueskill']['values']['id'][$i], 
					'name'		=> $_QR['queueskill']['values']['name'][$i], 
					'description'	=> $_QR['queueskill']['values']['desc'][$i], 
					'printscreen'	=> $_QR['queueskill']['values']['printscr'][$i]
				);
			}

			// save item
			if($appqueue->skills_setadd($_QR['queueskill']['name'], $queueskills) === false
			|| $appqueue->skills_add($_QR['queueskill']['name']   , $queueskills) === false)
			{
				$error = $appqueue->get_error();

				$_TPL->set_var('info', array('queueskill' => $_QR['queueskill']));
				$_TPL->set_var('data', $queueskills);
				
				$fm_save = false;
			}
			else
			{
				// must reload configuration files
				$ipbx->discuss('module reload app_queue.so');

				$_QRY->go($_TPL->url('service/ipbx/call_center/queueskills'), $param);
			}
		}

		$_TPL->set_var('fm_save', $fm_save);
		break;

	case 'edit':
		$appqueue = &$ipbx->get_application('queue');

		// id not set or skillcat[id] not found => redirect to list view
		if(isset($_QR['id']) === false || ($info = $appqueue->skills_get($_QR['id'])) === false)
		{ $_QRY->go($_TPL->url('service/ipbx/call_center/queueskills'), $param); }
		
		$act = 'edit';
		if(isset($_QR['fm_send']) 		=== true
		&& dwho_issa('queueskill', $_QR) 	=== true)
		{
			$queueskills = array();

			// count() -1 because we skip last line (is empty & used to add new empty lines)
			$count = count($_QR['queueskill']['values']['id']) - 1;
			for($i = 0; $i < $count; $i++)
			{
				$queueskills[] = array(
					'id'		=> $_QR['queueskill']['values']['id'][$i], 
					'name'		=> $_QR['queueskill']['values']['name'][$i], 
					'description'	=> $_QR['queueskill']['values']['desc'][$i], 
					'printscreen'	=> $_QR['queueskill']['values']['printscr'][$i]
				);
			}

			if($appqueue->skills_setedit($_QR['queueskill']['name'], $queueskills) === true
			&& $appqueue->skills_edit($_QR['id'], $_QR['queueskill']['name'], $queueskills))
			{
				// save is ok: redirecting to list view
				$fm_save = false;
				// must reload configuration files
				$ipbx->discuss('module reload app_queue.so');

				$_QRY->go($_TPL->url('service/ipbx/call_center/queueskills'), $param);
			}

			// on update error
			$error = $appqueue->get_error();
			$info['values'] = $queueskills;
		}

		$_TPL->set_var('data', $info['values']);

		$info = array('queueskill' => array('name' => $info['name'], 'id' => $info['id']));
		$_TPL->set_var('info' , $info);
		break;

	case 'delete':
		// delete selected items
		$appqueue = &$ipbx->get_application('queue');

		if(isset($_QR['id']))
			$appqueue->skills_delete($_QR['id']);

		// must reload configuration files
		$ipbx->discuss('module reload app_queue.so');
		$_QRY->go($_TPL->url('service/ipbx/call_center/queueskills'),$param);

		// $ipbx->discuss('xivo[userlist,update]');
		break;

	case 'deletes':
		// delete multiple items
		$param['page'] = $page;

		if(($values = dwho_issa_val('queueskills',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_center/queueskills'),$param);

		$appqueue = &$ipbx->get_application('queue');
		$nb = count($values);

		for($i = 0; $i < $nb; $i++)
		{
			// we delete each value one by one
			$appqueue->skills_delete($values[$i]);
		}

		// must reload configuration files
		$ipbx->discuss('module reload app_queue.so');
		$_QRY->go($_TPL->url('service/ipbx/call_center/queueskills'), $param);
		break;


	case 'list':
	default:
		// list mode :: view all queueskills (modulo the filter)

		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appqueue = &$ipbx->get_application('queue');

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list	  	= $appqueue->skills_getall($search, $limit);
		$total		= $appqueue->skills_count($search);

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/call_center/queueskills'),$param);
		}

		$_TPL->set_var('pager'	, dwho_calc_page($page, $nbbypage, $total));
		$_TPL->set_var('list'		, $list);
		$_TPL->set_var('search'	, $search);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('contexts',$contexts);
$_TPL->set_var('error',$error);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_center/queueskills');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_center/queueskills/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
