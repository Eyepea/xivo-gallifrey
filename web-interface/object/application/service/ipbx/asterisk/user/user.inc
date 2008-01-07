<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_user extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_protocolname	= false;
	var $_origin		= array();
	var $_status		= '';
	var $_cnt		= 0;
	var $_userfeatures	= null;
	var $_protocol		= null;
	var $_extensions	= null;
	var $_extenumbers	= null;
	var $_rightcall		= null;
	var $_dialstatus	= null;
	var $_queuemember	= false;
	var $_uservoicemail	= false;
	var $_phonefunckey	= false;
	var $_autoprov		= false;
	var $_musiconhold	= false;
	var $_conf		= array();
	var $_param		= array();

	function xivo_application_service_asterisk_user(&$service,&$dso,$param=array())
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_userfeatures = &$this->_service->get_module('userfeatures')) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = &$this->get_abstract('extenumbers')) === false
		|| ($this->_rightcall = &$this->get_abstract('rightcall')) === false
		|| ($this->_dialstatus = &$this->get_abstract('dialstatus')) === false)
			return(false);

		$this->_queuemember = &$this->_service->get_module('queuemember');
		$this->_uservoicemail = &$this->_service->get_module('uservoicemail');
		$this->_phonefunckey = &$this->_service->get_module('phonefunckey');
		$this->_autoprov = &$this->_service->get_module('autoprov');

		$this->_load_config(dirname(__FILE__));

		$this->_param['internal'] = false;

		if(is_array($param) === true)
		{
			if(isset($param['internal']) === true)
				$this->_param['internal'] = $param['internal'];
		}

		if(($size = $this->_chk_upload_file()) === false)
			die();

		$this->_conf['import']['file']['size'] = $size;

		return(true);
	}

	function _reset()
	{
		$this->_protocolname = false;

		return(parent::_reset());
	}

	function get($id,$disable=null,$full=true,$internal=false,$where=false)
	{
		$full = (bool) $full;
		$where = (bool) $where;

		$this->_info = $this->_origin = array();

		$this->_userfeatures->set_dso_param('internal',$internal);

		if($where === false)
			$this->_info['ufeatures'] = $this->_userfeatures->get($id);
		else
			$this->_info['ufeatures'] = $this->_userfeatures->get_where($id);

		if($this->_info['ufeatures'] === false
		|| ($this->_protocol = &$this->_service->get_protocol_module($this->_info['ufeatures']['protocol'])) === false
		|| ($this->_info['protocol'] = $this->_protocol->get($this->_info['ufeatures']['protocolid'],$disable)) === false)
		{
			$this->_userfeatures->set_dso_param('internal',$this->_param['internal']);
			return(false);
		}

		$this->_userfeatures->set_dso_param('internal',$this->_param['internal']);

		$this->_origin['protocol'] = $this->_protocol->get_origin();
		$this->_origin['ufeatures'] = $this->_userfeatures->get_origin();

		if($full === false)
			return($this->_info);

		$this->_get_exten();
		$this->_get_hints();
		$this->_get_extenumbers();
		$this->_get_groupmember();
		$this->_get_queuemember();
		$this->_get_rightcall();
		$this->_get_dialstatus();
		$this->_get_voicemail();
		$this->_get_phonefunckey();
		$this->_get_autoprov();
		$this->get_musiconhold();

		return($this->_info);
	}

	function get_where($arr,$disable=null,$full=true,$internal=false)
	{
		return($this->get($arr,$disable,$full,$internal,true));
	}

	function get_nb($arr=null,$disable=null,$initialized=null)
	{
		return($this->_dso->get_nb($arr,$disable,$initialized));
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		if(isset($this->_info['protocol']['context']) === false)
			return(false);

		$this->_info['exten'] = $this->_extensions->get_exten('macro',
								    $this->_info['ufeatures']['number'],
								    $this->_info['protocol']['context']);

		$this->_origin['exten'] = $this->_extensions->get_origin();

		return($this->_info['exten']);
	}

	function _get_hints()
	{
		$this->_info['hints'] = false;
		$this->_origin['hints'] = false;

		if(isset($this->_info['protocol']['context']) === false)
			return(false);

		$this->_info['hints'] = $this->_extensions->get_hints($this->_info['protocol']['name'],
							       $this->_info['ufeatures']['protocol'],
							       $this->_info['ufeatures']['number']);
			
		$this->_origin['hints'] = $this->_extensions->get_origin();

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

		if($this->_queuemember === false
		|| ($module = &$this->_service->get_module($type.'features')) === false
		|| ($this->_info[$membertype] = $this->_queuemember->get_all_where($where)) === false
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

		$this->_origin[$membertype] = $this->_queuemember->get_origin_list();

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

		if($this->_uservoicemail === false)
			return(false);

		$voicemail_where = array();
		$voicemail_where['mailbox'] = $this->_info['ufeatures']['number'];
		$voicemail_where['context'] = $this->_info['ufeatures']['context'];
		$this->_info['voicemail'] = $this->_uservoicemail->get_where($voicemail_where);
		$this->_origin['voicemail'] = $this->_uservoicemail->get_origin();

		return($this->_info['voicemail']);
	}

	function _get_phonefunckey()
	{
		$this->_info['phonefunckey'] = false;
		$this->_origin['phonefunckey'] = false;

		if($this->_phonefunckey === false)
			return(false);

		$phonefunckey_where = array();
		$phonefunckey_where['iduserfeatures'] = $this->_info['ufeatures']['id'];
		$this->_info['phonefunckey'] = $this->_phonefunckey->get_all_where($phonefunckey_where,
										   true,
										   array('fknum' => SORT_ASC));
		$this->_origin['phonefunckey'] = $this->_phonefunckey->get_origin_list();

		return($this->_info['phonefunckey']);
	}

	function _get_autoprov()
	{
		$this->_info['autoprov'] = false;
		$this->_origin['autoprov'] = false;

		if($this->_autoprov === false)
			return(false);

		$autoprov_where = array();
		$autoprov_where['iduserfeatures'] = $this->_info['ufeatures']['id'];
		$this->_info['autoprov'] = $this->_autoprov->get_where($autoprov_where);
		$this->_origin['autoprov'] = $this->_autoprov->get_origin();

		return($this->_info['autoprov']);
	}

	function get_phonefunckey_type()
	{
		return($this->_phonefunckey->get_type_list());
	}

	function get_musiconhold()
	{
		if($this->_musiconhold !== false)
			return($this->_musiconhold);
		else if(($musiconhold = &$this->_service->get_module('musiconhold')) === false)
			return(false);

		if(($this->_musiconhold = $musiconhold->get_all_category(null,false)) !== false)
			ksort($this->_musiconhold);

		return($this->_musiconhold);
	}

	function get_bsfilter_list()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		$rs = $this->_dso->get_bsfilter($this->_info['ufeatures']['id'],
						$this->_info['ufeatures']['context'],
						$this->_info['ufeatures']['bsfilter']);

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];
			$ref = $this->_userfeatures->_prepare($ref);
			$ref['callfilteridentity'] = $ref['callfiltername'].' / '.$ref['identity'];
		}

		if(empty($rs) === true)
			$rs = false;

		return($rs);
	}

	function get_config_import_file()
	{
		return($this->_chk_exists_cfg('import','file',true));
	}

	function _get_config_file_maxsize()
	{
		$r = false;

		if(($import_file = $this->_chk_exists_cfg('import','file',true)) !== false
		&& isset($import_file['size']) === true)
			$r = xivo_get_memory($import_file['size'],false);

		return($r);
	}

	function get_elements()
	{
		$r = array();

		$r['protocol'] = $this->_service->get_protocol_element();
		$r['ufeatures'] = $this->_userfeatures->get_element();
		$r['dialstatus'] = $this->_dialstatus->get_element();

		if($this->_queuemember !== false)
			$r['qmember'] = $this->_queuemember->get_element();
		else
			$r['qmember'] = false;

		if($this->_uservoicemail !== false)
			$r['voicemail'] = $this->_uservoicemail->get_element();
		else
			$r['voicemail'] = false;

		if($this->_phonefunckey !== false)
			$r['phonefunckey'] = $this->_phonefunckey->get_element();
		else
			$r['phonefunckey'] = false;

		if($this->_autoprov !== false)
			$r['autoprov'] = $this->_autoprov->get_element();
		else
			$r['autoprov'] = false;

		return($r);
	}

	function get_all_context()
	{
		if(($r = $this->_userfeatures->get_all_context()) !== false)
			ksort($r);

		return($r);
	}

	function get_cnt()
	{
		return(xivo_uint($this->_cnt));
	}

	function get_users_search($search,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('search',null,$disable,$search,$order,$limit,$assoc,$initialized));
	}

	function get_users_search_number($search,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('search_number',null,$disable,$search,$order,$limit,$assoc,$initialized));
	}

	function get_users_list($protocols=null,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('list',$protocols,$disable,null,$order,$limit,$assoc,$initialized));
	}

	function get_users_context($context,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('context',null,$disable,$context,$order,$limit,$assoc,$initialized));
	}

	function _get_users($action,$protocols=null,$disable=null,$search='',$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		$action = (string) $action;
		$assoc = (bool) $assoc;
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$initialized,$order,$limit);
				break;
			case 'search_number':
				$rs = $this->_dso->search($search,$disable,$initialized,$order,$limit,1);
				break;
			case 'context':
				$rs = $this->_dso->search($search,$disable,$initialized,$order,$limit,2);
				break;
			case 'list':
			default:
				if(is_array($protocols) === true && empty($protocols) === false)
					$protocols = array_values($protocols);
				else
					$protocols = null;

				$rs = $this->_dso->get_all($protocols,$disable,$initialized,$order,$limit);
		}

		$this->_cnt = $this->_dso->get_cnt();

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];

			switch($ref['protocol'])
			{
				case XIVO_SRE_IPBX_AST_PROTO_SIP:
					$ref['initialized'] = xivo_haslen($ref['fullcontact']);
					$ref['secret'] = $ref['sipsecret'];
					$ref['callerid'] = $ref['sipcallerid'];
					break;
				case XIVO_SRE_IPBX_AST_PROTO_IAX:
					$ref['initialized'] = true;
					$ref['secret'] = $ref['iaxsecret'];
					$ref['callerid'] = $ref['iaxcallerid'];
					break;
				default:
					$ref['initialized'] = true;
					break;
			}

			$r[$ref['id']] = $this->_userfeatures->_prepare($ref);
		}
	
		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_dialstatus_result()
	{
		return($this->_dialstatus->get_result_for_display());
	}

	function get_phonefunckey_result()
	{
		$result = $this->get_result('phonefunckey');

		if(is_array($result) === false
		|| ($nb = count($result)) === 0)
			return(null);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[$i] = $this->_phonefunckey->_prepare($result[$i]);

		return($r);
	}

	function get_destination_list()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['ufeatures']['id'];

		$r = parent::get_destination_list('user',$id);
		$r['sounds'] = $this->_dialstatus->get_sound();

		return($r);
	}

	function set_add($arr,$protocolname)
	{
		return($this->_set('add',$arr,$protocolname));
	}

	function set_edit($arr,$protocolname)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr,$protocolname));
	}

	function _set($action,$arr,$protocolname)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('protocol',$arr) === false
		|| xivo_issa('ufeatures',$arr) === false)
			return(false);

		$this->_protocolname = $protocolname;

		$this->set_protocol($arr['protocol'],$protocolname);

		$arr['ufeatures']['protocol'] = $protocolname;
		$arr['ufeatures']['protocolid'] = 0;

		if($protocolname === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
			$name = $this->get_result_var('protocol','interface');
		else
			$name = $this->get_result_var('protocol','name');

		$context = $this->get_result_var('protocol','context');

		$arr['ufeatures']['name'] = $name;
		$arr['ufeatures']['context'] = $context;

		$this->set_userfeatures($arr['ufeatures']);

		$number = (string) $this->get_result_var('ufeatures','number');
		$protocolname = (string) $this->get_result_var('ufeatures','protocol');

		if(xivo_issa('group',$arr) === true
		&& xivo_issa('group-select',$arr) === true
		&& $this->set_groupmember($arr['group'],$arr['group-select'],$name,$context,$number,$protocolname) === true)
		{
			$callgroup = $this->get_result('callgroup');

			if(is_array($callgroup) === true && isset($callgroup[0]) === true)
				$callgroup = implode(',',$callgroup);
			else
				$callgroup = '';
			
			if($this->_protocolname !== XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
				$this->_set_result_var('protocol','callgroup',$callgroup);
		}

		if(xivo_issa('queue',$arr) === true && xivo_issa('queue-select',$arr) === true)
			$this->set_queuemember($arr['queue'],$arr['queue-select'],$name,$context,$number,$protocolname);

		if(xivo_issa('dialstatus',$arr) === true)
			$this->set_dialstatus($arr['dialstatus']);

		$numhaslen = xivo_haslen($number);

		if($this->_errnb === 0 && $numhaslen === true)
		{
			$appdata = array('appdata' => XIVO_SRE_IPBX_AST_USER_MACRO);
			if(($rsexten = $this->set_macro($action,'exten',$number,$appdata,$context)) !== false)
				$this->_set_result('exten',$rsexten);

			$this->set_extenumbers($action,'user',$number,$context);
		}

		if($this->_protocolname === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
		{
			if($this->get_errnb() > 0)
				$this->_protocolname = $r = false;
			else
				$r = true;

			return($r);
		}

		if($this->_errnb === 0 && $numhaslen === true)
		{
			$callerid = (string) $this->get_result_var('protocol','callerid');
			$callerid = $this->_service->mk_callerid($number,$callerid);
			$this->_set_result_var('protocol','callerid',$callerid);

			$this->_set_result_var('protocol','mailbox',$number.'@'.$context);

			$enablehint = (bool) $this->get_result_var('ufeatures','enablehint');

			if($enablehint === true)
				$enablehint = false;
			else
				$enablehint = true;

			if(($hints = $this->set_hints($action,$name,$protocolname,$number,$enablehint)) === false)
				$this->_set_error('hints');

			$this->_set_result('hints',$hints);
		}

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if(xivo_issa('voicemail',$arr) === true)
			$this->set_voicemail($arr['voicemail'],$context,$number);

		if(xivo_issa('phonefunckey',$arr) === true)
			$this->set_phonefunckey($arr['phonefunckey']);

		if(xivo_issa('autoprov',$arr) === true)
			$this->set_autoprov($action,$arr['autoprov'],$protocolname);

		if($this->get_errnb() > 0)
			$this->_protocolname = $r = false;
		else
			$r = true;

		return($r);
	}

	function set_protocol($arr,$protocolname)
	{
		if(is_array($arr) === false
		|| ($protocol = &$this->_service->get_protocol_module($protocolname)) === false)
		{
			$this->_set_result('protocol',null);
			$this->_set_error('protocol','invalid module');
			return(false);
		}

		if(($rs = $protocol->chk_values($arr)) === false)
		{
			$this->_set_result('protocol',$protocol->get_filter_result());
			$this->_set_error('protocol',$protocol->get_filter_error());
			return(false);
		}

		if(xivo_issa('allow',$rs) === true)
			$rs['allow'] = implode(',',$rs['allow']);

		$this->_set_result('protocol',$rs);

		return(true);
	}

	function set_userfeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('ufeatures',null);
			$this->_set_error('ufeatures','invalid data');
			return(false);
		}

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		if(($rs = $this->_userfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('ufeatures',$this->_userfeatures->get_filter_result());
			$this->_set_error('ufeatures',$this->_userfeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('ufeatures',$rs);

		return(true);
	}

	function set_hints($action,$name,$protocolname,$number,$disable=null)
	{
		if($action !== 'add' && $action !== 'edit')
			return(false);

		if($action === 'edit' && xivo_issa('hints',$this->_info) !== false)
			$rs = $this->_extensions->chk_hints($name,$protocolname,$number,$disable);
		else
			$rs = $this->_extensions->new_hints($name,$protocolname,$number,$disable);

		return($rs);
	}

	function set_groupmember($group,$select,$name,$context='',$number='',$protocolname='')
	{
		$r = $this->_set_member_type('group',$group,$select,$name,$context,$number,$protocolname);

		return($r);
	}

	function set_queuemember($queue,$select,$name,$context='',$number='',$protocolname='')
	{
		$r = $this->_set_member_type('queue',$queue,$select,$name,$context,$number,$protocolname);

		return($r);
	}

	function _set_member_type($type,$list,$select,$name,$context='',$number='',$protocolname='')
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| is_array($list) === false
		|| is_array($select) === false
		|| ($module = &$this->_service->get_module($type.'features')) === false
		|| $this->_queuemember === false)
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
			|| ($interface = $this->mk_interface($name,$chantype,$context,$number,$protocolname)) === false
			|| ($typeid = $module->get_id(array('name' => $qname))) === false)
				continue;

			$info['queue_name'] = $qname;
			$info['interface'] = $interface['interface'];
			$info['channel'] = $interface['channel'];

			$values = array_merge($list[$qname],$info);

			if(($rs = $this->_queuemember->chk_values($values)) === false)
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
		|| $this->_uservoicemail === false)
			return(false);

		$arr['context'] = $context;

		if($number !== '')
			$arr['mailbox'] = $number;

		if(($rs = $this->_uservoicemail->chk_values($arr)) === false)
		{
			$rs = array_merge($this->_uservoicemail->get_filter_result(),array('commented' => 0));

			$this->_set_result('voicemail',$rs);
			$this->_set_error('voicemail',$this->_uservoicemail->get_filter_error());

			return(false);
		}

		$rs['commented'] = 0;
		$this->_set_result('voicemail',$rs);

		return(true);
	}

	function set_phonefunckey($list)
	{
		if(($list = xivo_group_array('fknum',$list)) === false)
			return(false);

		$reslist = $errlist = $tmp = array();

		$nb = count($list);

		for($i = $j = 0;$i < $nb;$i++)
		{
			$ref = &$list[$i];
			$ref['iduserfeatures'] = 0;

			if(($val = $this->_phonefunckey->mk_values_array($ref)) === false)
				continue;
			else if(($info = $this->_phonefunckey->chk_values($val)) === false)
			{
				$errlist[$j] = $this->_phonefunckey->get_filter_error();
				$reslist[$j++] = $this->_phonefunckey->get_filter_result();
				continue;
			}
			else if(isset($tmp[$info['fknum']]) === true)
			{
				$errlist[$j] = 'wrong phonefunckey';
				$reslist[$j++] = $info;
				continue;
			}

			$tmp[$info['fknum']] = 1;
			$reslist[$j++] = $info;
		}

		$this->_set_result('phonefunckey',$reslist);

		if(empty($errlist) === false)
		{
			$this->_set_error('phonefunckey',$errlist);
			return(false);
		}
		
		return(true);
	}

	function set_autoprov($action,$arr,$protocolname)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($arr) === false
		|| $this->_autoprov === false
		|| ($list = $this->_autoprov->get_autoprov_list()) === false)
			return(false);

		if($action === 'edit' && xivo_issa('autoprov',$this->_info) === true)
		{
			if(xivo_haslen($arr,'modact') === false)
				return(null);

			$modact = $arr['modact'];
			$arr = $this->_info['autoprov'];
			$arr['modact'] = $modact;
			$arr['proto'] = $protocolname;
		}
		else if(xivo_haslen($arr,'vendormodel') === false
		|| xivo_haslen($arr,'macaddr') === false)
			return(null);
		else if(($pos = strpos($arr['vendormodel'],'.')) === false)
			$arr['vendormodel'] = $arr['modact'] = '';
		else
		{
			if(isset($arr['modact']) === false)
				$arr['modact'] = 'prov';

			$arr['iduserfeatures'] = 0;
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

			$arr['proto'] = $protocolname;
		}

		if(($rs = $this->_autoprov->chk_values($arr)) === false)
		{
			$this->_set_result('autoprov',$this->_autoprov->get_filter_result());
			$this->_set_error('autoprov',$this->_autoprov->get_filter_error());
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
		|| $this->_protocolname === false
		|| ($protocolid = $this->add_protocol()) === false
		|| ($ufeaturesid = $this->add_userfeatures($protocolid)) === false
		|| $this->add_macro('exten') === false
		|| $this->add_hints() === false
		|| $this->add_extenumbers($ufeaturesid) === false
		|| $this->add_groupmember($ufeaturesid) === false
		|| $this->add_queuemember($ufeaturesid) === false
		|| $this->add_rightcall($ufeaturesid) === false
		|| $this->add_dialstatus($ufeaturesid) === false
		|| $this->add_voicemail() === false
		|| $this->add_phonefunckey($ufeaturesid) === false
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
		|| ($protocol = &$this->_service->get_protocol_module($this->_protocolname)) === false)
			return(false);

		$this->_return['protocol'] = $protocol->add($rs);

		return($this->_return['protocol']);
	}

	function add_userfeatures($protocolid)
	{
		if(($protocolid = xivo_uint($protocolid)) === 0
		|| ($rs = $this->get_result('ufeatures')) === null
		|| $rs === false)
			return(false);

		$rs['protocolid'] = $protocolid;
		$this->_return['userfeatures'] = $this->_userfeatures->add($rs);

		return($this->_return['userfeatures']);
	}

	function add_hints()
	{
		if(($rs = $this->get_result('hints')) === null || $rs === false)
			return($rs);

		$this->_return['hints'] = $this->_extensions->add_hints($rs);

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
		
		$id = $this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $ufeaturesid;

			unset($rs[$i]['gfeaturesid'],$rs[$i]['qfeaturesid']);

			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
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

		$this->_return['uservoicemail'] = $this->_uservoicemail->add($rs);

		return($this->_return['uservoicemail']);
	}

	function add_phonefunckey($ufeaturesid)
	{
		$rs = false;

		if(($ufeaturesid = xivo_uint($ufeaturesid)) === 0
		|| ($rs = $this->get_result('phonefunckey')) === null
		|| $rs === false)
			return($rs);

		$context = $this->get_result_var('ufeatures','context');
		$bsfilter = $this->get_result_var('ufeatures','bsfilter');

		$id = $this->_return['phonefunckey'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];
			$ref['iduserfeatures'] = $ufeaturesid;

			if(($ref['typeextenumbers'] !== null
			|| $ref['typeextenumbersright'] !== null) === true
			&& $this->_phonefunckey->is_valid($ref['typeextenumbers'],
							  $ref['typevalextenumbers'],
							  $ref['typeextenumbersright'],
							  $ref['typevalextenumbersright'],
							  $ref['iduserfeatures'],
							  null,
							  $context) === false)
				return(false);
			else if($ref['typeextenumbers'] === 'extenfeatures'
			&& $ref['typevalextenumbers'] === 'bsfilter'
			&& $this->_dso->chk_bsfilter($ref['iduserfeatures'],
						     $context,
						     $bsfilter,
						     $ref['typevalextenumbersright']) === false)
				return(false);

			$id['iduserfeatures'] = $ref['iduserfeatures'];
			$id['fknum'] = $ref['fknum'];

			if($this->_phonefunckey->add($ref) === false)
				return(false);

			$this->_return['phonefunckey'][] = $id;
		}

		if(isset($this->_return['phonefunckey'][0]) === false)
			$this->_return['phonefunckey'] = false;

		return(true);
	}

	function add_phone($ufeaturesid)
	{
		$rs = false;

		if(($ufeaturesid = xivo_uint($ufeaturesid)) === 0
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$rs['iduserfeatures'] = $ufeaturesid;

		if($this->_autoprov->authoritative($rs,$rs['modact']) === false)
			$this->_return['autoprov'] = false;
		else
			$this->_return['autoprov'] = $ufeaturesid;

		return($this->_return['autoprov']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_protocolname === false
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
					$protocol = &$this->_service->get_protocol_module($this->_protocolname);
					$protocol->delete($val);
					break;
				case 'exten':
					$this->_extensions->delete_exten($val);
					break;
				case 'hints':
					$this->_extensions->delete_hints($val);
					break;
				case 'groupmember':
				case 'queuemember':
					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$this->_queuemember->delete($val[$j]);
					}
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
					break;
				case 'dialstatus':
					$this->_dialstatus->reverse_add();
					break;
				case 'phonefunckey':
					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$this->_phonefunckey->delete($val[$j]);
					}
					break;
				case 'autoprov':
					$this->_autoprov->userdeleted($val);
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
		|| $this->_protocolname === false
		|| ($protocolret = $this->edit_protocol()) === false)
		{
			$this->_status = '';
			return(false);
		}
		else if($this->edit_userfeatures($protocolret) === false)
		{
			$this->edit_origin_protocol($protocolret);
			$this->_status = '';
			return(false);
		}

		$unlinks = false;

		if($this->_info['ufeatures']['number'] !== '')
		{
			if($this->get_result('exten') === null
			|| $this->get_result('extenumbers') === null)
			{
				$unlinks = true;
				$this->delete_macro('exten');
				$this->delete_extenumbers();
				$this->delete_hints();
			}
			else if($this->edit_macro('exten') === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit($protocolret);
				$this->_status = '';
				return(false);
			}
			else if($this->_protocolname === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
				$this->delete_hints();
			else
			{
				if($this->_info['ufeatures']['protocol'] === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
					$hints = $this->add_hints();
				else
					$hints = $this->edit_hints();

				if($hints === false)
				{
					$this->_reverse_edit($protocolret);
					$this->_status = '';
					return(false);
				}
			}
		}
		else if($this->add_macro('exten') === false
		|| $this->add_extenumbers($this->_info['ufeatures']['id']) === false
		|| ($this->_protocolname !== XIVO_SRE_IPBX_AST_PROTO_CUSTOM
		   && $this->add_hints() === false) === true)
		{
			$this->_reverse_edit($protocolret);
			$this->_status = '';
			return(false);
		}

		if($this->edit_groupmember() === false
		|| $this->edit_queuemember() === false
		|| $this->edit_rightcall() === false
		|| $this->edit_dialstatus() === false
		|| $this->edit_voicemail() === false
		|| $this->edit_phonefunckey() === false)
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
		|| ($protocol = &$this->_service->get_protocol_module($this->_protocolname)) === false)
			return(false);

		if($this->_info['ufeatures']['protocol'] === $this->_protocolname)
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
		else if($this->_info['ufeatures']['protocol'] === $this->_protocolname)
			return($protocol->edit_origin($this->_origin['protocol']));
		else if(($protocolid = xivo_uint($protocolid)) === 0
		|| ($proto = &$this->_service->get_protocol_module($this->_protocolname)) === false
		|| $proto->delete($protocolid) === false)
			return(false);

		return($protocol->add_origin($this->_origin['protocol']));
	}

	function edit_userfeatures($protocolid)
	{
		if(xivo_issa('ufeatures',$this->_info) === false
		|| xivo_issa('protocol',$this->_info) === false
		|| ($rs = $this->get_result('ufeatures')) === null
		|| $rs === false)
			return(false);

		if($this->_info['ufeatures']['protocol'] === $this->_protocolname)
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

		$this->_return['userfeatures'] = $this->_userfeatures->edit($this->_info['ufeatures']['id'],$rs,$provisioning);

		return($this->_return['userfeatures']);
	}

	function edit_hints()
	{
		$rs = false;

		if(xivo_issa('hints',$this->_info) === false
		|| ($rs = $this->get_result('hints')) === null
		|| $rs === false)
			return($rs);

		$this->_return['hints'] = $this->_extensions->edit($this->_info['hints']['id'],$rs);

		return($this->_return['hints']);
	}

	function edit_groupmember()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);
		else if(xivo_issa('groupmember',$this->_info) === false)
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
		else if(xivo_issa('queuemember',$this->_info) === false)
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
	
		$id = $this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $this->_info['ufeatures']['id'];

			unset($rs[$i]['gfeaturesid'],$rs[$i]['qfeaturesid']);

			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
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
		else if(xivo_issa('ufeatures',$this->_info) === false
		|| $this->_uservoicemail === false
		|| ($rs = $this->get_result('voicemail')) === false)
			return(false);

		if($this->_info['ufeatures']['number'] === '')
			$r = $this->_uservoicemail->delete($this->_info['voicemail']['uniqueid']);
		else if($rs === null)
			$r = $this->_uservoicemail->disable($this->_info['voicemail']['uniqueid']);
		else
			$r = $this->_uservoicemail->edit($this->_info['voicemail']['uniqueid'],$rs);

		return(($this->_return['uservoicemail'] = $r));
	}

	function edit_phonefunckey()
	{
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);
		else if(xivo_issa('phonefunckey',$this->_info) === false)
			return($this->add_phonefunckey($this->_info['ufeatures']['id']));

		$this->_return['phonefunckey'] = $this->delete_phonefunckey();

		if(($rs = $this->get_result('phonefunckey')) === null || $rs === false)
			return($rs);
	
		$context = $this->get_result_var('ufeatures','context');
		$bsfilter = $this->get_result_var('ufeatures','bsfilter');

		$id = $this->_return['phonefunckey'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];
			$ref['iduserfeatures']  = $this->_info['ufeatures']['id'];

			if(($ref['typeextenumbers'] !== null
			|| $ref['typeextenumbersright'] !== null) === true
			&& $this->_phonefunckey->is_valid($ref['typeextenumbers'],
							  $ref['typevalextenumbers'],
							  $ref['typeextenumbersright'],
							  $ref['typevalextenumbersright'],
							  $ref['iduserfeatures'],
							  null,
							  $context) === false)
				return(false);
			else if($ref['typeextenumbers'] === 'extenfeatures'
			&& $ref['typevalextenumbers'] === 'bsfilter'
			&& $this->_dso->chk_bsfilter($ref['iduserfeatures'],
						     $context,
						     $bsfilter,
						     $ref['typevalextenumbersright']) === false)
				return(false);

			$id['iduserfeatures'] = $ref['iduserfeatures'];
			$id['fknum'] = $ref['fknum'];

			if($this->_phonefunckey->add($ref) === false)
				return(false);

			$this->_return['phonefunckey'][] = $id;
		}

		if(isset($this->_return['phonefunckey'][0]) === false)
			$this->_return['phonefunckey'] = false;

		return(true);
	}

	function edit_phone()
	{
		$rs = false;

		if(xivo_issa('ufeatures',$this->_info) === false
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$rs['iduserfeatures'] = $this->_info['ufeatures']['id'];

		if(xivo_issa('autoprov',$this->_info) !== false)
			$this->_return['autoprov'] = $this->_autoprov->notification($rs,$rs['modact']);
		else
			$this->_return['autoprov'] = $this->_autoprov->authoritative($rs,$rs['modact']);

		return($this->_return['autoprov']);
	}

	function _reverse_edit($protocolid=0)
	{
		if($this->get_errnb() > 0
		|| $this->_protocolname === false
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
					$this->_userfeatures->edit_origin($this->_origin['ufeatures']);
					break;
				case 'uservoicemail':
					if($this->_origin['voicemail'] === false)
						$this->_uservoicemail->delete($val);
					else
						$this->_uservoicemail->edit_origin($this->_origin['voicemail']);
					break;
				case 'exten':
					if($this->_origin['exten'] === false)
						$this->_extensions->delete_exten($val);
					else
						$this->_extensions->edit_origin($this->_origin['exten']);
					break;
				case 'hints':
					if($this->_origin['hints'] === false)
						$this->_extensions->delete_hints($val);
					else
						$this->_extensions->edit_origin($this->_origin['hints']);
					break;
				case 'groupmember':
				case 'queuemember':
					if($key === 'groupmember')
						$this->delete_groupmember();
					else
						$this->delete_queuemember();

					$this->_queuemember->add_origin_list($this->_origin[$key]);
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
				case 'phonefunckey':
					$this->delete_phonefunckey();
					$this->_phonefunckey->add_origin_list($this->_origin['phonefunckey']);
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
		|| $this->delete_macro('exten') === false
		|| $this->delete_hints() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_groupmember() === false
		|| $this->delete_queuemember() === false
		|| $this->delete_rightcall() === false
		|| $this->delete_dialstatus() === false
		|| $this->delete_voicemail() === false
		|| $this->delete_phonefunckey() === false
		|| $this->delete_phone() === false
		|| $this->delete_callfiltermember() === false)
		{
			$this->_reverse_delete();
			$this->_status = '';

			return(false);
		}

		if(isset($this->_return['exten']) === true
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
		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		$r = $this->_userfeatures->delete($this->_info['ufeatures']['id']);

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
		else if(xivo_issa('ufeatures',$this->_info) === false
		|| $this->_queuemember === false)
			return(false);

		$queuemember_where = array();
		$queuemember_where['usertype'] = 'user';
		$queuemember_where['category'] = $type;
		$queuemember_where['userid'] = $this->_info['ufeatures']['id'];

		$r = $this->_queuemember->delete_where($queuemember_where);

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
		else if($this->_uservoicemail === false)
			return(false);

		$r = $this->_uservoicemail->delete($this->_info['voicemail']['id']);

		if($this->_status === 'delete')
			$this->_return['voicemail'] = $r;

		return($r);
	}

	function delete_phonefunckey()
	{
		if(xivo_issa('phonefunckey',$this->_info) === false)
			return(null);
		else if(xivo_issa('ufeatures',$this->_info) === false
		|| $this->_phonefunckey === false)
			return(false);

		$phonefunckey_where = array();
		$phonefunckey_where['iduserfeatures'] = $this->_info['ufeatures']['id'];

		$r = $this->_phonefunckey->delete_where($phonefunckey_where);

		if($this->_status === 'delete')
			$this->_return['phonefunckey'] = $r;

		return($r);
	}

	function delete_phone()
	{
		if(xivo_issa('autoprov',$this->_info) === false)
			return(null);
		else if($this->_autoprov === false)
			return(false);

		$r = $this->_autoprov->userdeleted($this->_info['autoprov']['iduserfeatures']);

		if($this->_status === 'delete')
			$this->_return['autoprov'] = $r;

		return($r);
	}

	function delete_callfiltermember()
	{
		if(($callfiltermember = &$this->_service->get_module('callfiltermember')) === false)
			return(true);

		$callfiltermember_where = array();
		$callfiltermember_where['type'] = 'user';
		$callfiltermember_where['typeval'] = $this->_info['ufeatures']['id'];

		$callfiltermember->delete_where($callfiltermember_where);

		return(true);
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
					if($this->_origin['ufeatures'] !== false)
						$this->_userfeatures->add_origin($this->_origin['ufeatures']);
					break;
				case 'uservoicemail':
					if($this->_origin['voicemail'] !== false)
						$this->_uservoicemail->add_origin($this->_origin['voicemail']);
					break;
				case 'exten':
				case 'hints':
					if($this->_origin[$key] !== false)
						$this->_extensions->add_origin($this->_origin[$key]);
					break;
				case 'groupmember':
				case 'queuemember':
					$this->_queuemember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'dialstatus':
					$this->_dialstatus->add_origin_list();
					break;
				case 'phonefunckey':
					$this->_phonefunckey->add_origin_list($this->_origin['phonefunckey']);
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

		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if(is_object($this->_protocol) === false)
			$r = false;
		else if($this->enable_protocol() === false
		|| $this->enable_userfeatures() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if(is_object($this->_protocol) === false)
			$r = false;
		else if($this->disable_protocol() === false
		|| $this->disable_userfeatures() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return(true);
	}

	function enable_protocol()
	{
		return($this->_enable_disable_protocol(false));
	}

	function disable_protocol()
	{
		return($this->_enable_disable_protocol(true));
	}

	function _enable_disable_protocol($disable=false)
	{
		$disable = (bool) $disable;

		if(xivo_issa('protocol',$this->_info) === false)
			return(false);

		if($disable === false)
			$r = $this->_protocol->enable($this->_info['protocol']['id']);
		else
			$r = $this->_protocol->disable($this->_info['protocol']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['protocol'] = $r;

		return($r);
	}

	function enable_userfeatures()
	{
		return($this->_enable_disable_userfeatures(false));
	}

	function disable_userfeatures()
	{
		return($this->_enable_disable_userfeatures(true));
	}

	function _enable_disable_userfeatures($disable=false)
	{
		$disable = (bool) $disable;

		if(xivo_issa('ufeatures',$this->_info) === false)
			return(false);

		if($disable === false)
			$r = $this->_userfeatures->enable($this->_info['ufeatures']['id']);
		else
			$r = $this->_userfeatures->disable($this->_info['ufeatures']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['userfeatures'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_status !== 'enable' && $this->_status !== 'disable') === true
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		if($this->_status === 'enable')
			$disable = true;
		else
			$disable = false;

		$this->_status = '';

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if($this->_return[$key] === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$this->_enable_disable_protocol($disable);
					break;
				case 'userfeatures':
					$this->_enable_disable_userfeatures($disable);
					break;
			}
		}
		return(true);
	}

	function import_csv()
	{
		$file = new xivo_file();

		if(xivo::load_class('xivo::file::csv') === false
		|| $file->get_upload('import',$this->_conf['import']['file']) === false)
			return(false);

		$filecsv = new xivo_file_csv();

		if(($data = $filecsv->parse($file->info['tmp_name'],0,'|',null,true,
					    $this->_conf['import']['field'])) === false
		|| ($nb = count($data)) === 0)
		{
			xivo_file::rm($file->info['tmp_name']);
			return(false);
		}

		$appincall = &$this->_service->get_application('incall');

		$incall['incall'] = $incall = array();
		$incall['incall']['type'] = 'user';

		$r = true;

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$data[$i];

			if(xivo_issa('voicemail',$ref) === true)
			{
				$ref['voicemail']['voicemail-active'] = true;
				$ref['ufeatures']['enablevoicemail'] = true;
			}

			if(xivo_issa('protocol',$ref) === false
			|| isset($ref['protocol']['protocol']) === false
			|| $this->set_add($ref,$ref['protocol']['protocol']) === false
			|| $this->add() === false)
				$r = false;
			else if(isset($ref['incall']) === true && xivo_empty($ref['incall']) === false)
			{
				$incall['incall']['typeval'] = $this->get_return_var('userfeatures');
				$incall['incall']['exten'] = $ref['incall'];

				if($appincall->set_add($incall) === false
				|| $appincall->add() === false)
					$r = false;
			}
		}

		xivo_file::rm($file->info['tmp_name']);

		return($r);
	}
}

?>
