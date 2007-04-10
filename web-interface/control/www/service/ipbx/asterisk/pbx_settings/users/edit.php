<?php

$ufeatures = &$ipbx->get_module('userfeatures');

$info = array();

$return = &$info;

if(isset($_QR['id']) === false || ($info['ufeatures'] = $ufeatures->get($_QR['id'])) === false
|| ($protocol = &$ipbx->get_protocol_module($info['ufeatures']['protocol'])) === false
|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'])) === false)
	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');

$gfeatures = &$ipbx->get_module('groupfeatures');
$extensions = &$ipbx->get_module('extensions');
$extenumbers = &$ipbx->get_module('extenumbers');
$musiconhold = &$ipbx->get_module('musiconhold');
$ugroup = &$ipbx->get_module('usergroup');
$voicemail = &$ipbx->get_module('uservoicemail');

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

$info['voicemail'] = $voicemail->get_by_mailbox($info['protocol']['mailbox']);
$info['usergroup'] = $ugroup->get_by_user($info['ufeatures']['id']);

$allow = $info['protocol']['allow'];
$groups = $info['protocol']['callgroup'];

$status = $result = array();
$status['localexten'] = $status['extenumbers'] = $status['hints'] = $status['usergroup'] = $status['voicemail'] = false;

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
			xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');
	}
	else
	{
		$chg_protocol = $provisioning = false;
		$old_protocol = &$protocol;
	}

	if(($result['protocol'] = $protocol->chk_values($_QR['protocol'],true,true)) === false)
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
	$_QR['ufeatures']['protocol'] = $_QR['protocol']['protocol'];
	$_QR['ufeatures']['protocolid'] = 0;

	if(($result['ufeatures'] = $ufeatures->chk_values($_QR['ufeatures'],true,true)) === false)
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

				if(($result['localexten'] = $localexten->chk_values($local_exten,true,true)) === false)
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

			if(($result['localexten'] = $localexten->chk_values($local_exten,true,true)) === false)
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

				if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false
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

			if(($result['extenumbers'] = $extenumbers->chk_values($exten_numbers,true,true)) === false)
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
				else if(($result['hints'] = $hintsexten->chk_values($hints,true,true)) === false)
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

				if(($result['hints'] = $hintsexten->chk_values($hints,true,true)) === false)
				{
					$edit = false;
					$result['hints'] = $hintsexten->get_filter_result();
				}
			}
		}
	}

	$groups = array();

	if(xivo_issa('group',$_QR) !== false)
	{
		$arr_group = array_values($_QR['group']);

		if(($nb = count($arr_group)) !== 0)
		{
			$usergroup = false;

			if(isset($_QR['usergroup']) === false || xivo_bool($result['ufeatures']['ringgroup']) === false)
				$_QR['usergroup'] = false;

			for($i = 0;$i < $nb;$i++)
			{
				if(($ginfo = $gfeatures->get($arr_group[$i],false)) === false)
					continue;

				$groups[] = $ginfo['id'];

				if($_QR['usergroup'] !== false && (int) $ginfo['id'] === (int) $_QR['usergroup'])
					$usergroup = $ginfo['id'];
			}

			if(isset($groups[0]) === false)
				$usergroup = false;

			if($info['usergroup'] !== false)
			{
				$result['usergroup'] = $info['usergroup'];
				$result['usergroup']['groupid'] = $usergroup;

				if($usergroup === false)
					$status['usergroup'] = 'delete';
				else if(($result['usergroup'] = $ugroup->chk_values($result['usergroup'],true,true)) === false)
					$result['usergroup'] = array_merge($info['usergroup'],$ugroup->get_filter_result());
				else
					$status['usergroup'] = 'edit';
			}
			else if($usergroup !== false)
			{
				$result['usergroup'] = array();
				$result['usergroup']['userid'] = $info['ufeatures']['id'];
				$result['usergroup']['groupid'] = $usergroup;

				if(($result['usergroup'] = $ugroup->chk_values($result['usergroup'],true,true)) === false)
					$result['usergroup'] = $ugroup->get_filter_result();
				else
					$status['usergroup'] = 'add';
			}
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

			if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'],true,true)) === false)
				$result['voicemail'] = array_merge($info['voicemail'],$voicemail->get_filter_result());
			else
				$status['voicemail'] = 'edit';

			$result['voicemail']['commented'] = 0;
		}
	}
	else if($result['ufeatures']['number'] !== '' && xivo_issa('voicemail',$_QR) === true)
	{
		$_QR['voicemail']['mailbox'] = $result['ufeatures']['number'];

		if(($result['voicemail'] = $voicemail->chk_values($_QR['voicemail'],true,true)) === false)
			$result['voicemail'] = $voicemail->get_filter_result();
		else
			$status['voicemail'] = 'add';
	}

	if($edit === false)
		break;

	if($chg_protocol === true)
	{
		if(($protocol_id = $protocol->add($result['protocol'])) === false)
			break;

		$old_protocol->delete($info['protocol']['id']);
	}
	else
	{
		$protocol_id = $info['protocol']['id'];
		
		if($protocol->edit($protocol_id,$result['protocol']) === false)
			break;
	}

	$result['ufeatures']['protocolid'] = $protocol_id;

	if($ufeatures->edit($info['ufeatures']['id'],$result['ufeatures'],$provisioning) === false)
	{
		if($chg_protocol === false)
			$protocol->edit_origin();
		else
		{
			$protocol->delete($protocol_id);
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
			$protocol->delete($protocol_id);
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
	$dfeatures_where['disable'] = 0;

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
			&& ($rs_dfeatures = $dfeatures->edit_where($dfeatures_where,array('disable' => 1))) === false)
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
			$protocol->delete($protocol_id);
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
			$protocol->delete($protocol_id);
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
					$dfeatures->edit_list_where($info['dfeatures'],array('disable' => 0));
				break 2;
			default:
				break 2;
		}
	}

 	$mqueues = array();
	$qmember = &$ipbx->get_module('queuemember');

	$interface = $ipbx->mk_interface($info['protocol']['name'],
					 $info['ufeatures']['protocol'],
					 $info['ufeatures']['number'],
					 $info['protocol']['context']);

	$new_interface = $ipbx->mk_interface($result['protocol']['name'],
					     $result['ufeatures']['protocol'],
				     	     $result['ufeatures']['number'],
				     	     $result['protocol']['context']);

	if(isset($groups[0]) === false || $interface === false)
	{
		$protocol->edit($protocol_id,array('callgroup' => ''));

		if($info['usergroup'] !== false)
			$ugroup->delete($info['usergroup']['id']);

		if($interface !== false)
			$qmember->delete_by_interface($interface);
	}
	else if($new_interface !== false)
	{
		$qmember_list = false;

		if($chg_protocol === true)
			$qmember->delete_by_interface($interface);
		else
			$qmember_list = $qmember->get_list_by_interface($interface);

		if($qmember_list !== false && $qmember->delete_by_interface($interface) === false)
			$protocol->edit($protocol_id,array('callgroup' => $info['protocol']['callgroup']));
		else
		{
			$nb = count($groups);

			for($i = 0;$i < $nb;$i++)
			{
				if(($ginfo = $gfeatures->get($groups[$i],false)) === false)
				{
					unset($groups[$i]);
					continue;
				}

				$mqueues = array('queue_name'	=> $ginfo['name'],
						 'interface'	=> $interface,
						 'call_limit'	=> $result['ufeatures']['simultcalls']);

				if($qmember->add($mqueues) === false)
				{
					unset($groups[$i]);
					continue;
				}
			}

			if(($grp_list = implode(',',$groups)) === '' && $status['usergroup'] === 'edit')
				$status['usergroup'] = 'delete';

			if($protocol->edit($protocol_id,array('callgroup' => $grp_list)) === false)
			{
				if($status['usergroup'] === 'edit')
					$status['usergroup'] = 'delete';

				$qmember->delete_by_interface($interface);
			}

			switch($status['usergroup'])
			{
				case 'add':
					if(in_array($result['usergroup']['groupid'],$groups) === true)
						$ugroup->add($result['usergroup']);
					break;
				case 'edit':
					if(in_array($result['usergroup']['groupid'],$groups) === true)
						$ugroup->edit($info['usergroup']['id'],$result['usergroup']);
					else
						$ugroup->delete($info['usergroup']['id']);
					break;
				case 'delete':
					$ugroup->delete($info['usergroup']['id']);
					break;
			}
		}
	}

	switch($status['voicemail'])
	{
		case 'add':
			$voicemail->add($result['voicemail']);
			break;
		case 'edit':
			$voicemail->edit($info['voicemail']['id'],$result['voicemail'],false);
			break;
		case 'delete':
			$voicemail->delete($info['voicemail']['id']);
			break;
		case 'disable':
			$voicemail->disable($info['voicemail']['id'],true);
			break;
	}	

	xivo_go($_HTML->url('service/ipbx/pbx_settings/users'),'act=list');
}
while(false);

$group = false;
$group_list = $return['protocol']['group'] = array();

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

		for($i = 0;$i < $nb;$i++)
		{
			if(($ginfo = $gfeatures->get($grp[$i],false)) === false)
				continue;

			$return['protocol']['group'][] = $ginfo;
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

if($info['usergroup'] === false)
	$return['usergroup'] = null;

if($info['voicemail'] === false)
	$return['voicemail'] = null;

$return['protocol']['allow'] = $allow;

$_HTML->assign('id',$info['ufeatures']['id']);
$_HTML->assign('info',$return);
$_HTML->assign('ract',$act);
$_HTML->assign('group',$group);
$_HTML->assign('group_list',$group_list);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('element',$element);
$_HTML->assign('moh_list',$moh_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');

?>
