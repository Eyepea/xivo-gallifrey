<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_queue extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_queuefeatures	= null;
	var $_queue		= null;
	var $_extensions	= null;
	var $_extenumbers	= null;
	var $_queuemember	= false;
	var $_musiconhold	= false;
	var $_announce		= false;

	function xivo_application_service_asterisk_queue(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_queuefeatures = &$this->_service->get_module('queuefeatures')) === false
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
		|| ($this->_extenumbers = &$this->get_abstract('extenumbers')) === false)
			return(false);

		$this->_queuemember = &$this->_service->get_module('queuemember');

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['qfeatures'] = $this->_queuefeatures->get($id)) === false
		|| ($this->_info['queue'] = $this->_queue->get($this->_info['qfeatures']['name'],$disable)) === false)
			return(false);

		$this->_origin['qfeatures'] = $this->_queuefeatures->get_origin();
		$this->_origin['queue'] = $this->_queue->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_exten();
		$this->_get_extenumbers();
		$this->_get_user();
		$this->_get_agentgroup();
		$this->_get_agent();
		$this->get_musiconhold();
		$this->get_announce();

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$arr['category'] = 'queue';

		return($this->_queue->get_nb($arr,$disable));
	}

	function _get_exten()
	{
		$this->_info['exten'] = false;
		$this->_origin['exten'] = false;

		$this->_info['exten'] = $this->_extensions->get_exten('macro',
								    $this->_info['qfeatures']['number'],
								    $this->_info['qfeatures']['context']);

		$this->_origin['exten'] = $this->_extensions->get_origin();

		return($this->_info['exten']);
	}

	function _get_extenumbers()
	{
		return(parent::_get_extenumbers('queue',$this->_info['qfeatures']['id']));
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

		if($this->_queuemember === false
		|| ($this->_info[$type] = $this->_queuemember->get_all_where($where)) === false
		|| isset($this->_info[$type][0]) === false)
			return(false);

		$this->_origin[$type] = $this->_queuemember->get_origin_list();

		return($this->_info[$type]);
	}

	function get_elements()
	{
		$r = array();

		$r['qfeatures'] = $this->_queuefeatures->get_element();
		$r['queue'] = $this->_queue->get_element();

		return($r);
	}

	function get_queues_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		$assoc = (bool) $assoc;

		if(($rs = $this->_queuefeatures->get_all(null,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_queuefeatures->get_cnt();
			return(false);
		}

		$this->_cnt = $this->_queuefeatures->get_cnt();

		$this->_queuemember = &$this->_service->get_module('queuemember');

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['qfeatures'] = $rs[$i];

			if(($info['queue'] = $this->_queue->get($info['qfeatures']['name'],$disable)) === false
			|| ($info['nb_qmember'] = $this->_queuemember->get_nb(
							array('queue_name' => $info['queue']['name']),
							$disable)) === false)
				continue;

			$r[$info['qfeatures']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_announce()
	{
		if($this->_announce !== false)
			return($this->_announce);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
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
			$appdata = array('appdata' => XIVO_SRE_IPBX_AST_QUEUE_MACRO);
			if(($rsexten = $this->set_macro($action,'exten',$number,$appdata,$context)) !== false)
				$this->_set_result('exten',$rsexten);

			$this->set_extenumbers($action,'queue',$number,$context);
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
		if(is_array($arr) === false)
		{
			$this->_set_result('qfeatures',null);
			$this->_set_error('qfeatures','invalid data');
			return(false);
		}
		else if(($rs = $this->_queuefeatures->chk_values($arr)) === false)
		{
			$this->_set_result('qfeatures',$this->_queuefeatures->get_filter_result());
			$this->_set_error('qfeatures',$this->_queuefeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('qfeatures',$rs);

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

		if(($arr = $this->_set_announce($arr)) === false)
		{
			$this->_set_result('queue',null);
			$this->_set_error('queue','invalid announce');
			return(false);
		}

		$arr['category'] = 'queue';

		if(($rs = $this->_queue->chk_values($arr)) === false)
		{
			$this->_set_result('queue',$this->_queue->get_filter_result());
			$this->_set_error('queue',$this->_queue->get_filter_error());
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
		|| $this->_queuemember === false)
			return(false);

		$where = array();
		$where['userid'] = 0;

		if($type === 'agentgroup')
		{
			$modulename = 'agentgroup';
			$where['usertype'] = 'agent';
			$where['category'] = 'group';

			if(($agent = &$this->_service->get_module('agent')) === false)
				return(false);
		}
		else
		{
			$modulename = $type.'features';
			$where['usertype'] = $type;
			$where['category'] = 'queue';
		}

		if(($module = &$this->_service->get_module($modulename)) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);

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
			else if($type === 'agentgroup' && ($ainfo = $agent->get($uinfo['groupid'])) === false)
			{
				$this->_set_error('agentgroup','wrong agentgroup');
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
				if($type === 'agent')
					$interface = $this->mk_agent_interface($uinfo['number']);
				else if($type === 'agentgroup')
					$interface = $this->mk_agent_interface($ainfo['var_val'],true);
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

			$info['queue_name'] = $name;

			if(($rs = $this->_queuemember->chk_values($info)) === false
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
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$qfeaturesid = null;

		if($this->get_errnb() > 0
		|| ($qfeaturesid = $this->add_queuefeatures()) === false
		|| $this->add_queue() === false
		|| $this->add_macro('exten') === false
		|| $this->add_extenumbers($qfeaturesid) === false
		|| $this->add_user() === false
		|| $this->add_agentgroup() === false
		|| $this->add_agent() === false)
		{
			$r = false;
			if($qfeaturesid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_queuefeatures()
	{
		if(($rs = $this->get_result('qfeatures')) === null
		|| $rs === false)
			return(false);

		$this->_return['queuefeatures'] = $this->_queuefeatures->add($rs);

		return($this->_return['queuefeatures']);
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

		$id = $this->_return[$type] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
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
				case 'exten':
					$this->_extensions->delete_exten($val);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$this->_queuemember->delete($val[$j]);
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
		else if($this->edit_queue() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$unlinks = false;

		if($this->_info['qfeatures']['number'] !== '')
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
		|| $this->add_extenumbers($this->_info['qfeatures']['id']) === false)
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
		if(xivo_issa('qfeatures',$this->_info) === false
		|| ($rs = $this->get_result('qfeatures')) === null
		|| $rs === false)
			return(false);

		$this->_return['queuefeatures'] = $this->_queuefeatures->edit($this->_info['qfeatures']['id'],$rs);

		return($this->_return['queuefeatures']);
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

		return($this->_edit_user_type('user'));
	}

	function edit_agentgroup($name)
	{
		if(is_scalar($name) === false)
			return(false);
		else if(xivo_issa('agentgroup',$this->_info) === false)
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
		else if(xivo_issa('agent',$this->_info) === false)
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
		else if(($rs = $this->get_result($type)) === null || $rs === false)
			return($rs);

		$id = $this->_return[$type] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$id['queue_name'] = $rs[$i]['queue_name'];
			$id['interface'] = $rs[$i]['interface'];

			if($this->_queuemember->add($rs[$i]) === false)
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
					$this->_queuefeatures->edit_origin($this->_origin['qfeatures']);
					break;
				case 'exten':
					if($this->_origin['exten'] === false)
						$this->_extensions->delete_exten($val);
					else
						$this->_extensions->edit_origin($this->_origin['exten']);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					if(xivo_issa('qfeatures',$this->_info) === false)
						break;

					$this->delete_user($this->_info['qfeatures']['name']);

					$this->_queuemember->add_origin_list($this->_origin[$key]);
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
		|| $this->delete_macro('exten') === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_user($this->_info['qfeatures']['name']) === false
		|| $this->delete_agentgroup($this->_info['qfeatures']['name']) === false
		|| $this->delete_agent($this->_info['qfeatures']['name']) === false)
		{
			$this->_reverse_delete();
			$this->_status = '';
			return(false);
		}

		if(isset($this->_return['exten']) === true
		|| isset($this->_return['extensions']) === true)
			$this->unlinks();

		$this->_status = '';

		return(true);
	}

	function delete_queuefeatures()
	{
		if(xivo_issa('qfeatures',$this->_info) === false)
			return(false);

		$r = $this->_queuefeatures->delete($this->_info['qfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['queuefeatures'] = $r;

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
		|| $this->_queuemember === false)
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

		$r = $this->_queuemember->delete_where($queuemember_where);

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
					$this->_queuefeatures->recover($this->_info['qfeatures']['id']);
					break;
				case 'exten':
					if($this->_origin['exten'] !== false)
						$this->_extensions->add_origin($this->_origin['exten']);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					$this->_queuemember->add_origin_list($this->_origin[$key]);
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

		return(true);
	}
}

?>
