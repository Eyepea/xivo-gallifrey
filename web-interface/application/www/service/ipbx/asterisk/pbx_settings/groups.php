<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$queue = &$ipbx->get_module('queue');
$gfeatures = &$ipbx->get_module('groupfeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');
$qmember = &$ipbx->get_module('queuemember');

if(($user_list = $ipbx->get_user_queue_info()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('key' => 'fullname'));
	uasort($user_list,array(&$sort,'str_usort'));
}

$param = array();
$param['act'] = 'list';

$info = $result = $user_slt = array();

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
				break;

			$_QR['queue']['category'] = 'group';

			$result = array();

			if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'])) === false)
			{
				$add = false;
				$result['gfeatures'] = $gfeatures->get_filter_result();
			}

			$_QR['queue']['name'] = $result['gfeatures']['name'];

			if(($result['queue'] = $queue->chk_values($_QR['queue'])) === false)
			{
				$add = false;
				$result['queue'] = $queue->get_filter_result();
			}

			$local_exten = $exten_numbers = $result['gmember'] = null;

			if($add === true && $result['gfeatures']['number'] !== '')
			{
				$local_exten = array();
				$local_exten['exten'] = $result['gfeatures']['number'];
				$local_exten['priority'] = 1;
				$local_exten['app'] = 'Macro';
				$local_exten['appdata'] = 'supergroup';

				if($result['gfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['gfeatures']['context'];

				if(($result['local_exten'] = $extensions->chk_values($local_exten)) === false)
				{
					$add = false;
					$result['local_exten'] = $extensions->get_filter_result();
				}

				$exten_numbers = array();
				$exten_numbers['number'] = $result['local_exten']['exten'];
				$exten_numbers['context'] = $result['local_exten']['context'];

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
				|| $extenumbers->get($result['extenumbers']) !== false)
				{
					$add = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			if($user_list !== false && ($arr_user = xivo_issa_val('user',$_QR)) !== false)
			{
				$mqinfo = array('call-limit' => 0,'category' => 'group','usertype' => 'user');
				$result['gmember'] = array();

				$nb = count($arr_user);

				for($i = 0;$i < $nb;$i++)
				{
					if(isset($user_list[$arr_user[$i]]) === false)
						continue;

					if($add === false)
					{
						$user_slt[$arr_user[$i]] = $user_list[$arr_user[$i]];
						unset($user_list[$arr_user[$i]]);
						$ref = &$user_slt[$arr_user[$i]];
					}
					else
						$ref = &$user_list[$arr_user[$i]];

					$mqinfo['queue_name'] = $result['gfeatures']['name'];
					$mqinfo['userid'] = $arr_user[$i];
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($add === true)
					{
						$user_slt[$arr_user[$i]] = $user_list[$arr_user[$i]];
						unset($user_list[$arr_user[$i]]);
					}

					$result['gmember'][] = $mqinfo;
				}

				if(isset($result['gmember'][0]) === false)
					$result['gmember'] = null;
			}

			if($add === false || ($gfeaturesid = $gfeatures->add($result['gfeatures'])) === false)
				break;

			if(($queueid = $queue->add($result['queue'])) === false)
			{
				$gfeatures->delete($gfeaturesid);
				break;
			}

			if($local_exten !== null && ($local_extenid = $extensions->add($result['local_exten'])) === false)
			{
				$gfeatures->delete($gfeaturesid);
				$queue->delete($queueid);
				break;
			}

			if($exten_numbers !== null && ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
			{
				$gfeatures->delete($gfeaturesid);
				$queue->delete($queueid);

				if($local_exten !== null)
					$extensions->delete($local_extenid);
				break;
			}

			if($result['gmember'] !== null && ($nb = count($result['gmember'])) !== 0)
			{
				for($i = 0;$i < $nb;$i++)
					$qmember->add($result['gmember'][$i]);
			}

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}
		while(false);

		$element = array();
		$element['queue'] = $queue->get_element();
		$element['gfeatures'] = $gfeatures->get_element();

		$_HTML->assign('user_slt',$user_slt);
		$_HTML->assign('user_list',$user_list);
		$_HTML->assign('element',$element);
		$_HTML->assign('info',$result);
		break;
	case 'edit':
		$edit = true;

		$return = &$info;

		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'])) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$user_orig = $user_list;

		$info['guser'] = $qmember->get_all_where(array('queue_name' => $info['gfeatures']['name'],
								'usertype' => 'user',
								'category' => 'group'));

		if($info['guser'] !== false && $user_list !== false)
		{
			$nb = count($info['guser']);

			for($i = 0;$i < $nb;$i++)
			{
				if(isset($user_list[$info['guser'][$i]['userid']]) === false)
					continue;

				$user_slt[$info['guser'][$i]['userid']] = $user_list[$info['guser'][$i]['userid']];
				unset($user_list[$info['guser'][$i]['userid']]);
			}
		}

		$status = $ugroup_list = array();
		$status['localexten'] = $status['extenumbers'] = $status['user'] = false;

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('gfeatures',$_QR) === false || xivo_issa('queue',$_QR) === false)
					break;

			$_QR['queue']['category'] = 'group';

			$return = &$result;

			if(($result['gfeatures'] = $gfeatures->chk_values($_QR['gfeatures'])) === false)
			{
				$edit = false;
				$result['mfeatures'] = array_merge($info['gfeatures'],$gfeatures->get_filter_result());
			}

			$_QR['queue']['name'] = $result['gfeatures']['name'];

			if(($result['queue'] = $queue->chk_values($_QR['queue'])) === false)
			{
				$edit = false;
				$result['queue'] = $queue->get_filter_result();
			}

			$exten_where = array();
			$exten_where['exten'] = $info['gfeatures']['number'];
			$exten_where['app'] = 'Macro';
			$exten_where['appdata'] = 'supergroup';

			if($info['gfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['gfeatures']['context'];

			if(($info['localexten'] = $extensions->get($exten_where)) !== false)
			{
				if($result['gfeatures']['number'] === '')
					$status['localexten'] = 'delete';
				else
				{
					$status['localexten'] = 'edit';

					$local_exten = $info['localexten'];
					$local_exten['exten'] = $result['gfeatures']['number'];

					if($result['gfeatures']['context'] === '')
						$local_exten['context'] = 'local-extensions';
					else
						$local_exten['context'] = $result['gfeatures']['context'];

					if(($result['localexten'] = $extensions->chk_values($local_exten)) === false)
					{
						$edit = false;
						$result['localexten'] = array_merge($info['localexten'],$extensions->get_filter_result());
					}
				}
			}
			else if($result['gfeatures']['number'] !== '')
			{
				$status['localexten'] = 'add';

				$local_exten = $exten_where;
				$local_exten['exten'] = $result['gfeatures']['number'];
				$local_exten['priority'] = 1;

				if($result['gfeatures']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['gfeatures']['context'];

				if(($result['localexten'] = $extensions->chk_values($local_exten)) === false)
				{
					$edit = false;
					$result['localexten'] = $extensions->get_filter_result();
				}
			}

			$exten_numbers = array();
			$exten_numbers['number'] = $result['gfeatures']['number'];

			if($result['gfeatures']['context'] === '')
				$exten_numbers['context'] = 'local-extensions';
			else
				$exten_numbers['context'] = $result['gfeatures']['context'];

			$exten_where = array();
			$exten_where['number'] = $info['gfeatures']['number'];

			if($info['gfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['gfeatures']['context'];

			if(($info['extenumbers'] = $extenumbers->get($exten_where)) !== false)
			{
				if($result['gfeatures']['number'] === '')
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
			else if($result['gfeatures']['number'] !== '')
			{
				$status['extenumbers'] = 'add';

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false)
				{
					$edit = false;
					$result['extenumbers'] = $extenumbers->get_filter_result();
				}
			}

			do
			{
				if($user_orig === false)
					break;

				if(xivo_issa('user',$_QR) === false)
				{
					if($info['guser'] !== false)
						$status['user'] = 'delete';
					break;
				}

				$arr_user = array_values($_QR['user']);
				$mqinfo = array('call-limit' => 0,'category' => 'group','usertype' => 'user');
				$result['gmember'] = array();

				$nb = count($arr_user);

				for($i = 0;$i < $nb;$i++)
				{
					if(isset($user_orig[$arr_user[$i]]) === false)
						continue;

					if($edit === false)
					{
						$user_slt[$arr_user[$i]] = $user_orig[$arr_user[$i]];
						unset($user_list[$arr_user[$i]]);
						$ref = &$user_slt[$arr_user[$i]];
					}
					else
						$ref = &$user_orig[$arr_user[$i]];

					$mqinfo['queue_name'] = $result['gfeatures']['name'];
					$mqinfo['userid'] = $arr_user[$i];
					$mqinfo['interface'] = $ref['interface'];
					$mqinfo['channel'] = $ref['channel'];

					if(($mqinfo = $qmember->chk_values($mqinfo)) === false)
						continue;

					if($edit === true)
					{
						$ugroup_list[$arr_user[$i]] = 1;
						$user_slt[$arr_user[$i]] = $user_orig[$arr_user[$i]];
						unset($user_list[$arr_user[$i]]);
					}

					$result['gmember'][] = $mqinfo;
				}

				if(isset($result['gmember'][0]) === false)
				{
					$result['gmember'] = null;

					if($info['guser'] !== false)
						$status['user'] = 'delete';
				}
				else if($info['guser'] !== false)
					$status['user'] = 'edit';
				else
					$status['user'] = 'add';
			}
			while(false);

			if($edit === false || $gfeatures->edit($info['gfeatures']['id'],$result['gfeatures']) === false)
				break;

			if($queue->edit($info['queue']['name'],$result['queue']) === false)
			{
				$gfeatures->edit_origin();
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
				$gfeatures->edit_origin();
				$queue->edit_origin();
				break;
			}

			$rs_dfeatures = null;

			$dfeatures = &$ipbx->get_module('didfeatures');
			$dfeatures_where = array();
			$dfeatures_where['type'] = 'group';
			$dfeatures_where['typeid'] = $info['gfeatures']['id'];
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
				$gfeatures->edit_origin();
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

			if($info['queue']['name'] !== $result['queue']['name']
			&& $qmember->edit_where(array('queue_name' => $info['queue']['name']),
						array('queue_name' => $result['queue']['name'])) === false)
			{
				$gfeatures->edit_origin();
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

			$rs_gmember = null;

			$guser_where = array();
			$guser_where['queue_name'] = $info['queue']['name'];
			$guser_where['usertype'] = 'user';
			$guser_where['category'] = 'group';

			switch($status['user'])
			{
				case 'edit':
					if(($rs_gmember = $qmember->delete_where($guser_where)) === false)
						break;

					$ugroup = $ipbx->get_module('usergroup');
					$info['ugroup'] = $ugroup->get_all_where(array('groupid' => $info['gfeatures']['id']));

					if($info['ugroup'] !== false && empty($ugroup_list) === false)
					{
						$nb = count($info['ugroup']);

						for($i = 0;$i < $nb;$i++)
						{
							$ref = &$info['ugroup'][$i];
							if(isset($ugroup_list[$ref['userid']]) === false)
								$ugroup->delete($ref['id']);
						}
					}
				case 'add':
					$nb = count($result['gmember']);

					for($i = 0;$i < $nb;$i++)
						$qmember->add($result['gmember'][$i]);
					break;
				case 'delete':
					$ugroup = $ipbx->get_module('usergroup');

					if(($rs_gmember = $qmember->delete_where($guser_where)) !== false)
						$ugroup->delete_by_group($info['gfeatures']['id']);
					break;
				default:
					$rs_gmember = null;
			}

			if($rs_gmember === false)
			{
				$gfeatures->edit_origin();
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

			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}
		while(false);

		$element = array();
		$element['queue'] = $queue->get_element();
		$element['gfeatures'] = $gfeatures->get_element();

		$_HTML->assign('user_slt',$user_slt);
		$_HTML->assign('user_list',$user_list);
		$_HTML->assign('id',$info['gfeatures']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('element',$element);
		break;
	case 'delete':
		$param['page'] = $page;
		$qmember = &$ipbx->get_module('queuemember');

		if(isset($_QR['id']) === false
		|| ($info['gfeatures'] = $gfeatures->get($_QR['id'])) === false
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'],false)) === false
		|| $qmember->get_nb_by_name($info['queue']['name']) !== 0)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		do
		{
			if($gfeatures->delete($info['gfeatures']['id']) === false)
				break;

			if($queue->delete($info['queue']['name']) === false)
			{
				$gfeatures->recover($info['gfeatures']['id']);
				break;
			}

			$localexten_where = array();
			$localexten_where['exten'] = $info['gfeatures']['number'];
			$localexten_where['app'] = 'Macro';
			$localexten_where['appdata'] = 'supergroup';

			if($info['gfeatures']['context'] === '')
				$localexten_where['context'] = 'local-extensions';
			else
				$localexten_where['context'] = $info['gfeatures']['context'];

			if(($info['extensions'] = $extensions->get($localexten_where)) !== false
			&& $extensions->delete($info['extensions']['id']) === false)
			{
				$gfeatures->recover($info['gfeatures']['id']);
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
				$dfeatures_where['type'] = 'group';
				$dfeatures_where['typeid'] = $info['gfeatures']['id'];
				$dfeatures_where['commented'] = 0;

				if($extenumbers->delete($info['extenumbers']['id']) === false
				|| (($info['dfeatures'] = $dfeatures->get_list_where($dfeatures_where,false)) !== false
				   && $dfeatures->edit_where($dfeatures_where,array('commented' => 1)) === false) === true)
				{
					$gfeatures->recover($info['gfeatures']['id']);
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

		xivo_go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($groups = $ipbx->get_groups_list()) !== false)
		{
			$total = count($groups);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
			usort($groups,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$groups);
}

$_HTML->assign('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/pbx_settings/groups');

$_HTML->assign('bloc','pbx_settings/groups/'.$act);
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
