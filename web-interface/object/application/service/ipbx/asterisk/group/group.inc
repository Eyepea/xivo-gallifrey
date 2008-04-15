<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_group extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_groupfeatures	= false;
	var $_queue		= false;
	var $_extensions	= false;
	var $_extenumbers	= false;
	var $_rightcall		= false;
	var $_dialstatus	= false;
	var $_queuemember	= null;
	var $_musiconhold	= array();

	function xivo_application_service_asterisk_group(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_groupfeatures = &$this->_service->get_module('groupfeatures')) === false
		|| ($this->_queue = &$this->_service->get_module('queue')) === false)
			return(false);

		if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'group'))) === false
		|| ($this->_rightcall = $this->get_abstract('rightcall',array('type' => 'group'))) === false
		|| ($this->_dialstatus = $this->get_abstract('dialstatus',array('category' => 'group'))) === false)
			return(false);

		$this->_queuemember = &$this->_service->get_module('queuemember');

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['gfeatures'] = $this->_groupfeatures->get($id)) === false
		|| ($this->_info['queue'] = $this->_queue->get($this->_info['gfeatures']['name'],$disable)) === false)
			return(false);

		$this->_origin['gfeatures'] = $this->_groupfeatures->get_origin();
		$this->_origin['queue'] = $this->_queue->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_exten();
		$this->_get_extenumbers($this->_info['gfeatures']['id']);
		$this->_get_user();
		$this->_get_rightcall($this->_info['gfeatures']['id']);
		$this->_get_dialstatus($this->_info['gfeatures']['id']);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$arr['category'] = 'group';

		return($this->_queue->get_nb($arr,$disable));
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		$this->_info['exten'] = $this->_extensions->get_exten('macro',
								    $this->_info['gfeatures']['number'],
								    $this->_info['gfeatures']['context']);

		$this->_origin['exten'] = $this->_extensions->get_origin();

		return($this->_info['exten']);
	}

	function _get_user()
	{
		$this->_info['user'] = false;
		$this->_origin['user'] = false;

		$where = array();
		$where['usertype'] = 'user';
		$where['category'] = 'group';
		$where['queue_name'] = $this->_info['gfeatures']['name'];

		if($this->_queuemember === false
		|| ($this->_info['user'] = $this->_queuemember->get_all_where($where)) === false
		|| isset($this->_info['user'][0]) === false)
			return(false);

		$this->_origin['user'] = $this->_queuemember->get_origin_list();

		return($this->_info['user']);
	}

	function get_elements()
	{
		$r = array();

		$r['gfeatures'] = $this->_groupfeatures->get_element();
		$r['queue'] = $this->_queue->get_element();
		$r['dialstatus'] = $this->_dialstatus->get_element();

		return($r);
	}

	function get_groups_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_groupfeatures->get_all(false,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_groupfeatures->get_cnt();
			return(false);
		}

		$this->_cnt = $this->_groupfeatures->get_cnt();

		$this->_queuemember = &$this->_service->get_module('queuemember');

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['gfeatures'] = $rs[$i];

			if(($info['queue'] = $this->_queue->get($info['gfeatures']['name'],$disable)) === false
			|| ($info['nb_qmember'] = $this->_queuemember->get_nb(
							array('queue_name' => $info['queue']['name']),
							$disable)) === false)
				continue;

			$r[$info['gfeatures']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_dialstatus_result()
	{
		return($this->_dialstatus->get_result_for_display());
	}

	function get_dialstatus_destination_list()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['gfeatures']['id'];

		$r = $this->get_destination_list('group',$id);
		$r['sounds'] = $this->_dialstatus->get_sound();

		return($r);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('gfeatures',$arr) === false
		|| xivo_issa('queue',$arr) === false)
			return(false);

		$this->set_groupfeatures($arr['gfeatures']);

		$name = $this->get_result_var('gfeatures','name');
		$number = (string) $this->get_result_var('gfeatures','number');
		$context = (string) $this->get_result_var('gfeatures','context');

		$arr['queue']['name'] = $name;

		$this->set_queue($arr['queue']);

		if($this->_errnb === 0 && $number !== '')
		{
			$appdata = array('appdata' => XIVO_SRE_IPBX_AST_GROUP_MACRO);
			if(($rsexten = $this->set_macro($action,'exten',$number,$appdata,$context)) !== false)
				$this->_set_result('exten',$rsexten);

			$this->set_extenumbers($action,$number,$context);
		}

		if(xivo_issa('user',$arr) === true)
			$this->set_user($action,$arr['user'],$name);

		if(xivo_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		if(xivo_issa('dialstatus',$arr) === true)
			$this->set_dialstatus($arr['dialstatus']);

		return(($this->get_errnb() < 1));
	}

	function set_groupfeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('gfeatures',null);
			$this->_set_error('gfeatures','invalid data');
			return(false);
		}
		else if(($rs = $this->_groupfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('gfeatures',$this->_groupfeatures->get_filter_result());
			$this->_set_error('gfeatures',$this->_groupfeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('gfeatures',$rs);

		return(true);
	}

	function set_queue($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid data');
			return(false);
		}

		$this->get_musiconhold();

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		$arr['category'] = 'group';

		if(($rs = $this->_queue->chk_values($arr)) === false)
		{
			$this->_set_result('queue',$this->_queue->get_filter_result());
			$this->_set_error('queue',$this->_queue->get_filter_error());
			return(false);
		}

		$this->_set_result('queue',$rs);

		return(true);
	}

	function set_user($action,$list,$name)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_array($list) === false
		|| is_scalar($name) === false
		|| ($ufeatures = &$this->_service->get_module('userfeatures')) === false
		|| $this->_queuemember === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$where = array();
		$where['usertype'] = 'user';
		$where['userid'] = 0;
		$where['category'] = 'group';

		if($action === 'edit')
		{
			if(xivo_issa('gfeatures',$this->_info) === false)
				return(false);

			$where['queue_name'] = $this->_info['gfeatures']['name'];
		}

		$tmp = $reslist = $callgroup = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($uinfo = $ufeatures->get($list[$i])) === false)
			{
				$this->_set_error('user','wrong user');
				continue;
			}

			$where['userid'] = $uinfo['id'];

			$info = $where;
			$info['penalty'] = $info['call-limit'] = 0;

			if($action === 'edit'
			&& $this->_queuemember->get_where($where) !== false
			&& ($orig = $this->_queuemember->get_origin()) !== false)
				$info = array_merge($info,$orig);
			else
			{
				$interface = $this->mk_interface($uinfo['name'],
								 null,
								 $uinfo['context'],
								 $uinfo['number'],
								 $uinfo['protocol']);

				if($interface === false)
					continue;

				$info['interface'] = $interface['interface'];
				$info['channel'] = $interface['channel'];
			}

			$info['queue_name'] = $name;

			if(($rs = $this->_queuemember->chk_values($info)) === false
			|| isset($tmp[$uinfo['id']]) === true)
				continue;

			if($uinfo['protocol'] === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
				$callgroup[$uinfo['id']] = false;
			else
			{
				$callgroup[$uinfo['id']] = array('protocol' => $uinfo['protocol'],
								 'protocolid' => $uinfo['protocolid']);
			}

			$tmp[$uinfo['id']] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('user',false);
			return(false);
		}

		if(empty($callgroup) === false)
			$this->_set_result('user_callgroup',$callgroup);
		else
			$this->_set_result('user_callgroup',null);

		$this->_set_result('user',$reslist);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$gfeaturesid = null;

		if($this->get_errnb() > 0
		|| ($gfeaturesid = $this->add_groupfeatures()) === false
		|| $this->add_queue() === false
		|| $this->add_macro('exten') === false
		|| $this->add_extenumbers($gfeaturesid) === false
		|| $this->add_user() === false
		|| $this->add_rightcall($gfeaturesid) === false
		|| $this->add_dialstatus($gfeaturesid) === false)
		{
			if($gfeaturesid !== null)
				$this->_reverse_add();

			$this->_status = '';
			return(false);
		}

		$this->add_user_callgroup($gfeaturesid);

		$this->_status = '';

		return(true);
	}

	function add_groupfeatures()
	{
		if(($rs = $this->get_result('gfeatures')) === null
		|| $rs === false)
			return(false);

		$this->_return['groupfeatures'] = $this->_groupfeatures->add($rs);

		return($this->_return['groupfeatures']);
	}

	function add_queue()
	{
		if(($rs = $this->get_result('queue')) === null
		|| $rs === false
		|| $this->_queue->add($rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function add_user()
	{
		if(($rs = $this->get_result('user')) === null || $rs === false)
			return($rs);

		$id = $this->_return['user'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return['user'][] = $id;
		}

		if(isset($this->_return['user'][0]) === false)
			$this->_return['user'] = false;

		return(true);
	}

	function add_user_callgroup($gfeaturesid)
	{
		$rs = false;

		if(($gfeaturesid = xivo_ulongint($gfeaturesid)) === 0
		|| ($rs = $this->get_result('user_callgroup')) === null)
			return($rs);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$callgroup = array('callgroup' => '');

		foreach($rs as $val)
		{
			if($val === false
			|| ($protocol = &$this->_service->get_protocol_module($val['protocol'])) === false
			|| ($info = $protocol->get($val['protocolid'])) === false)
				continue;
			else if(is_array($info['callgroup']) === false)
				$callgroup['callgroup'] = $gfeaturesid;
			else if(in_array($gfeaturesid,$info['callgroup']) === true)
				continue;
			else
			{
				$callgroup['callgroup'] = $info['callgroup'];
				$callgroup['callgroup'][] = $gfeaturesid;
				$callgroup['callgroup'] = implode(',',$callgroup['callgroup']);
			}

			$protocol->edit($info['id'],$callgroup);
		}

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
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
				case 'groupfeatures':
					$this->_groupfeatures->delete($val);
					break;
				case 'queue':
					$this->_queue->delete($val);
					break;
				case 'exten':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add($this->_status);
					break;
				case 'user':
					if(is_array($val) === false)
						continue;

					foreach($val as $queuememberval)
					{
						if($queuememberval !== false)
							$this->_queuemember->delete($queuememberval);
					}
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
					break;
				case 'dialstatus':
					$this->_dialstatus->reverse_add();
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
		|| $this->edit_groupfeatures() === false)
		{
			$this->_status = '';
			return(false);
		}
		else if($this->edit_queue() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$unlinks = false;

		if($this->_info['gfeatures']['number'] !== '')
		{
			if($this->get_result('exten') === null
			|| $this->get_result('extenumbers') === null)
			{
				$unlinks = true;
				$this->delete_macro('exten');
				$this->delete_extenumbers();
			}
			else if($this->edit_macro('exten') === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit();
				$this->_status = '';
				return(false);
			}
		}
		else if($this->add_macro('exten') === false
		|| $this->add_extenumbers($this->_info['gfeatures']['id']) === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($this->edit_user($this->_info['gfeatures']['name']) === false
		|| $this->edit_rightcall() === false
		|| $this->edit_dialstatus() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($unlinks === true)
			$this->unlinks();

		$this->edit_user_callgroup($this->_info['gfeatures']['id']);

		$this->_status = '';

		return(true);
	}

	function edit_groupfeatures()
	{
		if(xivo_issa('gfeatures',$this->_info) === false
		|| ($rs = $this->get_result('gfeatures')) === null
		|| $rs === false)
			return(false);

		$this->_return['groupfeatures'] = $this->_groupfeatures->edit($this->_info['gfeatures']['id'],$rs);

		return($this->_return['groupfeatures']);
	}

	function edit_queue()
	{
		if(xivo_issa('queue',$this->_info) === false
		|| ($rs = $this->get_result('queue')) === null
		|| $rs === false
		|| $this->_queue->edit($this->_info['queue']['name'],$rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function edit_user($name)
	{
		if(is_scalar($name) === false)
			return(false);
		else if(xivo_issa('user',$this->_info) === false)
			return($this->add_user());

		$this->_return['user'] = $this->delete_user($name);

		if(($rs = $this->get_result('user')) === null)
			return($this->_return['user']);
		else if($rs === false)
			return(false);

		$id = $this->_return['user'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return['user'][] = $id;
		}

		if(isset($this->_return['user'][0]) === false)
			$this->_return['user'] = false;

		return(true);
	}

	function edit_rightcall()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_rightcall($this->_info['gfeatures']['id']));
	}

	function edit_dialstatus()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			return(false);

		return(parent::edit_dialstatus($this->_info['gfeatures']['id']));
	}

	function edit_user_callgroup($gfeaturesid)
	{
		if(($gfeaturesid = xivo_ulongint($gfeaturesid)) === 0
		|| isset($this->_origin['user']) === false)
			return(false);
		else if(is_array($this->_origin['user']) === false
		|| ($nb = count($this->_origin['user'])) === 0)
			return($this->add_user_callgroup($gfeaturesid));

		$ufeatures = &$this->_service->get_module('userfeatures');

		$rs = $this->get_result('user_callgroup');

		$callgroup = array('callgroup' => '');

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$this->_origin['user'][$i];

			if(is_array($rs) === false || isset($rs[$ref['userid']]) === false)
			{
				if(($uinfo = $ufeatures->get($ref['userid'])) === false
				|| $uinfo['protocol'] === XIVO_SRE_IPBX_AST_PROTO_CUSTOM
				|| ($protocol = &$this->_service->get_protocol_module($uinfo['protocol'])) === false
				|| ($pinfo = $protocol->get($uinfo['protocolid'])) === false
				|| is_array($pinfo['callgroup']) === false
				|| ($key = array_search($gfeaturesid,$pinfo['callgroup'])) === false)
					continue;

				$callgroup['callgroup'] = $pinfo['callgroup'];
				unset($callgroup['callgroup'][$key]);
				$callgroup['callgroup'] = implode(',',$callgroup['callgroup']);

				$protocol->edit($pinfo['id'],$callgroup);
			}
			else if($rs[$ref['userid']] !== false)
			{
				$res = $rs[$ref['userid']];
				unset($rs[$ref['userid']]);

				if(($protocol = &$this->_service->get_protocol_module($res['protocol'])) === false
				|| ($pinfo = $protocol->get($res['protocolid'])) === false)
					continue;
				else if(is_array($pinfo['callgroup']) === false)
					$callgroup['callgroup'] = $gfeaturesid;
				else if(in_array($gfeaturesid,$pinfo['callgroup']) === true)
					continue;
				else
				{
					$callgroup['callgroup'] = $pinfo['callgroup'];
					$callgroup['callgroup'][] = $gfeaturesid;
					$callgroup['callgroup'] = implode(',',$callgroup['callgroup']);
				}

				$protocol->edit($pinfo['id'],$callgroup);
			}
		}

		if(empty($rs) === false)
		{
			$this->_set_result('user_callgroup',$rs);
			$this->add_user_callgroup($gfeaturesid);
		}

		return(true);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
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
				case 'groupfeatures':
					if(isset($this->_origin['gfeatures']) === false
					|| $this->_origin['gfeatures'] === false)
						$this->delete_groupfeatures();
					else
						$this->_groupfeatures->edit_origin($this->_origin['gfeatures']);
					break;
				case 'queue':
					if(isset($this->_origin['queue']) === false
					|| $this->_origin['queue'] === false)
						$this->_queue->delete($val);
					else
						$this->_queue->edit_origin($this->_origin['queue']);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === false
					|| $this->_origin['exten'] === false)
						$this->delete_macro('exten');
					else
						$this->_extensions->edit_origin($this->_origin['exten']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'user':
					if(xivo_issa('gfeatures',$this->_info) === false)
						continue;

					$this->delete_user($this->_info['gfeatures']['name']);

					if(isset($this->_origin['user']) === true
					&& $this->_origin['user'] !== false)
						$this->_queuemember->add_origin_list($this->_origin['user']);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_edit($this->_status);
					break;
				case 'dialstatus':
					$this->_dialstatus->reverse_edit($this->_status);
					break;
			}
		}

		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_groupfeatures() === false
		|| $this->delete_queue() === false
		|| $this->delete_macro('exten') === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_user($this->_info['gfeatures']['name']) === false
		|| $this->delete_rightcall() === false
		|| $this->delete_dialstatus() === false)
		{
			$this->_reverse_delete();
			$this->_status = '';
			return(false);
		}

		if(isset($this->_return['exten']) === true
		|| isset($this->_return['extenumbers']) === true)
			$this->unlinks();

		$this->delete_user_callgroup($this->_info['gfeatures']['id']);

		$this->_status = '';

		return(true);
	}

	function delete_groupfeatures()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			return(false);

		$r = $this->_groupfeatures->delete($this->_info['gfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['groupfeatures'] = $r;

		return($r);
	}

	function delete_queue()
	{
		if(xivo_issa('queue',$this->_info) === false)
			return(false);

		$r = $this->_queue->delete($this->_info['queue']['name']);

		if($this->_status === 'delete')
			$this->_return['queue'] = $r;

		return($r);
	}

	function delete_user($name)
	{
		if(xivo_issa('user',$this->_info) === false)
			return(null);
		else if(xivo_issa('gfeatures',$this->_info) === false
		|| is_scalar($name) === false
		|| $this->_queuemember === false)
			return(false);

		$queuemember_where = array();
		$queuemember_where['usertype'] = 'user';
		$queuemember_where['category'] = 'group';
		$queuemember_where['queue_name'] = $name;

		$r = $this->_queuemember->delete_where($queuemember_where);

		if($this->_status === 'delete')
			$this->_return['user'] = $r;

		return($r);
	}

	function delete_user_callgroup($gfeaturesid)
	{
		$this->_set_result('user_callgroup',null);

		return($this->edit_user_callgroup($gfeaturesid));
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
				case 'groupfeatures':
					if(xivo_issa('gfeatures',$this->_info) === true)
						$this->_groupfeatures->recover($this->_info['gfeatures']['id']);
					break;
				case 'queue':
					if(isset($this->_origin['queue']) === true
					&& $this->_origin['queue'] !== false)
						$this->_queue->add_origin($this->_origin['queue']);
					break;
				case 'exten':
					if(isset($this->_origin['exten']) === true
					&& $this->_origin['exten'] !== false)
						$this->_extensions->add_origin($this->_origin['exten']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'user':
					if(isset($this->_origin['user']) === true
					&& $this->_origin['user'] !== false)
						$this->_queuemember->add_origin_list($this->_origin['user']);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'dialstatus':
					$this->_dialstatus->add_origin_list();
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('gfeatures',$this->_info) === false)
			return(false);
		else if(($incall = &$this->_service->get_module('incall')) !== false)
		{
			$incall_where = array();
			$incall_where['type'] = 'group';
			$incall_where['typeval'] = $this->_info['gfeatures']['id'];

			$incall->unlinked_where($incall_where);
		}

		$this->_dialstatus->unlinked('group',$this->_info['gfeatures']['id']);

		if(($schedule = &$this->_service->get_module('schedule')) === false)
			return(true);
		
		$schedule_where = array();
		$schedule_where['typetrue'] = 'group';
		$schedule_where['typevaltrue'] = $this->_info['gfeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		$schedule_where = array();
		$schedule_where['typefalse'] = 'group';
		$schedule_where['typevalfalse'] = $this->_info['gfeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		return(true);
	}
}

?>
