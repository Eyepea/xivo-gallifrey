<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$queue = &$ipbx->get_module('queue');
$qfeatures = &$ipbx->get_module('queuefeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');
$qmember = &$ipbx->get_module('queuemember');

if(($user_list = $ipbx->get_user_queue_info()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'fullname'));
	uasort($user_list,array(&$sort,'str_usort'));
}

if(($agent_list = $ipbx->get_agent_queue_info()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'fullname'));
	uasort($agent_list,array(&$sort,'str_usort'));
}

if(($agroup_list = $ipbx->get_agent_group_queue_info()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'fullname'));
	uasort($agroup_list,array(&$sort,'str_usort'));
}

$param = array();
$param['act'] = 'list';

$info = $result = $user_slt = $agent_slt = $agroup_slt = array();

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		$sounds = &$ipbx->get_module('sounds');
		$musiconhold = &$ipbx->get_module('musiconhold');
		
		if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
			ksort($moh_list);

		$announce_list = $sounds->get_list('acd',true);

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('qfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
				break;

			if($moh_list === false || isset($_QR['queue']['musiconhold'],$moh_list[$_QR['queue']['musiconhold']]) === false)
				$_QR['queue']['musiconhold'] = '';

			if($announce_list !== false)
			{
				if(isset($_QR['queue']['announce'],$announce_list[$_QR['queue']['announce']]) === false)
					$_QR['queue']['announce'] = '';

				if(isset($_QR['queue']['queue-youarenext'],$announce_list[$_QR['queue']['queue-youarenext']]) === false)
					$_QR['queue']['queue-youarenext'] = '';

				if(isset($_QR['queue']['queue-thereare'],$announce_list[$_QR['queue']['queue-thereare']]) === false)
					$_QR['queue']['queue-thereare'] = '';

				if(isset($_QR['queue']['queue-callswaiting'],$announce_list[$_QR['queue']['queue-callswaiting']]) === false)
					$_QR['queue']['queue-callswaiting'] = '';

				if(isset($_QR['queue']['queue-holdtime'],$announce_list[$_QR['queue']['queue-holdtime']]) === false)
					$_QR['queue']['queue-holdtime'] = '';

				if(isset($_QR['queue']['queue-minutes'],$announce_list[$_QR['queue']['queue-minutes']]) === false)
					$_QR['queue']['queue-minutes'] = '';

				if(isset($_QR['queue']['queue-seconds'],$announce_list[$_QR['queue']['queue-seconds']]) === false)
					$_QR['queue']['queue-seconds'] = '';

				if(isset($_QR['queue']['queue-thankyou'],$announce_list[$_QR['queue']['queue-thankyou']]) === false)
					$_QR['queue']['queue-thankyou'] = '';

				if(isset($_QR['queue']['queue-lessthan'],$announce_list[$_QR['queue']['queue-lessthan']]) === false)
					$_QR['queue']['queue-lessthan'] = '';

				if(isset($_QR['queue']['queue-reporthold'],$announce_list[$_QR['queue']['queue-reporthold']]) === false)
					$_QR['queue']['queue-reporthold'] = '';

				if(isset($_QR['queue']['periodic-announce'],$announce_list[$_QR['queue']['periodic-announce']]) === false)
					$_QR['queue']['periodic-announce'] = '';
			}
			else
			{
				$_QR['queue']['announce'] = '';
				$_QR['queue']['queue-youarenext'] = $_QR['queue']['queue-thereare'] = '';
				$_QR['queue']['queue-callswaiting'] = $_QR['queue']['queue-holdtime'] = '';
				$_QR['queue']['queue-minutes'] = $_QR['queue']['seconds'] = '';
				$_QR['queue']['queue-thankyou'] = $_QR['queue']['lessthan'] = '';
				$_QR['queue']['queue-reporthold'] = $_QR['queue']['periodic-announce'] = '';
			}

			$result = array();

			if(($result['qfeatures'] = $qfeatures->chk_values($_QR['qfeatures'])) === false)
			{
				$add = false;
				$result['qfeatures'] = $qfeatures->get_filter_result();
			}

			$_QR['queue']['category'] = 'queue';
			$_QR['queue']['name'] = $result['qfeatures']['name'];

			if(($result['queue'] = $queue->chk_values($_QR['queue'])) === false)
			{
				$add = false;
				$result['queue'] = $queue->get_filter_result();
			}

			$local_exten = $exten_numbers = $result['qmember'] = null;

			if($add === true && $result['qfeatures']['number'] !== '')
			{
				$local_exten = array();
				$local_exten['exten'] = $result['qfeatures']['number'];
				$local_exten['priority'] = 1;
				$local_exten['app'] = 'Macro';
				$local_exten['appdata'] = 'superqueue';

				if($result['qfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['qfeatures']['context'];

				if(($result['local_exten'] = $extensions->chk_values($local_exten)) === false)
				{
					$add = false;
					$result['local_exten'] = $extensions->get_filter_result();
				}

				$exten_numbers = array();
				$exten_numbers['number'] = $result['local_exten']['exten'];
				$exten_numbers['context'] = $result['local_exten']['context'];

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
				|| $extenumbers->get_where($result['extenumbers']) !== false)
				{
					$add = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			if($user_list !== false && ($arr_user = xivo_issa_val('user',$_QR)) !== false)
			{
				$mqinfo = array('call-limit' => 0,'category' => 'queue','usertype' => 'user');

				$result['qmember'] = array();

				$nb = count($arr_user);

				for($i = 0;$i < $nb;$i++)
				{
					$user_id = &$arr_user[$i];

					if(isset($user_list[$user_id]) === false)
						continue;

					if($add === false)
					{
						$user_slt[$user_id] = $user_list[$user_id];
						unset($user_list[$user_id]);
						$ref = &$user_slt[$user_id];
					}
					else
						$ref = &$user_list[$user_id];

					$mqinfo['queue_name'] = $result['qfeatures']['name'];
					$mqinfo['userid'] = $user_id;
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($add === true)
					{
						$user_slt[$user_id] = $user_list[$user_id];
						unset($user_list[$user_id]);
					}

					$result['qmember'][] = $mqinfo;
				}

				if(isset($result['qmember'][0]) === false)
					$result['qmember'] = null;
			}

			if($agent_list !== false && ($arr_agent = xivo_issa_val('agent',$_QR)) !== false)
			{
				$mqinfo = array('category' => 'queue','usertype' => 'agent');

				if($result['qmember'] === null)
					$result['qmember'] = array();

				$nb = count($arr_agent);

				for($i = 0;$i < $nb;$i++)
				{
					$agent_id = &$arr_agent[$i];

					if(isset($agent_list[$agent_id]) === false)
						continue;

					if($add === false)
					{
						$agent_slt[$agent_id] = $agent_list[$agent_id];
						unset($agent_list[$agent_id]);
						$ref = &$agent_slt[$agent_id];
					}
					else
						$ref = &$agent_list[$agent_id];

					$mqinfo['queue_name'] = $result['qfeatures']['name'];
					$mqinfo['userid'] = $agent_id;
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($add === true)
					{
						$agent_slt[$agent_id] = $agent_list[$agent_id];
						unset($agent_list[$agent_id]);
					}

					$result['qmember'][] = $mqinfo;
				}

				if(isset($result['qmember'][0]) === false)
					$result['qmember'] = null;
			}

			if($agroup_list !== false && ($arr_agroup = xivo_issa_val('agroup',$_QR)) !== false)
			{
				$mqinfo = array('category' => 'group','usertype' => 'agent');

				if($result['qmember'] === null)
					$result['qmember'] = array();

				$nb = count($arr_agroup);

				for($i = 0;$i < $nb;$i++)
				{
					$agroup_id = &$arr_agroup[$i];

					if(isset($agroup_list[$agroup_id]) === false)
						continue;

					if($add === false)
					{
						$agroup_slt[$agroup_id] = $agroup_list[$agroup_id];
						unset($agroup_list[$agroup_id]);
						$ref = &$agroup_slt[$agroup_id];
					}
					else
						$ref = &$agroup_list[$agroup_id];

					$mqinfo['queue_name'] = $result['qfeatures']['name'];
					$mqinfo['userid'] = $agroup_id;
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($add === true)
					{
						$agroup_slt[$agroup_id] = $agroup_list[$agroup_id];
						unset($agroup_list[$agroup_id]);
					}

					$result['qmember'][] = $mqinfo;
				}

				if(isset($result['qmember'][0]) === false)
					$result['qmember'] = null;
			}

			if($add === false || ($qfeaturesid = $qfeatures->add($result['qfeatures'])) === false)
				break;

			if(($queueid = $queue->add($result['queue'])) === false)
			{
				$qfeatures->delete($qfeaturesid);
				break;
			}

			if($local_exten !== null && ($local_extenid = $extensions->add($result['local_exten'])) === false)
			{
				$qfeatures->delete($qfeaturesid);
				$queue->delete($queueid);
				break;
			}

			if($exten_numbers !== null && ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
			{
				$qfeatures->delete($qfeaturesid);
				$queue->delete($queueid);

				if($local_exten !== null)
					$extensions->delete($local_extenid);
				break;
			}

			if($result['qmember'] !== null && ($nb = count($result['qmember'])) !== 0)
			{
				for($i = 0;$i < $nb;$i++)
					$qmember->add($result['qmember'][$i]);
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		}
		while(false);

		$element = array();
		$element['queue'] = $queue->get_element();
		$element['qfeatures'] = $qfeatures->get_element();

		$_HTML->assign('user_slt',$user_slt);
		$_HTML->assign('user_list',$user_list);
		$_HTML->assign('agent_slt',$agent_slt);
		$_HTML->assign('agent_list',$agent_list);
		$_HTML->assign('agroup_list',$agroup_list);
		$_HTML->assign('agroup_slt',$agroup_slt);
		$_HTML->assign('moh_list',$moh_list);
		$_HTML->assign('announce_list',$announce_list);
		$_HTML->assign('element',$element);
		$_HTML->assign('info',$result);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$edit = true;

		$return = &$info;

		if(isset($_QR['id']) === false
		|| ($info['qfeatures'] = $qfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['qfeatures']['name'],false)) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		$user_info = array();
		$user_orig = $user_list;

		$info['quser'] = $qmember->get_all_where(array('queue_name' => $info['qfeatures']['name'],
								'usertype' => 'user',
								'category' => 'queue'));

		if($info['quser'] !== false && $user_list !== false)
		{
			$nb = count($info['quser']);

			for($i = 0;$i < $nb;$i++)
			{
				$userid = &$info['quser'][$i]['userid'];

				if(isset($user_list[$userid]) === false)
					continue;

				$user_slt[$userid] = $user_list[$userid];
				$user_info[$userid] = $info['quser'][$i];
				unset($user_list[$userid]);
			}
		}

		$agent_info = array();
		$agent_orig = $agent_list;

		$info['qagent'] = $qmember->get_all_where(array('queue_name' => $info['qfeatures']['name'],
								'usertype' => 'agent',
								'category' => 'queue'));

		if($info['qagent'] !== false && $agent_list !== false)
		{
			$nb = count($info['qagent']);

			for($i = 0;$i < $nb;$i++)
			{
				$agentid = &$info['qagent'][$i]['userid'];

				if(isset($agent_list[$agentid]) === false)
					continue;

				$agent_slt[$agentid] = $agent_list[$agentid];
				$agent_info[$agentid] = $info['qagent'][$i];
				unset($agent_list[$agentid]);
			}
		}

		$agroup_info = array();
		$agroup_orig = $agroup_list;

		$info['qagroup'] = $qmember->get_all_where(array('queue_name' => $info['qfeatures']['name'],
								'usertype' => 'agent',
								'category' => 'group'));

		if($info['qagroup'] !== false && $agroup_list !== false)
		{
			$nb = count($info['qagroup']);

			for($i = 0;$i < $nb;$i++)
			{
				$agroupid = &$info['qagroup'][$i]['userid'];

				if(isset($agroup_list[$agroupid]) === false)
					continue;

				$agroup_slt[$agroupid] = $agroup_list[$agroupid];
				$agroup_info[$agroupid] = $info['qagroup'][$i];
				unset($agroup_list[$agroupid]);
			}
		}

		$sounds = &$ipbx->get_module('sounds');
		$musiconhold = &$ipbx->get_module('musiconhold');
		
		if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
			ksort($moh_list);

		$announce_list = $sounds->get_list('acd',true);

		$status = array();
		$status['localexten'] = $status['extenumbers'] = $status['quser'] = $status['qagent'] = $status['qagroup'] = false;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('qfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
					break;

			if($moh_list === false || isset($_QR['queue']['musiconhold'],$moh_list[$_QR['queue']['musiconhold']]) === false)
				$_QR['queue']['musiconhold'] = '';

			if($announce_list !== false)
			{
				if(isset($_QR['queue']['announce'],$announce_list[$_QR['queue']['announce']]) === false)
					$_QR['queue']['announce'] = '';

				if(isset($_QR['queue']['queue-youarenext'],$announce_list[$_QR['queue']['queue-youarenext']]) === false)
					$_QR['queue']['queue-youarenext'] = '';

				if(isset($_QR['queue']['queue-thereare'],$announce_list[$_QR['queue']['queue-thereare']]) === false)
					$_QR['queue']['queue-thereare'] = '';

				if(isset($_QR['queue']['queue-callswaiting'],$announce_list[$_QR['queue']['queue-callswaiting']]) === false)
					$_QR['queue']['queue-callswaiting'] = '';

				if(isset($_QR['queue']['queue-holdtime'],$announce_list[$_QR['queue']['queue-holdtime']]) === false)
					$_QR['queue']['queue-holdtime'] = '';

				if(isset($_QR['queue']['queue-minutes'],$announce_list[$_QR['queue']['queue-minutes']]) === false)
					$_QR['queue']['queue-minutes'] = '';

				if(isset($_QR['queue']['queue-seconds'],$announce_list[$_QR['queue']['queue-seconds']]) === false)
					$_QR['queue']['queue-seconds'] = '';

				if(isset($_QR['queue']['queue-thankyou'],$announce_list[$_QR['queue']['queue-thankyou']]) === false)
					$_QR['queue']['queue-thankyou'] = '';

				if(isset($_QR['queue']['queue-lessthan'],$announce_list[$_QR['queue']['queue-lessthan']]) === false)
					$_QR['queue']['queue-lessthan'] = '';

				if(isset($_QR['queue']['queue-reporthold'],$announce_list[$_QR['queue']['queue-reporthold']]) === false)
					$_QR['queue']['queue-reporthold'] = '';

				if(isset($_QR['queue']['periodic-announce'],$announce_list[$_QR['queue']['periodic-announce']]) === false)
					$_QR['queue']['periodic-announce'] = '';
			}
			else
			{
				$_QR['queue']['announce'] = '';
				$_QR['queue']['queue-youarenext'] = $_QR['queue']['queue-thereare'] = '';
				$_QR['queue']['queue-callswaiting'] = $_QR['queue']['queue-holdtime'] = '';
				$_QR['queue']['queue-minutes'] = $_QR['queue']['seconds'] = '';
				$_QR['queue']['queue-thankyou'] = $_QR['queue']['lessthan'] = '';
				$_QR['queue']['queue-reporthold'] = $_QR['queue']['periodic-announce'] = '';
			}

			$return = &$result;

			if(($result['qfeatures'] = $qfeatures->chk_values($_QR['qfeatures'])) === false)
			{
				$edit = false;
				$result['qfeatures'] = array_merge($info['qfeatures'],$qfeatures->get_filter_result());
			}

			$_QR['queue']['name'] = $result['qfeatures']['name'];
			$_QR['queue']['category'] = 'queue';

			if(($result['queue'] = $queue->chk_values($_QR['queue'])) === false)
			{
				$edit = false;
				$result['queue'] = $queue->get_filter_result();
			}

			$exten_where = array();
			$exten_where['exten'] = $info['qfeatures']['number'];
			$exten_where['app'] = 'Macro';
			$exten_where['appdata'] = 'superqueue';

			if($info['qfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['qfeatures']['context'];

			if(($info['localexten'] = $extensions->get($exten_where)) !== false)
			{
				if($result['qfeatures']['number'] === '')
					$status['localexten'] = 'delete';
				else
				{
					$status['localexten'] = 'edit';

					$local_exten = $info['localexten'];
					$local_exten['exten'] = $result['qfeatures']['number'];

					if($result['qfeatures']['context'] === '')
						$local_exten['context'] = 'local-extensions';
					else
						$local_exten['context'] = $result['qfeatures']['context'];

					if(($result['localexten'] = $extensions->chk_values($local_exten)) === false)
					{
						$edit = false;
						$result['localexten'] = array_merge($info['localexten'],$extensions->get_filter_result());
					}
				}
			}
			else if($result['qfeatures']['number'] !== '')
			{
				$status['localexten'] = 'add';

				$local_exten = $exten_where;
				$local_exten['exten'] = $result['qfeatures']['number'];
				$local_exten['priority'] = 1;

				if($result['qfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['qfeatures']['context'];

				if(($result['localexten'] = $extensions->chk_values($local_exten)) === false)
				{
					$edit = false;
					$result['localexten'] = $extensions->get_filter_result();
				}
			}

			$exten_numbers = array();
			$exten_numbers['number'] = $result['qfeatures']['number'];

			if($result['qfeatures']['context'] === '')
				$exten_numbers['context'] = 'local-extensions';
			else
				$exten_numbers['context'] = $result['qfeatures']['context'];

			$exten_where = array();
			$exten_where['number'] = $info['qfeatures']['number'];

			if($info['qfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['qfeatures']['context'];

			if(($info['extenumbers'] = $extenumbers->get($exten_where)) !== false)
			{
				if($result['qfeatures']['number'] === '')
					$status['extenumbers'] = 'delete';
				else
				{
					$status['extenumbers'] = 'edit';

					if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
					|| (($extenum = $extenumbers->get($result['extenumbers'])) !== false
					   && (int) $extenum['id'] !== (int) $info['extenumbers']['id']) === true)
					{
						$edit = false;
						$result['extenumbers'] = array_merge($info['extenumbers'],$extenumbers->get_filter_result());
					}
				}
			}
			else if($result['qfeatures']['number'] !== '')
			{
				$status['extenumbers'] = 'add';

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false)
				{
					$edit = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			$result['quser'] = $result['qagent'] = $result['qagroup'] = null;

			do
			{
				if($user_orig === false)
					break;

				if(xivo_issa('user',$_QR) === false)
				{
					if($info['quser'] !== false)
						$status['quser'] = 'delete';
					break;
				}

				$arr_user = array_values($_QR['user']);
				$mqinfo = array('call-limit' => 0,'category' => 'queue','usertype' => 'user');

				$result['quser'] = array();

				$nb = count($arr_user);

				for($i = 0;$i < $nb;$i++)
				{
					$user_id = &$arr_user[$i];

					if(isset($user_orig[$user_id]) === false)
						continue;

					if($edit === false)
					{
						$user_slt[$user_id] = $user_orig[$user_id];
						$ref = &$user_slt[$user_id];
						unset($user_list[$user_id]);
					}
					else if(isset($user_info[$user_id]) === true)
					{
						$ref = &$user_info[$user_id];
						$mqinfo['penalty'] = $ref['penalty'];
						$mqinfo['call-limit'] = $ref['call-limit'];
					}
					else
						$ref = &$user_orig[$user_id];

					$mqinfo['queue_name'] = $result['qfeatures']['name'];
					$mqinfo['userid'] = $user_id;
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($edit === true)
					{
						$user_slt[$user_id] = $user_orig[$user_id];
						unset($user_list[$user_id]);
					}

					$result['quser'][] = $mqinfo;
				}

				if(isset($result['quser'][0]) === false)
				{
					$result['quser'] = null;

					if($info['quser'] !== false)
						$status['quser'] = 'delete';
				}
				else if($info['quser'] !== false)
					$status['quser'] = 'edit';
				else
					$status['quser'] = 'add';
			}
			while(false);

			do
			{
				if($agent_orig === false)
					break;

				if(xivo_issa('agent',$_QR) === false)
				{
					if($info['qagent'] !== false)
						$status['qagent'] = 'delete';
					break;
				}

				$arr_agent = array_values($_QR['agent']);
				$mqinfo = array('category' => 'queue','usertype' => 'agent');

				if($result['qagent'] === null)
					$result['qagent'] = array();

				$nb = count($arr_agent);

				for($i = 0;$i < $nb;$i++)
				{
					$agent_id = &$arr_agent[$i];

					if(isset($agent_orig[$agent_id]) === false)
						continue;

					if($edit === false)
					{
						$agent_slt[$agent_id] = $agent_orig[$agent_id];
						$ref = &$agent_slt[$agent_id];
						unset($agent_list[$agent_id]);
					}
					else if(isset($agent_info[$agent_id]) === true)
					{
						$ref = &$agent_info[$agent_id];
						$mqinfo['penalty'] = $ref['penalty'];
					}
					else
						$ref = &$agent_orig[$agent_id];

					$mqinfo['queue_name'] = $result['qfeatures']['name'];
					$mqinfo['userid'] = $agent_id;
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($edit === true)
					{
						$agent_slt[$agent_id] = $agent_orig[$agent_id];
						unset($agent_list[$agent_id]);
					}

					$result['qagent'][] = $mqinfo;
				}

				if(isset($result['qagent'][0]) === false)
				{
					$result['qagent'] = null;

					if($info['qagent'] !== false)
						$status['qagent'] = 'delete';
				}
				else if($info['qagent'] !== false)
					$status['qagent'] = 'edit';
				else
					$status['qagent'] = 'add';
			}
			while(false);

			do
			{
				if($agroup_orig === false)
					break;

				if(xivo_issa('agroup',$_QR) === false)
				{
					if($info['qagroup'] !== false)
						$status['qagroup'] = 'delete';
					break;
				}

				$arr_agroup = array_values($_QR['agroup']);
				$mqinfo = array('category' => 'group','usertype' => 'agent');

				if($result['qagroup'] === null)
					$result['qagroup'] = array();

				$nb = count($arr_agroup);

				for($i = 0;$i < $nb;$i++)
				{
					$agroup_id = &$arr_agroup[$i];

					if(isset($agroup_orig[$agroup_id]) === false)
						continue;

					if($edit === false)
					{
						$agroup_slt[$agroup_id] = $agroup_orig[$agroup_id];
						$ref = &$agroup_slt[$agroup_id];
						unset($agroup_list[$agroup_id]);
					}
					else if(isset($agroup_info[$agroup_id]) === true)
					{
						$ref = &$agroup_info[$agroup_id];
						$mqinfo['penalty'] = $ref['penalty'];
					}
					else
						$ref = &$agroup_orig[$agroup_id];

					$mqinfo['queue_name'] = $result['qfeatures']['name'];
					$mqinfo['userid'] = $agroup_id;
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($edit === true)
					{
						$agroup_slt[$agroup_id] = $agroup_orig[$agroup_id];
						unset($agroup_list[$agroup_id]);
					}

					$result['qagroup'][] = $mqinfo;
				}

				if(isset($result['qagroup'][0]) === false)
				{
					$result['qagroup'] = null;

					if($info['qagroup'] !== false)
						$status['qagroup'] = 'delete';
				}
				else if($info['qagroup'] !== false)
					$status['qagroup'] = 'edit';
				else
					$status['qagroup'] = 'add';
			}
			while(false);

			if($edit === false || $qfeatures->edit($info['qfeatures']['id'],$result['qfeatures']) === false)
				break;

			if($queue->edit($info['queue']['name'],$result['queue']) === false)
			{
				$qfeatures->edit_origin();
				break;
			}

			switch($status['localexten'])
			{
				case 'add':
					$rs_localexten = $extensions->add($result['localexten']);
					break;
				case 'edit':
					$rs_localexten = $extensions->edit($info['localexten']['id'],$result['localexten']);
					break;
				case 'delete':
					$rs_localexten = $extensions->delete($info['localexten']['id']);
					break;
				default:
					$rs_localexten = null;
			}

			if($rs_localexten === false)
			{
				$qfeatures->edit_origin();
				$queue->edit_origin();
				break;
			}

			$rs_dfeatures = null;

			$dfeatures = &$ipbx->get_module('didfeatures');
			$dfeatures_where = array();
			$dfeatures_where['type'] = 'queue';
			$dfeatures_where['typeid'] = $info['qfeatures']['id'];
			$dfeatures_where['commented'] = 0;

			switch($status['extenumbers'])
			{
				case 'add':
					$rs_extenumbers = $extenumbers->add($result['extenumbers']);
					break;
				case 'edit':
					$rs_extenumbers = $extenumbers->edit($info['extenumbers']['id'],$result['extenumbers']);
					break;
				case 'delete':
					if(($rs_extenumbers = $extenumbers->delete($info['extenumbers']['id'])) !== false
					&& ($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
					&& ($rs_dfeatures = $dfeatures->edit_where($dfeatures_where,array('commented' => 1))) === false)
						$rs_extenumbers = false;
					break;
				default:
					$rs_extenumbers = null;
			}

			if($rs_extenumbers === false)
			{
				$qfeatures->edit_origin();
				$queue->edit_origin();

				if($rs_dfeatures === false)
					$extenumbers->add_origin();

				if($rs_localexten === null)
					break;

				switch($status['localexten'])
				{
					case 'add':
						$extensions->delete($rs_localexten);
						break 2;
					case 'edit':
						$extensions->edit_origin();
						break 2;
					case 'delete':
						$extensions->add_origin();
						break 2;
					default:
						break 2;
				}
			}

			$rs_quser = null;

			$quser_where = array();
			$quser_where['queue_name'] = $info['queue']['name'];
			$quser_where['usertype'] = 'user';
			$quser_where['category'] = 'queue';

			switch($status['quser'])
			{
				case 'edit':
					if(($rs_quser = $qmember->delete_where($quser_where)) === false)
						break;
				case 'add':
					$nb = count($result['quser']);

					for($i = 0;$i < $nb;$i++)
						$qmember->add($result['quser'][$i]);
					break;
				case 'delete':
					$rs_quser = $qmember->delete_where($quser_where);
					break;
				default:
					$rs_quser = null;
			}

			if($rs_quser === false)
			{
				$qfeatures->edit_origin();
				$queue->edit_origin();

				if($rs_localexten !== null)
				{
					switch($status['localexten'])
					{
						case 'add':
							$extensions->delete($rs_localexten);
							break;
						case 'edit':
							$extensions->edit_origin();
							break;
						case 'delete':
							$extensions->add_origin();
							break;
						default:
							break;
					}
				}

				if($rs_extenumbers === null)
					break;

				switch($status['extenumbers'])
				{
					case 'add':
						$extenumbers->delete($rs_extenumbers);
						break 2;
					case 'edit':
						$extenumbers->edit_origin();
						break 2;
					case 'delete':
						$extenumbers->add_origin();

						if($rs_dfeatures === true)
							$dfeatures->edit_list($info['dfeatures'],array('commented' => 0));
						break 2;
					default:
						break 2;
				}
			}

			$rs_qagent = null;

			$qagent_where = $quser_where;
			$qagent_where['usertype'] = 'agent';

			switch($status['qagent'])
			{
				case 'edit':
					if(($rs_qagent = $qmember->delete_where($qagent_where)) === false)
						break;
				case 'add':
					$nb = count($result['qagent']);

					for($i = 0;$i < $nb;$i++)
						$qmember->add($result['qagent'][$i]);
					break;
				case 'delete':
					$rs_qagent = $qmember->delete_where($qagent_where);
					break;
				default:
					$rs_qagent = null;
			}

			if($rs_qagent === false)
			{
				$qfeatures->edit_origin();
				$queue->edit_origin();

				if($rs_quser !== null)
				{
					switch($status['quser'])
					{
						case 'add':
							$qmember->delete_where($quser_where);
							break;
						case 'edit':
							$qmember->delete_where($quser_where);
						case 'delete':
							$nb = count($info['quser']);

							for($i = 0;$i < $nb;$i++)
								$qmember->add($info['quser'][$i]);
						default:
							break;
					}
				}

				if($rs_localexten !== null)
				{
					switch($status['localexten'])
					{
						case 'add':
							$extensions->delete($rs_localexten);
							break;
						case 'edit':
							$extensions->edit_origin();
							break;
						case 'delete':
							$extensions->add_origin();
							break;
						default:
							break;
					}
				}

				if($rs_extenumbers === null)
					break;

				switch($status['extenumbers'])
				{
					case 'add':
						$extenumbers->delete($rs_extenumbers);
						break 2;
					case 'edit':
						$extenumbers->edit_origin();
						break 2;
					case 'delete':
						$extenumbers->add_origin();

						if($rs_dfeatures === true)
							$dfeatures->edit_list($info['dfeatures'],array('commented' => 0));
						break 2;
					default:
						break 2;
				}
			}

			$rs_qagroup = null;

			$qagroup_where = $qagent_where;
			$qagroup_where['category'] = 'group';

			switch($status['qagroup'])
			{
				case 'edit':
					if(($rs_qagroup = $qmember->delete_where($qagroup_where)) === false)
						break;
				case 'add':
					$nb = count($result['qagroup']);

					for($i = 0;$i < $nb;$i++)
						$qmember->add($result['qagroup'][$i]);
					break;
				case 'delete':
					$rs_qagroup = $qmember->delete_where($qagroup_where);
					break;
				default:
					$rs_qagroup = null;
			}

			if($rs_qagroup === false)
			{
				$qfeatures->edit_origin();
				$queue->edit_origin();

				if($rs_quser !== null)
				{
					switch($status['quser'])
					{
						case 'add':
							$qmember->delete_where($quser_where);
							break;
						case 'edit':
							$qmember->delete_where($quser_where);
						case 'delete':
							$nb = count($info['quser']);

							for($i = 0;$i < $nb;$i++)
								$qmember->add($info['quser'][$i]);
						default:
							break;
					}
				}

				if($rs_qagent !== null)
				{
					switch($status['qagent'])
					{
						case 'add':
							$qmember->delete_where($qagent_where);
							break;
						case 'edit':
							$qmember->delete_where($qagent_where);
						case 'delete':
							$nb = count($info['qagent']);

							for($i = 0;$i < $nb;$i++)
								$qmember->add($info['qagent'][$i]);
						default:
							break;
					}
				}

				if($rs_localexten !== null)
				{
					switch($status['localexten'])
					{
						case 'add':
							$extensions->delete($rs_localexten);
							break;
						case 'edit':
							$extensions->edit_origin();
							break;
						case 'delete':
							$extensions->add_origin();
							break;
						default:
							break;
					}
				}

				if($rs_extenumbers === null)
					break;

				switch($status['extenumbers'])
				{
					case 'add':
						$extenumbers->delete($rs_extenumbers);
						break 2;
					case 'edit':
						$extenumbers->edit_origin();
						break 2;
					case 'delete':
						$extenumbers->add_origin();

						if($rs_dfeatures === true)
							$dfeatures->edit_list($info['dfeatures'],array('commented' => 0));
						break 2;
					default:
						break 2;
				}
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		}
		while(false);

		$element = array();
		$element['queue'] = $queue->get_element();
		$element['qfeatures'] = $qfeatures->get_element();

		$_HTML->assign('user_slt',$user_slt);
		$_HTML->assign('user_list',$user_list);
		$_HTML->assign('agent_slt',$agent_slt);
		$_HTML->assign('agent_list',$agent_list);
		$_HTML->assign('agroup_slt',$agroup_slt);
		$_HTML->assign('agroup_list',$agroup_list);
		$_HTML->assign('moh_list',$moh_list);
		$_HTML->assign('announce_list',$announce_list);
		$_HTML->assign('id',$info['qfeatures']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('element',$element);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;
		$qmember = &$ipbx->get_module('queuemember');

		if(isset($_QR['id']) === false
		|| ($info['qfeatures'] = $qfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['qfeatures']['name'],false)) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		do
		{
			if($qfeatures->delete($info['qfeatures']['id']) === false)
				break;

			if($queue->delete($info['queue']['name']) === false)
			{
				$qfeatures->add_origin();
				break;
			}

			$localexten_where = array();
			$localexten_where['exten'] = $info['qfeatures']['number'];
			$localexten_where['app'] = 'Macro';
			$localexten_where['appdata'] = 'superqueue';

			if($info['qfeatures']['context'] === '')
				$localexten_where['context'] = 'local-extensions';
			else
				$localexten_where['context'] = $info['qfeatures']['context'];

			if(($info['extensions'] = $extensions->get($localexten_where)) !== false
			&& $extensions->delete($info['extensions']['id']) === false)
			{
				$qfeatures->delete($info['qfeatures']['id']);
				$queue->add_origin();
				break;
			}

			$extenum_where = array();
			$extenum_where['number'] = $localexten_where['exten'];
			$extenum_where['context'] = $localexten_where['context'];

			$info['dfeatures'] = false;

			if(($info['extenumbers'] = $extenumbers->get($extenum_where)) !== false)
			{
				$dfeatures = &$ipbx->get_module('didfeatures');
				$dfeatures_where = array();
				$dfeatures_where['type'] = 'queue';
				$dfeatures_where['typeid'] = $info['qfeatures']['id'];
				$dfeatures_where['commented'] = 0;

				if($extenumbers->delete($info['extenumbers']['id']) === false
				|| (($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
				   && $dfeatures->edit_where($dfeatures_where,array('commented' => 1)) === false) === true)
				{
					$qfeatures->delete($info['qfeatures']['id']);
					$queue->add_origin();

					if($info['localexten'] !== false)
						$extensions->add_origin();

					if($info['dfeatures'] !== false)
						$extenumbers->add_origin();
					break;
				}
			}

			$qmember_where = array('queue_name' => $info['qfeatures']['name']);

			if($qmember->get_where($qmember_where) !== false
			&& $qmember->delete_where($qmember_where) === false)
			{
				$qfeatures->delete($info['qfeatures']['id']);
				$queue->add_origin();

				if($info['localexten'] !== false)
					$extensions->add_origin();

				if($info['extenumbers'] !== false)
					$extenumbers->add_origin();

				if($info['dfeatures'] !== false)
					$dfeatures->edit_list($info['dfeatures'],array('commented' => 0));
				break;
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($queues = $ipbx->get_queues_list()) !== false)
		{
			$total = count($queues);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'qfeatures','key' => 'name'));
			usort($queues,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$queues);
}

$_HTML->assign('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/queues');

$_HTML->assign('bloc','pbx_settings/queues/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
