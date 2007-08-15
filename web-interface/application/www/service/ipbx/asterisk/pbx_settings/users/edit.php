<?php

$ufeatures = &$ipbx->get_module('userfeatures');

$info = array();

$return = &$info;

if(isset($_QR['id']) === false
|| ($info['ufeatures'] = $ufeatures->get($_QR['id'])) === false
|| ($protocol = &$ipbx->get_protocol_module($info['ufeatures']['protocol'])) === false
|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$gfeatures = &$ipbx->get_module('groupfeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');
$musiconhold = &$ipbx->get_module('musiconhold');
$ugroup = &$ipbx->get_module('usergroup');
$qmember = &$ipbx->get_module('queuemember');
$voicemail = &$ipbx->get_module('uservoicemail');
$autoprov = &$ipbx->get_module('autoprov');

$gmember_slt = $gmember_unslt = false;

if(($groups = $ipbx->get_group_user($info['ufeatures']['id'])) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'queue','key' => 'name'));
	usort($groups,array(&$sort,'str_usort'));

	$gmember_slt = $gmember_unslt = array();

	$nb = count($groups);

	for($i = 0;$i < $nb;$i++)
	{
		$name = &$groups[$i]['queue']['name'];

		if($groups[$i]['member'] !== false)
			$gmember_slt[$name] = $groups[$i]['member'];
		else
			$gmember_unslt[$name] = $name;
	}

	if(empty($gmember_slt) === true)
		$gmember_slt = false;

	if(empty($gmember_unslt) === true)
	{
		$gmember_unslt = false;

		if($gmember_slt === false)
			$groups = false;
	}
}

$qmember_slt = $qmember_unslt = false;

if(($queues = $ipbx->get_queue_user($info['ufeatures']['id'])) !== false)
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

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

if(($autoprov_list = $autoprov->get_autoprov_list()) !== false)
	ksort($autoprov_list);

$info['usergroup'] = $ugroup->get_where(array('userid' => $info['ufeatures']['id']));
$info['voicemail'] = $voicemail->get_where(array(
					'mailbox' => $info['ufeatures']['number'],
					'context' => $info['ufeatures']['context']));
$info['autoprov'] = $autoprov->get_where(array('iduserfeatures' => $info['ufeatures']['id']));

$allow = $info['protocol']['allow'];

$status = $result = array();
$status['localexten'] = $status['extenumbers'] = $status['hints'] = false;
$status['queue'] = $status['group'] = $status['usergroup'] = $status['voicemail'] = false;

$edit = true;

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('protocol',$_QR) === false || xivo_issa('ufeatures',$_QR) === false)
		break;

	$return = &$result;

	if($moh_list === false || isset($_QR['ufeatures']['musiconhold'],$moh_list[$_QR['ufeatures']['musiconhold']]) === false)
		$_QR['ufeatures']['musiconhold'] = '';
		
	if(xivo_issa('allow',$_QR['protocol']) === false)
		unset($_QR['protocol']['allow'],$_QR['protocol']['disallow']);

	if($info['ufeatures']['protocol'] !== $_QR['protocol']['protocol'])
	{
		$chg_protocol = $provisioning = true;
		$old_protocol = $protocol;

		if(($protocol = &$ipbx->get_protocol_module($_QR['protocol']['protocol'])) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
	}
	else
	{
		$chg_protocol = $provisioning = false;
		$old_protocol = &$protocol;
	}

	if(($result['protocol'] = $protocol->chk_values($_QR['protocol'])) === false)
	{
		$edit = false;
		$result['protocol'] = array_merge($info['protocol'],$protocol->get_filter_result());
	}
	
	if(is_array($result['protocol']['allow']) === true)
	{
		$allow = $result['protocol']['allow'];
		$result['protocol']['allow'] = implode(',',$result['protocol']['allow']);
	}

	$_QR['ufeatures']['name'] = $result['protocol']['name'];
	$_QR['ufeatures']['context'] = $result['protocol']['context'];
	$_QR['ufeatures']['protocol'] = $_QR['protocol']['protocol'];
	$_QR['ufeatures']['protocolid'] = 0;

	if(($result['ufeatures'] = $ufeatures->chk_values($_QR['ufeatures'])) === false)
	{
		$edit = false;
		$result['ufeatures'] = array_merge($info['ufeatures'],$ufeatures->get_filter_result());
	}

	$result['protocol']['mailbox'] = $result['ufeatures']['number'];

	if($edit === true)
	{
		$exten_where = array();
		$exten_where['exten'] = $info['ufeatures']['number'];
		$exten_where['app'] = 'Macro';
		$exten_where['appdata'] = 'superuser';

		if($info['protocol']['context'] === '')
			$exten_where['context'] = 'local-extensions';
		else
			$exten_where['context'] = $info['protocol']['context'];

		$localexten = $extensions;

		if(($info['localexten'] = $localexten->get_where($exten_where)) !== false)
		{
			if($result['ufeatures']['number'] === '')
				$status['localexten'] = 'delete';
			else
			{
				$status['localexten'] = 'edit';

				$local_exten = $info['localexten'];
				$local_exten['exten'] = $result['ufeatures']['number'];

				if($result['protocol']['context'] === '')
					$local_exten['context'] = 'local-extensions';
				else
					$local_exten['context'] = $result['protocol']['context'];

				if(($result['localexten'] = $localexten->chk_values($local_exten)) === false)
				{
					$edit = false;
					$result['localexten'] = array_merge($info['localexten'],$localexten->get_filter_result());
				}
			}
		}
		else if($result['ufeatures']['number'] !== '')
		{
			$status['localexten'] = 'add';

			$local_exten = $exten_where;
			$local_exten['exten'] = $result['ufeatures']['number'];
			$local_exten['priority'] = 1;

			if($result['protocol']['context'] === '')
				$local_exten['context'] = 'local-extensions';
			else
				$local_exten['context'] = $result['protocol']['context'];

			if(($result['localexten'] = $localexten->chk_values($local_exten)) === false)
			{
				$edit = false;
				$result['localexten'] = $localexten->get_filter_result();
			}
		}

		$exten_numbers = array();
		$exten_numbers['number'] = $result['ufeatures']['number'];

		if($result['protocol']['context'] === '')
			$exten_numbers['context'] = 'local-extensions';
		else
			$exten_numbers['context'] = $result['protocol']['context'];

		$exten_where = array();
		$exten_where['number'] = $info['ufeatures']['number'];

		if($info['protocol']['context'] === '')
			$exten_where['context'] = 'local-extensions';
		else
			$exten_where['context'] = $info['protocol']['context'];

		if(($info['extenumbers'] = $extenumbers->get_where($exten_where)) !== false)
		{
			if($result['ufeatures']['number'] === '')
				$status['extenumbers'] = 'delete';
			else
			{
				$status['extenumbers'] = 'edit';

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false
				|| (($extenum = $extenumbers->get_where($result['extenumbers'])) !== false
				   && (int) $extenum['id'] !== (int) $info['extenumbers']['id']) === true)
				{
					$edit = false;
					$result['extenumbers'] = array_merge($info['extenumbers'],$extenumbers->get_filter_result());
				}
			}
		}
		else if($result['ufeatures']['number'] !== '')
		{
			$status['extenumbers'] = 'add';

			if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers)) === false)
			{
				$edit = false;
				$result['extenumbers'] = $extenumbers->get_filter_result();
			}
		}

		$callerid = '';

		if($result['ufeatures']['number'] === '')
		{
			if($info['ufeatures']['number'] !== '')
			{
				$callerid = preg_replace('/<'.preg_quote($info['ufeatures']['number']).'>$/','',$result['protocol']['callerid']);
				$result['protocol']['callerid'] = trim($callerid);
			}
		}
		else
		{
			if($info['ufeatures']['number'] === '')
				$callerid = $result['protocol']['callerid'];
			else
			{
				$callerid = preg_replace('/<'.preg_quote($info['ufeatures']['number']).'>$/','',$result['protocol']['callerid']);
				$callerid = trim($callerid);
			}

			$callerid .= ' <'.$result['ufeatures']['number'].'>';
		}
			
		if(($result['protocol']['callerid'] = $protocol->set_chk_value('callerid',$callerid)) === false)
		{
			$edit = false;
			$result['protocol']['callerid'] = '';
		}

		$exten_where = array();
		$exten_where['app'] = $ipbx->mk_interface($info['protocol']['name'],$info['ufeatures']['protocol']);

		if(($hints_interface = $ipbx->mk_interface($result['protocol']['name'],$result['ufeatures']['protocol'])) !== false)
		{
			$exten_where['context'] = 'hints';
			$exten_where['exten'] = $info['ufeatures']['number'];

			$hintsexten = $extensions;

			if(($info['hints'] = $hintsexten->get_where($exten_where)) !== false)
			{
				$status['hints'] = 'edit';

				$hints = $info['hints'];
				$hints['exten'] = $result['ufeatures']['number'];
				$hints['app'] = $hints_interface;

				if($result['ufeatures']['number'] === '')
					$status['hints'] = 'delete';
				else if(($result['hints'] = $hintsexten->chk_values($hints)) === false)
				{
					$edit = false;
					$result['hints'] = $hintsexten->get_filter_result();
				}
			}
			else if($result['ufeatures']['number'] !== '')
			{
				$status['hints'] = 'add';

				$hints = $exten_where;
				$hints['exten'] = $result['ufeatures']['number'];
				$hints['priority'] = -1;
				$hints['app'] = $hints_interface;

				if(($result['hints'] = $hintsexten->chk_values($hints)) === false)
				{
					$edit = false;
					$result['hints'] = $hintsexten->get_filter_result();
				}
			}
		}
	}

	$ugroup_where = array(
			'usertype' => 'user',
			'userid' => $info['ufeatures']['id'],
			'category' => 'group');

	if(isset($_QR['usergroup']) === false || xivo_bool($result['ufeatures']['ringgroup']) === false)
		$_QR['usergroup'] = false;

	$usergroup = false;
	$callgroup = array();
	$group_add = $group_edit = $group_del = $group_tmp = array();

	if(($group_slt = xivo_issa_val('group-select',$_QR)) !== false && xivo_issa('group',$_QR) !== false)
	{
		$nb = count($group_slt);
		$ugroup_info = $ugroup_where;

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$group_slt[$i];
			$ugroup_info['queue_name'] = $qname;

			if(isset($group_tmp[$qname]) === true || isset($_QR['group'][$qname]) === false
			|| ($chantype = xivo_ak('chantype',$_QR['group'][$qname],true)) === false
			|| ($chantype !== 'default' && $chantype !== XIVO_SRE_IPBX_AST_CHAN_LOCAL) === true
			|| ($chantype === XIVO_SRE_IPBX_AST_CHAN_LOCAL
			   && $result['ufeatures']['number'] === '') === true
			|| ($gid = $gfeatures->get_id(array('name' => $qname))) === false)
			{
				if(isset($gmember_slt[$qname]) === true)
				{
					$status['group'] = true;
					$group_tmp[$qname] = 1;
					$group_del[] = $ugroup_info;
				}
				continue;
			}
		
			if(isset($gmember_unslt[$qname]) === true)
				$ref_group = &$group_add;
			else if(isset($gmember_slt[$qname]) === true)
				$ref_group = &$group_edit;
			else
				continue;

			if($chantype === 'default')
			{
				$ugroup_info['interface'] = $ipbx->mk_interface(
								$result['protocol']['name'],
								$result['ufeatures']['protocol']);

				$ugroup_info['channel'] = $ipbx->get_channel_by_protocol($result['ufeatures']['protocol']);
			}
			else if($chantype === XIVO_SRE_IPBX_AST_CHAN_LOCAL)
			{
				$ugroup_info['interface'] = $ipbx->mk_interface(
								$result['protocol']['name'],
								null,
								$result['ufeatures']['number'],
								$result['ufeatures']['context']);

				$ugroup_info['channel'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
			}

			$ugroup_tmp = array_merge($_QR['group'][$qname],$ugroup_info);

			if(($ginfo = $qmember->chk_values($ugroup_tmp)) === false)
				continue;

			$status['group'] = true;
			$group_tmp[$qname] = 1;
			$ref_group[] = $ginfo;
			$callgroup[] = $gid;
			
			if($_QR['usergroup'] !== false && $_QR['usergroup'] === $qname)
				$usergroup = $gid;
		}
	}

	if($gmember_slt !== false)
	{
		$group_slt = array_keys($gmember_slt);
		$ugroup_info = $ugroup_where;

		$nb = count($group_slt);

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$group_slt[$i];
			if(isset($group_tmp[$qname]) === true)
				continue;

			$status['group'] = true;
			$ugroup_info['queue_name'] = $qname;
			$group_del[] = $ugroup_info;
		}
	}

	if($info['usergroup'] !== false)
	{
		$result['usergroup'] = $info['usergroup'];
		$result['usergroup']['groupid'] = $usergroup;

		if($usergroup === false)
			$status['usergroup'] = 'delete';
		else if(($result['usergroup'] = $ugroup->chk_values($result['usergroup'])) === false)
			$result['usergroup'] = array_merge($info['usergroup'],$ugroup->get_filter_result());
		else
			$status['usergroup'] = 'edit';
	}
	else if($usergroup !== false)
	{
		$result['usergroup'] = array();
		$result['usergroup']['userid'] = $info['ufeatures']['id'];
		$result['usergroup']['groupid'] = $usergroup;

		if(($result['usergroup'] = $ugroup->chk_values($result['usergroup'])) === false)
			$result['usergroup'] = $ugroup->get_filter_result();
		else
			$status['usergroup'] = 'add';
	}

	$uqueue_where = array(
			'usertype' => 'user',
			'userid' => $info['ufeatures']['id'],
			'category' => 'queue');

	$queue_add = $queue_edit = $queue_del = $queue_tmp = array();

	if(($queue_slt = xivo_issa_val('queue-select',$_QR)) !== false && xivo_issa('queue',$_QR) !== false)
	{
		$nb = count($queue_slt);
		$uqueue_info = $uqueue_where;

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$queue_slt[$i];
			$uqueue_info['queue_name'] = $qname;

			if(isset($queue_tmp[$qname]) === true
			|| isset($_QR['queue'][$qname],$_QR['queue'][$qname]['chantype']) === false
			|| ($_QR['queue'][$qname]['chantype'] !== 'default'
			   && $_QR['queue'][$qname]['chantype'] !== XIVO_SRE_IPBX_AST_CHAN_LOCAL) === true
			|| ($_QR['queue'][$qname]['chantype'] === XIVO_SRE_IPBX_AST_CHAN_LOCAL
			   && $result['ufeatures']['number'] === '') === true)
			{
				if(isset($qmember_slt[$qname]) === true)
				{
					$status['queue'] = true;
					$queue_tmp[$qname] = 1;
					$queue_del[] = $uqueue_info;
				}
				continue;
			}

			$chantype = &$_QR['queue'][$qname]['chantype'];

			if(isset($qmember_unslt[$qname]) === true)
				$ref_queue = &$queue_add;
			else if(isset($qmember_slt[$qname]) === true)
				$ref_queue = &$queue_edit;
			else
				continue;

			if($chantype === 'default')
			{
				$uqueue_info['interface'] = $ipbx->mk_interface(
								$result['protocol']['name'],
								$result['ufeatures']['protocol']);

				$uqueue_info['channel'] = $ipbx->get_channel_by_protocol($result['ufeatures']['protocol']);
			}
			else if($chantype === XIVO_SRE_IPBX_AST_CHAN_LOCAL)
			{
				$uqueue_info['interface'] = $ipbx->mk_interface(
								$result['protocol']['name'],
								null,
								$result['ufeatures']['number'],
								$result['ufeatures']['context']);

				$uqueue_info['channel'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
			}

			$uqueue_tmp = array_merge($_QR['queue'][$qname],$uqueue_info);

			if(($qinfo = $qmember->chk_values($uqueue_tmp)) === false)
				continue;

			$status['queue'] = true;
			$queue_tmp[$qname] = 1;
			$ref_queue[] = $qinfo;
		}
	}

	if($qmember_slt !== false)
	{
		$queue_slt = array_keys($qmember_slt);
		$uqueue_info = $uqueue_where;

		$nb = count($queue_slt);

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$queue_slt[$i];
			if(isset($queue_tmp[$qname]) === true)
				continue;

			$status['queue'] = true;
			$uqueue_info['queue_name'] = $qname;
			$queue_del[] = $uqueue_info;
		}
	}

	if($info['voicemail'] !== false)
	{
		$result['voicemail'] = $info['voicemail'];
		$result['voicemail']['commented'] = 1;

		if($result['ufeatures']['number'] === '')
			$status['voicemail'] = 'delete';
		else if(xivo_issa('voicemail',$_QR) === false)
			$status['voicemail'] = 'disable';
		else
		{
			$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];
			$_QR['voicemail']['context'] = $result['ufeatures']['context'];

			if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'])) === false)
				$result['voicemail'] = array_merge($info['voicemail'],$voicemail->get_filter_result());
			else
				$status['voicemail'] = 'edit';

			$result['voicemail']['commented'] = 0;
		}
	}
	else if($result['ufeatures']['number'] !== '' && xivo_issa('voicemail',$_QR) === true)
	{
		$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];
		$_QR['voicemail']['context'] = $result['ufeatures']['context'];

		if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'])) === false)
			$result['voicemail'] = $voicemail->get_filter_result();
		else
			$status['voicemail'] = 'add';
	}

	$send_autoprov = false;

	if($info['autoprov'] !== false)
	{
		if(xivo_issa('autoprov',$_QR) === true && isset($_QR['autoprov']['modact']) === true)
		{
			$aprovinfo = $info['autoprov'];
			$aprovinfo['modact'] = $_QR['autoprov']['modact'];
			$aprovinfo['proto'] = $result['ufeatures']['protocol'];

			if(($result['autoprov'] = $autoprov->chk_values($aprovinfo)) === false)
				$result['autoprov'] = $autoprov->get_filter_result();
			else
				$send_autoprov = true;
		}
	}
	else if(xivo_issa('autoprov',$_QR) === true && is_array($autoprov_list) === true
	&& isset($_QR['autoprov']['vendormodel'],$_QR['autoprov']['macaddr'],$_QR['autoprov']['modact']) === true)
	{
		if(($pos = strpos($_QR['autoprov']['vendormodel'],'.')) === false)
			$_QR['autoprov']['vendormodel'] = '';
		else
		{
			$vendor = substr($_QR['autoprov']['vendormodel'],0,$pos);
			$model = substr($_QR['autoprov']['vendormodel'],$pos+1);

			if(xivo_issa($vendor,$autoprov_list) === true
			&& xivo_issa('model',$autoprov_list[$vendor]) === true
			&& xivo_issa($model,$autoprov_list[$vendor]['model']) === true)
			{
				$_QR['autoprov']['vendor'] = $vendor;
				$_QR['autoprov']['model'] = $model;
			}
		}

		if(preg_match_all('/0[-: ]{1}|([A-F0-9]{2})[-: ]?/i',$_QR['autoprov']['macaddr'],$match) >= 6)
		{
			$_QR['autoprov']['macaddr'] = '';

			for($i = 0;$i < 6;$i++)
			{
				if($match[1][$i] === '')
					$match[1][$i] = '00';

				$_QR['autoprov']['macaddr'] .= ':'.strtoupper($match[1][$i]);
			}

			$_QR['autoprov']['macaddr'] = substr($_QR['autoprov']['macaddr'],1);
		}

		$_QR['autoprov']['proto'] = $result['ufeatures']['protocol'];
		$_QR['autoprov']['iduserfeatures'] = $info['ufeatures']['id'];
		
		if(($result['autoprov'] = $autoprov->chk_values($_QR['autoprov'])) === false)
			$result['autoprov'] = $autoprov->get_filter_result();
		else
			$send_autoprov = true;
	}

	if($edit === false)
		break;

	if($chg_protocol === true)
	{
		if(($protocolid = $protocol->add($result['protocol'])) === false)
			break;

		$old_protocol->delete($info['protocol']['id']);
	}
	else
	{
		$protocolid = $info['protocol']['id'];
		
		if($protocol->edit($protocolid,$result['protocol']) === false)
			break;
	}

	$result['ufeatures']['protocolid'] = $protocolid;

	if($ufeatures->edit($info['ufeatures']['id'],$result['ufeatures'],$provisioning) === false)
	{
		if($chg_protocol === false)
			$protocol->edit_origin();
		else
		{
			$protocol->delete($protocolid);
			$old_protocol->add_origin();
		}
		break;
	}

	switch($status['localexten'])
	{
		case 'add':
			$rs_localexten = $localexten->add($result['localexten']);
			break;
		case 'edit':
			$rs_localexten = $localexten->edit($info['localexten']['id'],$result['localexten']);
			break;
		case 'delete':
			$rs_localexten = $localexten->delete($info['localexten']['id']);
			break;
		default:
			$rs_localexten = null;
	}

	if($rs_localexten === false)
	{
		if($chg_protocol === false)
			$protocol->edit_origin();
		else
		{
			$protocol->delete($protocolid);
			$old_protocol->add_origin();
		}

		$ufeatures->edit_origin();
					
		break;
	}

	$rs_dfeatures = null;

	$dfeatures = &$ipbx->get_module('didfeatures');
	$dfeatures_where = array();
	$dfeatures_where['type'] = 'user';
	$dfeatures_where['typeid'] = $info['ufeatures']['id'];
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
		if($chg_protocol === false)
			$protocol->edit_origin();
		else
		{
			$protocol->delete($protocolid);
			$old_protocol->add_origin();
		}

		$ufeatures->edit_origin();

		if($rs_dfeatures === false)
			$extenumbers->add_origin();

		if($rs_localexten === null)
			break;

		switch($status['localexten'])
		{
			case 'add':
				$localexten->delete($rs_localexten);
				break 2;
			case 'edit':
				$localexten->edit_origin();
				break 2;
			case 'delete':
				$localexten->add_origin();
				break 2;
			default:
				break 2;
		}
	}

	switch($status['hints'])
	{
		case 'add':
			$rs_hints = $hintsexten->add($result['hints']);
			break;
		case 'edit':
			$rs_hints = $hintsexten->edit($info['hints']['id'],$result['hints']);
			break;
		case 'delete':
			$rs_hints = $hintsexten->delete($info['hints']['id']);
			break;
		default:
			$rs_hints = null;
	}

	if($rs_hints === false)
	{
		if($chg_protocol === false)
			$protocol->edit_origin();
		else
		{
			$protocol->delete($protocolid);
			$old_protocol->add_origin();
		}

		$ufeatures->edit_origin();

		if($rs_localexten !== null)
		{
			switch($status['localexten'])
			{
				case 'add':
					$localexten->delete($rs_localexten);
					break;
				case 'edit':
					$localexten->edit_origin();
					break;
				case 'delete':
					$localexten->add_origin();
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

	if(isset($callgroup[0]) === false)
		$callgroup_list = '';
	else
		$callgroup_list = implode(',',$callgroup);

	if($protocol->edit($protocolid,array('callgroup' => $callgroup_list)) === false)
	{
		$callgroup = array();
		$status['group'] = false;
		$status['usergroup'] = 'delete';
	}

	if($status['group'] === true)
	{
		if(($nb = count($group_del)) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$ugroup_where['queue_name'] = $group_del[$i]['queue_name'];
				$qmember->delete_where($ugroup_where);
			}
		}

		if(isset($group_add[0]) === true)
			$qmember->add_list($group_add);

		if(($nb = count($group_edit)) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$ugroup_where['queue_name'] = $group_edit[$i]['queue_name'];
				$qmember->edit_where($ugroup_where,$group_edit[$i]);
			}
		}
	}

	switch($status['usergroup'])
	{
		case 'add':
			if(in_array($result['usergroup']['groupid'],$callgroup) === true)
				$ugroup->add($result['usergroup']);
			break;
		case 'edit':
			if(in_array($result['usergroup']['groupid'],$callgroup) === true)
				$ugroup->edit($info['usergroup']['id'],$result['usergroup']);
			else
				$ugroup->delete($info['usergroup']['id']);
			break;
		case 'delete':
			$ugroup->delete($info['usergroup']['id']);
			break;
	}

	if($status['queue'] === true)
	{
		if(($nb = count($queue_del)) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$uqueue_where['queue_name'] = $queue_del[$i]['queue_name'];
				$qmember->delete_where($uqueue_where);
			}
		}

		if(isset($queue_add[0]) === true)
			$qmember->add_list($queue_add);

		if(($nb = count($queue_edit)) !== 0)
		{
			for($i = 0;$i < $nb;$i++)
			{
				$uqueue_where['queue_name'] = $queue_edit[$i]['queue_name'];
				$qmember->edit_where($uqueue_where,$queue_edit[$i]);
			}
		}
	}

	switch($status['voicemail'])
	{
		case 'add':
			$voicemail->add($result['voicemail']);
			break;
		case 'edit':
			$result['voicemail']['commented'] = 0;
			$voicemail->edit($info['voicemail']['id'],$result['voicemail']);
			break;
		case 'delete':
			$voicemail->delete($info['voicemail']['id']);
			break;
		case 'disable':
			$voicemail->disable($info['voicemail']['id'],true);
			break;
	}

	if($send_autoprov === true)
	{
		if($info['autoprov'] !== false && (bool) $info['autoprov']['isinalan'] === true)
			$autoprov->notification($result['autoprov'],$_QR['autoprov']['modact']);
		else
			$autoprov->authoritative($result['autoprov'],$_QR['autoprov']['modact']);
	}

	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
}
while(false);

$element = array();
$element['protocol'] = $ipbx->get_protocol_element();
$element['ufeatures'] = $ufeatures->get_element();
$element['voicemail'] = $voicemail->get_element();
$element['autoprov'] = $autoprov->get_element();
$element['qmember'] = $qmember->get_element();

if(xivo_issa('allow',$element['protocol']['sip']) === true && xivo_issa('value',$element['protocol']['sip']['allow']) === true)
{
	if(empty($allow) === false)
	{
		if(is_array($allow) === false)
			$allow = explode(',',$allow);

		$element['protocol']['sip']['allow']['value'] = array_diff($element['protocol']['sip']['allow']['value'],$allow);
	}
}

if(xivo_issa('allow',$element['protocol']['iax']) === true && xivo_issa('value',$element['protocol']['iax']['allow']) === true)
{
	if(empty($allow) === false)
	{
		if(is_array($allow) === false)
			$allow = explode(',',$allow);

		$element['protocol']['iax']['allow']['value'] = array_diff($element['protocol']['iax']['allow']['value'],$allow);
	}
}

if(isset($return['usergroup']) === false || empty($return['usergroup']) === true)
	$return['usergroup'] = null;

if(isset($return['voicemail']) === false || empty($return['voicemail']) === true)
	$return['voicemail'] = null;

if(isset($return['autoprov']) === false || empty($return['autoprov']) === true)
	$return['autoprov'] = null;

$return['protocol']['allow'] = $allow;

$_HTML->assign('id',$info['ufeatures']['id']);
$_HTML->assign('info',$return);
$_HTML->assign('ract',$act);
$_HTML->assign('groups',$groups);
$_HTML->assign('gmember_slt',$gmember_slt);
$_HTML->assign('gmember_unslt',$gmember_unslt);
$_HTML->assign('queues',$queues);
$_HTML->assign('qmember_slt',$qmember_slt);
$_HTML->assign('qmember_unslt',$qmember_unslt);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('element',$element);
$_HTML->assign('moh_list',$moh_list);
$_HTML->assign('autoprov_list',$autoprov_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

?>
