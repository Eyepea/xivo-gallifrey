<?php

$ufeatures = &$ipbx->get_module('userfeatures');
$gfeatures = &$ipbx->get_module('groupfeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');
$musiconhold = &$ipbx->get_module('musiconhold');
$ugroup = &$ipbx->get_module('usergroup');
$qmember = &$ipbx->get_module('queuemember');
$voicemail = &$ipbx->get_module('uservoicemail');
$autoprov = &$ipbx->get_module('autoprov');

$gmember_unslt = false;

if(($groups = $ipbx->get_groups_list()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
	usort($groups,array(&$sort,'str_usort'));

	$gmember_unslt = array();

	$nb = count($groups);

	for($i = 0;$i < $nb;$i++)
	{
		$name = &$groups[$i]['queue']['name'];
		$gmember_unslt[$name] = $name;
	}

	if(empty($gmember_unslt) === true)
		$gmember_unslt = $groups = false;
}

$qmember_unslt = false;

if(($queues = $ipbx->get_queues_list()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'qfeatures','key' => 'name'));
	usort($queues,array(&$sort,'str_usort'));

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

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

if(($autoprov_list = $autoprov->get_autoprov_list()) !== false)
	ksort($autoprov_list);

$allow = array();

$result = null;

$add = true;

do
{
	if(isset($_QR['fm_send']) === false
	|| xivo_issa('protocol',$_QR) === false
	|| xivo_issa('ufeatures',$_QR) === false
	|| isset($_QR['protocol']['protocol']) === false
	|| ($protocol = &$ipbx->get_protocol_module($_QR['protocol']['protocol'])) === false)
		break;

	$result = array();

	if($moh_list === false || isset($_QR['ufeatures']['musiconhold'],$moh_list[$_QR['ufeatures']['musiconhold']]) === false)
		$_QR['ufeatures']['musiconhold'] = '';
		
	if(xivo_issa('allow',$_QR['protocol']) === false)
		unset($_QR['protocol']['allow'],$_QR['protocol']['disallow']);

	if(($result['protocol'] = $protocol->chk_values($_QR['protocol'])) === false)
	{
		$add = false;
		$result['protocol'] = $protocol->get_filter_result();
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
		$add = false;
		$result['ufeatures'] = $ufeatures->get_filter_result();
	}

	$result['protocol']['mailbox'] = $result['ufeatures']['number'];

	$local_exten = $exten_numbers = $hints = null;

	if($add === true && $result['ufeatures']['number'] !== '')
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

		if(($result['local_exten'] = $extensions->chk_values($local_exten)) === false)
		{
			$add = false;
			$result['local_exten'] = $extensions->get_filter_result();
		}

		$callerid = preg_replace('/<'.preg_quote($result['ufeatures']['number']).'>$/','',$result['protocol']['callerid']);
		$callerid = trim($callerid).' <'.$result['ufeatures']['number'].'>';

		if(($result['protocol']['callerid'] = $protocol->set_chk_value('callerid',$callerid)) === false)
		{
			$add = false;
			$result['protocol']['callerid'] = '';
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

		$hints = array();

		if(($hints['app'] = $ipbx->mk_interface($result['protocol']['name'],$result['ufeatures']['protocol'])) !== false)
		{
			$hints['context'] = 'hints';
			$hints['exten'] = $result['ufeatures']['number'];
			$hints['priority'] = -1;

			if(($result['hints'] = $extensions->chk_values($hints)) === false)
			{
				$add = false;
				$result['hints'] = $extensions->get_filter_result();
			}
		}
		else
			$add = false;
	}

	$ugroup_where = array(
			'usertype' => 'user',
			'userid' => 0,
			'category' => 'group');

	if(isset($_QR['usergroup']) === false || xivo_bool($result['ufeatures']['ringgroup']) === false)
		$_QR['usergroup'] = false;

	$usergroup = $add_usergroup = false;
	$callgroup = array();
	$group_add = $group_tmp = array();

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

			$group_tmp[$qname] = 1;
			$group_add[] = $ginfo;
			$callgroup[] = $gid;
			
			if($_QR['usergroup'] !== false && $_QR['usergroup'] === $qname)
				$usergroup = $gid;
		}
	}

	if($usergroup !== false)
	{
		$result['usergroup'] = array();
		$result['usergroup']['userid'] = 0;
		$result['usergroup']['groupid'] = $usergroup;

		if(($result['usergroup'] = $ugroup->chk_values($result['usergroup'])) === false)
			$result['usergroup'] = $ugroup->get_filter_result();
		else
			$add_usergroup = true; 
	}

	$uqueue_where = array(
			'usertype' => 'user',
			'userid' => 0,
			'category' => 'queue');

	$queue_add = $queue_tmp = array();

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
				continue;

			$chantype = &$_QR['queue'][$qname]['chantype'];

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

			$queue_tmp[$qname] = 1;
			$queue_add[] = $qinfo;
		}
	}

	$add_voicemail = false;

	if(xivo_issa('voicemail',$_QR) === true)
	{
		if($result['ufeatures']['number'] !== '')
			$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];

		$_QR['voicemail']['context'] = $result['ufeatures']['context'];

		if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'])) === false)
			$result['voicemail'] = $voicemail->get_filter_result();
		else
			$add_voicemail = true;

		$result['voicemail']['commented'] = 0;
	}

	$send_autoprov = false;

	if(xivo_issa('autoprov',$_QR) === true && is_array($autoprov_list) === true
	&& isset($_QR['autoprov']['vendormodel'],$_QR['autoprov']['macaddr']) === true)
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

		$_QR['autoprov']['modact'] = 'prov';
		$_QR['autoprov']['proto'] = $result['ufeatures']['protocol'];
		$_QR['autoprov']['iduserfeatures'] = 0;
		
		if(($result['autoprov'] = $autoprov->chk_values($_QR['autoprov'])) === false)
			$result['autoprov'] = $autoprov->get_filter_result();
		else
			$send_autoprov = true;
	}

	if($add === false || ($protocolid = $protocol->add($result['protocol'])) === false)
		break;
		
	$result['ufeatures']['protocolid'] = $protocolid;

	if(($ufeaturesid = $ufeatures->add($result['ufeatures'])) === false)
	{
		$protocol->delete($protocolid);
		break;
	}

	if($local_exten !== null && ($local_extenid = $extensions->add($result['local_exten'])) === false)
	{
		$protocol->delete($protocolid);
		$ufeatures->delete($ufeaturesid);
		break;
	}

	if($exten_numbers !== null && ($extenumid = $extenumbers->add($result['extenumbers'])) === false)
	{
		$protocol->delete($protocolid);
		$ufeatures->delete($ufeaturesid);

		if($local_exten !== null)
			$extensions->delete($local_extenid);
		break;
	}

	if($hints !== null && ($hintsid = $extensions->add($result['hints'])) === false)
	{
		$protocol->delete($protocolid);
		$ufeatures->delete($ufeaturesid);

		if($local_extenid !== null)
			$extensions->delete($local_extenid);

		if($exten_numbers !== null)
			$extenumbers->delete($extenumid);
		break;
	}

	if(isset($callgroup[0]) === false)
		$callgroup_list = '';
	else
		$callgroup_list = implode(',',$callgroup);

	if($protocol->edit($protocolid,array('callgroup' => $callgroup_list)) === false)
	{
		$group_add = $callgroup = array();
		$add_usergroup = false;
	}

	if(($nb = count($group_add)) !== 0)
	{
		for($i = 0;$i < $nb;$i++)
		{
			$group_add[$i]['userid'] = $ufeaturesid;
			$qmember->add($group_add[$i]);
		}
	}

	if($add_usergroup === true)
	{
		$result['usergroup']['userid'] = $ufeaturesid;
		
		if(in_array($result['usergroup']['groupid'],$callgroup) === true)
			$ugroup->add($result['usergroup']);
	}

	if(($nb = count($queue_add)) !== 0)
	{
		for($i = 0;$i < $nb;$i++)
		{
			$queue_add[$i]['userid'] = $ufeaturesid;
			$qmember->add($queue_add[$i]);
		}
	}

	if($add_voicemail === true)
		$voicemail->add($result['voicemail']);

	if($send_autoprov === true)
	{
		$result['autoprov']['iduserfeatures'] = $ufeaturesid;
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

if($result !== null)
{
	$result['protocol']['allow'] = $allow;

	if(xivo_issa('usergroup',$result) === false)
		$result['usergroup'] = null;

	if(xivo_issa('voicemail',$result) === false)
		$result['voicemail'] = null;

	if(xivo_issa('autoprov',$result) === false)
		$result['autoprov'] = null;
}

$_HTML->assign('info',$result);
$_HTML->assign('ract',$act);
$_HTML->assign('groups',$groups);
$_HTML->assign('gmember_slt',false);
$_HTML->assign('gmember_unslt',$gmember_unslt);
$_HTML->assign('queues',$queues);
$_HTML->assign('qmember_slt',false);
$_HTML->assign('qmember_unslt',$qmember_unslt);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('element',$element);
$_HTML->assign('moh_list',$moh_list);
$_HTML->assign('autoprov_list',$autoprov_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

?>
