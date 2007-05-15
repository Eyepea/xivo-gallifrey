<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$queue = &$ipbx->get_module('queue');
$qfeatures = &$ipbx->get_module('queuefeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');
$qmember = &$ipbx->get_module('queuemember');

if(($member_list = $ipbx->get_members_list()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort();
	uasort($member_list,array(&$sort,'str_usort'));
}

$param = array();
$param['act'] = 'list';

$info = $result = $member_slt = array();

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		$sounds = &$ipbx->get_module('sounds');
		$musiconhold = &$ipbx->get_module('musiconhold');
		
		if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
			ksort($moh_list);

		$announce_list = $sounds->get_list('acd');

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('qfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
				break;

			if($moh_list === false || isset($_QR['queue']['musiconhold'],$moh_list[$_QR['queue']['musiconhold']]) === false)
				$_QR['queue']['musiconhold'] = '';

			$result = array();

			if(($result['qfeatures'] = $qfeatures->chk_values($_QR['qfeatures'],true,true)) === false)
			{
				$add = false;
				$result['qfeatures'] = $qfeatures->get_filter_result();
			}

			$_QR['queue']['category'] = 'queue';
			$_QR['queue']['name'] = $result['qfeatures']['name'];

			if(($result['queue'] = $queue->chk_values($_QR['queue'],true,true)) === false)
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

				if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false)
				{
					$add = false;
					$result['local_exten'] = $extensions->get_filter_result();
				}

				$exten_numbers = array();
				$exten_numbers['number'] = $result['local_exten']['exten'];
				$exten_numbers['context'] = $result['local_exten']['context'];

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false
				|| $extenumbers->get_where($result['extenumbers']) !== false)
				{
					$add = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			if($member_list !== false && xivo_issa('member',$_QR) === true)
			{
				$member = array_values($_QR['member']);
				$mqinfo = array('call-limit' => 0);
				$result['qmember'] = array();

				if(($nb = count($member)) !== 0)
				{
					for($i = 0;$i < $nb;$i++)
					{
						if(isset($member_list[$member[$i]]) === false)
							continue;

						if($add === false)
						{
							$member_slt[$member[$i]] = $member_list[$member[$i]];
							unset($member_list[$member[$i]]);
						}

						$mqinfo['queue_name'] = $result['qfeatures']['name'];
						$mqinfo['interface'] = $member[$i];

						if(($mqinfo = $qmember->chk_values($mqinfo,true,true)) === false)
							continue;

						if($add === true)
						{
							$member_slt[$member[$i]] = $member_list[$member[$i]];
							unset($member_list[$member[$i]]);
						}

						$result['qmember'][] = $mqinfo;
					}
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

		$_HTML->assign('member_slt',$member_slt);
		$_HTML->assign('member_list',$member_list);
		$_HTML->assign('moh_list',$moh_list);
		$_HTML->assign('announce_list',$announce_list);
		$_HTML->assign('element',$element);
		$_HTML->assign('info',$result);
		break;
	case 'edit':
		$edit = true;

		$return = &$info;

		if(isset($_QR['id']) === false
		|| ($info['qfeatures'] = $qfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['qfeatures']['name'],false)) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		$status = array();
		$status['local_exten'] = $status['extenumbers'] = false;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('qfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
					break;

			$return = &$result;

			if(($result['qfeatures'] = $qfeatures->chk_values($_QR['qfeatures'],true,true)) === false)
			{
				$edit = false;
				$result['mfeatures'] = array_merge($info['qfeatures'],$qfeatures->get_filter_result());
			}

			$_QR['queue']['name'] = $result['qfeatures']['name'];

			if(($result['queue'] = $queue->chk_values($_QR['queue'],true,true)) === false)
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

			if(($info['localexten'] = $extensions->get_where($exten_where)) !== false)
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

					if(($result['localexten'] = $extensions->chk_values($local_exten,true,true)) === false)
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

				if(($result['localexten'] = $extensions->chk_values($local_exten,true,true)) === false)
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

			if(($info['extenumbers'] = $extenumbers->get_where($exten_where)) !== false)
			{
				if($result['qfeatures']['number'] === '')
					$status['extenumbers'] = 'delete';
				else
				{
					$status['extenumbers'] = 'edit';

					if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false
					|| (($extenum = $extenumbers->get_where($result['extenumbers'])) !== false
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

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false)
				{
					$edit = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

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

			if($info['queue']['name'] === $result['queue']['name'])
				xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

			$qmember = &$ipbx->get_module('queuemember');

			if($qmember->edit_where(array('queue_name' => $info['queue']['name']),array('queue_name' => $result['queue']['name'])) === false)
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
							$dfeatures->edit_list_where($info['dfeatures'],array('commented' => 0));
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

		$_HTML->assign('id',$info['qfeatures']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('element',$element);
		break;
	case 'delete':
		$param['page'] = $page;
		$qmember = &$ipbx->get_module('queuemember');

		if(isset($_QR['id']) === false
		|| ($info['qfeatures'] = $qfeatures->get($_QR['id'],false)) === false
		|| ($info['queue'] = $queue->get($info['qfeatures']['name'],false)) === false
		|| $qmember->get_nb_by_name($info['queue']['name']) !== 0)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);

		do
		{
			if($qfeatures->disable($info['qfeatures']['id']) === false)
				break;

			if($queue->delete($info['queue']['name']) === false)
			{
				$qfeatures->enable($info['qfeatures']['id']);
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

			if(($info['extensions'] = $extensions->get_where($localexten_where)) !== false
			&& $extensions->delete($info['extensions']['id']) === false)
			{
				$qfeatures->enable($info['qfeatures']['id']);
				$queue->add_origin();
				break;
			}

			$extenum_where = array();
			$extenum_where['number'] = $localexten_where['exten'];
			$extenum_where['context'] = $localexten_where['context'];

			$info['dfeatures'] = false;

			if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
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
					$qfeatures->enable($info['qfeatures']['id']);
					$queue->add_origin();

					if($info['localexten'] !== false)
						$extensions->add_origin();

					if($info['dfeatures'] !== false)
						$extenumbers->add_origin();
					break;
				}
			}
		}
		while(false);

		xivo_go($_HTML->url('service/ipbx/pbx_settings/queues'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($queues = $ipbx->get_queues_list(false)) !== false)
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
