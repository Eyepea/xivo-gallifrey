<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$group = isset($_QR['group']) === true ? strval($_QR['group']) : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$agent = &$ipbx->get_module('agent');
$afeatures = &$ipbx->get_module('agentfeatures');
$agroup = &$ipbx->get_module('agentgroup');

if(($list_grps = $ipbx->get_agent_groups_list()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'agroup','key' => 'name'));
	usort($list_grps,array(&$sort,'str_usort'));
}

$_HTML->set_var('list_grps',$list_grps);

$param = array();
$param['act'] = 'list';

$info = $result = array();

switch($act)
{
	case 'add':
		$add = true;
		$result = null;
		$agent_slt = $agent_unslt = array();

		if(($agents = $ipbx->get_agents_list()) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'sort','key' => 'identity'));
			usort($agents,array(&$sort,'str_usort'));
		}

		$qmember = &$ipbx->get_module('queuemember');

		$qmember_unslt = false;

		$appqueue = &$ipbx->get_application('queue',null,false);

		if(($queues = $appqueue->get_queues_list(null,array('name' => SORT_ASC))) !== false)
		{
			$qmember_unslt = array();

			$nb = count($queues);

			for($i = 0;$i < $nb;$i++)
			{
				$name = &$queues[$i]['queue']['name'];
				$qmember_unslt[$name] = $name;
			}

			if(empty($qmember_unslt) === true)
				$qmember_unslt = $queues = false;
		}

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('agroup',$_QR) === false)
				break;

			$result = array();

			$_QR['agroup']['groupid'] = 0;

			if(($result['agroup'] = $agroup->chk_values($_QR['agroup'])) === false)
			{
				$add = false;
				$result['agroup'] = $agroup->get_filter_result();
			}

			if($agents !== false && ($arr_agent = xivo_issa_val('agent-select',$_QR)) !== false)
			{
				$nb = count($agents);

				for($i = 0;$i < $nb;$i++)
				{
					$ref = &$agents[$i];	

					$agent_info = array();
					$agent_info['sort'] = &$ref['sort'];
					$agent_info['agentid'] = $ref['afeatures']['agentid'];
					$agent_info['numgroup'] = $ref['afeatures']['numgroup'];

					if(in_array($ref['afeatures']['id'],$arr_agent) === false)
					{
						$agent_unslt[] = $agent_info;
						continue;
					}

					$agent_slt[$ref['afeatures']['id']] = $agent_info;
				}
			}

			$aqueue_where = array(
					'usertype' => 'agent',
					'userid' => 0,
					'category' => 'group');

			$queue_add = $queue_tmp = array();

			if(($queue_slt = xivo_issa_val('queue-select',$_QR)) !== false && xivo_issa('queue',$_QR) !== false)
			{
				$nb = count($queue_slt);
				$aqueue_info = $aqueue_where;
				$aqueue_info['call-limit'] = 0;
				$aqueue_info['channel'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;

				for($i = 0;$i < $nb;$i++)
				{
					$qname = &$queue_slt[$i];

					if(isset($queue_tmp[$qname]) === true)
						continue;

					$aqueue_info['queue_name'] = $qname;
					$aqueue_info['interface'] = $ipbx->mk_agent_interface(0,true);

					$aqueue_tmp = array_merge($_QR['queue'][$qname],$aqueue_info);

					if(($qinfo = $qmember->chk_values($aqueue_tmp)) !== false)
					{
						$queue_tmp[$qname] = 1;
						$queue_add[] = $qinfo;
					}
				}
			}

			if($add === false || ($agroupid = $agroup->add($result['agroup'])) === false)
				break;

			if(($group = $agent->chk_value('group',$agroupid)) === false
			|| ($groupid = $agent->add_group($group)) === false)
			{
				$agroup->delete($agroupid);
				break;
			}

			if($agroup->edit($agroupid,array('groupid' => $groupid)) === false)
			{
				$agroup->delete($agroupid);
				$agent->delete($groupid);
				break;
			}

			if(empty($agent_slt) === false)
			{
				$nb = count($arr_agent);

				for($i = 0;$i < $nb;$i++)
				{
					$agent_id = &$arr_agent[$i];

					if(isset($agent_slt[$agent_id]) === false)
						continue;

					$ref = &$agent_slt[$agent_id];

					if($afeatures->edit($agent_id,array('numgroup' => $agroupid)) === false)
						continue;

					if($agent->edit_agent($ref['agentid'],array('group' => $agroupid)) === false)
						$afeatures->edit($agent_id,array('numgroup' => $ref['numgroup']));
				}
			}

			if(($nb = count($queue_add)) !== 0)
			{
				for($i = 0;$i < $nb;$i++)
				{
					$queue_add[$i]['userid'] = $agroupid;

					if(($queue_add[$i]['interface'] = $ipbx->mk_agent_interface($agroupid,true)) === false)
						continue;

					$qmember->add($queue_add[$i]);
				}
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);
		}
		while(false);

		$element = array();
		$element['agroup'] = $agroup->get_element();
		$element['qmember'] = $qmember->get_element();

		if(isset($agent_unslt[0]) === false && empty($agent_slt) === true)
			$agent_unslt = $agents;

		$_HTML->set_var('queues',$queues);
		$_HTML->set_var('qmember_slt',false);
		$_HTML->set_var('qmember_unslt',$qmember_unslt);
		$_HTML->set_var('agents',($agents !== false));
		$_HTML->set_var('agent_slt',$agent_slt);
		$_HTML->set_var('agent_unslt',$agent_unslt);
		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$edit = true;

		$return = &$info;

		if(($info['agroup'] = $agroup->get($group)) === false
		|| ($info['agent'] = $agent->get($info['agroup']['groupid'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);

		$group = $info['agroup']['id'];

		$agent_slt = $agent_unslt = array();

		if(($agents = $ipbx->get_agents_list()) !== false)
		{
			$nb = count($agents);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'sort','key' => 'var_metric'));
			usort($agents,array(&$sort,'num_usort'));

			for($i = 0;$i < $nb;$i++)
			{
				$ref = &$agents[$i];	

				$agent_info = array();
				$agent_info['sort'] = &$ref['sort'];
				$agent_info['agentid'] = $ref['afeatures']['agentid'];
				$agent_info['numgroup'] = $ref['afeatures']['numgroup'];

				if((int) $agents[$i]['afeatures']['numgroup'] !== (int) $group)
				{
					$agent_info['afeaturesid'] = $ref['afeatures']['id'];
					$agent_unslt[] = $agent_info;
					continue;
				}

				$agent_slt[$ref['afeatures']['id']] = $agent_info;
			}
		}

		$qmember = &$ipbx->get_module('queuemember');

		$qmember_slt = $qmember_unslt = false;

		if(($queues = $ipbx->get_group_agent($info['agroup']['id'])) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'queue','key' => 'name'));
			usort($queues,array(&$sort,'str_usort'));

			$qmember_slt = $qmember_unslt = array();

			$nb = count($queues);

			for($i = 0;$i < $nb;$i++)
			{
				$name = &$queues[$i]['queue']['name'];

				if($queues[$i]['member'] !== false)
					$qmember_slt[$name] = $queues[$i]['member'];
				else
					$qmember_unslt[$name] = $name;
			}

			if(empty($qmember_slt) === true)
				$qmember_slt = false;

			if(empty($qmember_unslt) === true)
			{
				$qmember_unslt = false;

				if($qmember_slt === false)
					$queues = false;
			}
		}

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('agroup',$_QR) === false)
				break;

			$_QR['agroup']['groupid'] = $info['agent']['id'];

			$return = &$result;

			if(($result['agroup'] = $agroup->chk_values($_QR['agroup'])) === false)
			{
				$edit = false;
				$result['agroup'] = $agroup->get_filter_result();
			}

			$agent_slt = $agent_unslt = array();

			if($agents !== false)
			{
				if(($arr_agent = xivo_issa_val('agent-select',$_QR)) === false)
					$arr_agent = array();

				$nb = count($agents);

				for($i = 0;$i < $nb;$i++)
				{
					$ref = &$agents[$i];	

					$agent_info = array();
					$agent_info['sort'] = &$ref['sort'];
					$agent_info['agentid'] = $ref['afeatures']['agentid'];
					$agent_info['numgroup'] = $ref['afeatures']['numgroup'];

					if(in_array($ref['afeatures']['id'],$arr_agent) === false)
					{
						$agent_info['afeaturesid'] = $ref['afeatures']['id'];
						$agent_unslt[] = $agent_info;
						continue;
					}

					$agent_slt[$ref['afeatures']['id']] = $agent_info;
				}
			}

			$aqueue_where = array(
					'usertype' => 'agent',
					'userid' => $info['agroup']['id'],
					'category' => 'group');

			$edit_queue = false;
			$queue_add = $queue_edit = $queue_del = $queue_tmp = array();

			if(($queue_slt = xivo_issa_val('queue-select',$_QR)) !== false && xivo_issa('queue',$_QR) !== false)
			{
				$nb = count($queue_slt);

				$aqueue_info = $aqueue_where;
				$aqueue_info['call-limit'] = 0;
				$aqueue_info['channel'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;
				$aqueue_info['interface'] = $ipbx->mk_agent_interface($info['agroup']['id'],true);

				for($i = 0;$i < $nb;$i++)
				{
					$qname = &$queue_slt[$i];
					$aqueue_info['queue_name'] = $qname;

					if(isset($queue_tmp[$qname]) === true)
					{
						if(isset($qmember_slt[$qname]) === true)
						{
							$edit_queue = true;
							$queue_tmp[$qname] = 1;
							$queue_del[] = $aqueue_info;
						}
						continue;
					}

					if(isset($qmember_unslt[$qname]) === true)
						$ref_queue = &$queue_add;
					else if(isset($qmember_slt[$qname]) === true)
						$ref_queue = &$queue_edit;
					else
						continue;

					$aqueue_tmp = array_merge($_QR['queue'][$qname],$aqueue_info);

					if(($qinfo = $qmember->chk_values($aqueue_tmp)) !== false)
					{
						$edit_queue = true;
						$queue_tmp[$qname] = 1;
						$ref_queue[] = $qinfo;
					}
				}
			}

			if($qmember_slt !== false)
			{
				$queue_slt = array_keys($qmember_slt);
				$aqueue_info = $aqueue_where;

				$nb = count($queue_slt);

				for($i = 0;$i < $nb;$i++)
				{
					$qname = &$queue_slt[$i];
					$aqueue_info['queue_name'] = $qname;

					if(isset($queue_tmp[$qname]) === true)
						continue;

					$edit_queue = true;
					$queue_del[] = $aqueue_info;
				}
			}

			if($edit === false || $agroup->edit($info['agroup']['id'],$result['agroup']) === false)
				break;

			if(isset($agent_unslt[0]) === true && ($defgroup = $agroup->get_defgroup()) !== false)
			{
				$nb = count($agent_unslt);

				for($i = 0;$i < $nb;$i++)
				{
					$ref = &$agent_unslt[$i];

					if((int) $ref['numgroup'] === $defgroup || (int) $ref['numgroup'] !== (int) $info['agroup']['id'])
						continue;

					$agent_id = $ref['agentid'];

					if($afeatures->edit($ref['afeaturesid'],array('numgroup' => $defgroup)) === false)
						continue;

					if($agent->edit_agent($ref['agentid'],array('group' => $defgroup)) === false)
						$afeatures->edit($ref['afeaturesid'],array('numgroup' => $ref['numgroup']));
				}
			}

			if(empty($agent_slt) === false)
			{
				$nb = count($arr_agent);

				$agent_order = array();

				for($i = 0;$i < $nb;$i++)
				{
					$agent_id = &$arr_agent[$i];

					if(isset($agent_slt[$agent_id]) === false)
						continue;

					$ref = &$agent_slt[$agent_id];
					
					if((int) $ref['numgroup'] === (int) $info['agroup']['id'])
					{
						$agent_order[] = $ref['agentid'];
						continue;
					}

					if($afeatures->edit($agent_id,array('numgroup' => $info['agroup']['id'])) === false)
						continue;

					if($agent->edit_agent($ref['agentid'],array('group' => $info['agroup']['id'])) === false)
						$afeatures->edit($agent_id,array('numgroup' => $ref['numgroup']));
					else
						$agent_order[] = $ref['agentid'];
				}

				if(isset($agent_order[0]) === true)
					$agent->order_agent($info['agroup']['id'],$agent_order);
			}
			
			if($edit_queue === true)
			{
				if(($nb = count($queue_del)) !== 0)
				{
					for($i = 0;$i < $nb;$i++)
					{
						$aqueue_where['queue_name'] = $queue_del[$i]['queue_name'];
						$qmember->delete_where($aqueue_where);
					}
				}

				if(isset($queue_add[0]) === true)
					$qmember->add_list($queue_add);

				if(($nb = count($queue_edit)) !== 0)
				{
					for($i = 0;$i < $nb;$i++)
					{
						$aqueue_where['queue_name'] = $queue_edit[$i]['queue_name'];
						$qmember->edit_where($aqueue_where,$queue_edit[$i]);
					}
				}
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);
		}
		while(false);

		$element = array();
		$element['agroup'] = $agroup->get_element();
		$element['qmember'] = $qmember->get_element();

		if(isset($agent_unslt[0]) === false && empty($agent_slt) === true)
			$agent_unslt = $agents;

		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$element);
		$_HTML->set_var('agents',($agents !== false));
		$_HTML->set_var('agent_slt',$agent_slt);
		$_HTML->set_var('agent_unslt',$agent_unslt);
		$_HTML->set_var('queues',$queues);
		$_HTML->set_var('qmember_slt',$qmember_slt);
		$_HTML->set_var('qmember_unslt',$qmember_unslt);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		if(($info['agroup'] = $agroup->get($group)) === false
		|| ($info['agent'] = $agent->get($info['agroup']['groupid'])) === false
		|| $info['agroup']['deletable'] === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);

		do
		{
			$qmember = &$ipbx->get_module('queuemember');
			$aqueue_where = array('usertype' => 'agent');

			if(($agents = $ipbx->get_agents_list($info['agroup']['id'])) !== false)
			{
				$aqueue_where['category'] = 'queue';

				$nb = count($agents);

				for($i = 0;$i < $nb;$i++)
				{
					$ref_agent = &$agents[$i];

					if($afeatures->delete($ref_agent['afeatures']['id']) === false)
						break 2;

					if($agent->delete_agent($ref_agent['agent']['id']) === false)
					{
						$afeatures->add($ref_agent['afeatures'],$ref_agent['afeatures']['id']);
						break 2;
					}

					$aqueue_where['userid'] = $ref_agent['afeatures']['id'];

					$qmember->delete_where($aqueue_where);
				}
			}

			if($agroup->delete($info['agroup']['id']) === false)
				break;

			if($agent->delete($info['agent']['id']) === false)
			{
				$agroup->recover($info['agroup']['id']);
				break;
			}

			$aqueue_where['category'] = 'group';
			$aqueue_where['userid'] = $info['agroup']['id'];

			$qmember->delete_where($aqueue_where);
		}
		while(false);

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'addagent':
		if($list_grps === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);

		$act = 'addagent';
		$param['act'] = 'listagent';

		$add = true;
		$result = null;

		$sounds = &$ipbx->get_module('sounds');
		$musiconhold = &$ipbx->get_module('musiconhold');
		$qmember = &$ipbx->get_module('queuemember');

		$beep_list = $sounds->get_list('beep',true);
		$goodbye_list = $sounds->get_list('goodbye',true);

		if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
			ksort($moh_list);

		$qmember_unslt = false;

		$appqueue = &$ipbx->get_application('queue',null,false);

		if(($queues = $appqueue->get_queues_list(null,array('name' => SORT_ASC))) !== false)
		{
			$qmember_unslt = array();

			$nb = count($queues);

			for($i = 0;$i < $nb;$i++)
			{
				$name = &$queues[$i]['queue']['name'];
				$qmember_unslt[$name] = $name;
			}

			if(empty($qmember_unslt) === true)
				$qmember_unslt = $queues = false;
		}

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('afeatures',$_QR) === false
			|| xivo_issa('agent',$_QR) === false || isset($_QR['afeatures']['numgroup']) === false
			|| ($info['agroup'] = $agroup->get($_QR['afeatures']['numgroup'])) === false)
				break;

			$group = $param['group'] = $info['agroup']['id'];

			$result = array();

			if($moh_list === false || isset($_QR['agent']['musiconhold'],$moh_list[$_QR['agent']['musiconhold']]) === false)
				$_QR['agent']['musiconhold'] = '';

			if($beep_list === false || isset($_QR['agent']['custom_beep'],$beep_list[$_QR['agent']['custom_beep']]) === false)
				$_QR['agent']['custom_beep'] = '';

			if($goodbye_list === false || isset($_QR['agent']['goodbye'],$goodbye_list[$_QR['agent']['goodbye']]) === false)
				$_QR['agent']['goodbye'] = '';

			$_QR['afeatures']['agentid'] = 0;

			if(($result['afeatures'] = $afeatures->chk_values($_QR['afeatures'])) === false)
			{
				$add = false;
				$result['afeatures'] = $afeatures->get_filter_result();
			}

			$agentval = $result['afeatures']['number'].',';

			if($result['afeatures']['passwd'] !== '')
				$agentval .= $result['afeatures']['passwd'];

			if($result['afeatures']['firstname'] !== '' || $result['afeatures']['lastname'] !== '')
				$agentval .= ','.$result['afeatures']['firstname'].' '.$result['afeatures']['lastname'];

			$_QR['agent']['agent'] = rtrim($agentval,',');
			$_QR['agent']['group'] = $result['afeatures']['numgroup'];

			if(($result['agent'] = $agent->chk_values($_QR['agent'])) === false)
			{
				$add = false;
				$result['agent'] = $agent->get_filter_result();
			}

			$aqueue_where = array(
					'usertype' => 'agent',
					'userid' => 0,
					'category' => 'queue');

			$queue_add = $queue_tmp = array();

			if(($queue_slt = xivo_issa_val('queue-select',$_QR)) !== false && xivo_issa('queue',$_QR) !== false)
			{
				$nb = count($queue_slt);
				$aqueue_info = $aqueue_where;
				$aqueue_info['call-limit'] = 0;
				$aqueue_info['channel'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;

				for($i = 0;$i < $nb;$i++)
				{
					$qname = &$queue_slt[$i];

					if(isset($queue_tmp[$qname]) === true)
						continue;

					$aqueue_info['queue_name'] = $qname;
					$aqueue_info['interface'] = $ipbx->mk_agent_interface($result['afeatures']['number']);

					$aqueue_tmp = array_merge($_QR['queue'][$qname],$aqueue_info);

					if(($qinfo = $qmember->chk_values($aqueue_tmp)) === false)
						continue;

					$queue_tmp[$qname] = 1;
					$queue_add[] = $qinfo;
				}
			}

			if($add === false || ($agentid = $agent->add_agent($result['agent'])) === false)
				break;

			$result['afeatures']['agentid'] = $agentid;

			if(($afeaturesid = $afeatures->add($result['afeatures'])) === false)
			{
				$agent->delete_agent($agentid);
				break;
			}

			if(($nb = count($queue_add)) !== 0)
			{
				for($i = 0;$i < $nb;$i++)
				{
					$queue_add[$i]['userid'] = $afeaturesid;
					$qmember->add($queue_add[$i]);
				}
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);
		}
		while(false);

		$element = array();
		$element['afeatures'] = $afeatures->get_element();
		$element['agent'] = $agent->get_element();
		$element['qmember'] = $qmember->get_element();

		$_HTML->set_var('info',$result);
		$_HTML->set_var('element',$element);
		$_HTML->set_var('queues',$queues);
		$_HTML->set_var('qmember_slt',false);
		$_HTML->set_var('qmember_unslt',$qmember_unslt);
		$_HTML->set_var('moh_list',$moh_list);
		$_HTML->set_var('beep_list',$beep_list);
		$_HTML->set_var('goodbye_list',$goodbye_list);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'editagent':
		if($list_grps === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);

		$return = &$info;

		if(($info['agroup'] = $agroup->get($group)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);

		$group = $param['group'] = $info['agroup']['id'];

		$act = 'editagent';
		$param['act'] = 'listagent';

		$edit = true;

		if(isset($_QR['id']) === false
		|| ($info['afeatures'] = $afeatures->get($_QR['id'])) === false
		|| ($info['agent'] = $agent->get_agent($info['afeatures']['agentid'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);

		$id = $info['afeatures']['id'];
		$info['agent']['group'] = $info['afeatures']['numgroup'];

		$sounds = &$ipbx->get_module('sounds');
		$musiconhold = &$ipbx->get_module('musiconhold');
		$qmember = &$ipbx->get_module('queuemember');

		$beep_list = $sounds->get_list('beep',true);
		$goodbye_list = $sounds->get_list('goodbye',true);

		if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
			ksort($moh_list);

		$qmember_slt = $qmember_unslt = false;

		if(($queues = $ipbx->get_queue_agent($info['afeatures']['id'])) !== false)
		{
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'qfeatures','key' => 'name'));
			usort($queues,array(&$sort,'str_usort'));

			$qmember_slt = $qmember_unslt = array();

			$nb = count($queues);

			for($i = 0;$i < $nb;$i++)
			{
				$name = &$queues[$i]['queue']['name'];

				if($queues[$i]['member'] !== false)
					$qmember_slt[$name] = $queues[$i]['member'];
				else
					$qmember_unslt[$name] = $name;
			}

			if(empty($qmember_slt) === true)
				$qmember_slt = false;

			if(empty($qmember_unslt) === true)
			{
				$qmember_unslt = false;

				if($qmember_slt === false)
					$queues = false;
			}
		}
		
		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('agent',$_QR) === false || xivo_issa('afeatures',$_QR) === false)
				break;

			$return = &$result;

			if($moh_list === false || isset($_QR['agent']['musiconhold'],$moh_list[$_QR['agent']['musiconhold']]) === false)
				$_QR['agent']['musiconhold'] = '';

			if($beep_list === false || isset($_QR['agent']['custom_beep'],$beep_list[$_QR['agent']['custom_beep']]) === false)
				$_QR['agent']['custom_beep'] = '';

			if($goodbye_list === false || isset($_QR['agent']['goodbye'],$goodbye_list[$_QR['agent']['goodbye']]) === false)
				$_QR['agent']['goodbye'] = '';

			$_QR['afeatures']['agentid'] = $info['afeatures']['agentid'];

			if(($result['afeatures'] = $afeatures->chk_values($_QR['afeatures'])) === false)
			{
				$edit = false;
				$result['afeatures'] = $afeatures->get_filter_result();
			}

			$agentval = $result['afeatures']['number'].',';

			if($result['afeatures']['passwd'] !== '')
				$agentval .= $result['afeatures']['passwd'];

			if($result['afeatures']['firstname'] !== '' || $result['afeatures']['lastname'] !== '')
				$agentval .= ','.$result['afeatures']['firstname'].' '.$result['afeatures']['lastname'];

			$_QR['agent']['agent'] = rtrim($agentval,',');
			$_QR['agent']['group'] = $result['afeatures']['numgroup'];

			if(($result['agent'] = $agent->chk_values($_QR['agent'])) === false)
			{
				$edit = false;
				$result['agent'] = $agent->get_filter_result();
			}

			if((int) $result['afeatures']['numgroup'] === (int) $info['afeatures']['numgroup'])
				unset($result['agent']['group']);

			$aqueue_where = array(
					'usertype' => 'agent',
					'userid' => $info['afeatures']['id'],
					'category' => 'queue');

			$edit_queue = false;
			$queue_add = $queue_edit = $queue_del = $queue_tmp = array();

			if(($queue_slt = xivo_issa_val('queue-select',$_QR)) !== false && xivo_issa('queue',$_QR) !== false)
			{
				$nb = count($queue_slt);
				$aqueue_info = $aqueue_where;
				$aqueue_info['call-limit'] = 0;
				$aqueue_info['channel'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;

				for($i = 0;$i < $nb;$i++)
				{
					$qname = &$queue_slt[$i];
					$aqueue_info['queue_name'] = $qname;

					if(isset($queue_tmp[$qname]) === true)
					{
						if(isset($qmember_slt[$qname]) === true)
						{
							$edit_queue = true;
							$queue_tmp[$qname] = 1;
							$queue_del[] = $aqueue_info;
						}
						continue;
					}

					if(isset($qmember_unslt[$qname]) === true)
						$ref_queue = &$queue_add;
					else if(isset($qmember_slt[$qname]) === true)
						$ref_queue = &$queue_edit;
					else
						continue;

					$aqueue_info['interface'] = $ipbx->mk_agent_interface($result['afeatures']['number']);

					$aqueue_tmp = array_merge($_QR['queue'][$qname],$aqueue_info);

					if(($qinfo = $qmember->chk_values($aqueue_tmp)) === false)
						continue;

					$edit_queue = true;
					$queue_tmp[$qname] = 1;
					$ref_queue[] = $qinfo;
				}
			}

			if($qmember_slt !== false)
			{
				$queue_slt = array_keys($qmember_slt);
				$aqueue_info = $aqueue_where;

				$nb = count($queue_slt);

				for($i = 0;$i < $nb;$i++)
				{
					$qname = &$queue_slt[$i];
					$aqueue_info['queue_name'] = $qname;

					if(isset($queue_tmp[$qname]) === true)
						continue;

					$edit_queue = true;
					$queue_del[] = $aqueue_info;
				}
			}

			if($edit === false || $afeatures->edit($info['afeatures']['id'],$result['afeatures']) === false)
				break;

			if($agent->edit_agent($info['agent']['id'],$result['agent']) === false)
			{
				$afeatures->edit_origin();
				break;
			}

			if($edit_queue === true)
			{
				if(isset($queue_add[0]) === true)
					$qmember->add_list($queue_add);

				if(($nb = count($queue_edit)) !== 0)
				{
					for($i = 0;$i < $nb;$i++)
					{
						$aqueue_where['queue_name'] = $queue_edit[$i]['queue_name'];
						$qmember->edit_where($aqueue_where,$queue_edit[$i]);
					}
				}

				if(($nb = count($queue_del)) !== 0)
				{
					for($i = 0;$i < $nb;$i++)
					{
						$aqueue_where['queue_name'] = $queue_del[$i]['queue_name'];
						$qmember->delete_where($aqueue_where);
					}
				}
			}

			$param['group'] = $result['afeatures']['numgroup'];

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);
		}
		while(false);

		$element = array();
		$element['afeatures'] = $afeatures->get_element();
		$element['agent'] = $agent->get_element();
		$element['qmember'] = $qmember->get_element();

		$_HTML->set_var('id',$id);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('element',$element);
		$_HTML->set_var('queues',$queues);
		$_HTML->set_var('qmember_slt',$qmember_slt);
		$_HTML->set_var('qmember_unslt',$qmember_unslt);
		$_HTML->set_var('moh_list',$moh_list);
		$_HTML->set_var('beep_list',$beep_list);
		$_HTML->set_var('goodbye_list',$goodbye_list);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'deleteagent':
		if(($info['agroup'] = $agroup->get($group)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),'act=list');

		$param['act'] = 'listagent';
		$param['page'] = $page;
		$param['group'] = $info['agroup']['id'];

		do
		{
			if(isset($_QR['id']) === false
			|| ($info['afeatures'] = $afeatures->get($_QR['id'])) === false
			|| ($info['agent'] = $agent->get_agent($info['afeatures']['agentid'])) === false)
				break;

			if($afeatures->delete($info['afeatures']['id']) === false)
				break;

			if($agent->delete_agent($info['agent']['id']) === false)
			{
				$afeatures->add_origin();
				break;
			}

			$qmember = &$ipbx->get_module('queuemember');

			$aqueue_where = array('usertype'	=> 'agent',
					      'userid'		=> $info['afeatures']['id'],
					      'category'	=> 'queue');

			$qmember->delete_where($aqueue_where);
		}
		while(false);

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);
		break;
	case 'listagent':
		if(($info['agroup'] = $agroup->get($group)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/agents'),$param);

		$total = 0;
		$act = $param['act'] = 'listagent';

		if(($agents = $ipbx->get_agents_list($info['agroup']['id'])) !== false)
		{
			$total = count($agents);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'sort','key' => 'var_metric'));
			usort($agents,array(&$sort,'num_usort'));
		}

		$_HTML->set_var('group',$info['agroup']['id']);
		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$agents);
		break;
	default:
		$act = 'list';
		$total = 0;

		if($list_grps !== false)
			$total = count($list_grps);

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
}

$_HTML->set_var('act',$act);
$_HTML->set_var('group',$group);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/agents/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
