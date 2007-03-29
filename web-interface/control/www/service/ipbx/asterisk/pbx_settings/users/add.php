<?php

$ufeatures = &$ipbx->get_module('userfeatures');
$gfeatures = &$ipbx->get_module('groupfeatures');
$extensions = &$ipbx->get_module('extensions');
$musiconhold = &$ipbx->get_module('musiconhold');
$ugroup = &$ipbx->get_module('usergroup');
$voicemail = &$ipbx->get_module('uservoicemail');

if(($moh_list = $musiconhold->get_all_category()) !== false)
	ksort($moh_list);

$result = $info = array();

do
{
	if(isset($_QR['fm_send']) === false
	|| xivo_issa('protocol',$_QR) === false
	|| xivo_issa('ufeatures',$_QR) === false
	|| isset($_QR['protocol']['protocol']) === false
	|| ($protocol = &$ipbx->get_protocol_module($_QR['protocol']['protocol'])) === false)
		break;

	if($moh_list === false || isset($_QR['ufeatures']['musiconhold'],$moh_list[$_QR['ufeatures']['musiconhold']]) === false)
		$_QR['ufeatures']['musiconhold'] = '';
		
	if(xivo_issa('allow',$_QR['protocol']) === false)
		unset($_QR['protocol']['allow'],$_QR['protocol']['disallow']);

	if(($result['protocol'] = $protocol->chk_values($_QR['protocol'],true,true)) === false)
	{
		$info['protocol'] = $protocol->get_filter_result();
		break;
	}

	if(is_array($result['protocol']['allow']) === true)
		$result['protocol']['allow'] = implode(',',$result['protocol']['allow']);

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

	if($result['ufeatures']['number'] !== '')
	{
		$local_exten = array();
		$local_exten['exten'] = $result['ufeatures']['number'];
		$local_exten['priority'] = 1;
		$local_exten['app'] = 'Macro';
		$local_exten['appdata'] = 'superuser';

		if($result['protocol']['context'] === '')
			$local_exten['context'] = 'local-extensions';
		else
			$local_exten['context'] = $result['protocol']['context'];

		if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false
		|| ($local_extenid = $extensions->add($result['local_exten'])) === false)
		{
			$protocol->delete($pid);
			$ufeatures->delete($uid);
			break;
		}

		$callerid = $result['protocol']['callerid'].' <'.$result['ufeatures']['number'].'>';

		if(($callerid = $protocol->set_chk_value('callerid',$callerid)) === false
		|| $protocol->edit($pid,array('callerid' => $callerid)) === false)
		{
			$protocol->delete($pid);
			$ufeatures->delete($uid);
			$extensions->delete($local_extenid);
			break;	
		}
	}

	// while waiting protocol[mailbox]
	$protocol->edit($pid,array('mailbox' => $result['ufeatures']['number']));

	$hints = array();
	$hints['app'] = $ipbx->mk_interface($result['protocol']['name'],$result['ufeatures']['protocol']);

	if($result['ufeatures']['number'] !== '' && $hints['app'] !== false)
	{
		$hints['context'] = 'hints';
		$hints['exten'] = $result['ufeatures']['number'];
		$hints['priority'] = -1;

		if(($result['hints'] = $extensions->chk_values($hints,true,true)) !== false)
			$extensions->add($result['hints']);
	}

	do
	{
		if(($interface = $ipbx->mk_interface($result['protocol']['name'],
						     $result['ufeatures']['protocol'],
						     $result['ufeatures']['number'],
						     $result['protocol']['context'])) === false)
			break;

		if(xivo_issa('group',$_QR) === false)
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

	if($result['ufeatures']['number'] !== '' && xivo_issa('voicemail',$_QR) === true)
	{
		$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];

		if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'],true,true)) === false
		|| $voicemail->add($result['voicemail']) === false)
			$info['voicemail'] = $voicemail->get_filter_result();
	}

	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');
}
while(false);

$group = false;
$group_list = array();

if(($group_list = $gfeatures->get_all(false)) !== false)
	$group = true;

$protocol_elt = $ipbx->get_protocol_element();

if(xivo_issa('allow',$protocol_elt['sip']) === true && xivo_issa('value',$protocol_elt['sip']['allow']) === true)
{
	if(xivo_issa('protocol',$info) === true && xivo_ak('allow',$info['protocol']) === true && empty($info['protocol']['allow']) === false)
		$protocol_elt['sip']['allow']['value'] = array_diff($protocol_elt['sip']['allow']['value'],$info['protocol']['allow']);
}

if(xivo_issa('allow',$protocol_elt['iax']) === true && xivo_issa('value',$protocol_elt['iax']['allow']) === true)
{
	if(xivo_issa('protocol',$info) === true && xivo_ak('allow',$info['protocol']) === true && empty($info['protocol']['allow']) === false)
		$protocol_elt['iax']['allow']['value'] = array_diff($protocol_elt['iax']['allow']['value'],$info['protocol']['allow']);
}

$_HTML->assign('info',$info);
$_HTML->assign('ract',$act);
$_HTML->assign('group',$group);
$_HTML->assign('group_list',$group_list);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('protocol_elt',$protocol_elt);
$_HTML->assign('ufeatures_elt',$ufeatures->get_element());
$_HTML->assign('voicemail_elt',$voicemail->get_element());
$_HTML->assign('moh_list',$moh_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');

?>
