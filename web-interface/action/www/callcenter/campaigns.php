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

$act 	  	= isset($_QR['act']) === true ? $_QR['act'] : '';
$page 		= isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;
$search 	= isset($_QR['search']) === true ? strval($_QR['search']) : '';
$context 	= isset($_QR['context']) === true ? strval($_QR['context']) : '';

$param = array();
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;
else if($context !== '')
	$param['context'] = $context;

$contexts = false;
$error = false;
$param['act'] = 'list';

$campmod = &$ipbx->get_module('campaign_campaign');
$filtermod = &$ipbx->get_module('campaign_campaign_filter');
$fm_save = $error = null;

switch($act)
{
	case 'add':
		$fm_save = true;

		if(isset($_QR['fm_send']) 	 === true 
		&& dwho_issa('campaign', $_QR) === true)
		{	
			if(!isset($_QR['campaign_occasional']))
				$_QR['campaign']['end'] = null;

			if((isset($_QR['campaign_occasional']) && !is_null($_QR['campaign']['end']) || !isset($_QR['campaign_occasional'])) === true
			&& ($campaign = $campmod->chk_values($_QR['campaign']))!== false
			&& ($id = $campmod->add($campaign)) !== false)
			{
				foreach($_QR['agents'] as $agent)
					$filtermod->add(array(
						'campaign_id' => $id,
						'type'        => 'agent',
						'value'       => $agent
						));

				foreach($_QR['queues'] as $queue)
					$filtermod->add(array(
						'campaign_id' => $id,
						'type'        => 'queue',
						'value'       => $queue
					));

				if(strlen($_QR['skills']) > 0)
					$filtermod->add(array(
						'campaign_id' => $id,
						'type'        => 'skill',
						'value'       => $_QR['skills']
					));

				$way = null;
				if($_QR['way']['in'] == 1 && $_QR['way']['out'] == 1)
					$way = 'both';
				else if($_QR['way']['in'] == 1)
					$way = 'in';
				else if($_QR['way']['out'] == 1)
					$way = 'out';

				if(!is_null($way))
					$filtermod->add(array(
						'campaign_id' => $id,
						'type'        => 'way',
						'value'       => $way
					));

				$_QRY->go($_TPL->url('callcenter/campaigns'), $param);
				break;
			}

			$fm_save = false;
			$error = $campaign===false?$campmod->get_filter_error():array('name' => 'record_error');
			if(isset($_QR['campaign_occasional']) && strlen($_QR['campaign']['end']) == 0)
			{ $error['end'] = 'empty'; }

			$info  = array(
				'campaign' => $_QR['campaign'], 
				'agents' => array(), 
				'queues' => array(),
				'filters' => array('way' => $_QR['way'])
			);

			if(isset($_QR['campaign_occasional']))
			{ $info['campaign']['occasional'] = true ; }
		}

		$appagent = &$ipbx->get_application('agent',null,false);
		$agents  = array();
		foreach($appagent->get_agents_list(null,null,null,null,true) as $agent)
		{
			if(in_array($agent['agentfeatures']['id'], $_QR['agents']))
			{ $info['agents'][$agent['agentfeatures']['id']] = $agent['agentfeatures']; }
			else
			{ $agents[$agent['agentfeatures']['id']] = $agent['agentfeatures']; }
		}

		$appqueue = &$ipbx->get_application('queue',null,false);
		$queues  = array();
		foreach($appqueue->get_queues_list(null,null,null,null,true) as $queue)
		{
			if(in_array($queue['id'], $_QR['queues']))
			{ $info['queues'][$queue['id']] = $queue; }
			else
			{ $queues[$queue['id']] = $queue; }
		}

		$_TPL->set_var('agents',array_values($agents));
		$_TPL->set_var('queues',array_values($queues));
		$_TPL->set_var('fm_save', $fm_save);
		break;

	case 'edit':
		$fm_save  = true;

		if(isset($_QR['fm_send']) 		=== true
		&& dwho_issa('campaign', $_QR) 	=== true)
		{
			if(!isset($_QR['campaign_occasional']) || strlen($_QR['campaign']['end']) == 0)
				$_QR['campaign']['end'] = null;

			if((isset($_QR['campaign_occasional']) && !is_null($_QR['campaign']['end']) || !isset($_QR['campaign_occasional'])) === true
			&& ($campaign = $campmod->chk_values($_QR['campaign']))!== false
			&& $campmod->edit($_QR['id'], $campaign) !== false)
			{
				$filtermod->delete_where(array('campaign_id' => $_QR['id']));
				foreach($_QR['agents'] as $agent)
					$filtermod->add(array(
						'campaign_id' => $_QR['id'],
						'type'        => 'agent',
						'value'       => $agent
					));

				foreach($_QR['queues'] as $queue)
					$filtermod->add(array(
						'campaign_id' => $_QR['id'],
						'type'        => 'queue',
						'value'       => $queue
					));

				if(strlen($_QR['skills']) > 0)
					$filtermod->add(array(
						'campaign_id' => $_QR['id'],
						'type'        => 'skill',
						'value'       => $_QR['skills']
					));

				$way = null;
				if($_QR['way']['in'] == 1 && $_QR['way']['out'] == 1)
					$way = 'both';
				else if($_QR['way']['in'] == 1)
					$way = 'in';
				else if($_QR['way']['out'] == 1)
					$way = 'out';

				if(!is_null($way))
					$filtermod->add(array(
						'campaign_id' => $_QR['id'],
						'type'        => 'way',
						'value'       => $way
					));
	
				$_QRY->go($_TPL->url('callcenter/campaigns'), $param);
			}

			$fm_save = false;
			if(isset($_QR['campaign_occasional']) && is_null($_QR['campaign']['end']))
			{ $error = array('end' => 'empty'); }
			else
			{
				$error = $campaign===false?$campmod->get_filter_error():array('name' => 'record_error');
			}
		
			$_QR['campaign']['id'] = $_QR['id'];	
			$info  = array(
				'campaign' => $_QR['campaign'], 
				'agents' => array(), 
				'queues' => array(),
				'skills' => $_QR['skills'],
				'filters' => array('way' => $_QR['way'])
			);

			if(isset($_QR['campaign_occasional']))
			{ $info['campaign']['occasional'] = true ; }

		} else {
			$info = array(
				'campaign' => $campmod->get($_QR['id']),
				'filters'  => array(
						'agents' => array(),
						'queues' => array(),
						'skills' => '',	
						'way'    => array('in' => false, 'out' => false)
					)
			);
		}

		$appagent = &$ipbx->get_application('agent',null,false);
		$agents  = array();
		foreach($appagent->get_agents_list(null,null,null,null,true) as $agent)
		{
			if(!$fm_save && in_array($agent['agentfeatures']['id'], $_QR['agents']))
			{ $info['agents'][$agent['agentfeatures']['id']] = $agent['agentfeatures']; }
			else
			{ $agents[$agent['agentfeatures']['id']] = $agent['agentfeatures']; }
		}

		$appqueue = &$ipbx->get_application('queue',null,false);
		$queues  = array();
		foreach($appqueue->get_queues_list(null,null,null,null,true) as $queue)
		{
			if(!$fm_save && in_array($queue['id'], $_QR['queues']))
			{ $info['queues'][$queue['id']] = $queue; }
			else
			{ $queues[$queue['id']] = $queue; }
		}


		if($fm_save)
		{
			foreach($filtermod->get_all_where(array('campaign_id' => $_QR['id'])) as $filter)
			{
				switch($filter['type'])
				{
					case 'way':
						if($filter['value'] == 'in' || $filter['value'] == 'both')
							$info['filters']['way']['in'] = true;
						if($filter['value'] == 'out' || $filter['value'] == 'both')
							$info['filters']['way']['out'] = true;
						break;

					case 'agent':
						$info['agents'][] = $agents[$filter['value']];
						unset($agents[$filter['value']]);
						break;

					case 'skill':
						$info['filters']['skills'] = $filter['value'];
						break;

					case 'queue':
						$info['queues'][] = $queues[$filter['value']];
						unset($queues[$filter['value']]);
						break;
				}
			}
		}

		$_TPL->set_var('agents',array_values($agents));
		$_TPL->set_var('queues',array_values($queues));
		$_TPL->set_var('fm_save' , $fm_save);
		break;

	case 'delete':
		if(isset($_QR['id']) && $_QR['id'] != 'notag')
		{
			$filtermod->delete_where(array('campaign_id' => $_QR['id']));
			$campmod->delete($_QR['id']);
		}

		$_QRY->go($_TPL->url('callcenter/campaigns'),$param);
		break;

	case 'deletes':
		// delete multiple items
		$param['page'] = $page;

		if(($values = dwho_issa_val('campaigns',$_QR)) === false)
			$_QRY->go($_TPL->url('callcenter/campaigns'),$param);

		$nb = count($values);

		for($i = 0; $i < $nb; $i++)
		{
			$filtermod->delete_where(array('campaign_id' => $values[$i]));
			$campmod->delete($values[$i]);
		}

		$_QRY->go($_TPL->url('callcenter/campaigns'), $param);
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

		$list	  	= $campmod->get_all(null,true,$search, $limit);
		$total		= $campmod->get_cnt($search);

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('callcenter/campaigns'),$param);
		}

		$_TPL->set_var('pager'	, dwho_calc_page($page, $nbbypage, $total));
		$_TPL->set_var('list'		, $list);
		$_TPL->set_var('search'	, $search);
}


$element = $campmod->get_element();

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');
$dhtml->set_js('js/callcenter/campaigns.js');
$dhtml->set_js('js/jquery.js');
$dhtml->set_js('js/datetimepicker/jquery_ui_datepicker/jquery_ui_datepicker.js');
//$dhtml->set_js('js/jquery-ui-datetimepicker.js');
$dhtml->set_js('js/datetimepicker/jquery_ui_datepicker/i18n/ui.datepicker-fr.js');
$dhtml->set_js('js/datetimepicker/jquery_ui_datepicker/timepicker_plug/timepicker.js');
$dhtml->set_css('js/datetimepicker/jquery_ui_datepicker/timepicker_plug/css/style.css');
$dhtml->set_css('js/datetimepicker/jquery_ui_datepicker/smothness/jquery_ui_datepicker.css');

$_TPL->set_var('act',$act);
$_TPL->set_var('fm_save', $fm_save);
$_TPL->set_var('info'   , $info);
$_TPL->set_var('error',$error);
$_TPL->set_var('element', $element);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/callcenter/menu');
$menu->set_toolbar('toolbar/callcenter/campaigns');

$_TPL->set_bloc('main','callcenter/campaigns/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
