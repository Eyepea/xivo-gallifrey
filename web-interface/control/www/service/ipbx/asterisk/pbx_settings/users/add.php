<?php

$ufeatures = &$ipbx->get_module('userfeatures');
$gfeatures = &$ipbx->get_module('groupfeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');
$musiconhold = &$ipbx->get_module('musiconhold');
$ugroup = &$ipbx->get_module('usergroup');
$voicemail = &$ipbx->get_module('uservoicemail');
$autoprov = &$ipbx->get_module('autoprov');

#xivo_print_r($ufeatures->get_autoprov_list());
#die();

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

if(($autoprov_list = $autoprov->get_autoprov_list()) !== false)
	ksort($autoprov_list);

$allow = $groups = array();

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

	if(($result['protocol'] = $protocol->chk_values($_QR['protocol'],true,true)) === false)
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
	$_QR['ufeatures']['protocol'] = $_QR['protocol']['protocol'];
	$_QR['ufeatures']['protocolid'] = 0;

	if(($result['ufeatures'] = $ufeatures->chk_values($_QR['ufeatures'],true,true)) === false)
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

		if(($result['local_exten'] = $extensions->chk_values($local_exten,true,true)) === false)
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

		if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false
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

			if(($result['hints'] = $extensions->chk_values($hints,true,true)) === false)
			{
				$add = false;
				$result['hints'] = $extensions->get_filter_result();
			}
		}
		else
			$add = false;
	}

	$add_usergroup = false;

	if(xivo_issa('group',$_QR) !== false)
	{
		$arr_group = array_values($_QR['group']);

		if(($nb = count($arr_group)) !== 0)
		{
			$infogrps = array();
			$usergroup = false;

			if(isset($_QR['usergroup']) === false || xivo_bool($result['ufeatures']['ringgroup']) === false)
				$_QR['usergroup'] = false;

			for($i = 0;$i < $nb;$i++)
			{
				if(($ginfo = $gfeatures->get($arr_group[$i],false)) === false)
					continue;

				$infogrps[] = $ginfo;
				$groups[] = $ginfo['id'];

				if($_QR['usergroup'] !== false && (int) $ginfo['id'] === (int) $_QR['usergroup'])
					$usergroup = $ginfo['id'];
			}

			if(isset($groups[0]) === false)
				$usergroup = false;

			if($usergroup !== false)
			{
				$result['usergroup'] = array();
				$result['usergroup']['userid'] = $result['userfeatures']['id'];
				$result['usergroup']['groupid'] = $usergroup;

				if(($result['usergroup'] = $ugroup->chk_values($result['usergroup'],true,true)) === false)
					$result['usergroup'] = $ugroup->get_filter_result();
				else
					$add_usergroup = true;
			}
		}
	}

	$add_voicemail = false;

	if(xivo_issa('voicemail',$_QR) === true)
	{
		if($result['ufeatures']['number'] !== '')
			$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];

		if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'],true,true)) === false)
			$result['voicemail'] = $voicemail->get_filter_result();
		else
			$add_voicemail = true;

		$result['voicemail']['commented'] = 0;
	}

	$add_autoprov = false;

	if(xivo_issa('autoprov',$_QR) === true && is_array($autoprov_list) === true)
	{
		if(isset($_QR['autoprov']['vendormodel']) === true)
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
		}

		if(isset($_QR['autoprov']['macaddr']) === true
		&& preg_match_all('/(0|([A-F0-9]{2}))[-: ]?/i',$_QR['autoprov']['macaddr'],$match) === 6)
		{
			$_QR['autoprov']['macaddr'] = '';

			for($i = 0;$i < 6;$i++)
			{
				if($match[1][$i] === '0')
					$match[1][$i] = '00';

				$_QR['autoprov']['macaddr'] .= ':'.strtoupper($match[1][$i]);
			}

			$_QR['autoprov']['macaddr'] = substr($_QR['autoprov']['macaddr'],1);
		}

		$_QR['autoprov']['proto'] = $result['ufeatures']['protocol'];
		$_QR['autoprov']['iduserfeatures'] = 0;
		
		if(($result['autoprov'] = $autoprov->chk_values($_QR['autoprov'],true,true)) === false)
			$result['autoprov'] = $autoprov->get_filter_result();
		else
			$add_autoprov = true;
	}

	if($add_autoprov === true)
	{
		$result['autoprov']['iduserfeatures'] = 11;
		$autoprov->informative($result['autoprov']);
		die();
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

 	$mqueues = array();

	$interface = $ipbx->mk_interface($result['protocol']['name'],
					 $result['ufeatures']['protocol'],
					 $result['ufeatures']['number'],
					 $result['protocol']['context']);

	if(isset($groups[0]) === true && $interface !== false)
	{
		$qmember = &$ipbx->get_module('queuemember');

		$nb = count($groups);

		for($i = 0;$i < $nb;$i++)
		{
			$mqueues = array('queue_name'	=> $infogrps[$i]['name'],
					 'interface'	=> $interface,
					 'call_limit'	=> $result['ufeatures']['simultcalls']);

			if($qmember->add($mqueues) === false)
			{
				unset($groups[$i]);
				continue;
			}
		}

		if(($grp_list = implode(',',$groups)) === '')
			$add_usergroup = false;

		if($protocol->edit($protocolid,array('callgroup' => $grp_list)) === false)
		{
			$add_usergroup = false;
			$qmember->delete_by_interface($interface);
		}

		if($add_usergroup !== false && in_array($result['usergroup']['groupid'],$groups) === true)
			$ugroup->add($result['usergroup']);
	}

	if($add_voicemail === true)
		$voicemail->add($result['voicemail']);

	if($add_autoprov === true)
	{
		$result['autoprov']['iduserfeatures'] = $ufeaturesid;
		$autoprov->informative($result['autoprov']);
	}

	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');
}
while(false);

$group = false;
$group_list = array();

if(($group_list = $gfeatures->get_list(false)) !== false)
{
	$group = true;

	if(empty($groups) === true)
		$gelt = $group_list;
	else
	{
		$gelt = array_values(array_diff($group_list,$groups));
		$grp = array_values(array_diff($groups,$gelt));

		if(isset($grp[0]) === false)
			$grp = $group_list;

		$nb = count($grp);

		$result['protocol']['group'] = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($ginfo = $gfeatures->get($grp[$i],false)) === false)
				continue;

			$result['protocol']['group'][] = $ginfo;
		}
	}

	$group_list = array();

	$nb = count($gelt);

	for($i = 0;$i < $nb;$i++)
	{
		if(($ginfo = $gfeatures->get($gelt[$i],false)) === false)
			continue;
		
		$group_list[] = $ginfo;
	}
}

$element = array();
$element['protocol'] = $ipbx->get_protocol_element();
$element['ufeatures'] = $ufeatures->get_element();
$element['voicemail'] = $voicemail->get_element();

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
}

$_HTML->assign('info',$result);
$_HTML->assign('ract',$act);
$_HTML->assign('group',$group);
$_HTML->assign('group_list',$group_list);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('element',$element);
$_HTML->assign('moh_list',$moh_list);
$_HTML->assign('autoprov_list',$autoprov_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');

?>
