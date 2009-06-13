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
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = $result = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_CUSTOM));

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true && xivo_issa('protocol',$_QR) === true)
		{
			if($apptrunk->set_add($_QR) === false
			|| $apptrunk->add() === false)
			{
				$fm_save = false;
				$result = $apptrunk->get_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('element',$apptrunk->get_elements());
		$_HTML->set_var('context_list',$apptrunk->get_context_list());
		break;
	case 'edit':
		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_CUSTOM));

		if(isset($_QR['id']) === false
		|| ($info = $apptrunk->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && xivo_issa('protocol',$_QR) === true)
		{
			$return = &$result;

			if($apptrunk->set_edit($_QR) === false
			|| $apptrunk->edit() === false)
			{
				$fm_save = false;
				$result = $apptrunk->get_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);
		}

		$_HTML->set_var('id',$info['trunkfeatures']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('element',$apptrunk->get_elements());
		$_HTML->set_var('context_list',$apptrunk->get_context_list());
		break;
	case 'delete':
		$param['page'] = $page;

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_CUSTOM));

		if(isset($_QR['id']) === false || $apptrunk->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

		$apptrunk->delete();

		$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('trunks',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_CUSTOM));

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($apptrunk->get($values[$i]) !== false)
				$apptrunk->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('trunks',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_CUSTOM));

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($apptrunk->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$apptrunk->disable();
			else
				$apptrunk->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$apptrunk = &$ipbx->get_application('trunk',
						    array('protocol' => XIVO_SRE_IPBX_AST_PROTO_CUSTOM),
						    false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $apptrunk->get_trunks_list(null,$order,$limit);
		$total = $apptrunk->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/trunk_management/custom');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/trunk_management/custom/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
