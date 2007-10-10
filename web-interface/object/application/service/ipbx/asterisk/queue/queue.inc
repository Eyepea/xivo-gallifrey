<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_queue extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_extenumbers	= null;
	var $_musiconhold	= false;
	var $_announce		= false;

	function xivo_application_service_asterisk_queue(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_extenumbers = &$this->get_abstract('extenumbers')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($qfeatures = &$this->_service->get_module('queuefeatures')) === false
		|| ($this->_info['qfeatures'] = $qfeatures->get($id)) === false
		|| ($queue = &$this->_service->get_module('queue')) === false
		|| ($this->_info['queue'] = $queue->get($this->_info['qfeatures']['name'])) === false)
			return(false);

		$this->_origin['qfeatures'] = $qfeatures->get_origin();
		$this->_origin['queue'] = $queue->get_origin();

		$this->_get_localexten();
		$this->_get_extenumbers();
		$this->_get_user();
		$this->_get_agentgroup();
		$this->_get_agent();
		$this->get_musiconhold();
		$this->get_announce();

		return($this->_info);
	}

	function _get_localexten()
	{
		$this->_info['localexten'] = false;
		$this->_origin['localexten'] = false;

		if(($extensions = &$this->_service->get_module('extensions')) === false)
			return(false);

		$this->_info['localexten'] = $extensions->get_exten('macro',
								    $this->_info['qfeatures']['number'],
								    $this->_info['qfeatures']['context']);

		$this->_origin['localexten'] = $extensions->get_origin();

		return($this->_info['localexten']);
	}

	function _get_extenumbers()
	{
		$number = $this->_info['qfeatures']['number'];
		$context = $this->_info['qfeatures']['context'];

		return(parent::_get_extenumbers($number,$context));
	}

	function _get_user()
	{
		return($this->_get_user_type('user'));
	}

	function _get_agentgroup()
	{
		return($this->_get_user_type('agentgroup'));
	}

	function _get_agent()
	{
		return($this->_get_user_type('agent'));
	}

	function _get_user_type($type)
	{
		$this->_info[$type] = false;
		$this->_origin[$type] = false;

		if($type !== 'user' && $type !== 'agent' && $type !== 'agentgroup')
			return(false);

		$where = array();

		if($type === 'agentgroup')
		{
			$where['usertype'] = 'agent';
			$where['category'] = 'group';
		}
		else
		{
			$where['usertype'] = $type;
			$where['category'] = 'queue';
		}

		$where['queue_name'] = $this->_info['qfeatures']['name'];

		if(($queuemember = &$this->_service->get_module('queuemember')) === false
		|| ($this->_info[$type] = $queuemember->get_all_where($where)) === false
		|| isset($this->_info[$type][0]) === false)
			return(false);

		$this->_origin[$type] = $queuemember->get_origin_list();

		return($this->_info[$type]);
	}

	function get_element()
	{
		$r = array();

		if(($qfeatures = &$this->_service->get_module('queuefeatures')) !== false)
			$r['qfeatures'] = $qfeatures->get_element();
		else
			$r['qfeatures'] = false;

		if(($queue = &$this->_service->get_module('queue')) !== false)
			$r['queue'] = $queue->get_element();
		else
			$r['queue'] = false;

		return($r);
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

	function get_announce()
	{
		if($this->_announce !== false)
			return($this->_announce);

		if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);

		$this->_announce = $sounds->get_list('acd',true);

		return($this->_announce);
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
		|| xivo_issa('qfeatures',$arr) === false
		|| xivo_issa('queue',$arr) === false)
			return(false);

		$this->set_queuefeatures($arr['qfeatures']);

		$name = $this->get_result_var('qfeatures','name');
		$number = (string) $this->get_result_var('qfeatures','number');
		$context = (string) $this->get_result_var('qfeatures','context');

		$arr['queue']['name'] = $name;

		$this->set_queue($arr['queue']);

		if($this->_errnb === 0 && $number !== '')
		{
			$appdata = array('appdata' => 'superqueue');

			if(($localexten = $this->set_macro($action,'localexten',$number,$appdata,$context)) === false)
				$this->_set_error('localexten');

			$this->_set_result('localexten',$localexten);

			$this->set_extenumbers($action,$number,$context);
		}

		if(xivo_issa('user',$arr) === true)
			$this->set_user($action,$arr['user'],$name);

		if(xivo_issa('agentgroup',$arr) === true)
			$this->set_agentgroup($action,$arr['agentgroup'],$name);

		if(xivo_issa('agent',$arr) === true)
			$this->set_agent($action,$arr['agent'],$name);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_queuefeatures($arr)
	{
		if(is_array($arr) === false
		|| ($qfeatures = &$this->_service->get_module('queuefeatures')) === false)
		{
			$this->_set_result('qfeatures',null);
			$this->_set_error('qfeatures','invalid module');
			return(false);
		}

		if(($rs = $qfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('qfeatures',$qfeatures->get_filter_result());
			$this->_set_error('qfeatures',$qfeatures->get_filter_error());

			return(false);
		}

		$this->_set_result('qfeatures',$rs);

		return(true);
	}

	function set_queue($arr)
	{
		if(is_array($arr) === false
		|| ($queue = &$this->_service->get_module('queue')) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid module');
			return(false);
		}

		if(($arr = $this->_set_announce($arr)) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid announce');
			return(false);
		}

		$arr['category'] = 'queue';

		if(($rs = $queue->chk_values($arr)) === false)
		{
			$this->_set_result('queue',$queue->get_filter_result());
			$this->_set_error('queue',$queue->get_filter_error());

			return(false);
		}

		$this->_set_result('queue',$rs);

		return(true);
	}

	function _set_announce($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$this->get_announce();

		if(isset($arr['announce'],$this->_announce[$arr['announce']]) === false)
			$arr['announce'] = '';

		if(isset($arr['queue-youarenext'],$this->_announce[$arr['queue-youarenext']]) === false)
			$arr['queue-youarenext'] = '';

		if(isset($arr['queue-thereare'],$this->_announce[$arr['queue-thereare']]) === false)
			$arr['queue-thereare'] = '';

		if(isset($arr['queue-callswaiting'],$this->_announce[$arr['queue-callswaiting']]) === false)
			$arr['queue-callswaiting'] = '';

		if(isset($arr['queue-holdtime'],$this->_announce[$arr['queue-holdtime']]) === false)
			$arr['queue-holdtime'] = '';

		if(isset($arr['queue-minutes'],$this->_announce[$arr['queue-minutes']]) === false)
			$arr['queue-minutes'] = '';

		if(isset($arr['queue-seconds'],$this->_announce[$arr['queue-seconds']]) === false)
			$arr['queue-seconds'] = '';

		if(isset($arr['queue-thankyou'],$this->_announce[$arr['queue-thankyou']]) === false)
			$arr['queue-thankyou'] = '';

		if(isset($arr['queue-thankyou'],$this->_announce[$arr['queue-thankyou']]) === false)
			$arr['queue-thankyou'] = '';

		if(isset($arr['queue-lessthan'],$this->_announce[$arr['queue-lessthan']]) === false)
			$arr['queue-lessthan'] = '';

		if(isset($arr['queue-reporthold'],$this->_announce[$arr['queue-reporthold']]) === false)
			$arr['queue-reporthold'] = '';

		if(isset($arr['periodic-announce'],$this->_announce[$arr['periodic-announce']]) === false)
			$arr['periodic-announce'] = '';

		return($arr);
	}

	function set_user($action,$list,$name)
	{
		$r = $this->_set_user_type('user',$action,$list,$name);

		return($r);
	}

	function set_agentgroup($action,$list,$name)
	{
		$r = $this->_set_user_type('agentgroup',$action,$list,$name);

		return($r);
	}

	function set_agent($action,$list,$name)
	{
		$r = $this->_set_user_type('agent',$action,$list,$name);

		return($r);
	}

	function _set_user_type($type,$action,$list,$name)
	{
		if(($type !== 'user' && $type !== 'agent' && $type !== 'agentgroup') === true
		|| ($action !== 'add' && $action !== 'edit') === true
		|| is_array($list) === false
		|| is_scalar($name) === false
		|| ($queuemember = &$this->_service->get_module('queuemember')) === false)
			return(false);

		$info = array();
		$info['userid'] = 0;

		if($type === 'agentgroup')
		{
			$modulename = 'agentgroup';
			$info['usertype'] = 'agent';
			$info['category'] = 'group';
		}
		else
		{
			$modulename = $type.'features';
			$info['usertype'] = $type;
			$info['category'] = 'queue';
		}

		if(($module = &$this->_service->get_module($modulename)) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

		$where = $info;

		$info['queue_name'] = $name;

		if($action === 'edit')
		{
			if(xivo_issa('qfeatures',$this->_info) === false)
				return(false);

			$where['queue_name'] = $this->_info['qfeatures']['name'];
		}

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($uinfo = $module->get($list[$i])) === false)
			{
				$this->_set_error($type,'wrong '.$type);
				continue;
			}

			$info['penalty'] = $info['call-limit'] = 0;
			$info['userid'] = $where['userid'] = $uinfo['id'];

			if($action === 'edit'
			&& $queuemember->get_where($where) !== false
			&& ($orig = $queuemember->get_origin()) !== false)
				$info = array_merge($info,$orig);
			else
			{
				if($type === 'agent')
					$interface = $this->mk_agent_interface($uinfo['number']);
				else if($type === 'agentgroup')
					$interface = $this->mk_agent_interface($uinfo['groupid'],true);
				else
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

			if(($rs = $queuemember->chk_values($info)) === false
			|| isset($tmp[$uinfo['id']]) === true)
				continue;

			$tmp[$uinfo['id']] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result($type,false);
			return(false);
		}

		$this->_set_result($type,$reslist);

		return(true);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$qfeaturesid = null;

		if($this->get_errnb() > 0
		|| ($qfeaturesid = $this->add_queuefeatures()) === false
		|| $this->add_queue() === false
		|| $this->add_macro('localexten') === false
		|| $this->add_extenumbers() === false
		|| $this->add_user() === false
		|| $this->add_agentgroup() === false
		|| $this->add_agent() === false)
		{
			if($qfeaturesid !== null)
				$this->_reverse_add();

			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function add_queuefeatures()
	{
		if(($rs = $this->get_result('qfeatures')) === null
		|| $rs === false)
			return($rs);

		$qfeatures = &$this->_service->get_module('queuefeatures');

		$this->_return['queuefeatures'] = $qfeatures->add($rs);

		return($this->_return['queuefeatures']);
	}

	function add_queue()
	{
		if(($rs = $this->get_result('queue')) === null
		|| $rs === false)
			return($rs);

		$queue = &$this->_service->get_module('queue');

		if($queue->add($rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function add_user()
	{
		return($this->_add_user_type('user'));
	}

	function add_agentgroup()
	{
		return($this->_add_user_type('agentgroup'));
	}

	function add_agent()
	{
		return($this->_add_user_type('agent'));
	}

	function _add_user_type($type)
	{
		$rs = false;

		if(($type !== 'user' && $type !== 'agent' && $type !== 'agentgroup') === true
		|| ($rs = $this->get_result($type)) === null
		|| $rs === false)
			return($rs);

		$queuemember = &$this->_service->get_module('queuemember');

		$id = $this->_return[$type] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return[$type][] = $id;
		}

		if(isset($this->_return[$type][0]) === false)
			$this->_return[$type] = false;

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
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
				case 'localexten':
					$extensions = &$this->_service->get_module('extensions');
					$extensions->delete_exten($val);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					$queuemember = &$this->_service->get_module('queuemember');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$queuemember->delete($val[$j]);
					}
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
		|| $this->edit_queuefeatures() === false)
		{
			$this->_status = '';
			return(false);
		}

		if($this->edit_queue() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$unlinks = false;

		if($this->_info['qfeatures']['number'] !== '')
		{
			if($this->get_result('localexten') === null
			|| $this->get_result('extenumbers') === null)
			{
				$unlinks = true;
				$this->delete_macro('localexten');
				$this->delete_extenumbers();
			}
			else if($this->edit_macro('localexten') === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit();
				$this->_status = '';
				return(false);
			}
		}
		else if($this->add_macro('localexten') === false
		|| $this->add_extenumbers() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($this->edit_user($this->_info['qfeatures']['name']) === false
		|| $this->edit_agentgroup($this->_info['qfeatures']['name']) === false
		|| $this->edit_agent($this->_info['qfeatures']['name']) === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($unlinks === true)
			$this->unlinks();

		$this->_status = '';

		return(true);
	}

	function edit_queuefeatures()
	{
		$rs = false;

		if(xivo_issa('qfeatures',$this->_info) === false
		|| ($rs = $this->get_result('qfeatures')) === null
		|| $rs === false)
			return($rs);

		$qfeatures = &$this->_service->get_module('queuefeatures');
		$this->_return['queuefeatures'] = $qfeatures->edit($this->_info['qfeatures']['id'],$rs);

		return($this->_return['queuefeatures']);
	}

	function edit_queue()
	{
		$rs = false;

		if(xivo_issa('queue',$this->_info) === false
		|| ($rs = $this->get_result('queue')) === null
		|| $rs === false)
			return($rs);

		$queue = &$this->_service->get_module('queue');

		if($queue->edit($this->_info['queue']['name'],$rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function edit_user($name)
	{
		if(is_scalar($name) === false)
			return(false);

		if(xivo_issa('user',$this->_info) === false)
			return($this->add_user());

		$this->_return['user'] = $this->delete_user($name);

		if(($rs = $this->get_result('user')) === null)
			return($this->_return['user']);

		return($this->_edit_user_type('user'));
	}

	function edit_agentgroup($name)
	{
		if(is_scalar($name) === false)
			return(false);

		if(xivo_issa('agentgroup',$this->_info) === false)
			return($this->add_agentgroup());

		$this->_return['agentgroup'] = $this->delete_agentgroup($name);

		if(($rs = $this->get_result('agentgroup')) === null)
			return($this->_return['agentgroup']);

		return($this->_edit_user_type('agentgroup'));
	}

	function edit_agent($name)
	{
		if(is_scalar($name) === false)
			return(false);

		if(xivo_issa('agent',$this->_info) === false)
			return($this->add_agent());

		$this->_return['agent'] = $this->delete_agent($name);

		if(($rs = $this->get_result('agent')) === null)
			return($this->_return['agent']);

		return($this->_edit_user_type('agent'));
	}

	function _edit_user_type($type)
	{
		if($type !== 'user' && $type !== 'agent' && $type !== 'agentgroup')
			return(false);

		if(($rs = $this->get_result($type)) === null || $rs === false)
			return($rs);

		$queuemember = &$this->_service->get_module('queuemember');

		$id = $this->_return[$type] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($queuemember->add($rs[$i]) === false)
				return(false);

			$this->_return[$type][] = $id;
		}

		if(isset($this->_return[$type][0]) === false)
			$this->_return[$type] = false;

		return(true);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
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
				case 'queuefeatures':
					$qfeatures = &$this->_service->get_module('queuefeatures');
					$qfeatures->edit_origin($this->_origin['qfeatures']);
					break;
				case 'localexten':
					$extensions = &$this->_service->get_module('extensions');

					if($this->_origin['localexten'] === false)
						$extensions->delete_exten($val);
					else
						$extensions->edit_origin($this->_origin['localexten']);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					if(xivo_issa('qfeatures',$this->_info) === false)
						break;

					$this->delete_user($this->_info['qfeatures']['name']);

					$queuemember = &$this->_service->get_module('queuemember');
					$queuemember->add_origin_list($this->_origin[$key]);
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

		if($this->delete_queuefeatures() === false
		|| $this->delete_queue() === false
		|| $this->delete_macro('localexten') === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_user($this->_info['qfeatures']['name']) === false
		|| $this->delete_agentgroup($this->_info['qfeatures']['name']) === false
		|| $this->delete_agent($this->_info['qfeatures']['name']) === false)
		{
			$this->_reverse_delete();
			$this->_status = '';

			return(false);
		}

		if(isset($this->_return['localexten']) === true
		|| isset($this->_return['extensions']) === true)
			$this->unlinks();

		$this->_status = '';

		return(true);
	}

	function delete_queuefeatures()
	{
		if(xivo_issa('qfeatures',$this->_info) === false
		|| ($qfeatures = &$this->_service->get_module('queuefeatures')) === false)
			return(false);

		$r = $qfeatures->delete($this->_info['qfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['queuefeatures'] = $r;

		return($r);
	}

	function delete_queue()
	{
		if(xivo_issa('queue',$this->_info) === false
		|| ($queue = &$this->_service->get_module('queue')) === false)
			return(false);

		$r = $queue->delete($this->_info['queue']['name']);

		if($this->_status === 'delete')
			$this->_return['queue'] = $r;

		return($r);
	}

	function delete_user($name)
	{
		return($this->_delete_user_type('user',$name));
	}

	function delete_agentgroup($name)
	{
		return($this->_delete_user_type('agentgroup',$name));
	}

	function delete_agent($name)
	{
		return($this->_delete_user_type('agent',$name));
	}

	function _delete_user_type($type,$name)
	{
		if(($type !== 'user' && $type !== 'agent' && $type !== 'agentgroup') === true
		|| xivo_issa($type,$this->_info) === false)
			return(null);

		if(xivo_issa('qfeatures',$this->_info) === false
		|| is_scalar($name) === false
		|| ($queuemember = &$this->_service->get_module('queuemember')) === false)
			return(false);

		$queuemember_where = array();

		if($type === 'agentgroup')
		{
			$queuemember_where['usertype'] = 'agent';
			$queuemember_where['category'] = 'group';
		}
		else
		{
			$queuemember_where['usertype'] = $type;
			$queuemember_where['category'] = 'queue';
		}

		$queuemember_where['queue_name'] = $name;

		$r = $queuemember->delete_where($queuemember_where);

		if($this->_status === 'delete')
			$this->_return[$type] = $r;

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
				case 'queuefeatures':
					$qfeatures = &$this->_service->get_module('queuefeatures');
					$qfeatures->recover($this->_info['qfeatures']['id']);
					break;
				case 'localexten':
					$extensions = &$this->_service->get_module('extensions');

					if($this->_origin['localexten'] !== false)
						$extensions->add_origin($this->_origin['localexten']);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					$queuemember = &$this->_service->get_module('queuemember');
					$queuemember->add_origin_list($this->_origin[$key]);
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
		if(xivo_issa('qfeatures',$this->_info) === false)
			return(false);

		if(($incall = &$this->_service->get_module('incall')) !== false)
		{
			$incall_where = array();
			$incall_where['type'] = 'queue';
			$incall_where['typeval'] = $this->_info['qfeatures']['id'];

			$incall->unlinked_where($incall_where);
		}

		if(($dialstatus = &$this->_service->get_module('dialstatus')) !== false)
		{
			$dialstatus_where = array();
			$dialstatus_where['type'] = 'queue';
			$dialstatus_where['typeval'] = $this->_info['qfeatures']['id'];

			$dialstatus->unlinked_where($dialstatus_where);
		}

		if(($schedule = &$this->_service->get_module('schedule')) === false)
			return(true);
		
		$schedule_where = array();
		$schedule_where['typetrue'] = 'queue';
		$schedule_where['typevaltrue'] = $this->_info['qfeatures']['id'];

		$schedule->unlinked_where($schedule_where);

		$schedule_where = array();
		$schedule_where['typefalse'] = 'queue';
		$schedule_where['typevalfalse'] = $this->_info['qfeatures']['id'];

		$schedule->unlinked_where($schedule_where);
	}
}

?>
