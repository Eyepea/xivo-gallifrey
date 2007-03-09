<?php

$ufeatures = &$ipbx->get_module('userfeatures');

$info = array();

if(isset($_QR['id']) === false || ($info['ufeatures'] = $ufeatures->get($_QR['id'])) === false
|| ($protocol = &$ipbx->get_protocol_module($info['ufeatures']['protocol'])) === false
|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');

$gfeatures = &$ipbx->get_module('groupfeatures');
$voicemail = &$ipbx->get_module('uservoicemail');
$ugroup = &$ipbx->get_module('usergroup');

$info['voicemail'] = $voicemail->get_by_mailbox($info['protocol']['mailbox']);
$info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id']);

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('protocol',$_QR) === false || xivo_issa('ufeatures',$_QR) === false)
		break;

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

	if($chg_protocol === true)
		$old_protocol->delete($info['protocol']['id']);

	// while waiting protocol[mailbox]
	$protocol->edit($pid,array('mailbox' => $result['ufeatures']['number']));

	do
	{
		if(($interface = $ipbx->mk_interface($info['ufeatures']['protocol'],$info['protocol']['name'])) === false)
		{
			$protocol->edit($pid,array('callgroup' => ''));
			break;
		}

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
			break;
		}

		$qm_list = $qmember->get_list_by_interface($interface);

		if(($new_interface = $ipbx->mk_interface($result['ufeatures']['protocol'],$result['protocol']['name'])) === false)
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
			break;

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
		else if(xivo_ak('voicemail-active',$_QR,true) !== '1')
		{
			$info['voicemail']['commented'] = 1;
			$voicemail->edit($info['voicemail']['id'],$info['voicemail']);
		}
		else if(xivo_issa('voicemail',$_QR) === true)
		{
			$_QR['voicemail']['id'] = $info['voicemail']['id'];
			$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];
			$voicemail->edit($info['voicemail']['id'],$_QR['voicemail']);
		}
	}
	else if($result['ufeatures']['number'] !== '')
	{
		$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];
		$voicemail->add($_QR['voicemail']);
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

$_HTML->assign('ract',$act);
$_HTML->assign('ufeatures',$ufeatures);
$_HTML->assign('voicemail',$voicemail);
$_HTML->assign('info',$info);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('group_list',$group_list);
$_HTML->assign('group',$group);
$_HTML->assign('protocol_elt',$ipbx->get_protocol_element());
$_HTML->assign('ufeatures_elt',$ufeatures->get_element());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');

?>
