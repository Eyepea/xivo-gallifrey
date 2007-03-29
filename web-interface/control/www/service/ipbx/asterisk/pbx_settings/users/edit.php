<?php

$ufeatures = &$ipbx->get_module('userfeatures');

$info = array();

if(isset($_QR['id']) === false || ($info['ufeatures'] = $ufeatures->get($_QR['id'])) === false
|| ($protocol = &$ipbx->get_protocol_module($info['ufeatures']['protocol'])) === false
|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');

$gfeatures = &$ipbx->get_module('groupfeatures');
$voicemail = &$ipbx->get_module('uservoicemail');
$extensions = &$ipbx->get_module('extensions');
$musiconhold = &$ipbx->get_module('musiconhold');
$ugroup = &$ipbx->get_module('usergroup');

if(($moh_list = $musiconhold->get_all_category()) !== false)
	ksort($moh_list);

$info['voicemail'] = $voicemail->get_by_mailbox($info['protocol']['mailbox']);
$info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id']);

$localexten_status = false;

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('protocol',$_QR) === false || xivo_issa('ufeatures',$_QR) === false)
		break;

	if($moh_list === false || isset($_QR['ufeatures']['musiconhold'],$moh_list[$_QR['ufeatures']['musiconhold']]) === false)
		$_QR['ufeatures']['musiconhold'] = '';

	if(xivo_issa('allow',$_QR['protocol']) === false)
		unset($_QR['protocol']['allow'],$_QR['protocol']['disallow']);

	if($info['ufeatures']['protocol'] !== $_QR['protocol']['protocol'])
	{
		$chg_protocol = $provisioning = true;
		$old_protocol = $protocol;

		if(($protocol = &$ipbx->get_protocol_module($_QR['protocol']['protocol'])) === false)
			xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');
	}
	else
	{
		$chg_protocol = $provisioning = false;
		$old_protocol = &$protocol;
	}

	if(($result['protocol'] = $protocol->chk_values($_QR['protocol'],true,true)) === false)
	{
		$info['protocol'] = array_merge($info['protocol'],$protocol->get_filter_result());
		break;
	}

	if(is_array($result['protocol']['allow']) === true)
		$result['protocol']['allow'] = implode(',',$result['protocol']['allow']);

	if($chg_protocol === true)
	{
		if(($pid = $protocol->add($result['protocol'])) === false)
			break;
	}
	else
	{
		$pid = $info['protocol']['id'];
		if($protocol->edit($pid,$result['protocol']) === false)
			break;
	}

	$_QR['ufeatures']['protocol'] = $_QR['protocol']['protocol'];
	$_QR['ufeatures']['protocolid'] = $pid;
	$_QR['ufeatures']['name'] = $result['protocol']['name'];

	if(($result['ufeatures'] = $ufeatures->chk_values($_QR['ufeatures'],true,true)) === false
	|| $ufeatures->edit($info['ufeatures']['id'],$result['ufeatures'],$provisioning) === false)
	{
		$info['ufeatures'] = array_merge($info['ufeatures'],$ufeatures->get_filter_result());

		if($chg_protocol === true)
			$protocol->delete($pid);
		else
			$protocol->edit_origin();
		break;
	}

	$exten_where = array();
	$exten_where['exten'] = $info['ufeatures']['number'];
	$exten_where['app'] = 'Macro';
	$exten_where['appdata'] = 'superuser';

	if($info['protocol']['context'] === '')
		$exten_where['context'] = 'local-extensions';
	else
		$exten_where['context'] = $info['protocol']['context'];

	$info['extensions'] = $extensions->get_where($exten_where);

	if($result['ufeatures']['number'] === '')
	{
		if($info['ufeatures']['number'] !== '')
		{
			$callerid = preg_replace('/<'.preg_quote($info['ufeatures']['number']).'>$/','',$result['protocol']['callerid']);
			$protocol->edit($pid,array('callerid' => trim($callerid)));

			if($info['extensions'] !== false && $extensions->delete($info['extensions']['id']) !== false)
				$localexten_status = 'delete';
		}
	}
	else
	{
		if($info['extensions'] === false)
		{
			$local_exten = $exten_where;
			$local_exten['exten'] = $result['ufeatures']['number'];
			$local_exten['priority'] = 1;

			if($result['protocol']['context'] === '')
				$local_exten['context'] = 'local-extensions';
			else
				$local_exten['context'] = $result['protocol']['context'];

			if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
			|| ($local_extenid = $extensions->add($result['local_exten'])) === false)
			{
				$ufeatures->edit_origin();

				if($chg_protocol === true)
					$protocol->delete($pid);
				else
					$protocol->edit_origin();
				break;
			}

			$localexten_status = 'add';
		}
		else
		{
			$local_exten = $info['extensions'];
			$local_exten['exten'] = $result['ufeatures']['number'];

			if($result['protocol']['context'] === '')
				$local_exten['context'] = 'local-extensions';
			else
				$local_exten['context'] = $result['protocol']['context'];

			if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
			|| $extensions->edit($info['extensions']['id'],$result['local_exten']) === false)
			{
				$ufeatures->edit_origin();

				if($chg_protocol === true)
					$protocol->delete($pid);
				else
					$protocol->edit_origin();
				break;
			}

			$localexten_status = 'edit';
		}

		if($info['ufeatures']['number'] === '')
			$callerid = $result['protocol']['callerid'];
		else
		{
			$callerid = preg_replace('/<'.preg_quote($info['ufeatures']['number']).'>$/','',$result['protocol']['callerid']);
			$callerid = trim($callerid);
		}

		$callerid .= ' <'.$result['ufeatures']['number'].'>';

		if(($callerid = $protocol->set_chk_value('callerid',$callerid)) === false
		|| $protocol->edit($pid,array('callerid' => $callerid)) === false)
		{
			$ufeatures->edit_origin();

			switch($localexten_status)
			{
				case 'add':
					$extensions->delete($local_extenid);
					break;
				case 'edit':
					$extensions->edit_origin();
					break;
				case 'delete':
					$extensions->add_origin();
					break;
			}

			if($chg_protocol === true)
				$protocol->delete($pid);
			else
				$protocol->edit_origin();
			break;
		}
	}

	if($chg_protocol === true)
		$old_protocol->delete($info['protocol']['id']);

	// while waiting protocol[mailbox]
	$protocol->edit($pid,array('mailbox' => $result['ufeatures']['number']));

	$exten_where = array();
	$exten_where['app'] = $ipbx->mk_interface($info['protocol']['name'],$info['ufeatures']['protocol']);

	if(($hints_interface = $ipbx->mk_interface($result['protocol']['name'],$result['ufeatures']['protocol'])) !== false)
	{
		$exten_where['context'] = 'hints';
		$exten_where['exten'] = $info['ufeatures']['number'];

		if(($info['extensions'] = $extensions->get_where($exten_where)) !== false)
		{
			$hints = $info['extensions'];
			$hints['exten'] = $result['ufeatures']['number'];
			$hints['app'] = $hints_interface;

			if($result['ufeatures']['number'] === '')
				$extensions->delete($info['extensions']['id']);
			else if(($result['hints'] = $extensions->chk_values($hints,true,true)) !== false)
				$extensions->edit($info['extensions']['id'],$result['hints']);
		}
		else if($result['ufeatures']['number'] !== '')
		{
			$hints = $exten_where;
			$hints['exten'] = $result['ufeatures']['number'];
			$hints['priority'] = -1;
			$hints['app'] = $hints_interface;

			if(($result['hints'] = $extensions->chk_values($hints,true,true)) !== false)
				$extensions->add($result['hints']);
		}
	}

	do
	{
		if(($interface = $ipbx->mk_interface($info['protocol']['name'],
						     $info['ufeatures']['protocol'],
					     	     $info['ufeatures']['number'],
					     	     $info['protocol']['context'])) === false)
		{
			$protocol->edit($pid,array('callgroup' => ''));

			if($info['usergroup'] !== false)
				$ugroup->delete($info['usergroup']['id']);
			break;
		}
		
		$new_interface = $ipbx->mk_interface($result['protocol']['name'],
						     $result['ufeatures']['protocol'],
					     	     $result['ufeatures']['number'],
					     	     $result['protocol']['context']);

		$qmember = &$ipbx->get_module('queuemember');

		if(xivo_issa('group',$_QR) === false)
			$_QR['group'] = array();

		$arr_group = array_values($_QR['group']);
		$groups = '';

		if($chg_protocol === true)
			$qmember->delete_by_interface($interface);

		if(($nb = count($arr_group)) === 0 && $protocol->edit($pid,array('callgroup' => '')) !== false)
		{
			$qmember->delete_by_interface($interface);

			if($info['usergroup'] !== false)
				$ugroup->delete($info['usergroup']['id']);
			break;
		}

		$qm_list = $qmember->get_list_by_interface($interface);

		if($new_interface === false)
			break;

		if($qm_list !== false && $qmember->delete_by_interface($interface) === false)
		{
			$protocol->edit($pid,array('callgroup' => $info['protocol']['callgroup'])); 
			break;
		}

		$mqueues = array();

		$_QR['usergroup'] = isset($_QR['usergroup'],$result['ufeatures']['ringgroup']) === true ? $_QR['usergroup'] : false;
		$usergroup = false;

		for($i = 0;$i < $nb;$i++)
		{
			if(($ginfo = $gfeatures->get($arr_group[$i],false)) === false)
				continue;

			$mqueues = array('queue_name'	=> $ginfo['name'],
					'interface'	=> $new_interface,
					'call_limit'	=> $result['ufeatures']['simultcalls']);

			if($qmember->add($mqueues) === false)
			{
				$qmember->delete_by_interface($new_interface);

				if($qm_list !== false)
					$qmember->add_list($qm_list);
				break 2;
			}

			$groups .= ','.$ginfo['id'];

			if($_QR['usergroup'] !== false && $_QR['usergroup'] === $ginfo['id'])
				$usergroup = $ginfo['id'];
		}

		if($groups === '')
		{
			$protocol->edit($pid,array('callgroup' => ''));

			if($info['usergroup'] !== false)
				$ugroup->delete($info['usergroup']['id']);
			break;
		}

		$groups = substr($groups,1);

		if($protocol->edit($pid,array('callgroup' => $groups)) === false)
		{
			$qmember->delete_by_interface($new_interface);

			if($qm_list !== false)
				$qmember->add_list($qm_list);
			break;
		}

		if($usergroup === false)
			break;

		if($info['usergroup'] === false)
			$ugroup->add(array('userid' => $info['ufeatures']['id'],'groupid' => $usergroup));
		else
		{
			$info['usergroup']['groupid'] = $usergroup;
			$ugroup->edit($info['usergroup']['id'],$info['usergroup']);
		}
	}
	while(false);

	if($info['voicemail'] !== false)
	{
		if($result['ufeatures']['number'] === '')
			$voicemail->delete($info['voicemail']['id']);
		else if(xivo_issa('voicemail',$_QR) === false)
			$voicemail->disable($info['voicemail']['id'],true);
		else
		{
			$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];

			if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'],true,true)) === false
			|| $voicemail->edit($info['voicemail']['id'],$result['voicemail'],false) === false)
				$info['voicemail'] = array_merge($info['voicemail'],$voicemail->get_filter_result());
		}
	}
	else if($result['ufeatures']['number'] !== '' && xivo_issa('voicemail',$_QR) === true)
	{
		$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];

		if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'],true,true)) === false
		|| $voicemail->add($result['voicemail']) === false)
			$info['voicemail'] = $voicemail->get_filter_result();
	}

	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');
}
while(false);

$info['protocol']['group'] = array();

$group = false;

if(($group_list = $gfeatures->get_list(false)) !== false)
{
	$group = true;

	if(xivo_ak(0,$info['protocol']['callgroup']) === true)
	{
		$gelt = array_values(array_diff($group_list,$info['protocol']['callgroup']));
		$grp = array_values(array_diff($info['protocol']['callgroup'],$gelt));

		if(isset($grp[0]) === false)
			$grp = $group_list;

		$nb = count($grp);

		for($i = 0;$i < $nb;$i++)
		{
			if(($ginfo = $gfeatures->get($grp[$i],false)) === false)
				continue;

			$info['protocol']['group'][] = $ginfo;
		}
	}
	else
		$gelt = $group_list;


	$group_list = array();

	$nb = count($gelt);

	for($i = 0;$i < $nb;$i++)
	{
		if(($ginfo = $gfeatures->get($gelt[$i],false)) === false)
			continue;
		
		$group_list[] = $ginfo;
	}
}

$protocol_elt = $ipbx->get_protocol_element();

if(xivo_issa('allow',$protocol_elt['sip']) === true && xivo_issa('value',$protocol_elt['sip']['allow']) === true)
{
	if(xivo_issa('protocol',$info) === true && xivo_ak('allow',$info['protocol']) === true && empty($info['protocol']['allow']) === false)
	{
		if(is_array($info['protocol']['allow']) === false)
			$info['protocol']['allow'] = explode(',',$info['protocol']['allow']);

		$protocol_elt['sip']['allow']['value'] = array_diff($protocol_elt['sip']['allow']['value'],$info['protocol']['allow']);
	}
}

if(xivo_issa('allow',$protocol_elt['iax']) === true && xivo_issa('value',$protocol_elt['iax']['allow']) === true)
{
	if(xivo_issa('protocol',$info) === true && xivo_ak('allow',$info['protocol']) === true && empty($info['protocol']['allow']) === false)
	{
		if(is_array($info['protocol']['allow']) === false)
			$info['protocol']['allow'] = explode(',',$info['protocol']['allow']);

		$protocol_elt['iax']['allow']['value'] = array_diff($protocol_elt['iax']['allow']['value'],$info['protocol']['allow']);
	}
}

$_HTML->assign('ract',$act);
$_HTML->assign('info',$info);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('group_list',$group_list);
$_HTML->assign('group',$group);
$_HTML->assign('protocol_elt',$protocol_elt);
$_HTML->assign('ufeatures_elt',$ufeatures->get_element());
$_HTML->assign('voicemail_elt',$voicemail->get_element());
$_HTML->assign('moh_list',$moh_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');

?>
