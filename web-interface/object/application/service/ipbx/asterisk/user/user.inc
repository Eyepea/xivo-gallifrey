<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_user extends xivo_application_asterisk_abstract
{
	var $_protocol		= false;
	var $_origin		= array();
	var $_status		= '';
	var $_extenumbers	= null;
	var $_rightcall		= null;
	var $_dialstatus	= null;
	var $_musiconhold	= false;

	function xivo_application_service_asterisk_user(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_extenumbers = &$this->get_abstract('extenumbers')) === false)
			return(false);

		if(($this->_rightcall = &$this->get_abstract('rightcall')) === false)
			return(false);

		if(($this->_dialstatus = &$this->get_abstract('dialstatus')) === false)
			return(false);

		return(true);
	}

	function _reset()
	{
		$this->_protocol = false;

		return(parent::_reset());
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($ufeatures = &$this->_service->get_module('userfeatures')) === false
		|| ($this->_info['ufeatures'] = $ufeatures->get($id)) === false
		|| ($protocol = &$this->_service->get_protocol_module($this->_info['ufeatures']['protocol'])) === false
		|| ($this->_info['protocol'] = $protocol->get($this->_info['ufeatures']['protocolid'],$disable)) === false)
			return(false);

		$this->_origin['protocol'] = $protocol->get_origin();
		$this->_origin['ufeatures'] = $ufeatures->get_origin();

		$this->_get_localexten();
		$this->_get_hints();
		$this->_get_extenumbers();
		$this->_get_groupmember();
		$this->_get_queuemember();
		$this->_get_rightcall();
		$this->_get_dialstatus();
		$this->_get_voicemail();
		$this->_get_autoprov();
		$this->get_musiconhold();

		return($this->_info);
	}

	function _get_localexten()
	{
		$this->_info['localexten'] = false;
		$this->_origin['localexten'] = false;

		if(($extensions = &$this->_service->get_module('extensions')) === false
		|| isset($this->_info['protocol']['context']) === false)
			return(false);

		$this->_info['localexten'] = $extensions->get_exten('macro',
								    $this->_info['ufeatures']['number'],
								    $this->_info['protocol']['context']);

		$this->_origin['localexten'] = $extensions->get_origin();

		return($this->_info['localexten']);
	}

	function _get_hints()
	{
		$this->_info['hints'] = false;
		$this->_origin['hints'] = false;

		if(($extensions = &$this->_service->get_module('extensions')) === false
		|| isset($this->_info['protocol']['context']) === false)
			return(false);

		$this->_info['hints'] = $extensions->get_hints($this->_info['protocol']['name'],
							       $this->_info['ufeatures']['protocol'],
							       $this->_info['ufeatures']['number']);
			
		$this->_origin['hints'] = $extensions->get_origin();

		return($this->_info['hints']);
	}

	function _get_extenumbers()
	{
		return(parent::_get_extenumbers('user',$this->_info['ufeatures']['id']));
	}

	function _get_groupmember()
	{
		return($this->_get_member_type('group'));
	}

	function _get_queuemember()
	{
		return($this->_get_member_type('queue'));
	}

	function _get_member_type($type)
	{
		if($type !== 'group' && $type !== 'queue')
			return(false);

		$membertype = $type.'member';

		$this->_info[$membertype] = false;
		$this->_origin[$membertype] = false;

		$where = array();
		$where['category'] = $type;
		$where['usertype'] = 'user';
		$where['userid'] = $this->_info['ufeatures']['id'];

		if(($queuemember = &$this->_service->get_module('queuemember')) === false
		|| ($module = &$this->_service->get_module($type.'features')) === false
		|| ($this->_info[$membertype] = $queuemember->get_all_where($where)) === false
		|| ($nb = count($this->_info[$membertype])) === 0)
			return(false);

		$ref = &$this->_info[$membertype];

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = $module->get_id(array('name' => $ref[$i]['queue_name']))) === false)
				continue;

			if($type === 'queue')
				$ref[$i]['qfeaturesid'] = $id;
			else
				$ref[$i]['gfeaturesid'] = $id;
		}

		$this->_origin[$membertype] = $queuemember->get_origin_list();

		return($this->_info[$membertype]);
	}

	function _get_rightcall()
	{
		$this->_info['rightcall'] = $this->_rightcall->get('user',$this->_info['ufeatures']['id']);

		return($this->_info['rightcall']);
	}

	function _get_dialstatus()
	{
		$this->_info['dialstatus'] = $this->_dialstatus->get('user',$this->_info['ufeatures']['id']);

		return($this->_info['dialstatus']);
	}

	function _get_voicemail()
	{
		$this->_info['voicemail'] = false;
		$this->_origin['voicemail'] = false;

		if(($voicemail = &$this->_service->get_module('uservoicemail')) === false)
			return(false);

		$voicemail_where = array();
		$voicemail_where['mailbox'] = $this->_info['ufeatures']['number'];
		$voicemail_where['context'] = $this->_info['ufeatures']['context'];
		$this->_info['voicemail'] = $voicemail->get_where($voicemail_where);
		$this->_origin['voicemail'] = $voicemail->get_origin();

		return($this->_info['voicemail']);
	}

	function _get_autoprov()
	{
		$this->_info['autoprov'] = false;
		$this->_origin['autoprov'] = false;

		if(($autoprov = &$this->_service->get_module('autoprov')) === false)
			return(false);

		$autoprov_where = array();
		$autoprov_where['iduserfeatures'] = $this->_info['ufeatures']['id'];
		$this->_info['autoprov'] = $autoprov->get_where($autoprov_where);
		$this->_origin['autoprov'] = $autoprov->get_origin();

		return($this->_info['autoprov']);
	}

	function get_musiconhold()
	{
		if($this->_musiconhold !== false)
			return($this->_musiconhold);

		if(($musiconhold = &$this->_service->get_module('musiconhold')) === false)
			return(false);

		if(($this->_musiconhold = $musiconhold->get_all_category(null,false)) !== false)
			ksort($this->_musiconhold);

		return($this->_musiconhold);
	}

	function get_elements()
	{
		$r = array();

		$r['protocol'] = $this->_service->get_protocol_element();

		if(($ufeatures = &$this->_service->get_module('userfeatures')) !== false)
			$r['ufeatures'] = $ufeatures->get_element();
		else
			$r['ufeatures'] = false;

		if(($queuemember = &$this->_service->get_module('queuemember')) !== false)
			$r['qmember'] = $queuemember->get_element();
		else
			$r['qmember'] = false;

		if(($voicemail = &$this->_service->get_module('uservoicemail')) !== false)
			$r['voicemail'] = $voicemail->get_element();
		else
			$r['voicemail'] = false;

		if(($autoprov = &$this->_service->get_module('autoprov')) !== false)
			$r['autoprov'] = $autoprov->get_element();
		else
			$r['autoprov'] = false;

		if(($dialstatus = &$this->_service->get_module('dialstatus')) !== false)
			$r['dialstatus'] = $dialstatus->get_element();
		else
			$r['dialstatus'] = false;

		return($r);
	}

	function get_dialstatus_destination_list()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['ufeatures']['id'];

		return($this->_dialstatus->get_destination_list('user',$id));
	}

	function set_add($arr,$protocol)
	{
		return($this->_set('add',$arr,$protocol));
	}

	function set_edit($arr,$protocol)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr,$protocol));
	}

	function _set($action,$arr,$protocol)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('protocol',$arr) === false
		|| xivo_issa('ufeatures',$arr) === false)
			return(false);

		$this->_protocol = $protocol;

		$this->set_protocol($arr['protocol'],$protocol);

		$arr['ufeatures']['protocol'] = $protocol;
		$arr['ufeatures']['protocolid'] = 0;

		if($protocol === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
			$name = $this->get_result_var('protocol','interface');
		else
			$name = $this->get_result_var('protocol','name');

		$context = $this->get_result_var('protocol','context');

		$arr['ufeatures']['name'] = $name;
		$arr['ufeatures']['context'] = $context;

		$this->set_userfeatures($arr['ufeatures']);

		$number = (string) $this->get_result_var('ufeatures','number');
		$protocol = (string) $this->get_result_var('ufeatures','protocol');

		if(xivo_issa('group',$arr) === true
		&& xivo_issa('group-select',$arr) === true
		&& $this->set_groupmember($arr['group'],$arr['group-select'],$name,$context,$number,$protocol) === true)
		{
			$callgroup = $this->get_result('callgroup');

			if(is_array($callgroup) === true && isset($callgroup[0]) === true)
				$callgroup = implode(',',$callgroup);
			else
				$callgroup = '';

			if($this->_protocol !== XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
				$this->_set_result_var('protocol','callgroup',$callgroup);
		}

		if(xivo_issa('queue',$arr) === true && xivo_issa('queue-select',$arr) === true)
			$this->set_queuemember($arr['queue'],$arr['queue-select'],$name,$context,$number,$protocol);

		if(xivo_issa('dialstatus',$arr) === true)
			$this->set_dialstatus($arr['dialstatus']);

		if($this->_protocol === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
		{
			if($this->get_errnb() > 0)
				$this->_protocol = $r = false;
			else
				$r = true;

			return($r);
		}

		$this->_set_result_var('protocol','mailbox',$number);

		if($this->_errnb === 0 && $number !== '')
		{
			$appdata = array('appdata' => 'superuser');

			if(($localexten = $this->set_macro($action,'localexten',$number,$appdata,$context)) === false)
				$this->_set_error('localexten');

			$this->_set_result('localexten',$localexten);

			$callerid = (string) $this->get_result_var('protocol','callerid');
			$callerid = $this->_service->mk_callerid($number,$callerid);

			$this->_set_result_var('protocol','callerid',$callerid);

			if(($hints = $this->set_hints($action,$name,$protocol,$number)) === false)
				$this->_set_error('hints');

			$this->_set_result('hints',$hints);

			$this->set_extenumbers($action,'user',$number,$context);
		}

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if(xivo_issa('voicemail',$arr) === true)
			$this->set_voicemail($arr['voicemail'],$context,$number);

		if(xivo_issa('autoprov',$arr) === true)
			$this->set_autoprov($action,$arr['autoprov'],$protocol);

		if($this->get_errnb() > 0)
			$this->_protocol = $r = false;
		else
			$r = true;

		return($r);
	}

	function set_protocol($arr,$protocol)
	{
		if(is_array($arr) === false
		|| ($proto = &$this->_service->get_protocol_module($protocol)) === false)
		{
			$this->_set_result('protocol',null);
			$this->_set_error('protocol','invalid module');
			return(false);
		}

		if(($rs = $proto->chk_values($arr)) === false)
		{
			$this->_set_result('protocol',$proto->get_filter_result());
			$this->_set_error('protocol',$proto->get_filter_error());

			return(false);
		}

		if(xivo_issa('allow',$rs) === true)
			$rs['allow'] = implode(',',$rs['allow']);

		$this->_set_result('protocol',$rs);

		return(true);
	}

	function set_userfeatures($arr)
	{
		if(is_array($arr) === false
		|| ($ufeatures = &$this->_service->get_module('userfeatures')) === false)
		{
			$this->_set_result('ufeatures',null);
			$this->_set_error('ufeatures','invalid module');
			return(false);
		}

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		if(($rs = $ufeatures->chk_values($arr)) === false)
		{
			$this->_set_result('ufeatures',$ufeatures->get_filter_result());
			$this->_set_error('ufeatures',$ufeatures->get_filter_error());

			return(false);
		}

		$this->_set_result('ufeatures',$rs);

		return(true);
	}

	function set_hints($action,$name,$protocol,$number)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| ($extensions = &$this->_service->get_module('extensions')) === false)
			return(false);

		if($action === 'edit' && xivo_issa('hints',$this->_info) !== false)
			$rs = $extensions->chk_hints($name,$protocol,$number);
		else
			$rs = $extensions->new_hints($name,$protocol,$number);

		return($rs);
	}

	function set_groupmember($group,$select,$name,$context='',$number='',$protocol='')
	{
		$r = $this->_set_member_type('group',$group,$select,$name,$context,$number,$protocol);

		return($r);
	}

	function set_queuemember($queue,$select,$name,$context='',$number='',$protocol='')
	{
		$r = $this->_set_member_type('queue',$queue,$select,$name,$context,$number,$protocol);

		return($r);
	}

	function _set_member_type($type,$list,$select,$name,$context='',$number='',$protocol='')
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| is_array($list) === false
		|| is_array($select) === false
		|| ($module = &$this->_service->get_module($type.'features')) === false
		|| ($queuemember = &$this->_service->get_module('queuemember')) === false)
			return(false);

		$slt = array_values($select);

		if(($nb = count($slt)) === 0)
			return(null);

		$info = array();
		$info['usertype'] = 'user';
		$info['userid'] = 0;
		$info['category'] = $type;

		$key = $type.'member';

		$tmp = $reslist = array();

		if($type === 'group')
		{
			$callgroup = array();
			$info['penalty'] = 0;
		}

		for($i = 0;$i < $nb;$i++)
		{
			$qname = &$slt[$i];

			if(isset($tmp[$qname]) === true
			|| isset($list[$qname]) === false
			|| ($chantype = xivo_ak('chantype',$list[$qname],true)) === false
			|| ($interface = $this->mk_interface($name,$chantype,$context,$number,$protocol)) === false
			|| ($typeid = $module->get_id(array('name' => $qname))) === false)
				continue;

			$info['queue_name'] = $qname;
			$info['interface'] = $interface['interface'];
			$info['channel'] = $interface['channel'];

			$values = array_merge($list[$qname],$info);

			if(($rs = $queuemember->chk_values($values)) === false)
				continue;

			if($type === 'queue')
				$rs['qfeaturesid'] = $typeid;
			else
			{
				$rs['gfeaturesid'] = $typeid;
				$callgroup[] = $typeid;		
			}

			$tmp[$qname] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result($key,false);

			if($type === 'group')
				$this->_set_result('callgroup','');
			return(false);
		}

		$this->_set_result($key,$reslist);

		if($type === 'group')
			$this->_set_result('callgroup',$callgroup);

		return(true);
	}

	function set_rightcall($list)
	{
		if(($r = $this->_rightcall->set($list,'user')) === false)
			$this->_set_error('rightcall',$this->_rightcall->get_error());

		$this->_set_result('rightcall',$this->_rightcall->get_result());

		return($r);
	}

	function set_dialstatus($list)
	{
		if(($r = $this->_dialstatus->set($list,'user')) === false)
			$this->_set_error('dialstatus',$this->_dialstatus->get_error());

		$this->_set_result('dialstatus',$this->_dialstatus->get_result());

		return($r);
	}

	function set_voicemail($arr,$context,$number='')
	{
		if(is_array($arr) === false
		|| ($voicemail = &$this->_service->get_module('uservoicemail')) === false)
			return(false);

		$arr['context'] = $context;

		if($number !== '')
			$arr['mailbox'] = $number;

		if(($rs = $voicemail->chk_values($arr)) === false)
		{
			$rs = array_merge($voicemail->get_filter_result(),array('commented' => 0));

			$this->_set_result('voicemail',$rs);
			$this->_set_error('voicemail',$voicemail->get_filter_error());

			return(false);
		}

		$rs['commented'] = 0;
		$this->_set_result('voicemail',$rs);

		return(true);
	}

	function set_autoprov($action,$arr,$protocol)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($arr) === false
		|| ($autoprov = &$this->_service->get_module('autoprov')) === false
		|| ($list = $autoprov->get_autoprov_list()) === false)
			return(false);

		if(xivo_haslen($arr,'vendormodel') === false
		|| xivo_haslen($arr,'macaddr') === false)
			return(null);

		if($action === 'edit' && xivo_issa('autoprov',$this->_info) === true)
		{
			if(xivo_haslen($arr,'modact') === false)
				return(null);

			$modact = $arr['modact'];
			$arr = $this->_info['autoprov'];
			$arr['modact'] = $modact;
			$arr['proto'] = $protocol;
		}
		else if(($pos = strpos($arr['vendormodel'],'.')) === false)
			$arr['vendormodel'] = $arr['modact'] = '';
		else
		{
			$vendor = substr($arr['vendormodel'],0,$pos);
			$model = substr($arr['vendormodel'],$pos+1);

			if(xivo_issa($vendor,$list) === true
			&& xivo_issa('model',$list[$vendor]) === true
			&& xivo_issa($model,$list[$vendor]['model']) === true)
			{
				$arr['vendor'] = $vendor;
				$arr['model'] = $model;
			}

			if(($macaddr = xivo_filter::chk_macaddr($arr['macaddr'])) !== false)
				$arr['macaddr'] = $macaddr;
			else
				$arr['macaddr'] = '';

			$arr['modact'] = 'prov';
			$arr['proto'] = $protocol;
		}

		if(($rs = $autoprov->chk_values($arr)) === false)
		{
			$this->_set_result('autoprov',$autoprov->get_filter_result());
			$this->_set_error('autoprov',$autoprov->get_filter_error());

			return(false);
		}

		$this->_set_result('autoprov',$rs);
		$this->_set_result_var('autoprov','modact',$arr['modact']);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$ufeaturesid = null;

		if($this->get_errnb() > 0
		|| $this->_protocol === false
		|| ($protocolid = $this->add_protocol()) === false
		|| ($ufeaturesid = $this->add_userfeatures($protocolid)) === false
		|| $this->add_macro('localexten') === false
		|| $this->add_hints() === false
		|| $this->add_extenumbers($ufeaturesid) === false
		|| $this->add_groupmember($ufeaturesid) === false
		|| $this->add_queuemember($ufeaturesid) === false
		|| $this->add_rightcall($ufeaturesid) === false
		|| $this->add_dialstatus($ufeaturesid) === false
		|| $this->add_voicemail() === false
		|| $this->add_phone($ufeaturesid) === false)
		{
			if($ufeaturesid !== null)
				$this->_reverse_add();

			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function add_protocol()
	{
		if(($rs = $this->get_result('protocol')) === null
		|| ($protocol = &$this->_service->get_protocol_module($this->_protocol)) === false)
			return(false);

		$this->_return['protocol'] = $protocol->add($rs);

		return($this->_return['protocol']);
	}

	function add_userfeatures($protocolid)
	{
		$rs = false;

		if(($protocolid = xivo_uint($protocolid)) === 0
		|| ($rs = $this->get_result('ufeatures')) === null
		|| $rs === false)
			return($rs);

		$ufeatures = &$this->_service->get_module('userfeatures');

		$rs['protocolid'] = $protocolid;
		$this->_return['userfeatures'] = $ufeatures->add($rs);

		return($this->_return['userfeatures']);
	}

	function add_hints()
	{
		if(($rs = $this->get_result('hints')) === null || $rs === false)
			return($rs);

		$extensions = &$this->_service->get_module('extensions');
		$this->_return['hints'] = $extensions->add_hints($rs);

		return($this->_return['hints']);
	}

	function add_groupmember($ufeaturesid)
	{
		return($this->_add_member_type('group',$ufeaturesid));
	}

	function add_queuemember($ufeaturesid)
	{
		return($this->_add_member_type('queue',$ufeaturesid));
	}

	function _add_member_type($type,$ufeaturesid)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| ($ufeaturesid = xivo_uint($ufeaturesid)) === 0)
			return(false);

		$key = $type.'member';

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);
		
		$queuemember = &$this->_service->get_module('queuemember');

		$id = $this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $ufeaturesid;

			unset($rs[$i]['gfeaturesid'],$rs[$i]['qfeaturesid']);

			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return[$key][] = $id;
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function add_rightcall($ufeaturesid)
	{
		$r = $this->_rightcall->add($ufeaturesid);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
	}

	function add_dialstatus($ufeaturesid)
	{
		$r = $this->_dialstatus->add($ufeaturesid);

		$this->_return['dialstatus'] = $this->_dialstatus->get_return();

		return($r);
	}

	function add_voicemail()
	{
		if(($rs = $this->get_result('voicemail')) === null)
			return($rs);

		$voicemail = &$this->_service->get_module('uservoicemail');

		$this->_return['uservoicemail'] = $voicemail->add($rs);

		return($this->_return['uservoicemail']);
	}

	function add_phone($ufeaturesid)
	{
		$rs = false;

		if(($ufeaturesid = xivo_uint($ufeaturesid)) === 0
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$autoprov = &$this->_service->get_module('autoprov');

		$rs['iduserfeatures'] = $ufeaturesid;

		if($autoprov->authoritative($rs,$rs['modact']) === false)
			$this->_return['autoprov'] = false;
		else
			$this->_return['autoprov'] = $ufeaturesid;

		return($this->_return['autoprov']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_protocol === false
		|| $this->_status !== 'add'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$protocol = &$this->_service->get_protocol_module($this->_protocol);
					$protocol->delete($val);
					break;
				case 'localexten':
					$extensions = &$this->_service->get_module('extensions');
					$extensions->delete_exten($val);
					break;
				case 'hints':
					$extensions = &$this->_service->get_module('extensions');
					$extensions->delete_hints($val);
					break;
				case 'groupmember':
				case 'queuemember':
					$queuemember = &$this->_service->get_module('queuemember');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$queuemember->delete($val[$j]);
					}
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
					break;
				case 'dialstatus':
					$this->_dialstatus->reverse_add();
					break;
				case 'autoprov':
					$autoprov = &$this->_service->get_module('autoprov');
					$autoprov->userdeleted($val);
					break;
				default:
					if(($module = &$this->_service->get_module($key)) === false)
						continue;

					$module->delete($val);
			}
		}
		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';
		$this->_return = array();

		if(is_array($this->_info) === false
		|| empty($this->_info) === true
		|| $this->get_errnb() > 0
		|| $this->_protocol === false
		|| ($protocolret = $this->edit_protocol()) === false)
		{
			$this->_status = '';
			return(false);
		}

		if($this->edit_userfeatures($protocolret) === false)
		{
			$this->edit_origin_protocol($protocolret);
			$this->_status = '';
			return(false);
		}

		$unlinks = false;

		if($this->_info['ufeatures']['number'] !== '')
		{
			if($this->get_result('localexten') === null
			|| $this->get_result('hints') === null
			|| $this->get_result('extenumbers') === null)
			{
				$unlinks = true;
				$this->delete_macro('localexten');
				$this->delete_hints();
				$this->delete_extenumbers();
			}
			else if($this->edit_macro('localexten') === false
			|| $this->edit_hints() === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit($protocolret);
				$this->_status = '';
				return(false);
			}
		}
		else if($this->add_macro('localexten') === false
		|| $this->add_hints() === false
		|| $this->add_extenumbers($this->_info['ufeatures']['id']) === false)
		{
			$this->_reverse_edit($protocolret);
			$this->_status = '';
			return(false);
		}

		if($this->edit_groupmember() === false
		|| $this->edit_queuemember() === false
		|| $this->edit_rightcall() === false
		|| $this->edit_dialstatus() === false
		|| $this->edit_voicemail() === false)
		{
			$this->_reverse_edit($protocolret);
			$this->_status = '';
			return(false);
		}

		$this->edit_phone();

		if($unlinks === true)
			$this->unlinks();

		$this->_status = '';

		return(true);
	}

	function edit_protocol()
	{
		if(xivo_issa('protocol',$this->_info) === false
		|| xivo_issa('ufeatures',$this->_info) === false
		|| ($protocol = &$this->_service->get_protocol_module($this->_protocol)) === false)
			return(false);

		if($this->_info['ufeatures']['protocol'] === $this->_protocol)
		{
			if(($rs = $this->get_result('protocol')) === null)
				return(false);

			$this->_return['protocol'] = $protocol->edit($this->_info['protocol']['id'],$rs);

			return($this->_return['protocol']);
		}

		if(($return = $this->add_protocol()) === false)
			return(false);

		if($this->delete_protocol() === false)
		{
			$protocol->delete($return);
			return(false);
		}

		$this->_return['protocol'] = $return;

		return($this->_return['protocol']);
	}

	function edit_origin_protocol($protocolid=0)
	{
		if(xivo_issa('protocol',$this->_origin) === false
		|| xivo_issa('ufeatures',$this->_info) === false
		|| ($protocol = &$this->_service->get_protocol_module($this->_info['ufeatures']['protocol'])) === false)
			return(false);

		if($this->_info['ufeatures']['protocol'] === $this->_protocol)
			return($protocol->edit_origin($this->_origin['protocol']));

		if(($protocolid = xivo_uint($protocolid)) === 0
		|| ($proto = &$this->_service->get_protocol($this->_protocol)) === false
		|| $proto->delete($protocolid) === false)
			return(false);

		return($proto->add_origin($this->_origin['protocol']));
	}

	function edit_userfeatures($protocolid)
	{
		$rs = false;

		if(xivo_issa('ufeatures',$this->_info) === false
		|| xivo_issa('protocol',$this->_info) === false
		|| ($rs = $this->get_result('ufeatures')) === null
		|| $rs === false)
			return($rs);

		if($this->_info['ufeatures']['protocol'] === $this->_protocol)
		{
			$rs['protocolid'] = $this->_info['protocol']['id'];
			$provisioning = false;
		}
		else if(($protocolid = xivo_uint($protocolid)) !== 0)
		{
			$rs['protocolid'] = $protocolid;
			$provisioning = true;
		}
		else
			return(false);

		$ufeatures = &$this->_service->get_module('userfeatures');
		$this->_return['userfeatures'] = $ufeatures->edit($this->_info['ufeatures']['id'],$rs,$provisioning);

		return($this->_return['userfeatures']);
	}

	function edit_hints()
	{
		$rs = false;

		if(xivo_issa('hints',$this->_info) === false
		|| ($rs = $this->get_result('hints')) === null
		|| $rs === false)
			return($rs);

		$extensions = &$this->_service->get_module('extensions');
		$this->_return['hints'] = $extensions->edit($this->_info['hints']['id'],$rs);

		return($this->_return['hints']);
	}

	function edit_groupmember()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		if(xivo_issa('groupmember',$this->_info) === false)
			return($this->add_groupmember($this->_info['ufeatures']['id']));

		$this->_return['groupmember'] = $this->delete_groupmember();

		if($this->get_result('groupmember') === null)
			return($this->_return['groupmember']);

		return($this->_edit_member_type('group'));
	}

	function edit_queuemember()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		if(xivo_issa('queuemember',$this->_info) === false)
			return($this->add_queuemember($this->_info['ufeatures']['id']));

		$this->_return['queuemember'] = $this->delete_queuemember();

		if($this->get_result('queuemember') === null)
			return($this->_return['queuemember']);

		return($this->_edit_member_type('queue'));
	}

	function _edit_member_type($type)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		$key = $type.'member';

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);
	
		$queuemember = &$this->_service->get_module('queuemember');

		$id = $this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $this->_info['ufeatures']['id'];

			unset($rs[$i]['gfeaturesid'],$rs[$i]['qfeaturesid']);

			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return[$key][] = $id;
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function edit_rightcall()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		$r = $this->_rightcall->edit($this->_status,'user',$this->_info['ufeatures']['id']);

		$this->_return['rightcall'] = $this->_rightcall->get_return();

		return($r);
	}

	function edit_dialstatus()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		$r = $this->_dialstatus->edit($this->_status,'user',$this->_info['ufeatures']['id']);

		$this->_return['dialstatus'] = $this->_dialstatus->get_return();

		return($r);
	}

	function edit_voicemail()
	{
		if(xivo_issa('voicemail',$this->_info) === false)
			return($this->add_voicemail());

		if(xivo_issa('ufeatures',$this->_info) === false
		|| ($voicemail = &$this->_service->get_module('uservoicemail')) === false)
			return(false);

		if($this->_info['ufeatures']['number'] === '')
			$r = $voicemail->delete($this->_info['voicemail']['id']);
		else if($this->get_result('voicemail') === null)
			$r = $voicemail->disable($this->_info['voicemail']['id']);
		else
			$r = true;

		return(($this->_return['uservoicemail'] = $r));
	}

	function edit_phone()
	{
		$rs = false;

		if(xivo_issa('ufeatures',$this->_info) === false
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$autoprov = &$this->_service->get_module('autoprov');

		$rs['iduserfeatures'] = $this->_info['ufeatures']['id'];

		if(xivo_issa('autoprov',$this->_info) !== false)
			$this->_return['autoprov'] = $autoprov->notification($rs,$rs['modact']);
		else
			$this->_return['autoprov'] = $autoprov->authoritative($rs,$rs['modact']);

		return($this->_return['autoprov']);
	}

	function _reverse_edit($protocolid=0)
	{
		if($this->get_errnb() > 0
		|| $this->_protocol === false
		|| $this->_status !== 'edit'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$this->edit_origin_protocol($protocolid);
					break;
				case 'userfeatures':
					$ufeatures = &$this->_service->get_module('userfeatures');
					$ufeatures->edit_origin($this->_origin['ufeatures']);
					break;
				case 'uservoicemail':
					$voicemail = &$this->_service->get_module('uservoicemail');

					if($this->_origin['voicemail'] === false)
						$voicemail->delete($val);
					else
						$voicemail->edit_origin($this->_origin['voicemail']);
					break;
				case 'localexten':
					$extensions = &$this->_service->get_module('extensions');

					if($this->_origin['localexten'] === false)
						$extensions->delete_exten($val);
					else
						$extensions->edit_origin($this->_origin['localexten']);
					break;
				case 'hints':
					$extensions = &$this->_service->get_module('extensions');

					if($this->_origin['hints'] === false)
						$extensions->delete_hints($val);
					else
						$extensions->edit_origin($this->_origin['hints']);
					break;
				case 'groupmember':
				case 'queuemember':
					if($key === 'groupmember')
						$this->delete_groupmember();
					else
						$this->delete_queuemember();

					$queuemember = &$this->_service->get_module('queuemember');
					$queuemember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcall':
					if(xivo_issa('ufeatures',$this->_info) === false)
						break;

					$this->_rightcall->reverse_edit($this->_status,'user',$this->_info['ufeatures']['id']);
					break;
				case 'dialstatus':
					if(xivo_issa('ufeatures',$this->_info) === false)
						break;

					$this->_dialstatus->reverse_edit($this->_status,'user',$this->_info['ufeatures']['id']);
					break;
				case 'autoprov':
					break;
				default:
					if(($module = &$this->_service->get_module($key)) === false)
						continue;

					if($this->_origin[$key] === false)
						$module->delete($val);
					else
						$module->edit_origin($this->_origin[$key]);
			}
		}
		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_protocol() === false
		|| $this->delete_userfeatures() === false
		|| $this->delete_macro('localexten') === false
		|| $this->delete_hints() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_groupmember() === false
		|| $this->delete_queuemember() === false
		|| $this->delete_rightcall() === false
		|| $this->delete_dialstatus() === false
		|| $this->delete_voicemail() === false
		|| $this->delete_phone() === false)
		{
			$this->_reverse_delete();
			$this->_status = '';

			return(false);
		}

		if(isset($this->_return['localexten']) === true
		|| isset($this->_return['hints']) === true
		|| isset($this->_return['extensions']) === true)
			$this->unlinks();

		$this->_status = '';

		return(true);
	}

	function delete_protocol()
	{
		if(xivo_issa('protocol',$this->_info) === false
		|| xivo_issa('ufeatures',$this->_info) === false
		|| ($protocol = &$this->_service->get_protocol_module($this->_info['ufeatures']['protocol'])) === false)
			return(false);

		$r = $protocol->delete($this->_info['protocol']['id']);

		if($this->_status === 'delete')
			$this->_return['protocol'] = $r;

		return($r);
	}

	function delete_userfeatures()
	{
		if(xivo_issa('ufeatures',$this->_info) === false
		|| ($ufeatures = &$this->_service->get_module('userfeatures')) === false)
			return(false);

		$r = $ufeatures->delete($this->_info['ufeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['userfeatures'] = $r;

		return($r);
	}

	function delete_hints()
	{
		return($this->delete_macro('hints'));
	}

	function delete_groupmember()
	{
		return($this->_delete_member_type('group'));
	}

	function delete_queuemember()
	{
		return($this->_delete_member_type('queue'));
	}

	function _delete_member_type($type)
	{
		if(($type !== 'group' && $type !== 'queue') === true)
			return(false);

		$key = $type.'member';

		if(xivo_issa($key,$this->_info) === false)
			return(null);

		if(xivo_issa('ufeatures',$this->_info) === false
		|| ($queuemember = &$this->_service->get_module('queuemember')) === false)
			return(false);

		$queuemember_where = array();
		$queuemember_where['usertype'] = 'user';
		$queuemember_where['category'] = $type;
		$queuemember_where['userid'] = $this->_info['ufeatures']['id'];

		$r = $queuemember->delete_where($queuemember_where);

		if($this->_status === 'delete')
			$this->_return[$key] = $r;

		return($r);
	}

	function delete_rightcall()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['ufeatures']['id'];

		$r = $this->_rightcall->delete($this->_status,'user',$id);

		if($this->_status === 'delete')
			$this->_return['rightcall'] = $r;

		return($r);
	}

	function delete_dialstatus()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['ufeatures']['id'];

		$r = $this->_dialstatus->delete($this->_status,'user',$id);

		if($this->_status === 'delete')
			$this->_return['dialstatus'] = $r;

		return($r);
	}

	function delete_voicemail()
	{
		if(xivo_issa('voicemail',$this->_info) === false)
			return(null);

		if(($voicemail = &$this->_service->get_module('uservoicemail')) === false)
			return(false);

		$r = $voicemail->delete($this->_info['voicemail']['id']);

		if($this->_status === 'delete')
			$this->_return['voicemail'] = $r;

		return($r);
	}

	function delete_phone()
	{
		if(xivo_issa('autoprov',$this->_info) === false)
			return(null);

		if(($autoprov = &$this->_service->get_module('autoprov')) === false)
			return(false);

		$r = $autoprov->userdeleted($this->_info['autoprov']['iduserfeatures']);

		if($this->_status === 'delete')
			$this->_return['autoprov'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'delete'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if($this->_return[$key] === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$protocol = &$this->_service->get_protocol_module($this->_info['ufeatures']['protocol']);
					$protocol->add_origin($this->_origin['protocol']);
					break;
				case 'userfeatures':
					$ufeatures = &$this->_service->get_module('userfeatures');
					
					if($this->_origin['ufeatures'] !== false)
						$ufeatures->add_origin($this->_origin['ufeatures']);
					break;
				case 'uservoicemail':
					$voicemail = &$this->_service->get_module('uservoicemail');

					if($this->_origin['voicemail'] !== false)
						$voicemail->add_origin($this->_origin['voicemail']);
					break;
				case 'localexten':
				case 'hints':
					$extensions = &$this->_service->get_module('extensions');

					if($this->_origin[$key] !== false)
						$extensions->add_origin($this->_origin[$key]);
					break;
				case 'groupmember':
				case 'queuemember':
					$queuemember = &$this->_service->get_module('queuemember');
					$queuemember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'dialstatus':
					$this->_dialstatus->add_origin_list();
					break;
				case 'autoprov':
					break;
				default:
					if(($module = &$this->_service->get_module($key)) === false)
						continue;

					if($this->_origin[$key] !== false)
						$module->add_origin($this->_origin[$key]);
			}
		}
		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		if(($incall = &$this->_service->get_module('incall')) !== false)
		{
			$incall_where = array();
			$incall_where['type'] = 'user';
			$incall_where['typeval'] = $this->_info['ufeatures']['id'];

			$incall->unlinked_where($incall_where);
		}

		$this->_dialstatus->unlinked('user',$this->_info['ufeatures']['id']);

		if(($schedule = &$this->_service->get_module('schedule')) === false)
			return(true);
		
		$schedule_where = array();
		$schedule_where['typetrue'] = 'user';
		$schedule_where['typevaltrue'] = $this->_info['ufeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		$schedule_where = array();
		$schedule_where['typefalse'] = 'user';
		$schedule_where['typevalfalse'] = $this->_info['ufeatures']['id'];

		$schedule->unlinked_where($schedule_where);
	}
}

?>
