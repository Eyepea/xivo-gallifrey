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
	$sort = new xivo_sort(array('key' => 'identity'));
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

			$localextenid = $exten_numbers = $result['gmember'] = null;

			if($add === true && $result['gfeatures']['number'] !== '')
			{
				if($result['gfeatures']['context'] === '')
					$localextencontext = 'local-extensions';
				else
					$localextencontext = $result['gfeatures']['context'];

				if(($localextenid = $extensions->new_exten('macro',
								array('appdata' => 'supergroup'),
								$result['gfeatures']['number'],
								$localextencontext)) === false)
					$add = false;

				$exten_numbers = array();
				$exten_numbers['exten'] = $result['gfeatures']['number'];
				$exten_numbers['context'] = $localextencontext;

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
				|| $extenumbers->exists($result['extenumbers']) !== false)
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

			if($localextenid !== null && $extensions->add_exten($localextenid) === false)
			{
				$gfeatures->delete($gfeaturesid);
				$queue->delete($queueid);
				break;
			}

			if($exten_numbers !== null && ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
			{
				$gfeatures->delete($gfeaturesid);
				$queue->delete($queueid);

				if($localextenid !== null)
					$extensions->delete_exten($localextenid);
				break;
			}

			if($result['gmember'] !== null && ($nb = count($result['gmember'])) !== 0)
			{
				for($i = 0;$i < $nb;$i++)
					$qmember->add($result['gmember'][$i]);
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
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
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

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

			if($info['gfeatures']['number'] !== '')
			{
				if($info['gfeatures']['context'] === '')
					$localextencontext = 'local-extensions';
				else
					$localextencontext = $info['gfeatures']['context'];
		
				if($result['gfeatures']['context'] === '')
					$localexteneditcontext = 'local-extensions';
				else
					$localexteneditcontext = $result['gfeatures']['context'];
	
				if(($info['localexten'] = $extensions->get_exten('macro',
										 $info['gfeatures']['number'],
										 $localextencontext,
										 array('appdata' => 'supergroup'))) === false)
					$edit = false;
				else if($result['gfeatures']['number'] === '')
					$status['localexten'] = 'delete';
				else if(($localexten_edit = $extensions->chk_exten('macro',
										   null,
										   $result['gfeatures']['number'],
										   $localexteneditcontext)) === false)
					$edit = false;
				else
					$status['localexten'] = 'edit';
			}
			else if($result['gfeatures']['number'] !== '')
			{
				$status['localexten'] = 'add';
	
				if($result['gfeatures']['context'] === '')
					$localextencontext = 'local-extensions';
				else
					$localextencontext = $result['gfeatures']['context'];
	
				if(($localextenid = $extensions->new_exten('macro',
							array('appdata' => 'supergroup'),
							$result['gfeatures']['number'],
							$localextencontext)) === false)
					$edit = false;
			}

			$exten_numbers = array();
			$exten_numbers['exten'] = $result['gfeatures']['number'];

			if($result['gfeatures']['context'] === '')
				$exten_numbers['context'] = 'local-extensions';
			else
				$exten_numbers['context'] = $result['gfeatures']['context'];

			$exten_where = array();
			$exten_where['exten'] = $info['gfeatures']['number'];

			if($info['gfeatures']['context'] === '')
				$exten_where['context'] = 'local-extensions';
			else
				$exten_where['context'] = $info['gfeatures']['context'];

			if(($info['extenumbers'] = $extenumbers->get_where($exten_where)) !== false)
			{
				if($result['gfeatures']['number'] === '')
					$status['extenumbers'] = 'delete';
				else
				{
					$status['extenumbers'] = 'edit';

					if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
					|| $extenumbers->exists($result['extenumbers'],$info['extenumbers']['id']) !== false)
					{
						$edit = false;
						$result['extenumbers'] = array_merge($info['extenumbers'],$extenumbers->get_filter_result());
					}
				}
			}
			else if($result['gfeatures']['number'] !== '')
			{
				$status['extenumbers'] = 'add';

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
				|| $extenumbers->exists($result['extenumbers']) !== false)
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
					$rs_localexten = $extensions->add_exten($localextenid);
					break;
				case 'edit':
					$rs_localexten = $extensions->edit($info['localexten']['id'],$localexten_edit);
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

			switch($status['extenumbers'])
			{
				case 'add':
					$rs_extenumbers = $extenumbers->add($result['extenumbers']);
					break;
				case 'edit':
					$rs_extenumbers = $extenumbers->edit($info['extenumbers']['id'],$result['extenumbers']);
					break;
				case 'delete':
					$rs_extenumbers = $extenumbers->delete($info['extenumbers']['id']);
					break;
				default:
					$rs_extenumbers = null;
			}

			if($rs_extenumbers === false)
			{
				$gfeatures->edit_origin();
				$queue->edit_origin();

				if($rs_localexten === null)
					break;

				switch($status['localexten'])
				{
					case 'add':
						$extensions->delete_exten($localextenid);
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
							$extensions->delete_exten($localextenid);
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
						$ugroup->delete_where(array('groupid' => $info['gfeatures']['id']));
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
							$extensions->delete_exten($localextenid);
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
						break 2;
					default:
						break 2;
				}
			}

			if($status['extenumbers'] === 'delete')
			{
				$incall = &$ipbx->get_module('incall');

				$incall->unlinked_where(array('type' => 'group',
							      'typeval' => $info['gfeatures']['id']));

				$schedule = &$ipbx->get_module('schedule');

				$schedule->unlinked_where(array('typetrue' => 'group',
								'typevaltrue' => $info['gfeatures']['id']));
	
				$schedule->unlinked_where(array('typefalse' => 'group',
				   				'typevalfalse' => $info['gfeatures']['id']));
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
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
		|| ($info['queue'] = $queue->get($info['gfeatures']['name'])) === false
		|| $qmember->get_nb(array('queue_name' => $info['queue']['name'])) !== 0)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		do
		{
			if($gfeatures->delete($info['gfeatures']['id']) === false)
				break;

			if($queue->delete($info['queue']['name']) === false)
			{
				$gfeatures->recover($info['gfeatures']['id']);
				break;
			}

			if($info['gfeatures']['context'] === '')
				$localextencontext = 'local-extensions';
			else
				$localextencontext = $info['gfeatures']['context'];

			if(($info['localexten'] = $extensions->get_exten('macro',
								 $info['gfeatures']['number'],
								 $localextencontext,
								 array('appdata' => 'supergroup'))) !== false
			&& $extensions->delete($info['localexten']['id']) === false)
			{
				$gfeatures->recover($info['gfeatures']['id']);
				$queue->add_origin();
				break;
			}

			$extenum_where = array();
			$extenum_where['exten'] = $info['gfeatures']['number'];
			$extenum_where['context'] = $localextencontext;

			if(($info['extenumbers'] = $extenumbers->get_where($extenum_where)) !== false)
			{
				if($extenumbers->delete($info['extenumbers']['id']) === false)
				{
					$gfeatures->recover($info['gfeatures']['id']);
					$queue->add_origin();

					if($info['localexten'] !== false)
						$extensions->add_origin();
					break;
				}

				$incall = &$ipbx->get_module('incall');

				$incall->unlinked_where(array('type' => 'group',
							      'typeval' => $info['gfeatures']['id']));

				$schedule = &$ipbx->get_module('schedule');

				$schedule->unlinked_where(array('typetrue' => 'group',
								'typevaltrue' => $info['gfeatures']['id']));

				$schedule->unlinked_where(array('typefalse' => 'group',
								'typevalfalse' => $info['gfeatures']['id']));
			}
		}
		while(false);

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
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
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
