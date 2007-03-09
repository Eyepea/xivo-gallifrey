<?php

$ufeatures = &$ipbx->get_module('userfeatures');
$gfeatures = &$ipbx->get_module('groupfeatures');

$ugroup = &$ipbx->get_module('usergroup');

$result = $info = array();

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('protocol',$_QR) === false || xivo_issa('ufeatures',$_QR) === false
	|| isset($_QR['protocol']['protocol']) === false
	|| ($protocol = &$ipbx->get_protocol_module($_QR['protocol']['protocol'])) === false)
		break;

	if(($result['protocol'] = $protocol->chk_values($_QR['protocol'],true,true)) === false)
	{
		$info['protocol'] = $protocol->get_filter_result();
		break;
	}

	if(($pid = $protocol->add($result['protocol'])) === false)
		break;

	$_QR['ufeatures']['name'] = $result['protocol']['name'];
	$_QR['ufeatures']['protocol'] = $_QR['protocol']['protocol'];
	$_QR['ufeatures']['protocolid'] = $pid;

	if(($result['ufeatures'] = $ufeatures->chk_values($_QR['ufeatures'],true,true)) === false
	|| ($uid = $ufeatures->add($result['ufeatures'])) === false)
	{
		$info['ufeatures'] = $ufeatures->get_filter_result();
		$protocol->delete($pid);
		break;
	}

	// while waiting protocol[mailbox]
	$protocol->edit($pid,array('mailbox' => $result['ufeatures']['number']));

	do
	{
		if(xivo_issa('group',$_QR) === false
		|| ($interface = $ipbx->mk_interface($result['ufeatures']['protocol'],$result['protocol']['name'])) === false)
			break;

		$arr_group = array_values($_QR['group']);

		if(($nb = count($arr_group)) === 0)
			break;

		$groups = '';
		$mqueues = array();
		$qmember = &$ipbx->get_module('queuemember');

		$usergroup = false;

		if(isset($_QR['usergroup']) === false || xivo_bool($result['ufeatures']['ringgroup']) === false)
			$_QR['usergroup'] = false;

		for($i = 0;$i < $nb;$i++)
		{
			if(($ginfo = $gfeatures->get($arr_group[$i],false)) === false)
				continue;

			$mqueues = array('queue_name'	=> $ginfo['name'],
					'interface'	=> $interface,
					'call_limit'	=> $result['ufeatures']['simultcalls']);

			if($qmember->add($mqueues) === false)
			{
				$qmember->delete_by_interface($interface);
				break 2;
			}

			$groups .= ','.$ginfo['id'];

			if($_QR['usergroup'] !== false && $ginfo['id'] === $_QR['usergroup'])
				$usergroup = $ginfo['id'];
		}

		if($groups === '')
			break;

		$groups = substr($groups,1);

		if($protocol->edit($pid,array('callgroup' => $groups)) === false)
		{
			$qmember->delete_by_interface($interface);
			break;
		}

		if($usergroup === false)
			break;

		$ugroup = &$ipbx->get_module('usergroup');
		$ugroup->add(array('userid' => $uid,'groupid' => $usergroup));
	}
	while(false);

	if(xivo_ak('voicemail-active',$_QR,true) === '1'
	&& xivo_issa('voicemail',$_QR) === true
	&& $result['ufeatures']['number'] !== '')
	{
		$voicemail = &$ipbx->get_module('uservoicemail');
		$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];
		$voicemail->add($_QR['voicemail']);
	}

	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');
}
while(false);

$group = false;
$group_list = array();

if(($group_list = $gfeatures->get_all(false)) !== false)
	$group = true;

$_HTML->assign('info',$info);
$_HTML->assign('ract',$act);
$_HTML->assign('group',$group);
$_HTML->assign('group_list',$group_list);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('protocol_elt',$ipbx->get_protocol_element());
$_HTML->assign('ufeatures_elt',$ufeatures->get_element());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');

?>
