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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_queue extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_queuefeatures	= false;
	var $_queue		= false;
	var $_extensions	= false;
	var $_extenumbers	= false;
	var $_dialaction	= false;
	var $_contextnummember	= false;
	var $_contextmember	= false;
	var $_callerid		= false;
	var $_queuemember	= null;
	var $_musiconhold	= array();
	var $_announce		= array();
	var $_contextlist	= array();

	var $_skill		= false;
	var $_skillcat		= false;
	var $_skillrule 	= false;
	var $_userskill 	= false;

	var $_errors		= array();

	function xivo_application_service_asterisk_queue(&$service,&$dso,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_queuefeatures = &$this->_service->get_module('queuefeatures')) === false
		|| ($this->_queue = &$this->_service->get_module('queue')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_skill = &$this->_service->get_module('queueskill')) === false
		|| ($this->_skillcat = &$this->_service->get_module('queueskillcat')) === false
		|| ($this->_skillrule = &$this->_service->get_module('queueskillrule')) === false
		|| ($this->_userskill = &$this->_service->get_module('userqueueskill')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'queue'))) === false
		|| ($this->_dialaction = $this->get_abstract('dialaction',array('category' => 'queue'))) === false
		|| ($this->_contextnummember = $this->get_abstract('contextnummember',array('type' => 'queue'))) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'queue'))) === false
		|| ($this->_callerid = $this->get_abstract('callerid',array('type' => 'queue'))) === false)
			return(false);

		$this->_queuemember = &$this->_service->get_module('queuemember');

		return(true);
	}

	function get($id,$disable=null,$nocomponents=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['queuefeatures'] = $this->_queuefeatures->get($id)) === false
		|| ($this->_info['queue'] = $this->_queue->get($this->_info['queuefeatures']['name'],$disable)) === false)
			return(false);

		$this->_origin['queuefeatures'] = $this->_queuefeatures->get_origin();
		$this->_origin['queue'] = $this->_queue->get_origin();

		if($this->_components === false)
			return($this->_info);

		if(dwho_ak('queuemacro',$nocomponents,true) === false)
			$this->_get_queuemacro();

		if(dwho_ak('extenumbers',$nocomponents,true) === false)
			$this->_get_extenumbers($this->_info['queuefeatures']['id']);

		if(dwho_ak('user',$nocomponents,true) === false)
			$this->_get_user();

		if(dwho_ak('agentgroup',$nocomponents,true) === false)
			$this->_get_agentgroup();

		if(dwho_ak('agent',$nocomponents,true) === false)
			$this->_get_agent();

		if(dwho_ak('dialaction',$nocomponents,true) === false)
			$this->_get_dialaction($this->_info['queuefeatures']['id']);

		if(dwho_ak('contextnummember',$nocomponents,true) === false)
			$this->_get_contextnummember($this->_info['queuefeatures']['id']);

		if(dwho_ak('contextmember',$nocomponents,true) === false)
			$this->_get_contextmember($this->_info['queue']['name']);

		if(dwho_ak('callerid',$nocomponents,true) === false)
			$this->_get_callerid($this->_info['queuefeatures']['id']);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$arr['category'] = 'queue';

		return($this->_queue->get_nb($arr,$disable));
	}

	function _get_queuemacro()
	{
		$this->_info['queuemacro'] = false;
		$this->_origin['queuemacro'] = false;

		if(($this->_info['queuemacro'] = $this->_extensions->get_exten($this->_info['queuefeatures']['number'],
									       $this->_info['queuefeatures']['context'],
									       'macro')) !== false)
			$this->_origin['queuemacro'] = $this->_extensions->get_origin();

		return($this->_info['queuemacro']);
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

		$where['queue_name'] = $this->_info['queuefeatures']['name'];

		if(is_object($this->_queuemember) === false
		|| ($this->_info[$type] = $this->_queuemember->get_all_where($where)) === false
		|| isset($this->_info[$type][0]) === false)
			return(false);

		$this->_origin[$type] = $this->_queuemember->get_origin_list();

		return($this->_info[$type]);
	}

	function get_elements()
	{
		$r = array();

		$r['queuefeatures'] = $this->_queuefeatures->get_element();
		$r['queue'] = $this->_queue->get_element();
		$r['dialaction'] = $this->_dialaction->get_element();
		$r['callerid'] = $this->_callerid->get_element();

		return($r);
	}

	function get_queues_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_queues('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_queues_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_queues('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_queues($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$order,$limit);
				break;
			case 'list':
			default:
				$rs = $this->_dso->get_all($disable,$order,$limit);
		}

		$this->_cnt = $this->_dso->get_cnt();

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $identity = array();

		for($i = 0;$i < $nb;$i++)
		{
			$r[$rs[$i]['id']] = $rs[$i];

			$ref = &$r[$rs[$i]['id']];

			$identity['name'] = $ref['name'];
			$identity['number'] = $ref['number'];
			$identity['context'] = $ref['context'];

			$ref['identity'] = $this->_queuefeatures->mk_identity($identity);
			$ref['commented'] = (bool) $ref['commented'];
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_announce()
	{
		if(empty($this->_announce) === false)
			return($this->_announce);
		else if(($sounds = &$this->_service->get_module('sounds')) === false)
			return(false);
		else if(($this->_announce = $sounds->get_list('acd','pathnoext')) !== false)
			asort($this->_announce);

		return($this->_announce);
	}

	function get_dialaction_result()
	{
		return($this->_dialaction->get_result_for_display());
	}

	function get_dialaction_destination_list()
	{
		if(dwho_issa('queuefeatures',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['queuefeatures']['id'];

		$r = $this->get_destination_list('queue',$id);
		$r['sounds'] = $this->_dialaction->get_sound();

		return($r);
	}

	function _mk_queuemacro_appdata($queuefeaturesid,$timeout='')
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_QUEUE_MACRO;
		$arr['appdata'][1] = dwho_ulongint($queuefeaturesid);
		$arr['appdata'][2] = dwho_has_len($timeout) === true ? dwho_ufloat($timeout) : '';

		return($arr);
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
		|| dwho_issa('queuefeatures',$arr) === false
		|| dwho_issa('queue',$arr) === false)
			return(false);

		$this->set_queuefeatures($arr['queuefeatures']);

		$name = strtolower($this->get_result_var('queuefeatures','name'));
		$number = (string) $this->get_result_var('queuefeatures','number');
		$context = (string) $this->get_result_var('queuefeatures','context');

		$arr['queue']['name'] = $name;

		$this->set_queue($arr['queue']);

		if($this->_errnb === 0 && $number !== '')
		{
			if(($rsqueuemacro = $this->chk_macro('queuemacro',
							     $number,
							     $this->_mk_queuemacro_appdata(0),
							     $context)) !== false)
				$this->_set_result('queuemacro',$rsqueuemacro);

			$this->set_extenumbers($action,$number,$context);
		}

		if(dwho_issa('user',$arr) === true)
			$this->set_user($action,$arr['user'],$name);

		if(dwho_issa('agentgroup',$arr) === true)
			$this->set_agentgroup($action,$arr['agentgroup'],$name);

		if(dwho_issa('agent',$arr) === true)
			$this->set_agent($action,$arr['agent'],$name);

		if(dwho_issa('dialaction',$arr) === true)
			$this->set_dialaction($arr['dialaction']);

		$this->set_contextnummember($context,$number);

		$this->set_contextmember();

		if(dwho_issa('callerid',$arr) === true)
			$this->set_callerid($arr['callerid']);

		return(($this->get_errnb() < 1));
	}

	function set_queuefeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('queuefeatures',null);
			$this->_set_error('queuefeatures','invalid data');
			return(false);
		}
		else if(($rs = $this->_queuefeatures->chk_values($arr)) === false)
		{
			$this->_set_result('queuefeatures',$this->_queuefeatures->get_filter_result());
			$this->_set_error('queuefeatures',$this->_queuefeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('queuefeatures',$rs);

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

		if(isset($arr['periodic-announce']) === false)
			$arr['periodic-announce'] = '';
		else if(is_array($arr['periodic-announce']) === true)
		{
			$i = 0;
			$pannounce = '';

			foreach($arr['periodic-announce'] as $value)
			{
				if(isset($this->_announce[$value]) === false)
					continue;
				else if($i === 10)
					break;

				$pannounce .= '|'.$value;
				$i++;
			}

			$arr['periodic-announce'] = substr($pannounce,1);
		}
		else if(isset($this->_announce[$arr['periodic-announce']]) === false)
			$arr['periodic-announce'] = '';

		return($arr);
	}

	function _set_queuemacro($action,$queuefeaturesid)
	{
		$rs = false;

		if(($queuefeaturesid = dwho_ulongint($queuefeaturesid)) === 0
		|| ($rs = $this->get_result('queuemacro')) === null
		|| $rs === false)
			return($rs);
		else if(($rsmacro = $this->set_macro($action,
						     'queuemacro',
						     $rs['exten'],
						     $this->_mk_queuemacro_appdata($queuefeaturesid),
						     $rs['context'])) === false)
			return(false);

		$this->_set_result('queuemacro',$rsmacro);

		return(true);
	}

	function set_user($action,$list,$name)
	{
		return($this->_set_user_type('user',$action,$list,$name));
	}

	function set_agentgroup($action,$list,$name)
	{
		return($this->_set_user_type('agentgroup',$action,$list,$name));
	}

	function set_agent($action,$list,$name)
	{
		return($this->_set_user_type('agent',$action,$list,$name));
	}

	function _set_user_type($type,$action,$list,$name)
	{
		if(($type !== 'user' && $type !== 'agent' && $type !== 'agentgroup') === true
		|| ($action !== 'add' && $action !== 'edit') === true
		|| is_array($list) === false
		|| is_scalar($name) === false
		|| is_object($this->_queuemember) === false)
			return(false);

		$where = array();
		$where['userid'] = 0;

		if($type === 'agentgroup')
		{
			$modulename = 'agentgroup';
			$where['usertype'] = 'agent';
			$where['category'] = 'group';
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
		else if($action === 'edit')
		{
			if(dwho_issa('queuefeatures',$this->_info) === false)
				return(false);

			$where['queue_name'] = $this->_info['queuefeatures']['name'];
		}

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{			
			if(($uinfo = $module->get($list[$i])) === false)
			{
				$this->_set_error($type,'wrong '.$type);
				continue;
			}
			$where['userid'] = $uinfo['id'];

			$info = $where;
			$info['penalty'] = $info['call-limit'] = 0;
			$info['paused'] = null;

			if($action === 'edit'
			&& $this->_queuemember->get_where($where) !== false
			&& ($orig = $this->_queuemember->get_origin()) !== false)
				$info = array_merge($info,$orig);
			else
			{
				if($type === 'agent')
					$interface = $this->mk_agent_interface($uinfo['number']);
				else if($type === 'agentgroup')
					$interface = $this->mk_agent_interface($uinfo['id'],true);
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
			$info['skills']	    = '';
			// set user queueskill 
			if($this->_userskill->exists(array('userid' => $uinfo['id'])) !== false)
				$info['skills'] = $uinfo['id'];

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

	function set_contextmember()
	{
		$rs = array();
		$rs['queue-context']['context'] = (string) $this->get_result_var('queue','context');
		$rs['queue-context']['typeval'] = (string) $this->get_result_var('queue','name');
		$rs['queue-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$queuefeaturesid = null;

		if($this->get_errnb() > 0
		|| ($queuefeaturesid = $this->add_queuefeatures()) === false
		|| ($queuename = $this->add_queue()) === false
		|| $this->add_queuemacro($queuefeaturesid) === false
		|| $this->add_extenumbers($queuefeaturesid) === false
		|| $this->add_user() === false
		|| $this->add_agentgroup() === false
		|| $this->add_agent() === false
		|| $this->add_dialaction($queuefeaturesid) === false
		|| $this->add_contextnummember($queuefeaturesid) === false
		|| $this->add_contextmember($queuename) === false
		|| $this->add_callerid($queuefeaturesid) === false)
		{
			$r = false;
			if($queuefeaturesid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_queuefeatures()
	{
		if(($rs = $this->get_result('queuefeatures')) === null
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

	function add_queuemacro($queuefeaturesid)
	{
		if(($rs = $this->_set_queuemacro('add',$queuefeaturesid)) !== true)
			return($rs);

		return(parent::add_macro('queuemacro'));
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

		$ret = $this->_return[$type] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return[$type][] = $ret;
		}

		if(isset($this->_return[$type][0]) === false)
			$this->_return[$type] = false;

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
				case 'queuefeatures':
					$this->_queuefeatures->delete($val);
					break;
				case 'queue':
					$this->_queue->delete($val);
					break;
				case 'queuemacro':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add();
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					if(is_array($val) === false)
						continue;

					foreach($val as $value)
						$this->_queuemember->delete($value);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_add();
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_add();
					break;
				case 'contextmember':
					$this->_contextmember->reverse_add();
					break;
				case 'callerid':
					$this->_callerid->reverse_add();
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
		|| $this->edit_queuefeatures() === false)
		{
			$this->_status = '';
			return(false);
		}
		else if(($queuename = $this->edit_queue()) === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}
		else if($this->_info['queuefeatures']['number'] !== '')
		{
			if($this->get_result('queuemacro') === null
			|| $this->get_result('extenumbers') === null)
			{
				$this->delete_macro('queuemacro');
				$this->delete_extenumbers();
			}
			else if($this->edit_queuemacro($this->_info['queuefeatures']['id']) === false
			|| $this->edit_extenumbers() === false)
			{
				$this->_reverse_edit();
				$this->_status = '';
				return(false);
			}
		}
		else if($this->add_queuemacro($this->_info['queuefeatures']['id']) === false
		|| $this->add_extenumbers($this->_info['queuefeatures']['id']) === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		if($this->edit_user($this->_info['queuefeatures']['name']) === false
		|| $this->edit_agentgroup($this->_info['queuefeatures']['name']) === false
		|| $this->edit_agent($this->_info['queuefeatures']['name']) === false
		|| $this->edit_dialaction() === false
		|| $this->edit_contextnummember() === false
		|| $this->edit_contextmember($queuename) === false
		|| $this->edit_callerid() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function edit_queuefeatures()
	{
		if(dwho_issa('queuefeatures',$this->_info) === false
		|| ($rs = $this->get_result('queuefeatures')) === null
		|| $rs === false)
			return(false);

		$this->_return['queuefeatures'] = $this->_queuefeatures->edit($this->_info['queuefeatures']['id'],$rs);

		return($this->_return['queuefeatures']);
	}

	function edit_queue()
	{
		if(dwho_issa('queue',$this->_info) === false
		|| ($rs = $this->get_result('queue')) === null
		|| $rs === false
		|| $this->_queue->edit($this->_info['queue']['name'],$rs) === false)
			return(false);

		$this->_return['queue'] = $rs['name'];

		return($this->_return['queue']);
	}

	function edit_queuemacro($queuefeaturesid)
	{
		if(($rs = $this->_set_queuemacro('edit',$queuefeaturesid)) !== true)
			return($rs);

		return(parent::edit_macro('queuemacro'));
	}

	function edit_user($name)
	{
		if(is_scalar($name) === false)
			return(false);
		else if(dwho_issa('user',$this->_info) === false)
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
		else if(dwho_issa('agentgroup',$this->_info) === false)
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
		else if(dwho_issa('agent',$this->_info) === false)
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

		$ret = $this->_return[$type] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if($this->_queuemember->add($rs[$i]) === false)
				return(false);

			$ret['queue_name'] = $rs[$i]['queue_name'];
			$ret['interface'] = $rs[$i]['interface'];

			$this->_return[$type][] = $ret;
		}

		if(isset($this->_return[$type][0]) === false)
			$this->_return[$type] = false;

		return(true);
	}

	function edit_dialaction()
	{
		if(dwho_issa('queuefeatures',$this->_info) === false)
			return(false);

		return(parent::edit_dialaction($this->_info['queuefeatures']['id']));
	}

	function edit_contextnummember()
	{
		if(dwho_issa('queuefeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextnummember($this->_info['queuefeatures']['id']));
	}

	function edit_callerid()
	{
		if(dwho_issa('queuefeatures',$this->_info) === false)
			return(false);

		return(parent::edit_callerid($this->_info['queuefeatures']['id']));
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
				case 'queuefeatures':
					if(isset($this->_origin['queuefeatures']) === false
					|| $this->_origin['queuefeatures'] === false)
						$this->delete_queuefeatures();
					else
						$this->_queuefeatures->edit_origin($this->_origin['queuefeatures']);
					break;
				case 'queue':
					if(isset($this->_origin['queue']) === false
					|| $this->_origin['queue'] === false)
						$this->_queue->delete($val);
					else
						$this->_queue->edit_origin($this->_origin['queue']);
					break;
				case 'queuemacro':
					if(isset($this->_origin['queuemacro']) === false
					|| $this->_origin['queuemacro'] === false)
						$this->delete_macro('queuemacro');
					else
						$this->_extensions->edit_origin($this->_origin['queuemacro']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					if(dwho_issa('queuefeatures',$this->_info) === false)
						continue;

					$this->delete_user($this->_info['queuefeatures']['name']);

					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$this->_queuemember->add_origin_list($this->_origin[$key]);
					break;
				case 'dialaction':
					$this->_dialaction->reverse_edit($this->_status);
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_edit($this->_status);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_edit($this->_status);
					break;
				case 'callerid':
					$this->_callerid->reverse_edit($this->_status);
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

		if($this->delete_queuefeatures() === false
		|| $this->delete_queue() === false
		|| $this->delete_macro('queuemacro') === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_user($this->_info['queuefeatures']['name']) === false
		|| $this->delete_agentgroup($this->_info['queuefeatures']['name']) === false
		|| $this->delete_agent($this->_info['queuefeatures']['name']) === false
		|| $this->delete_dialaction() === false
		|| $this->delete_contextnummember() === false
		|| $this->delete_contextmember($this->_info['queuefeatures']['name']) === false
		|| $this->delete_callerid() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}
		else
			$this->unlinks();

		$this->_status = '';

		return($r);
	}

	function delete_queuefeatures()
	{
		if(dwho_issa('queuefeatures',$this->_info) === false)
			return(false);

		$r = $this->_queuefeatures->delete($this->_info['queuefeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['queuefeatures'] = $r;

		return($r);
	}

	function delete_queue()
	{
		if(dwho_issa('queue',$this->_info) === false)
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
		|| dwho_issa($type,$this->_info) === false)
			return(null);
		else if(dwho_issa('queuefeatures',$this->_info) === false
		|| is_scalar($name) === false
		|| is_object($this->_queuemember) === false)
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

		$where['queue_name'] = $name;

		$r = $this->_queuemember->delete_where($where);

		if($this->_status === 'delete')
			$this->_return[$type] = $r;

		return($r);
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
				case 'queuefeatures':
					if(isset($this->_origin['queuefeatures']) === true
					&& $this->_origin['queuefeatures'] !== false)
						$this->_queuefeatures->add_origin($this->_origin['queuefeatures']);
					break;
				case 'queue':
					if(isset($this->_origin['queue']) === true
					&& $this->_origin['queue'] !== false)
						$this->_queue->add_origin($this->_origin['queue']);
					break;
				case 'queuemacro':
					if(isset($this->_origin['queuemacro']) === true
					&& $this->_origin['queuemacro'] !== false)
						$this->_extensions->add_origin($this->_origin['queuemacro']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'user':
				case 'agentgroup':
				case 'agent':
					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$this->_queuemember->add_origin_list($this->_origin[$key]);
					break;
				case 'dialaction':
					$this->_dialaction->add_origin_list();
					break;
				case 'contextnummember':
					$this->_contextnummember->add_origin();
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
				case 'callerid':
					$this->_callerid->add_origin();
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(dwho_issa('queuefeatures',$this->_info) === false)
			return(false);

		$this->_dialaction->unlinked('queue',$this->_info['queuefeatures']['id']);

		return(true);
	}

	function add_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_add($data) === false
		|| $this->add() === false)
			return(false);

		return(true);
	}

	function edit_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_edit($data) === false
		|| $this->edit() === false)
			return(false);

		return(true);
	}

	function skills_count($search)
	{ 
		$res= $this->_dso->skills_count($search);
		return($res[0]['count']);
	}

	function skills_getall($search, $limit)
	{
		return($this->_dso->skills_getall($search, $limit));
	}

	function skills_get($id)
	{
		return $this->_dso->skills_getone((int) $id);
	}

	function skills_delete($id, $vals_only=false)
	{
		// deleting related user skills
		$skills = $this->_skill->get_list_where(array('catid' => $id));
		if(is_array($skills))
		{
			foreach($skills as $skid)
				$this->_userskill->delete_where(array('skillid' => $skid));
		}

		$res = $this->_skill->delete_where(array('catid' => $id));
		if(!$vals_only)
			$res = $this->_skillcat->delete($id);

		return($res);
	}

	function skills_setadd($name, $values)
	{
		$this->_reset();
		$ret = ($this->_skillcat->chk_value('name', $name) === false);
		$this->_errors = $this->_skillcat->get_filter_error();

		$this->_errors['values'] = array();
		for($i = count($values)-1; $i >= 0; $i--)
		{			
			$ret |= ($this->_skill->chk_values($values[$i]) === false);
			$this->_errors['values'][$i] = $this->_skill->get_filter_error();
		}

		return !$ret;
	}

	/**Save a new queue skill

		@param item $_QR array with skill values
		@verbatim
			array
			  'name' => string 'OS' (length=2)
			  'values' => 
			    array
			      0 => 
			        array
			          0 => string 'linux' (length=5)
			          1 => string 'best' (length=4)
			          2 => string 'LNX' (length=3)
		@endverbatim
	*/
	function skills_add($name, $values)
	{
		// save skill category and return autoincremented ID
		$catid =  $this->_skillcat->add(array('id' => null, 'name' => $name));		
		if(is_null($catid) || $catid === false)
		{ return(false); }

		// prepare skill save-array
		foreach(array_values($values) as $value)
		{
			$value['id'] = null;
			$value['catid'] = $catid;
			$valid = $this->_skill->add($value);
		}

		return(($this->get_errnb() < 1));
	}

	function skills_setedit($name, $values)
	{ return $this->skills_setadd($name, $values); }

	/**Update a queue skill

		@param item $_QR array with skill values
		@verbatim
			array
			  'name' => string 'OS' (length=2)
			  'values' => 
			    array
			      0 => 
			        array
				  'id'		=> string '4' (length=1)
			          'name' 	=> string 'linux' (length=5)
			          'description' => string 'best' (length=4)
			          'printscreen' => string 'LNX' (length=3)
		@endverbatim

		skill values are referenced by users (with weight) and rules,
		so we can't simply delete & recreate all the values
	*/
	function skills_edit($id, $name, $values)
	{
		$this->_reset();

		// update skill name
		$ret =  $this->_skillcat->edit($id, array('name' => $name));		
		if(is_null($ret) || $ret === false)
		{ return(false); }

		// we retrieve stored values ids
		$stored_values = $this->_skill->get_list_where(array('catid' => $id));
		if($stored_values === false)
		{ $stored_values = array(); }
		$update_values = array();	

		// insert new values
		// prepare skill save-array
		foreach(array_values($values) as $value)
		{
			$value['catid'] = $id;
		
			// new skill value
			if($value['id'] == -1)
			{ 
				$value['id'] = null;
				$valid = $this->_skill->add($value);
			}
			else
			{ 
				$ret = $this->_skill->edit($value['id'], $value);
				$valid = $value['id'];
			}

			$update_values[] = $valid;
		}

		$delete_values = array_diff($stored_values, $update_values);

		// WARNING: skills may be users by users & by skill rules
		//	- we also delete user skills
		//	  (rule skills just be ignored)
		foreach($delete_values as $id)
		{ 
			$this->_userskill->delete_where(array('skillid' => $id));
			$res = $this->_skill->delete($id); 
		}

		return(($this->get_errnb() < 1));
	}

	function skills_gettree()
	{
		return($this->_dso->skills_gettree());
	}

	/*** skill rules */

	function skillrules_count($search)
	{ 
		$res= $this->_dso->skillrules_count($search);
		return($res[0]['count']);
	}

	function skillrules_getall($search=null, $limit=null)
	{
		return($this->_dso->skillrules_getall($search, $limit));
	}
	
	function skillrules_save($name, $rules)
	{
		$this->_reset();
		$err = false;

		$name = trim($name);
		if(strlen($name) == 0)
		{ $err = true; $this->_set_error('name', 'empty'); }
		else if($this->_skillrule->exists(array('name' => $name)))
		{ $err = true; $this->_set_error('name', 'alreadyset'); }		

		$catrules = '';
		for($i = 0; $i < count($rules); $i++)
		{
			if(strlen($rules[$i]) == 0)
			{ $err = true; $this->_set_error("rule-$i",'empty'); }

			if(!$this->_skillrule->check_rule($rules[$i]))
			{ $err = true; $this->_set_error("rule-$i", 'syntaxincorrect'); }

			$catrules .= ";" . $rules[$i];
		}

		if(strlen($catrules) == 0)
		{ $err = true; $this->_set_error('all-rules', 'norule'); }

		if($err)
		{ return false; }
	
		// save skill category and return autoincremented ID
		$data	= array('name' => $name, 'rule' => substr($catrules, 1));
		$id =  $this->_skillrule->add($data);

		return(($this->get_errnb() < 1));
	}

	function skillrules_update($item)
	{
		$this->_reset();
		$err = false;

		$name 		= trim($item['name']);
		if(strlen($name) == 0)
		{ $err = true; $this->_set_error('name','empty'); }

		$ret = $this->_skillrule->get_where(array('name' => $name));
		if($ret !== false && ((int)$ret['id']) != ((int)$item['id']))
		{ $err = true; $this->_set_error('name', 'alreadyset'); }

		$catrules = '';
		for($i = 0; $i < count($item['rule']); $i++)
		{
			$rule = $item['rule'][$i];
			if(strlen($rule) == 0)
			{ $err = true; $this->_set_error("rule-$i",'empty'); }

			if(!$this->_skillrule->check_rule($rule))
			{ $err = true; $this->_set_error("rule-$i", 'syntaxincorrect'); }

			$catrules .= ";$rule";
		}

		if(strlen($catrules) == 0)
		{ $err = true; $this->_set_error('all-rules', 'norule'); }

		if($err)
		{ return false; }

		// update skill name
		$ret =  $this->_skillrule->edit($item['id'], 
			array('name' => $name, 'rule' => substr($catrules, 1)));
		return(!is_null($ret));
	}

	function skillrules_delete($id)
	{
		$ret =  $this->_skillrule->delete($id);

		return $ret;
	}

	function skillrules_getone($id)
	{
		$skr = $this->_dso->skillrules_getone((int) $id);
		if($skr !== false && array_key_exists('rule', $skr));
			$skr['rule'] = split(';', $skr['rule']);

		return($skr);
	}

	/** user skills */
	/**
		check values
	*/
	function userskills_setadd($userskills)
	{
		$this->_errors = array();
		$skillids = array();
		$err = true;

		for($i = count($userskills) - 1; $i >= 0; $i--)
		{	
			$this->_errors[$i] = array();

			if(strlen($userskills[$i]['weight']) == 0)
			{ $err = false; $this->_errors[$i]['weight'] = 'empty'; }
			else if(!ctype_digit($userskills[$i]['weight']))
			{ $err = false; $this->_errors[$i]['weight'] = 'notnumeric'; }
			else if(((int)$userskills[$i]['weight']) < 0 
			|| ((int)$userskills[$i]['weight']) > 100)
			{ $err = false; $this->_errors[$i]['weight'] = 'unbound'; } 

			if(strlen($userskills[$i]['skillid']) == 0)
			{ $err = false; $this->_errors[$i]['skillid'] = 'empty'; }
			if(array_key_exists($userskills[$i]['skillid'], $skillids))
			{ 
				$err = false;
				$this->_errors[$i]['skillid'] = 'doubloon'; 
				$this->_errors[$skillids[$userskills[$i]['skillid']]]['skillid'] = 'doubloon';
			} 
			else 
			{ $skillids[$userskills[$i]['skillid']] = $i; }
	
		}

		return($err);
	}

	function userskills_get_error()
	{ return $this->_errors; }

	function userskills_add($userskills)
	{
		$count = count($userskills); 
		for($i =  0; $i < $count; $i++)
			$this->_userskill->add($userskills[$i]);

		return true;
	}

	function userskills_setedit($userskills)
	{ return $this->userskills_setadd($userskills); }

	function userskills_edit($userid, $userskills)
	{ 
		$this->userskills_delete($userid);
		return $this->userskills_add($userskills);
	}

	function userskills_delete($userid)
	{ return $this->_userskill->delete_where(array('userid' => $userid)); }

	function userskills_get($userid)
	{ return $this->_userskill->get_all_where(array('userid' => $userid)); }
}

?>
