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

$act		= isset($_QR['act']) === true ? $_QR['act'] : '';
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

switch($act)
{
	case 'add':
		// add a new item (either display add form (fm_send not set)
		// 	OR save new entered item
		$appqueue = &$ipbx->get_application('queue');
		$fm_save = true;
		$error	 = null;

		$rules = array();			

		// we must save new item ?
		if(isset($_QR['fm_send'])		=== true 
		&& dwho_issa('queueskillrule',	$_QR) 	=== true)
		{	
			// extract rule lines
			$rules = array_slice($_QR['queueskillrule']['rule'], 0, 
				count($_QR['queueskillrule']['rule']) - 1);			

			// save item
			if($appqueue->skillrules_save($_QR['queueskillrule']['name'], $rules) === false)
			{
				$info = array('queueskillrule' => $_QR['queueskillrule']);
				$_TPL->set_var('info'		, $info);

				$fm_save = false;
			}
			else
			{
				// must reload configuration files
				$ipbx->discuss('module reload app_queue.so');
				$_QRY->go($_TPL->url('service/ipbx/call_center/queueskillrules'), $param);
			}
		}

		$_TPL->set_var('error'	, $appqueue->get_error());
		$_TPL->set_var('fm_save', $fm_save);
		$_TPL->set_var('rules', $rules);
		break;

	case 'edit':
		$appqueue = &$ipbx->get_application('queue');
	    $fm_save  = true;

		// id not set or skillcat[id] not found => redirect to list view
		if(isset($_QR['id']) === false || ($info = $appqueue->skillrules_getone($_QR['id'])) === false)
		{ $_QRY->go($_TPL->url('service/ipbx/call_center/queueskillrules'), $param); }
		
		$act = 'edit';
		if(isset($_QR['fm_send']) === true)
		{
			$_QR['queueskillrule']['id'] = (int) $_QR['id'];
			$_QR['queueskillrule']['rule'] = array_slice($_QR['queueskillrule']['rule'], 0, 
					count($_QR['queueskillrule']['rule']) - 1);	

			if($appqueue->skillrules_update($_QR['queueskillrule']) === true)
			{
				// must reload configuration files
				$ipbx->discuss('module reload app_queue.so');

				$_QRY->go($_TPL->url('service/ipbx/call_center/queueskillrules'), $param);
			}

			$fm_save = false;
			$info = $_QR['queueskillrule'];
		}
	
		$_TPL->set_var('error', $appqueue->get_error());
		$_TPL->set_var('info', array('queueskillrule' => $info));
		$_TPL->set_var('rules', $info['rule']);
		$_TPL->set_var('fm_save', $fm_save);
		break;

	case 'delete':
		// delete selected items
		$appqueue = &$ipbx->get_application('queue');

		if(isset($_QR['id']))
			$appqueue->skillrules_delete($_QR['id']);

		// must reload configuration files
		$ipbx->discuss('module reload app_queue.so');

		$_QRY->go($_TPL->url('service/ipbx/call_center/queueskillrules'),$param);
		break;

	case 'deletes':
		// delete multiple items
		$param['page'] = $page;

		if(($values = dwho_issa_val('queueskillrules',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_center/queueskillrules'),$param);

		$appqueue = &$ipbx->get_application('queue');
		$nb = count($values);

		for($i = 0; $i < $nb; $i++)
		{
			// we delete each value one by one
			$appqueue->skillrules_delete($values[$i]);
		}

		// must reload configuration files
		$ipbx->discuss('module reload app_queue.so');

		$_QRY->go($_TPL->url('service/ipbx/call_center/queueskillrules'), $param);
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

		$list	  	= $appqueue->skillrules_getall($search, $limit);
		$total		= $appqueue->skillrules_count($search);

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/call_center/queueskillrules'),$param);
		}

		$_TPL->set_var('pager'	, dwho_calc_page($page, $nbbypage, $total));
		$_TPL->set_var('list'	, $list);
		$_TPL->set_var('search'	, $search);
}

$_TPL->set_var('act',$act);
//$_TPL->set_var('error',$error);
$_TPL->set_var('contexts',$contexts);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_center/queueskillrules');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_center/queueskillrules/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
