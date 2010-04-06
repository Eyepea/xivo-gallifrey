<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


dwho::load_class('dwho_network');
dwho::load_class('dwho_http');

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_skill extends xivo_application_asterisk_abstract
{
	var $_dso				= null;
	var $_skillcat	= false;
	var $_skill			= false;

	function xivo_application_service_asterisk_skill(&$service,&$dso,$param=array(),$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		$param = array();
		if($this->load_init($param) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		$this->_load_config(dirname(__FILE__));

/*
		if(($size = $this->_verify_can_upload_file()) === false)
			die();

		$this->_conf['import']['file']['size'] = $size;
*/

		return(true);
	}

	function load_init($param=array())
	{
		$this->_reset_info();
		$this->_reset_params();
		$this->_reset();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		$this->_skillcat		= &$this->_service->get_module('skillcat');
		$this->_skill				= &$this->_service->get_module('skill');

		return(true);
	}

	/** Return all users
	**/
	function get_all()
	{
		$rs = $this->_dso->get_all();
		return ($rs);
	}

	function set_n_add($arr)
	{
		$this->_reset();

		$name 		= $arr['skill']['name'];

		/*if(is_array($arr) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid data');
			return(false);
		}*/
		$values		= $arr['skill']['values'];//array('values' => $arr['skill']['values']);
		/*
		if(is_array($arr) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid data');
			return(false);
		}
		*/

		//echo "add: $name / $values. ";
		// ret === inserted ID
		$catid =  $this->_skillcat->add(array('id' => null, 'name' => $name));
		//echo "ret= $catid. ";
		//echo "ret is none: " . ($catid == null?"YES":"NO") . ". ";
		if($catid != null)
		{

			//print_r($values[0]);
			foreach($values as $k => $v)
			{
				//print_r($v);
				$value = array(
					'id' 					=> null, 
					'catid' 			=> $catid,
					'name'				=> $v[0],
					'description'	=> $v[1],
					'printscreen'	=> $v[2]
				);

				$skid = $this->_skill->add($value);
			}
		}

		$ret = $this->_skill->add($values);

		//echo "errs= " . $this->get_errnb() . ". ";
		return(($this->get_errnb() < 1));
	}

	//function get($id,$disable=null,$internal=false,$where=false,$nocomponents=null)
	function get($id)
	{
		$this->_info = $this->_origin = array();
		
		return $this->_dso->get($id);
	}


/*

	function get_where($arr,$disable=null,$internal=false)
	{
		return($this->get($arr,$disable,$internal,true));
	}

	function get_nb($arr=null,$disable=null,$initialized=null)
	{
		return($this->_dso->get_nb($arr,$disable,$initialized));
	}

	function _get_usermacro()
	{
		$this->_info['usermacro'] = false;
		$this->_origin['usermacro'] = false;

		if(isset($this->_info['protocol']['context']) === true
		&& ($this->_info['usermacro'] = $this->_extensions->get_exten($this->_info['userfeatures']['number'],
									      $this->_info['protocol']['context'],
									      'macro')) !== false)
			$this->_origin['usermacro'] = $this->_extensions->get_origin();

		return($this->_info['usermacro']);
	}

	function _get_hints()
	{
		$this->_info['hints'] = false;
		$this->_origin['hints'] = false;

		if(isset($this->_info['protocol']['context']) === true
		&& ($this->_info['hints'] = $this->_extensions->get_hints($this->_info['protocol']['name'],
									  $this->_info['userfeatures']['protocol'],
									  $this->_info['userfeatures']['number'])) !== false)
			$this->_origin['hints'] = $this->_extensions->get_origin();

		return($this->_info['hints']);
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
		$where['userid'] = $this->_info['userfeatures']['id'];

		if(is_object($this->_queuemember) === false
		|| ($module = &$this->_service->get_module($type.'features')) === false
		|| ($this->_info[$membertype] = $this->_queuemember->get_all_where($where)) === false
		|| ($nb = count($this->_info[$membertype])) === 0)
		{
			$this->_info[$membertype] = false;
			return(false);
		}

		$ref = &$this->_info[$membertype];

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = $module->get_primary(array('name' => $ref[$i]['queue_name']))) === false)
				continue;
			else if($type === 'queue')
				$ref[$i]['queuefeaturesid'] = $id;
			else
				$ref[$i]['groupfeaturesid'] = $id;
		}

		$this->_origin[$membertype] = $this->_queuemember->get_origin_list();

		return($this->_info[$membertype]);
	}

	function _get_voicemail()
	{
		$this->_info['voicemail'] = $this->_info['voicemailfeatures'] = false;
		$this->_origin['voicemail'] = $this->_origin['voicemailfeatures'] = false;

		if(is_object($this->_voicemail) === false
		|| $this->_voicemail->get($this->_info['userfeatures']['voicemailid']) === false)
			return(false);

		$this->_info['voicemail'] = $this->_voicemail->get_info('voicemail');
		$this->_info['voicemailfeatures'] = $this->_voicemail->get_info('voicemailfeatures');

		$this->_origin['voicemail'] = $this->_voicemail->get_origin('voicemail');
		$this->_origin['voicemailfeatures'] = $this->_voicemail->get_origin('voicemailfeatures');

		return($this->_info['voicemail']);
	}

	function _get_agentfeatures()
	{
		$this->_info['agentfeatures'] = false;
		$this->_origin['agentfeatures'] = false;

		if(is_object($this->_agentfeatures) === false
		|| $this->_agentfeatures->get($this->_info['userfeatures']['agentid']) === false)
			return(false);

		$this->_origin['agentfeatures'] = $this->_agentfeatures->get_origin();

		return($this->_info['agentfeatures']);
	}

	function _get_phonefunckey()
	{
		$this->_info['phonefunckey'] = false;
		$this->_origin['phonefunckey'] = false;

		if(is_object($this->_phonefunckey) === false)
			return(false);

		$where = array();
		$where['iduserfeatures'] = $this->_info['userfeatures']['id'];

		$this->_info['phonefunckey'] = $this->_phonefunckey->get_all_where($where,
										   null,
										   true,
										   array('fknum' => SORT_ASC));
		$this->_origin['phonefunckey'] = $this->_phonefunckey->get_origin_list();

		return($this->_info['phonefunckey']);
	}

	function _get_autoprov()
	{
		$this->_info['autoprov'] = false;
		$this->_origin['autoprov'] = false;

		if(is_object($this->_autoprov) === false)
			return(false);

		$where = array();
		$where['iduserfeatures'] = $this->_info['userfeatures']['id'];

		$this->_info['autoprov'] = $this->_autoprov->get_where($where);
		$this->_origin['autoprov'] = $this->_autoprov->get_origin();

		return($this->_info['autoprov']);
	}

	function get_timezones()
	{
		if(is_object($this->_voicemail) === false)
			return(false);

		return($this->_voicemail->get_timezones());
	}

	function get_phonefunckey_type()
	{
		return($this->_phonefunckey->get_type_list());
	}

	function get_bsfilter_list()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);

		$rs = $this->_dso->get_bsfilter($this->_info['userfeatures']['id'],
						$this->_info['userfeatures']['context'],
						$this->_info['userfeatures']['bsfilter']);

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
		if(($import_file = $this->_chk_exists_cfg('import','file',true)) !== false
		&& isset($import_file['size']) === true)
			return(dwho_get_memory($import_file['size'],false));

		return(false);
	}

	function get_voicemail_list()
	{
		if(is_object($this->_voicemail) === false)
			return(false);

		$order = array();
		$order['fullname'] = SORT_ASC;
		$order['mailbox'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		return($this->_voicemail->get_voicemail_list(null,$order));
	}

	function get_agent_list()
	{
		if(is_object($this->_agentfeatures) === false)
			return(false);

		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;
		$order['number'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		return($this->_agentfeatures->get_all(null,true,$order));
	}

	function get_autoprov_list()
	{
		if(is_object($this->_autoprov) === false)
			return(false);

		return($this->_autoprov->get_autoprov_list());
	}

	function get_profileclient_list()
	{
		if(empty($this->_profileclientlist) === false)
			return($this->_profileclientlist);
		else if(($list = $this->_service->discuss('xivo[ctiprofilelist,get]',true)) === false
		|| is_array($list) === false
		|| isset($list[0]) === false
		|| dwho::load_class('dwho_json') === false
		|| ($list = dwho_json::decode($list[0],true)) === false
		|| is_array($list) === false
		|| empty($list) === true)
			return(false);

		$this->_profileclientlist = array();

		foreach($list as $key => $val)
		{
			if(preg_match('/^[a-z0-9_-]+$/',$key) === 1)
				$this->_profileclientlist[$key] = $val;
		}

		return($this->_profileclientlist);
	}

	function get_elements()
	{
		$r = array();

		$r['protocol'] = $this->_service->get_protocol_element();
		$r['userfeatures'] = $this->_userfeatures->get_element();
		$r['dialaction'] = $this->_dialaction->get_element();

		if(is_object($this->_queuemember) === true)
			$r['qmember'] = $this->_queuemember->get_element();
		else
			$r['qmember'] = false;

		if(is_object($this->_voicemail) === true
		&& ($vmelement = $this->_voicemail->get_elements()) !== false)
			$r = array_merge($vmelement,$r);
		else
			$r['voicemail'] = $r['voicemailfeatures'] = false;

		if(is_object($this->_phonefunckey) === true)
			$r['phonefunckey'] = $this->_phonefunckey->get_element();
		else
			$r['phonefunckey'] = false;

		if(is_object($this->_autoprov) === true)
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

	function get_users_search($search,$context='',$protocols=null,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('search',$protocols,$disable,$search,$context,$order,$limit,$assoc,$initialized));
	}

	function get_users_search_number($search,$context='',$protocols=null,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('search_number',$protocols,$disable,$search,$context,$order,$limit,$assoc,$initialized));
	}

	function get_users_list($protocols=null,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('list',$protocols,$disable,null,null,$order,$limit,$assoc,$initialized));
	}

	function get_users_context($context,$protocols=null,$disable=null,$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		return($this->_get_users('context',$protocols,$disable,null,$context,$order,$limit,$assoc,$initialized));
	}

	function _get_users($action,$protocols=null,$disable=null,$search='',$context='',$order=null,$limit=null,$assoc=false,$initialized=null)
	{
		$search = strval($search);

		if(is_array($protocols) === true && empty($protocols) === false)
			$protocols = array_values($protocols);
		else
			$protocols = null;

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$context,$protocols,$disable,$initialized,$order,$limit);
				break;
			case 'search_number':
				$rs = $this->_dso->search($search,$context,$protocols,$disable,$initialized,$order,$limit,'number');
				break;
			case 'context':
				$rs = $this->_dso->get_all($protocols,$disable,$initialized,$order,$limit,null,$context);
				break;
			case 'list':
			default:
				$rs = $this->_dso->get_all($protocols,$disable,$initialized,$order,$limit);
		}

		$this->_cnt = $this->_dso->get_cnt();

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];

			$userfeatures = $this->_userfeatures->_prepare(
						dwho_array_startswith_key($ref,'userfeatures.',true));

			switch($userfeatures['protocol'])
			{
				case XIVO_SRE_IPBX_AST_PROTO_SIP:
					$sip = dwho_array_startswith_key($ref,'usersip.',true);
					$sip['initialized'] = dwho_has_len($sip['fullcontact']);
					unset($sip['fullcontact']);
					$r[$userfeatures['id']] = array_merge($sip,$userfeatures);
					break;
				case XIVO_SRE_IPBX_AST_PROTO_IAX:
					$iax = dwho_array_startswith_key($ref,'useriax.',true);
					$iax['initialized'] = true;
					$r[$userfeatures['id']] = array_merge($iax,$userfeatures);
					break;
				case XIVO_SRE_IPBX_AST_PROTO_CUSTOM:
					$custom = dwho_array_startswith_key($ref,'usercustom.',true);
					$custom['initialized'] = true;
					$r[$userfeatures['id']] = array_merge($custom,$userfeatures);
					break;
				default:
					$this->_cnt = 0;
					return(false);
			}
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_dialaction_result()
	{
		return($this->_dialaction->get_result_for_display());
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
		if(dwho_issa('userfeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['userfeatures']['id'];

		$this->_get_destination();

		$r = $this->_destination;
		$r['users'] = $this->get_destination_users($id);
		$r['groups'] = $this->get_destination_groups();
		$r['queues'] = $this->get_destination_queues();
		$r['meetme'] = $this->get_destination_meetme();
		$r['voicemail'] = $this->get_destination_voicemail();
		$r['schedule'] = $this->get_destination_schedule();
		$r['sounds'] = $this->_dialaction->get_sound();
		$r['voicemenu'] = $this->get_destination_voicemenu();

		return($r);
	}

	function get_phonefunckey_identity()
	{
		if(dwho_issa('userfeatures',$this->_info) === false
		|| ($rs = $this->_dso->get_phonefunckey($this->_info['userfeatures']['id'])) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];

			$phonefunckey = $this->_phonefunckey->_prepare(
						dwho_array_startswith_key($ref,'phonefunckey.',true));

			$res = null;

			switch($phonefunckey['type'])
			{
				case 'user':
					$res = dwho_array_startswith_key($ref,'userfeatures.',true);
					$res['identity'] = $this->_service->identity('userfeatures',$res,false);
					break;
				case 'extenfeatures-groupaddmember':
				case 'extenfeatures-groupremovemember':
				case 'extenfeatures-grouptogglemember':
				case 'group':
					$res = dwho_array_startswith_key($ref,'groupfeatures.',true);
					$res['identity'] = $this->_service->identity('groupfeatures',$res,false);
					break;
				case 'extenfeatures-queueaddmember':
				case 'extenfeatures-queueremovemember':
				case 'extenfeatures-queuetogglemember':
				case 'queue':
					$res = dwho_array_startswith_key($ref,'queuefeatures.',true);
					$res['identity'] = $this->_service->identity('queuefeatures',$res,false);
					break;
				case 'meetme':
					$res = dwho_array_startswith_key($ref,'meetmefeatures.',true);
					$res['identity'] = $this->_service->identity('meetmefeatures',$res,false);
					break;
				case 'extenfeatures-agentdynamiclogin':
				case 'extenfeatures-agentstaticlogin':
				case 'extenfeatures-agentstaticlogoff':
				case 'extenfeatures-agentstaticlogtoggle':
					$res = dwho_array_startswith_key($ref,'agentfeatures.',true);
					$res['identity'] = $this->_service->identity('agentfeatures',$res,false);
					break;
			}

			$r[] = array('phonefunckey'	=> $phonefunckey,
				     'result'		=> $res);
		}

		if(empty($r) === true)
			return(false);

		return($r);
	}

	function _get_destination()
	{
		if(is_array($this->_destination) === true)
			return(true);

		$this->_destination = array();

		return(true);
	}

	function _mk_usermacro_appdata($userfeaturesid,$timeout='')
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_USER_MACRO;
		$arr['appdata'][1] = dwho_ulongint($userfeaturesid);
		$arr['appdata'][2] = dwho_has_len($timeout) === true ? dwho_ufloat($timeout) : '';

		return($arr);
	}

	function set_add($arr,$protocolname,$chkcontext=true)
	{
		return($this->_set('add',$arr,$protocolname,$chkcontext));
	}

	function set_edit($arr,$protocolname,$chkcontext=true)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr,$protocolname,$chkcontext));
	}

	function _set($action,$arr,$protocolname,$chkcontext=true)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| dwho_issa('protocol',$arr) === false
		|| dwho_issa('userfeatures',$arr) === false)
			return(false);

		$this->_protocolname = $protocolname;

		if(isset($arr['protocol']['callerid']) === true)
			$arr['protocol']['callerid'] = $this->_service->mk_callerid($arr['protocol']['callerid']);

		$this->set_protocol($arr['protocol'],$protocolname);

		$arr['userfeatures']['protocol'] = $protocolname;
		$arr['userfeatures']['protocolid'] = 0;

		if($protocolname === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
			$name = $this->get_result_var('protocol','interface');
		else
			$name = $this->get_result_var('protocol','name');

		$context = $this->get_result_var('protocol','context');

		$vmaction = $action;

		if($this->_protocolname === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
			$arr['voicemail-option'] = 'none';

		if(isset($arr['voicemail-option']) === true)
		{
			switch($arr['voicemail-option'])
			{
				case 'add':
				case 'none':
					$vmaction = $arr['voicemail-option'];
					$arr['userfeatures']['voicemailid'] = null;
					break;
				case 'search':
					$vmaction = 'edit';
					break;
			}
		}

		$arr['userfeatures']['name'] = $name;
		$arr['userfeatures']['context'] = $context;

		$this->set_userfeatures($action,$arr['userfeatures']);

		$number = (string) $this->get_result_var('userfeatures','number');
		$protocolname = (string) $this->get_result_var('userfeatures','protocol');
		$simultcalls = (int) $this->get_result_var('userfeatures','simultcalls');
		$musiconhold = (string) $this->get_result_var('userfeatures','musiconhold');

		if($this->_protocolname === XIVO_SRE_IPBX_AST_PROTO_SIP)
			$this->_set_result_var('protocol','call-limit',(2 * $simultcalls));

		if($this->_protocolname !== XIVO_SRE_IPBX_AST_PROTO_CUSTOM
		&& dwho_has_len($musiconhold) === true)
			$this->_set_result_var('protocol','mohsuggest',$musiconhold);

		if(dwho_issa('group',$arr) === true
		&& dwho_issa('group-select',$arr) === true
		&& $this->set_groupmember($arr['group'],
					  $arr['group-select'],
					  $name,
					  $context,
					  $number,
					  $protocolname) === true
		&& $this->_protocolname === XIVO_SRE_IPBX_AST_PROTO_SIP)
		{
			$callgroup = $this->get_result('callgroup');

			if(is_array($callgroup) === true && isset($callgroup[0]) === true)
				$callgroup = implode(',',$callgroup);
			else
				$callgroup = '';

			$this->_set_result_var('protocol','callgroup',$callgroup);
		}

		if(dwho_issa('queue',$arr) === true && dwho_issa('queue-select',$arr) === true)
			$this->set_queuemember($arr['queue'],
					       $arr['queue-select'],
					       $name,
					       $context,
					       $number,
					       $protocolname);

		if(dwho_issa('dialaction',$arr) === true)
			$this->set_dialaction($arr['dialaction']);

		if((bool) $chkcontext === true)
			$this->set_contextnummember($context,$number);

		$numhaslen = dwho_has_len($number);

		if($this->_errnb === 0 && $numhaslen === true)
		{
			if(($rsusermacro = $this->chk_macro('usermacro',
							    $number,
							    $this->_mk_usermacro_appdata(0),
							    $context)) !== false)
				$this->_set_result('usermacro',$rsusermacro);

			$this->set_extenumbers($action,$number,$context);
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
			$callerid = $this->_service->mk_callerid($callerid,$number);
			$this->_set_result_var('protocol','callerid',$callerid);

			$enablehint = (bool) $this->get_result_var('userfeatures','enablehint') === false;

			if(($hints = $this->set_hints($action,$name,$protocolname,$number,$enablehint)) === false)
				$this->_set_error('hints');

			$this->_set_result('hints',$hints);
		}

		if(dwho_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if(dwho_issa('voicemail',$arr) === true
		&& dwho_has_len($arr['voicemail'],'mailbox') === true)
		{
			if($vmaction === 'add')
				$arr['voicemail']['context'] = $context;

			if(isset($arr['voicemail']['deletevoicemail']) === false)
				$arr['voicemail']['deletevoicemail'] = 0;

			$this->_set_result_var('protocol','mailbox',$arr['voicemail']['mailbox'].'@'.$context);

			if($vmaction !== 'disable')
			{
				if(dwho_issa('voicemailfeatures',$arr) === true)
					$this->set_voicemail($vmaction,$arr['voicemail'],$arr['voicemailfeatures']);
				else
					$this->set_voicemail($vmaction,$arr['voicemail']);
			}
		}

		if(dwho_issa('phonefunckey',$arr) === true)
			$this->set_phonefunckey($arr['phonefunckey']);

		if(dwho_issa('autoprov',$arr) === true)
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

		if(isset($arr['host-type']) === true)
			$arr['host'] = $arr['host-type'];
		else if(isset($arr['host']) === false)
			$arr['host'] = '';

		if(isset($arr['host-static']) === true && $arr['host'] === 'static')
			$arr['host'] = $arr['host-static'];

		if(($rs = $protocol->chk_values($arr)) === false)
		{
			$this->_set_result('protocol',$protocol->get_filter_result());
			$this->_set_error('protocol',$protocol->get_filter_error());
			return(false);
		}

		if(dwho_issa('allow',$rs) === true)
			$rs['allow'] = implode(',',$rs['allow']);

		$this->_set_result('protocol',$rs);

		return(true);
	}

	function set_userfeatures($action,$arr)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| ($action === 'edit' && dwho_issa('userfeatures',$this->_origin) === false) === true)
			return(false);
		else if(is_array($arr) === false)
		{
			$this->_set_result('userfeatures',null);
			$this->_set_error('userfeatures','invalid data');
			return(false);
		}

		$this->get_musiconhold();

		if(isset($arr['musiconhold']) === true
		&& isset($this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		if($action === 'edit'
		&& isset($arr['profileclient']) === false
		&& isset($arr['enableclient']) === false)
			$arr['profileclient'] = $this->_origin['userfeatures']['profileclient'];

		$this->get_profileclient_list();

		if(isset($arr['profileclient']) === true
		&& isset($this->_profileclientlist[$arr['profileclient']]) === false)
			$arr['profileclient'] = '';

		if(isset($arr['destunc']) === true
		&& dwho_has_len($arr['destunc']) === false)
			$arr['enableunc'] = false;

		if(isset($arr['destrna']) === true
		&& dwho_has_len($arr['destrna']) === false)
			$arr['enablerna'] = false;

		if(isset($arr['destbusy']) === true
		&& dwho_has_len($arr['destbusy']) === false)
			$arr['enablebusy'] = false;

		if(isset($arr['outcallerid-type']) === true)
			$arr['outcallerid'] = $arr['outcallerid-type'];
		else if(isset($arr['outcallerid']) === false)
			$arr['outcallerid'] = '';

		if(isset($arr['outcallerid-custom']) === true
		&& $arr['outcallerid'] === 'custom')
			$arr['outcallerid'] = $arr['outcallerid-custom'];

		if(($rs = $this->_userfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('userfeatures',$this->_userfeatures->get_filter_result());
			$this->_set_error('userfeatures',$this->_userfeatures->get_filter_error());
			return(false);
		}
		else
		{
			if($action === 'edit')
				$id = $this->_origin['userfeatures']['id'];
			else
				$id = null;

			if($this->_userfeatures->loginclient_exists($rs['loginclient'],
								    $rs['context'],
								    $id) === true)
			{
				$this->_set_result('userfeatures',$rs);
				$this->_set_error('userfeatures','client login exists');
				return(false);
			}
		}

		if(isset($rs['loginclient']) === false
		|| dwho_has_len($rs['loginclient']) === false)
		{
			$rs['enableclient'] = false;
			$rs['passwdclient'] = '';
		}

		if($this->_protocolname === XIVO_SRE_IPBX_AST_PROTO_CUSTOM
		|| dwho_has_len($rs['agentid']) === false
		|| is_object($this->_agentfeatures) === false
		|| $this->_agentfeatures->get_primary($rs['agentid']) === false)
			$rs['agentid'] = null;

		$this->_set_result('userfeatures',$rs);

		return(true);
	}

	function _set_usermacro($action,$userfeaturesid)
	{
		$rs = false;

		if(($userfeaturesid = dwho_ulongint($userfeaturesid)) === 0
		|| ($rs = $this->get_result('usermacro')) === null
		|| $rs === false)
			return($rs);
		else if(($rsmacro = $this->set_macro($action,
						     'usermacro',
						     $rs['exten'],
						     $this->_mk_usermacro_appdata($userfeaturesid),
						     $rs['context'])) === false)
			return(false);

		$this->_set_result('usermacro',$rsmacro);

		return(true);
	}

	function set_hints($action,$name,$protocolname,$number,$disable=null)
	{
		if($action !== 'add' && $action !== 'edit')
			return(false);
		else if($action === 'edit' && dwho_issa('hints',$this->_info) !== false)
			return($this->_extensions->chk_hints($name,$protocolname,$number,$disable));

		return($this->_extensions->new_hints($name,$protocolname,$number,$disable));
	}

	function set_groupmember($group,$select,$name,$context='',$number='',$protocolname='')
	{
		return($this->_set_member_type('group',$group,$select,$name,$context,$number,$protocolname));
	}

	function set_queuemember($queue,$select,$name,$context='',$number='',$protocolname='')
	{
		return($this->_set_member_type('queue',$queue,$select,$name,$context,$number,$protocolname));
	}

	function _set_member_type($type,$list,$select,$name,$context='',$number='',$protocolname='')
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| is_array($list) === false
		|| is_array($select) === false
		|| ($module = &$this->_service->get_module($type.'features')) === false
		|| is_object($this->_queuemember) === false)
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
			|| ($chantype = dwho_ak('chantype',$list[$qname],true)) === false
			|| ($interface = $this->mk_interface($name,
							     $chantype,
							     $context,
							     $number,
							     $protocolname)) === false
			|| ($typeid = $module->get_primary(array('name' => $qname))) === false)
				continue;

			$info['queue_name'] = $qname;
			$info['interface'] = $interface['interface'];
			$info['channel'] = $interface['channel'];

			$values = array_merge($list[$qname],$info);

			if(($rs = $this->_queuemember->chk_values($values)) === false)
				continue;
			else if($type === 'queue')
				$rs['queuefeaturesid'] = $typeid;
			else
			{
				$rs['groupfeaturesid'] = $typeid;
				$callgroup[] = $typeid;
			}

			$tmp[$qname] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result($key,false);
			$this->_set_error($key,'empty');

			if($type === 'group')
				$this->_set_result('callgroup','');

			return(false);
		}

		$this->_set_result($key,$reslist);

		if($type === 'group')
			$this->_set_result('callgroup',$callgroup);

		return(true);
	}

	function set_voicemail($action,$voicemail,$voicemailfeatures=null)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($voicemail) === false
		|| empty($voicemail) === true
		|| is_object($this->_voicemail) === false)
			return(false);

		$rs = array();
		$rs['voicemail'] = $voicemail;

		if(is_array($voicemailfeatures) === true)
			$rs['voicemailfeatures'] = $voicemailfeatures;

		if($action === 'add')
			$r = $this->_voicemail->set_add($rs);
		else
			$r = $this->_voicemail->set_edit($rs,false);

		if(($voicemailerror = $this->_voicemail->get_error('voicemail')) !== null)
		{
			$this->_set_error('voicemail',$voicemailerror);
			$this->_set_result('voicemail',$this->_voicemail->get_result('voicemail'));
		}
		else $this->_set_result('voicemail',$this->_voicemail->get_result('voicemail'));

		if(($voicemailfeatureserror = $this->_voicemail->get_error('voicemailfeatures')) !== null)
		{
			$this->_set_error('voicemailfeatures',$voicemailfeatureserror);
			$this->_set_result('voicemailfeatures',$this->_voicemail->get_result('voicemailfeatures'));
		}
		else $this->_set_result('voicemailfeatures',$this->_voicemail->get_result('voicemailfeatures'));

		return($r);
	}

	function set_phonefunckey($list)
	{
		if(($list = dwho_group_array('fknum',$list)) === false)
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
			}
			else if(isset($tmp[$info['fknum']]) === true)
			{
				$errlist[$j] = 'wrong phonefunckey';
				$reslist[$j++] = $info;
			}
			else
			{
				$tmp[$info['fknum']] = 1;
				$reslist[$j++] = $info;
			}
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
		|| is_array($arr) === false)
			return(false);

		if($action === 'edit' && dwho_issa('autoprov',$this->_info) === true)
		{
			if(dwho_has_len($arr,'modact') === false)
				return(null);

			$modact = $arr['modact'];
			$arr = $this->_info['autoprov'];
			$arr['modact'] = $modact;
			$arr['proto'] = $protocolname;
		}
		else if(dwho_has_len($arr,'vendormodel') === false
		|| dwho_has_len($arr,'macaddr') === false)
			return(null);
		else if(($pos = strpos($arr['vendormodel'],'.')) === false)
			$arr['vendormodel'] = $arr['modact'] = '';
		else
		{
			if(isset($arr['modact']) === false)
				$arr['modact'] = 'prov';

			$arr['iduserfeatures'] = 0;
			$vendor = substr($arr['vendormodel'],0,$pos);
			$model = substr($arr['vendormodel'],$pos + 1);

			if(($list = $this->get_autoprov_list()) === false)
				return(false);

			if(dwho_issa($vendor,$list) === true
			&& dwho_issa('model',$list[$vendor]) === true
			&& dwho_issa($model,$list[$vendor]['model']) === true)
			{
				$arr['vendor'] = $vendor;
				$arr['model'] = $model;
			}

			if(($macaddr = dwho_network::normalize_macaddr($arr['macaddr'])) !== false
			&& dwho_network::valid_macaddr($macaddr) !== false)
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
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$userfeaturesid = null;

		if($this->get_errnb() > 0
		|| $this->_protocolname === false
		|| ($protocolid = $this->add_protocol()) === false
		|| ($userfeaturesid = $this->add_userfeatures($protocolid)) === false
		|| $this->update_protocol_setvar($protocolid,$userfeaturesid) === false
		|| $this->add_usermacro($userfeaturesid) === false
		|| $this->add_hints() === false
		|| $this->add_extenumbers($userfeaturesid) === false
		|| $this->add_groupmember($userfeaturesid) === false
		|| $this->add_queuemember($userfeaturesid) === false
		|| $this->add_rightcall($userfeaturesid) === false
		|| $this->add_dialaction($userfeaturesid) === false
		|| $this->add_contextnummember($userfeaturesid) === false
		|| $this->add_voicemail($userfeaturesid) === false
		|| $this->add_phonefunckey($userfeaturesid) === false
		|| $this->add_phone($userfeaturesid) === false)
		{
			$r = false;

			if($userfeaturesid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_protocol()
	{
		if(($rs = $this->get_result('protocol')) === null
		|| ($protocol = &$this->_service->get_protocol_module($this->_protocolname)) === false)
			return(false);

		return(($this->_return['protocol'] = $protocol->add($rs)));
	}

	function add_userfeatures($protocolid)
	{
		if(($protocolid = dwho_ulongint($protocolid)) === 0
		|| ($rs = $this->get_result('userfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['protocolid'] = $protocolid;

		return(($this->_return['userfeatures'] = $this->_userfeatures->add($rs)));
	}

	function update_protocol_setvar($protocolid,$userfeaturesid)
	{
		if($this->_protocolname === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
			return(null);
		else if(($protocolid = dwho_ulongint($protocolid)) === 0
		|| ($userfeaturesid = dwho_ulongint($userfeaturesid)) === 0
		|| ($protocol = &$this->_service->get_protocol_module($this->_protocolname)) === false)
			return(false);

		$val = array();
		$val['setvar'] = 'XIVO_USERID='.$userfeaturesid;

		return($protocol->edit($protocolid,$val));
	}

	function add_usermacro($userfeaturesid)
	{
		if(($rs = $this->_set_usermacro('add',$userfeaturesid)) !== true)
			return($rs);

		return(parent::add_macro('usermacro'));
	}

	function add_hints()
	{
		if(($rs = $this->get_result('hints')) === null || $rs === false)
			return($rs);

		return(($this->_return['hints'] = $this->_extensions->add_hints($rs)));
	}

	function add_groupmember($userfeaturesid)
	{
		return($this->_add_member_type('group',$userfeaturesid));
	}

	function add_queuemember($userfeaturesid)
	{
		return($this->_add_member_type('queue',$userfeaturesid));
	}

	function _add_member_type($type,$userfeaturesid)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| ($userfeaturesid = dwho_ulongint($userfeaturesid)) === 0)
			return(false);

		$key = $type.'member';

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);

		$ret = $this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $userfeaturesid;

			unset($rs[$i]['groupfeaturesid'],$rs[$i]['queuefeaturesid']);

			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return[$key][] = $ret;
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function add_voicemail($userfeaturesid)
	{
		if(($voicemailid = dwho_ulongint($this->get_result_var('userfeatures','voicemailid'))) === 0)
			return($this->_add_voicemail($userfeaturesid));
		else if(is_object($this->_voicemail) === false
		|| $this->_voicemail->get($voicemailid) === false
		|| ($rs = $this->get_result('voicemail')) === null
		|| $rs === false)
			return($rs);

		return(($this->_return['voicemail'] = $this->_voicemail->edit(false)));
	}

	function _add_voicemail($userfeaturesid)
	{
		if(($userfeaturesid = dwho_ulongint($userfeaturesid)) === 0
		|| ($rs = $this->get_result('voicemail')) === null
		|| $rs === false)
			return($rs);
		else if(($this->_return['voicemail'] = $this->_voicemail->add()) === true
		&& (($voicemailid = dwho_ulongint($voicemailid = $this->_voicemail->get_return('voicemail'))) === 0
		   || $this->_userfeatures->edit($userfeaturesid,array('voicemailid' => $voicemailid)) === false) === true)
			return(false);

		return($this->_return['voicemail']);
	}

	function add_phonefunckey($userfeaturesid)
	{
		$rs = false;

		if(($userfeaturesid = dwho_ulongint($userfeaturesid)) === 0
		|| ($rs = $this->get_result('phonefunckey')) === null
		|| $rs === false)
			return($rs);

		$context = $this->get_result_var('userfeatures','context');
		$bsfilter = $this->get_result_var('userfeatures','bsfilter');

		$id = $this->_return['phonefunckey'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];
			$ref['iduserfeatures'] = $userfeaturesid;

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

	function add_phone($userfeaturesid)
	{
		$rs = false;

		if(($userfeaturesid = dwho_ulongint($userfeaturesid)) === 0
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$rs['iduserfeatures'] = $userfeaturesid;

		if($this->_autoprov->authoritative($rs,$rs['modact']) === false)
			$this->_return['autoprov'] = false;
		else
			$this->_return['autoprov'] = $userfeaturesid;

		$this->_service->discuss('dialplan reload');

		return($this->_return['autoprov']);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_protocolname === false
		|| $this->_status !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$protocol = &$this->_service->get_protocol_module($this->_protocolname);
					$protocol->delete($val);
					break;
				case 'userfeatures':
					$this->_userfeatures->delete($val);
					break;
				case 'usermacro':
					$this->_extensions->delete_exten($val);
					break;
				case 'hints':
					$this->_extensions->delete_hints($val);
					break;
				case 'groupmember':
				case 'queuemember':
					if(is_array($val) === false)
						continue;

					foreach($val as $memberval)
						$this->_queuemember->delete($memberval);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
					break;
				case 'dialaction':
					$this->_dialaction->reverse_add();
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_add();
					break;
				case 'phonefunckey':
					if(is_array($val) === false)
						continue;

					foreach($val as $phonefkeyval)
					{
						if($phonefkeyval !== false)
							$this->_phonefunckey->delete($phonefkeyval);
					}
					break;
				case 'voicemail':
					$this->_voicemail->reverse_add();
					break;
				case 'autoprov':
					$this->_autoprov->userdeleted($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add();
					break;
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
		else if($this->_info['userfeatures']['number'] !== '')
		{
			if($this->get_result('usermacro') === null
			|| $this->get_result('extenumbers') === null)
			{
				$this->delete_macro('usermacro');
				$this->delete_extenumbers();
				$this->delete_hints();
			}
			else if($this->edit_usermacro($this->_info['userfeatures']['id']) === false
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
				if($this->_info['userfeatures']['protocol'] === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
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
		else if($this->add_usermacro($this->_info['userfeatures']['id']) === false
		|| $this->add_extenumbers($this->_info['userfeatures']['id']) === false
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
		|| $this->edit_dialaction() === false
		|| $this->edit_contextnummember() === false
		|| $this->edit_voicemail() === false
		|| $this->edit_phonefunckey() === false)
		{
			$this->_reverse_edit($protocolret);
			$this->_status = '';
			return(false);
		}

		$this->edit_phone();
		$this->_status = '';

		return(true);
	}

	function edit_protocol()
	{
		if(dwho_issa('protocol',$this->_info) === false
		|| dwho_issa('userfeatures',$this->_info) === false
		|| ($protocol = &$this->_service->get_protocol_module($this->_protocolname)) === false)
			return(false);
		else if($this->_info['userfeatures']['protocol'] === $this->_protocolname)
		{
			if(($rs = $this->get_result('protocol')) === null)
				return(false);

			return(($this->_return['protocol'] = $protocol->edit($this->_info['protocol']['id'],$rs)));
		}
		else if(($protocolid = $this->add_protocol()) === false)
			return(false);
		else if($this->delete_protocol() === false
		|| $this->update_protocol_setvar($protocolid,
						 $this->_info['userfeatures']['id']) === false)
		{
			$protocol->delete($protocolid);
			return(false);
		}

		return(($this->_return['protocol'] = $protocolid));
	}

	function edit_origin_protocol($protocolid=0)
	{
		if(dwho_issa('protocol',$this->_origin) === false
		|| dwho_issa('userfeatures',$this->_info) === false
		|| ($protocol = &$this->_service->get_protocol_module($this->_info['userfeatures']['protocol'])) === false)
			return(false);
		else if($this->_info['userfeatures']['protocol'] === $this->_protocolname)
			return($protocol->edit_origin($this->_origin['protocol']));
		else if(($protocolid = dwho_ulongint($protocolid)) === 0
		|| ($proto = &$this->_service->get_protocol_module($this->_protocolname)) === false
		|| $proto->delete($protocolid) === false)
			return(false);

		return($protocol->add_origin($this->_origin['protocol']));
	}

	function edit_userfeatures($protocolid)
	{
		if(dwho_issa('userfeatures',$this->_info) === false
		|| dwho_issa('protocol',$this->_info) === false
		|| ($rs = $this->get_result('userfeatures')) === null
		|| $rs === false)
			return(false);
		else if($this->_info['userfeatures']['protocol'] === $this->_protocolname)
		{
			$rs['protocolid'] = $this->_info['protocol']['id'];
			$provisioning = false;
		}
		else if(($protocolid = dwho_ulongint($protocolid)) !== 0)
		{
			$rs['protocolid'] = $protocolid;
			$provisioning = true;
		}
		else
			return(false);

		return(($this->_return['userfeatures'] = $this->_userfeatures->edit($this->_info['userfeatures']['id'],
										    $rs,
										    $provisioning)));
	}

	function edit_usermacro($userfeaturesid)
	{
		if(($rs = $this->_set_usermacro('edit',$userfeaturesid)) !== true)
			return($rs);

		return(parent::edit_macro('usermacro'));
	}

	function edit_hints()
	{
		$rs = false;

		if(dwho_issa('hints',$this->_info) === false
		|| ($rs = $this->get_result('hints')) === null
		|| $rs === false)
			return($rs);

		return(($this->_return['hints'] = $this->_extensions->edit($this->_info['hints']['id'],$rs)));
	}

	function edit_groupmember()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);
		else if(dwho_issa('groupmember',$this->_info) === false)
			return($this->add_groupmember($this->_info['userfeatures']['id']));

		$this->_return['groupmember'] = $this->delete_groupmember();

		if($this->get_result('groupmember') === null)
			return($this->_return['groupmember']);

		return($this->_edit_member_type('group'));
	}

	function edit_queuemember()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);
		else if(dwho_issa('queuemember',$this->_info) === false)
			return($this->add_queuemember($this->_info['userfeatures']['id']));

		$this->_return['queuemember'] = $this->delete_queuemember();

		if($this->get_result('queuemember') === null)
			return($this->_return['queuemember']);

		return($this->_edit_member_type('queue'));
	}

	function _edit_member_type($type)
	{
		if(($type !== 'group' && $type !== 'queue') === true
		|| dwho_issa('userfeatures',$this->_info) === false)
			return(false);

		$key = $type.'member';

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);

		$ret = $this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['userid'] = $this->_info['userfeatures']['id'];

			unset($rs[$i]['groupfeaturesid'],$rs[$i]['queuefeaturesid']);

			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return[$key][] = $ret;
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function edit_rightcall()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_rightcall($this->_info['userfeatures']['id']));
	}

	function edit_dialaction()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_dialaction($this->_info['userfeatures']['id']));
	}

	function edit_contextnummember()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextnummember($this->_info['userfeatures']['id']));
	}

	function edit_voicemail()
	{
		$rs = false;

		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);
		else if(($voicemailid = dwho_ulongint($this->get_result_var('userfeatures','voicemailid'))) === 0)
			return($this->_add_voicemail($this->_info['userfeatures']['id']));
		else if(is_object($this->_voicemail) === false
		|| $this->_voicemail->get($voicemailid) === false
		|| ($rs = $this->get_result('voicemail')) === null
		|| $rs === false)
			return($rs);

		return(($this->_return['voicemail'] = $this->_voicemail->edit(false)));
	}

	function edit_phonefunckey()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);
		else if(dwho_issa('phonefunckey',$this->_info) === false)
			return($this->add_phonefunckey($this->_info['userfeatures']['id']));

		$this->_return['phonefunckey'] = $this->delete_phonefunckey();

		if(($rs = $this->get_result('phonefunckey')) === null || $rs === false)
			return($rs);

		$context = $this->get_result_var('userfeatures','context');
		$bsfilter = $this->get_result_var('userfeatures','bsfilter');

		$id = $this->_return['phonefunckey'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];
			$ref['iduserfeatures'] = $this->_info['userfeatures']['id'];

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
		if(($rs = dwho_issa('userfeatures',$this->_info)) === false
		|| ($rs = $this->get_result('autoprov')) === null
		|| $rs === false)
			return($rs);

		$rs['iduserfeatures'] = $this->_info['userfeatures']['id'];

		if(dwho_issa('autoprov',$this->_info) !== false)
			$this->_return['autoprov'] = $this->_autoprov->notification($rs,$rs['modact']);
		else
			$this->_return['autoprov'] = $this->_autoprov->authoritative($rs,$rs['modact']);

		$this->_service->discuss('dialplan reload');

		return($this->_return['autoprov']);
	}

	function _reverse_edit($protocolid=0)
	{
		if($this->get_errnb() > 0
		|| $this->_protocolname === false
		|| $this->_status !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'protocol':
					$this->edit_origin_protocol($protocolid);
					break;
				case 'userfeatures':
					if(isset($this->_origin['userfeatures']) === false
					|| $this->_origin['userfeatures'] === false)
						$this->delete_userfeatures();
					else
						$this->_userfeatures->edit_origin($this->_origin['userfeatures']);
					break;
				case 'voicemail':
					$this->_voicemail->reverse_edit();
					break;
				case 'usermacro':
					if(isset($this->_origin['usermacro']) === false
					|| $this->_origin['usermacro'] === false)
						$this->delete_macro('usermacro');
					else
						$this->_extensions->edit_origin($this->_origin['usermacro']);
					break;
				case 'hints':
					if(isset($this->_origin['hints']) === false
					|| $this->_origin['hints'] === false)
						$this->delete_hints();
					else
						$this->_extensions->edit_origin($this->_origin['hints']);
					break;
				case 'groupmember':
				case 'queuemember':
					if($key === 'groupmember')
						$this->delete_groupmember();
					else
						$this->delete_queuemember();

					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$this->_queuemember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_edit($this->_status);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_edit($this->_status);
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_edit($this->_status);
					break;
				case 'phonefunckey':
					$this->delete_phonefunckey();

					if(isset($this->_origin['phonefunckey']) === true
					&& $this->_origin['phonefunckey'] !== false)
						$this->_phonefunckey->add_origin_list($this->_origin['phonefunckey']);
					break;
				case 'autoprov':
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
			}
		}
		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_protocol() === false
		|| $this->delete_userfeatures() === false
		|| $this->delete_macro('usermacro') === false
		|| $this->delete_hints() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_groupmember() === false
		|| $this->delete_queuemember() === false
		|| $this->delete_rightcall() === false
		|| $this->delete_dialaction() === false
		|| $this->delete_contextnummember() === false
		|| $this->delete_phonefunckey() === false
		|| $this->delete_phone() === false
		|| $this->delete_callfiltermember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}
		else
			$this->unlinks();

		$this->_status = '';

		return($r);
	}

	function delete_protocol()
	{
		if(dwho_issa('protocol',$this->_info) === false
		|| dwho_issa('userfeatures',$this->_info) === false
		|| ($protocol = &$this->_service->get_protocol_module($this->_info['userfeatures']['protocol'])) === false)
			return(false);

		$r = $protocol->delete($this->_info['protocol']['id']);

		if($this->_status === 'delete')
			$this->_return['protocol'] = $r;

		return($r);
	}

	function delete_userfeatures()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);

		$r = $this->_userfeatures->delete($this->_info['userfeatures']['id']);

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

		if(dwho_issa($key,$this->_info) === false)
			return(null);
		else if(dwho_issa('userfeatures',$this->_info) === false
		|| is_object($this->_queuemember) === false)
			return(false);

		$where = array();
		$where['usertype'] = 'user';
		$where['category'] = $type;
		$where['userid'] = $this->_info['userfeatures']['id'];

		$r = $this->_queuemember->delete_where($where);

		if($this->_status === 'delete')
			$this->_return[$key] = $r;

		return($r);
	}

	function delete_phonefunckey()
	{
		if(dwho_issa('phonefunckey',$this->_info) === false)
			return(null);
		else if(dwho_issa('userfeatures',$this->_info) === false
		|| is_object($this->_phonefunckey) === false)
			return(false);

		$where = array();
		$where['iduserfeatures'] = $this->_info['userfeatures']['id'];

		$r = $this->_phonefunckey->delete_where($where);

		if($this->_status === 'delete')
			$this->_return['phonefunckey'] = $r;

		return($r);
	}

	function delete_phone()
	{
		if(dwho_issa('autoprov',$this->_info) === false)
			return(null);
		else if(is_object($this->_autoprov) === false)
			return(false);

		$r = $this->_autoprov->userdeleted($this->_info['autoprov']['iduserfeatures']);

		if($this->_status === 'delete')
			$this->_return['autoprov'] = $r;

		if($r !== false)
			$this->_service->discuss('dialplan reload');

		return($r);
	}

	function delete_callfiltermember()
	{
		if(($callfiltermember = &$this->_service->get_module('callfiltermember')) === false)
			return(true);

		$where = array();
		$where['type'] = 'user';
		$where['typeval'] = $this->_info['userfeatures']['id'];

		$callfiltermember->delete_where($where);

		return(true);
	}

	function _reverse_delete()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'protocol':
					if(isset($this->_origin['protocol']) === false
					|| $this->_origin['protocol'] === false)
						continue;

					$protocol = &$this->_service->get_protocol_module($this->_info['userfeatures']['protocol']);
					$protocol->add_origin($this->_origin['protocol']);
					break;
				case 'userfeatures':
					if(isset($this->_origin['userfeatures']) === true
					&& $this->_origin['userfeatures'] !== false)
						$this->_userfeatures->add_origin($this->_origin['userfeatures']);
					break;
				case 'usermacro':
				case 'hints':
					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$this->_extensions->add_origin($this->_origin[$key]);
					break;
				case 'groupmember':
				case 'queuemember':
					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$this->_queuemember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'dialaction':
					$this->_dialaction->add_origin_list();
					break;
				case 'contextnummember':
					$this->_contextnummember->add_origin();
					break;
				case 'phonefunckey':
					if(isset($this->_origin['phonefunckey']) === true
					&& $this->_origin['phonefunckey'] !== false)
						$this->_phonefunckey->add_origin_list($this->_origin['phonefunckey']);
					break;
				case 'autoprov':
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);

		$this->_dialaction->unlinked('user',$this->_info['userfeatures']['id']);

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
		if(dwho_issa('protocol',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
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
		if(dwho_issa('userfeatures',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_userfeatures->enable($this->_info['userfeatures']['id']);
		else
			$r = $this->_userfeatures->disable($this->_info['userfeatures']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['userfeatures'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_status !== 'enable' && $this->_status !== 'disable') === true
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_status === 'enable';

		$this->_status = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
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

	function _prepare_import($arr)
	{
		if(($issa_protocol = dwho_issa('protocol',$arr)) === false
		|| isset($arr['protocol']['protocol']) === false)
			return(false);

		$issa_userfeatures = dwho_issa('userfeatures',$arr);

		if(dwho_issa('voicemail',$arr) === true
		&& empty($arr['voicemail']) === false
		&& dwho_has_len($arr['voicemail'],'mailbox') === true)
		{
			if($issa_userfeatures === true)
				$arr['userfeatures']['enablevoicemail'] = true;

			if($issa_protocol === true && isset($arr['protocol']['subscribemwi']) === false)
				$arr['protocol']['subscribemwi'] = true;
		}
		else
			unset($arr['voicemail']);

		if($issa_protocol === true
		&& $issa_userfeatures === true
		&& dwho_has_len($arr['protocol'],'callerid') === false)
		{
			$arr['protocol']['callerid'] = '';

			if(dwho_has_len($arr['userfeatures'],'firstname') === true)
				$arr['protocol']['callerid'] = $arr['userfeatures']['firstname'].' ';

			if(dwho_has_len($arr['userfeatures'],'lastname') === true)
				$arr['protocol']['callerid'] .= $arr['userfeatures']['lastname'];

			trim($arr['protocol']['callerid']);
		}

		if($issa_userfeatures === true
		&& dwho_has_len($arr['userfeatures'],'agentnumber') === true
		&& is_object($this->_agentfeatures) === true
		&& ($agentid = $this->_agentfeatures->get_primary(
						array('number' => $arr['userfeatures']['agentnumber']))) !== false)
			$arr['userfeatures']['agentid'] = $agentid;

		unset($arr['userfeatures']['agentnumber']);

		return($arr);
	}

	function import_from_array($data,$save=true,$chkcontext=true)
	{
		$save = (bool) $save;

		if(is_array($data) === false
		|| ($nb = count($data)) === 0)
			return(false);

		$appincall = &$this->_service->get_application('incall');

		$incall = array();
		$incall['dialaction'] = array();

		$r = array();
		$r['lines'] = array();
		$r['total'] = array();
		$r['total']['success'] = 0;
		$r['total']['error'] = 0;
		$r['total']['count'] = $nb;

		for($i = 0,$row = 1;$i < $nb;$i++,$row++)
		{
			if(($arr = $this->_prepare_import($data[$i])) === false
			|| $this->set_add($arr,$arr['protocol']['protocol'],$chkcontext) === false)
			{
				$r['lines'][$row] = 'error';
				$r['total']['error']++;
				continue;
			}
			else if(dwho_issa('incall',$arr) === false
			|| dwho_has_len($arr['incall'],'exten') === false)
			{
				if($save === true && $this->add() === false)
				{
					$r['lines'][$row] = 'error';
					$r['total']['error']++;
				}
				else
				{
					$r['lines'][$row] = 'success';
					$r['total']['success']++;
				}
				continue;
			}

			$incall['incall'] = $arr['incall'];

			$incall['dialaction']['answer'] = array();
			$incall['dialaction']['answer']['actiontype'] = 'user';
			$incall['dialaction']['answer']['actionarg1'] = 1;

			if(dwho_has_len($arr['incall'],'ringseconds') === true)
				$incall['dialaction']['answer']['actionarg2'] = $arr['incall']['ringseconds'];
			else
				$incall['dialaction']['answer']['actionarg2'] = '';

			unset($incall['incall']['ringseconds']);

			if($appincall->set_add($incall) === false)
			{
				$r['lines'][$row] = 'error';
				$r['total']['error']++;
				continue;
			}
			else if($save === false)
			{
				$r['lines'][$row] = 'success';
				$r['total']['success']++;
				continue;
			}
			else if($this->add() === false)
			{
				$r['lines'][$row] = 'error';
				$r['total']['error']++;
				continue;
			}

			$incall['dialaction']['answer']['actionarg1'] = $this->get_return('userfeatures');	

			if($appincall->set_add($incall) === false
			|| $appincall->add() === false)
			{
				$r['lines'][$row] = 'error';
				$r['total']['error']++;
			}
			else
			{
				$r['lines'][$row] = 'success';
				$r['total']['success']++;
			}
		}


		return($r);
	}

	function import_csv($save=true)
	{
		$save = (bool) $save;

		if(dwho::load_class('dwho::file::csv') === false
		|| ($http_response = dwho_http::factory('response')) === false
		|| ($fileinfo = $http_response->upload_file('import',
							    $this->_conf['import']['file'])) === false)
			return(false);

		$filecsv = new dwho_file_csv();

		if(($data = $filecsv->parse($fileinfo['tmp_name'],
					    0,
					    '|',
					    null,
					    true,
					    $this->_conf['import']['field'])) === false
		|| empty($data) === true)
		{
			dwho_file::rm($fileinfo['tmp_name']);
			return(false);
		}

		$r = $this->import_from_array($data,$save);

		if($save === true)
			dwho_file::rm($fileinfo['tmp_name']);

		return($r);
	}

	function add_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| ($arr = $this->_prepare_import($data)) === false
		|| $this->set_add($arr,$arr['protocol']['protocol']) === false
		|| $this->add() === false)
			return(false);

		return(true);
	}

	function edit_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| ($arr = $this->_prepare_import($data)) === false
		|| $this->set_edit($arr,$arr['protocol']['protocol']) === false
		|| $this->edit() === false)
			return(false);

		return(true);
	}
*/
}

?>
