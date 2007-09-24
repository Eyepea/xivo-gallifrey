<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk extends xivo_service_abstract
{
	var $_ini	= array();
	var $_type	= 'ipbx';
	var $_name	= 'asterisk';

	function xivo_service_asterisk($ini)
	{
		$this->_load_config();

		if(xivo_issa('general',$ini) === false)
			trigger_error('Invalid parameter in '.__CLASS__,E_USER_ERROR);

		$this->_ini = $ini;

		$this->_get_service_information();
	}

	function required($arr,$once=false,$includable=false)
	{
		$arr = (array) $arr;

		array_unshift($arr,dirname(__FILE__).XIVO_SEP_DIR.$this->_name);

		return(xivo_file::required($arr,$once,$includable));
	}

	function _get_service_information()
	{
		$uppertype = strtoupper($this->_type);
		$define_label = 'XIVO_SRE_'.$uppertype.'_LABEL';
		$define_version = 'XIVO_SRE_'.$uppertype.'_VERSION';

		if(isset($_SESSION['_SRE'][$this->_type][$this->_name]) === false)
		{
			define($define_label,'undefined');
			define($define_version,'undefined');

			return(false);
		}

		$sess_sre = &$_SESSION['_SRE'][$this->_type][$this->_name];

		$label = $version = false;

		if(xivo_issa('infos',$sess_sre) === true
		&& isset($sess_sre['infos']['label'],$sess_sre['infos']['version']) === true)
		{
			$label = (string) $sess_sre['infos']['label'];
			$version = (string) $sess_sre['infos']['version'];
		}
		else if(($ami = &$this->get_module('ami')) !== false
		&& ($infos = $ami->cmd('show version',true)) !== false
		&& is_array($infos) === true && isset($infos[0]) === true)
		{
			$infos = explode(' ',$infos[0],3);

			if(isset($infos[0]) === true)
			{
				$label = $infos[0];
				$sess_sre['infos']['label'] = $label;

				if(isset($infos[1]) === true)
				{
					$version = $infos[1];
					$sess_sre['infos']['version'] = $version;
				}
			}
		}

		if(($label === false || $version === false) === true
		&& xivo_issa('ini',$sess_sre) === true
		&& xivo_issa('general',$sess_sre['ini']) === true)
		{
			if($label === false && isset($sess_sre['ini']['general']['label']) === true)
				$label = (string) $sess_sre['ini']['general']['label'];

			if($version === false && isset($sess_sre['ini']['general']['version']) === true)
				$version = (string) $sess_sre['ini']['general']['version'];
		}
		
		if($label === false)
			$label = 'undefined';

		if($version === false)
			$version = 'undefined';

		define($define_label,$label);
		define($define_version,$version);
	}

	function get_users_search($name,$protocols=array(),$disable=null,$assoc=false)
	{
		return($this->_get_users('search',$protocols,$disable,$name,$assoc));
	}

	function get_users_list($protocols=array(),$disable=null,$assoc=false)
	{
		return($this->_get_users('list',$protocols,$disable,null,$assoc));
	}

	function get_users_context($name,$protocols=array(),$disable=null,$assoc=false)
	{
		$name = strval($name);

		if($name === '#main')
			$name = '';

		return($this->_get_users('context',$protocols,$disable,$name,$assoc));
	}

	function _get_users($action,$protocols=array(),$disable=null,$opts=array(),$assoc=false)
	{
		$action = (string) $action;
		$protocols = (array) $protocols;
		$assoc = (bool) $assoc;
		
		if(empty($protocols) === true)
			$protocols = array_keys($this->_conf['protocol']);
		else
			$protocols = array_values($protocols);

		$ufeatures = &$this->get_module('userfeatures');

		switch($action)
		{
			case 'search':
				$rs = $ufeatures->get_search($opts);
				break;
			case 'context':
				$rs = $ufeatures->get_all_where(array('context' => $opts));
				break;
			case 'list':
			default:
				$rs = $ufeatures->get_all();
		}

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['ufeatures'] = $rs[$i];

			if(in_array($info['ufeatures']['protocol'],$protocols) === false
			|| ($protocol = &$this->get_protocol_module($info['ufeatures']['protocol'])) === false
			|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'],$disable)) === false)
				continue;

			$r[$info['ufeatures']['id']] = $info;
		}
		
		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_trunks_list($disable=null)
	{
		$tfeatures = &$this->get_module('trunkfeatures');

		if(($rs = $tfeatures->get_all(null,null,true,array('trunk' => SORT_ASC))) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = $info = $trunk = array();

		xivo::load_class('xivo_sort');
		$sort = new xivo_sort(array('browse' => 'trunk','key' => 'name'));

		for($i = 0;$i < $nb;$i++)
		{
			$info['tfeatures'] = $rs[$i];

			if(isset($trunks[$info['tfeatures']['trunk']]) === false)
				$trunks[$info['tfeatures']['trunk']] = &$this->get_module('trunk'.$info['tfeatures']['trunk']);

			$trunkmodule = &$trunks[$info['tfeatures']['trunk']];

			if($trunkmodule === false)
				continue;

			if(isset($r[$info['tfeatures']['trunk']]) === false)
				$r[$info['tfeatures']['trunk']] = array();

			$ret = &$r[$info['tfeatures']['trunk']];

			if(($info['trunk'] = $trunkmodule->get($info['tfeatures']['trunkid'],$disable)) === false)
			{
				if(empty($ret) === true)
					unset($r[$info['tfeatures']['trunk']]);
				continue;
			}

			$info['trunk']['trunkfeaturesid'] = $info['tfeatures']['id'];
			$ret[] = $info;

			usort($ret,array(&$sort,'str_usort'));
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_groups_list($disable=null,$assoc=false)
	{
		$assoc = (bool) $assoc;

		$gfeatures = &$this->get_module('groupfeatures');
		$queue = &$this->get_module('queue');
		$qmember = &$this->get_module('queuemember');

		if(($rs = $gfeatures->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['gfeatures'] = $rs[$i];

			if(($info['queue'] = $queue->get($info['gfeatures']['name'],$disable)) === false
			|| ($info['nb_qmember'] = $qmember->get_nb(array('queue_name' => $info['queue']['name']),$disable)) === false)
				continue;

			$r[$info['gfeatures']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_queues_list($disable=null,$assoc=false)
	{
		$assoc = (bool) $assoc;

		$qfeatures = &$this->get_module('queuefeatures');
		$queue = &$this->get_module('queue');
		$qmember = &$this->get_module('queuemember');

		if(($rs = $qfeatures->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['qfeatures'] = $rs[$i];

			if(($info['queue'] = $queue->get($info['qfeatures']['name'],$disable)) === false
			|| ($info['nb_qmember'] = $qmember->get_nb(array('queue_name' => $info['queue']['name']),$disable)) === false)
				continue;

			$r[$info['qfeatures']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_agents_list($group=0,$disable=null)
	{
		$group = xivo_uint($group);

		$afeatures = &$this->get_module('agentfeatures');
		$agroup = &$this->get_module('agentgroup');
		$agent = &$this->get_module('agent');

		if($group === 0)
			$rs = $afeatures->get_all($disable);
		else
			$rs = $afeatures->get_all_where(array('numgroup' => $group),$disable);

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['afeatures'] = $rs[$i];

			if(($info['agent'] = $agent->get_agent($info['afeatures']['agentid'],$disable)) === false
			|| ($info['agroup'] = $agroup->get($info['afeatures']['numgroup'],$disable)) === false)
				continue;

			$info['sort'] = array();
			$info['sort']['id'] = $info['afeatures']['id'];
			$info['sort']['identity'] = $info['afeatures']['identity'];
			$info['sort']['var_metric'] = $info['agent']['var_metric'];

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_agent_groups_list($disable=null)
	{
		$agroup = &$this->get_module('agentgroup');
		$agent = &$this->get_module('agent');
		$afeatures = &$this->get_module('agentfeatures');

		if(($rs = $agroup->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['agroup'] = $rs[$i];

			if(($info['agent'] = $agent->get($info['agroup']['groupid'],$disable)) === false
			|| ($info['nb_agent'] = $afeatures->get_nb(array('numgroup' => $info['agroup']['id']),$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_queue_user($id=0,$disable=null,$assoc=false)
	{
		return($this->_get_queue_member('user',$id,$disable,$assoc));
	}

	function get_queue_agent($id=0,$disable=null,$assoc=false)
	{
		return($this->_get_queue_member('agent',$id,$disable,$assoc));
	}

	function _get_queue_member($usertype='',$userid=0,$disable=null,$assoc=false)
	{
		$usertype = (string) $usertype;
		$userid = xivo_uint($userid);
		$assoc = (bool) $assoc;
	
		$qfeatures = &$this->get_module('queuefeatures');
		$queue = &$this->get_module('queue');
		$qmember = &$this->get_module('queuemember');

		if(($rs = $qfeatures->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();
		$where = array('category' => 'queue');

		for($i = 0;$i < $nb;$i++)
		{
			$info['qfeatures'] = $rs[$i];

			if(($info['queue'] = $queue->get($info['qfeatures']['name'],$disable)) === false)
				continue;

			$where['queue_name'] = $info['qfeatures']['name'];

			if($usertype === 'user' || $usertype === 'agent')
			{
				$where['usertype'] = $usertype;

				if($userid !== 0)
				{
					$where['userid'] = $userid;

					if(($member = $qmember->get_all_where($where)) !== false)
						$info['member'] = $member[0];
					else
						$info['member'] = false;

					$r[$info['qfeatures']['id']] = $info;
					continue;
				}
			}

			$info['member'] = $qmember->get_all_where($where);

			$r[$info['qfeatures']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_group_user($userid=0,$disable=null,$assoc=false)
	{
		$userid = xivo_uint($userid);
		$assoc = (bool) $assoc;
	
		$gfeatures = &$this->get_module('groupfeatures');
		$queue = &$this->get_module('queue');
		$qmember = &$this->get_module('queuemember');

		if(($rs = $gfeatures->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();
		$where = array('usertype' => 'user','category' => 'group');

		for($i = 0;$i < $nb;$i++)
		{
			$info['gfeatures'] = $rs[$i];

			if(($info['queue'] = $queue->get($info['gfeatures']['name'],$disable)) === false)
				continue;

			$where['queue_name'] = $info['gfeatures']['name'];

			if($userid !== 0)
			{
				$where['userid'] = $userid;

				if(($member = $qmember->get_all_where($where)) !== false)
					$info['member'] = $member[0];
				else
					$info['member'] = false;

				$r[$info['gfeatures']['id']] = $info;
				continue;
			}

			$info['member'] = $qmember->get_all_where($where);

			$r[$info['gfeatures']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_group_agent($userid=0,$disable=null)
	{
		$userid = xivo_uint($userid);
	
		$agroup = &$this->get_module('agentgroup');
		$agent = &$this->get_module('agent');
		$queue = &$this->get_module('queue');
		$qmember = &$this->get_module('queuemember');

		if(($rs = $queue->get_all_where(array('category' => 'queue'),$disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();
		$where = array('usertype' => 'agent','category' => 'group');

		for($i = 0;$i < $nb;$i++)
		{
			$info['queue'] = $rs[$i];

			$where['queue_name'] = $info['queue']['name'];

			if($userid !== 0)
			{
				$where['userid'] = $userid;

				if(($member = $qmember->get_all_where($where)) !== false
				&& ($aginfo = $agent->get($member[0]['userid'],$disable)) !== false)
				{
					$info['member'] = $member[0];
					$info['agroup'] = $aginfo;
				}
				else
					$info['member'] = $info['agroup'] = false;

				$r[] = $info;
				continue;
			}

			if(($info['member'] = $qmember->get_all_where($where)) === false)
			{
				$info['agroup'] = false;
				$r[] = $info;
				continue;
			}

			$memberinfo = $agroupinfo = array();

			$cnt = count($info['member']);

			for($j = 0;$j < $cnt;$j++)
			{
				if(($aginfo = $agent->get($info['member'][$j]['userid'],$disable)) === false)
					continue;

				$memberinfo[] = $info['member'][$j];
				$agroupinfo[] = $aginfo;
			}

			$info['member'] = $memberinfo;
			$info['agroup'] = $agroupinfo;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_meetme_list($disable=null)
	{
		$meetme = &$this->get_module('meetme');
		$mfeatures = &$this->get_module('meetmefeatures');

		if(($rs = $meetme->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['meetme'] = $rs[$i];

			if(($info['mfeatures'] = $mfeatures->get_where(array('meetmeid' => $info['meetme']['id']))) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_user_queue_info($protocols=array(),$type='',$disable=null)
	{
		$protocols = (array) $protocols;
		
		if(empty($protocols) === true)
			$protocols = array_keys($this->_conf['protocol']);
		else
			$protocols = array_values($protocols);

		$type = (string) $type;

		if($type !== XIVO_SRE_IPBX_AST_CHAN_LOCAL
		|| $this->get_channel($type) !== XIVO_SRE_IPBX_AST_CHAN_LOCAL)
			$type = '';

		$ufeatures = &$this->get_module('userfeatures');

		if(($rs = $ufeatures->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['ufeatures'] = $rs[$i];

			if(in_array($info['ufeatures']['protocol'],$protocols) === false
			|| ($protocol = &$this->get_protocol_module($info['ufeatures']['protocol'])) === false
			|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'],$disable)) === false)
				continue;

			if($info['protocol']['context'] === '')
				$info['protocol']['context'] = 'default';
			
			if($type === '')
			{
				$interface = $this->mk_interface($info['protocol']['name'],
								 $info['ufeatures']['protocol']);
			}
			else
			{
				$interface = $this->mk_interface($info['protocol']['name'],
								 $info['ufeatures']['protocol'],
								 $info['ufeatures']['number'],
								 $info['protocol']['context']);
			}

			$channel = $this->get_channel_by_protocol($info['ufeatures']['protocol']);

			$r[$info['ufeatures']['id']] = array('interface' => $interface,
							     'identity' => $info['ufeatures']['identity'],
							     'channel' => $channel);
		}
			
		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_agent_queue_info($disable=null)
	{
		$afeatures = &$this->get_module('agentfeatures');
		$agent = &$this->get_module('agent');

		if(($rs = $afeatures->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['afeatures'] = $rs[$i];

			if(($info['agent'] = $agent->get_agent($info['afeatures']['agentid'],$disable)) === false)
				continue;

			$interface = $this->mk_agent_interface($info['afeatures']['number']);

			$r[$info['afeatures']['id']] = array('interface' => $interface,
							     'identity' => $info['afeatures']['identity'],
							     'channel' => XIVO_SRE_IPBX_AST_CHAN_AGENT);
		}
			
		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_agent_group_queue_info($disable=null)
	{
		$agroup = &$this->get_module('agentgroup');
		$agent = &$this->get_module('agent');

		if(($rs = $agroup->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['agroup'] = $rs[$i];

			if(($info['agent'] = $agent->get($info['agroup']['groupid'],$disable)) === false)
				continue;

			$interface = $this->mk_agent_interface($info['agroup']['id'],true);

			$r[$info['agroup']['id']] = array('interface' => $interface,
							  'identity' => $info['agroup']['name'],
							  'channel' => XIVO_SRE_IPBX_AST_CHAN_AGENT);
		}
			
		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_incall_list($disable=null)
	{
		$incall = &$this->get_module('incall');
		$extenumbers = &$this->get_module('extenumbers');

		if(($rs = $incall->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['incall'] = $rs[$i];
			$info['type'] = false;

			if(($info['extenumbers'] = $extenumbers->get($info['incall']['extenumid'])) === false
			|| ($info['incall']['linked'] === true
			   && ($info['type'] = $incall->is_valid($info['incall']['type'],
								 $info['incall']['typeval'])) === false) === true)
				continue;

			if($info['type'] === true)
				$info['type'] = false;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_incall_search($exten,$disable=null)
	{
		$incall = &$this->get_module('incall');
		$extenumbers = &$this->get_module('extenumbers');

		if(($rs = $extenumbers->get_search($exten)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['extenumbers'] = $rs[$i];
			$info['type'] = false;

			$incall_where = array('extenumid' => $info['extenumbers']['id']);

			if(($info['incall'] = $incall->get_where($incall_where)) === false
			|| ($info['incall']['linked'] === true
			   && ($info['type'] = $incall->is_valid($info['incall']['type'],
								 $info['incall']['typeval'])) === false) === true)
				continue;

			if($info['type'] === true)
				$info['type'] = false;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_generaloutcall_list($type=null,$disable=null)
	{
		$generaloutcall = &$this->get_module('generaloutcall');
		$extenumbers = &$this->get_module('extenumbers');

		if($type !== null
		&& ($elementype = $generaloutcall->get_element_value('type')) !== false
		&& in_array($type,$elementype,true) !== false)
			$rs = $generaloutcall->get_all_where(array('type' => $type),$disable);
		else
			$rs = $generaloutcall->get_all($disable);

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['generaloutcall'] = $rs[$i];

			if(($info['extenumbers'] = $extenumbers->get($info['generaloutcall']['extenumid'])) === false)
				continue;

			$r[$info['generaloutcall']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_outcall_list($disable=null,$assoc=false)
	{
		$assoc = (bool) $assoc;

		$outcall = &$this->get_module('outcall');
		$extenumbers = &$this->get_module('extenumbers');
		$tfeatures = &$this->get_module('trunkfeatures');

		if(($rs = $outcall->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = $trunks = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['outcall'] = $rs[$i];
			$info['tfeatures'] = $info['trunk'] = false;

			if(($info['extenumbers'] = $extenumbers->get($info['outcall']['extenumid'])) === false)
				continue;

			if($info['outcall']['linked'] === false)
			{
				$r[$info['outcall']['id']] = $info;
				continue;
			}

			if(($info['tfeatures'] = $tfeatures->get($info['outcall']['trunkfeaturesid'])) === false)
				continue;

			if(isset($trunks[$info['tfeatures']['trunk']]) === false)
				$trunks[$info['tfeatures']['trunk']] = &$this->get_module('trunk'.$info['tfeatures']['trunk']);

			$trunkmodule = &$trunks[$info['tfeatures']['trunk']];

			if($trunkmodule === false
			   || ($info['trunk'] = $trunkmodule->get($info['tfeatures']['trunkid'],$disable)) === false)
				continue;

			$r[$info['outcall']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_schedule_list($disable=null)
	{
		$schedule = &$this->get_module('schedule');

		if(($rs = $schedule->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['schedule'] = $rs[$i];
			$info['typetrue'] = $info['typefalse'] = false;

			if($info['schedule']['linked'] === true
			&& (($info['typetrue'] = $schedule->is_valid($info['schedule']['typetrue'],
								     $info['schedule']['typevaltrue'],
								     $info['schedule']['id'])) === false
			|| ($info['typefalse'] = $schedule->is_valid($info['schedule']['typefalse'],
								     $info['schedule']['typevalfalse'],
								     $info['schedule']['id'])) === false) === true)
				continue;

			if($info['typetrue'] === true)
				$info['typetrue'] = false;

			if($info['typefalse'] === true)
				$info['typefalse'] = false;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_rightcall_list($disable=null)
	{
		$rightcall = &$this->get_module('rightcall');
		$rightcallmember = &$this->get_module('rightcallmember');
		$rightcallexten = &$this->get_module('rightcallexten');

		if(($rs = $rightcall->get_all($disable)) === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['rightcall'] = $rs[$i];

			$where = array('rightcallid' => $info['rightcall']['id']);

			$info['rightcallmember'] = $rightcallmember->get_where($where);
			$info['rightcallexten'] = $rightcallexten->get_where($where);

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			return(false);

		return($r);
	}

	function get_protocol()
	{
		return($this->_conf['protocol']);
	}

	function chk_protocol($name)
	{
		$name = strval($name);

		return($this->_chk_exists_cfg('protocol',$name));
	}

	function get_protocol_name($name)
	{
		$r = false;

		$name = strval($name);

		if($this->chk_protocol($name) === true)
			$r = xivo_ak('name',$this->_conf['protocol'][$name],true);

		return($r);
	}

	function get_protocol_by_protocol_name($name)
	{
		$r = false;

		$name = strval($name);

		if(($arr = xivo_get_aks($this->_conf['protocol'])) === false)
			return($r);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$v = &$this->_conf['protocol'][$k];

			if(isset($v['name']) === true && $v['name'] === $name)
			{
				$r = $k;
				break;
			}
		}

		return($r);
	}

	function get_protocol_module_name($name)
	{
		$r = false;

		if($this->chk_protocol($name) === true)
			$r = xivo_ak('module',$this->_conf['protocol'][$name],true);

		return($r);
	}

	function &get_protocol_module($name)
	{
		$r = false;

		if(($module = $this->get_protocol_module_name($name)) !== false)
			$r = &$this->get_module($module);

		return(($ref = &$r));
	}

	function get_element($modules=array())
	{
		$r = false;

		$modules = (array) $modules;
		
		if(isset($modules[0]) === false)
			$modules = array_keys($this->_conf['module']);
			
		if(($arr = xivo_get_aks($modules)) === false)
			return($r);

		$r = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = $arr['keys'][$i];
			$v = &$modules[$arr['keys'][$i]];

			if(($module = &$this->get_module($v)) === false || ($list = $module->get_element()) === false)
				continue;
			
			$r[$v] = &$list;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_protocol_element($protocols=array(),$module=false)
	{
		$r = false;

		$protocols = (array) $protocols;
		$module = (bool) $module;

		if(isset($protocols[0]) === false)
			$protocols = $this->_conf['protocol'];			

		if(($arr = xivo_get_aks($protocols)) === false)
			return($r);

		$r = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = $arr['keys'][$i];
			$v = &$protocols[$k];

			if(isset($v['module']) === false || ($element = $this->get_element($v['module'])) === false)
				continue;

			if($module === true)
				$r[$k] = $element;
			else
			{
				if(($mods = xivo_get_aks($element)) === false)
					continue;

				for($j = 0;$j < $mods['cnt'];$j++)
					$r[$k] = $element[$mods['keys'][$j]];
			}

		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function chk_channel($channel)
	{
		$channel = strtolower(strval($channel));

		return($this->_chk_exists_cfg('channel',$channel));
	}

	function chk_channel_by_protocol($protocol)
	{
		$r = false;

		$protocol = strtolower(strval($protocol));

		if($this->chk_protocol($protocol) === true && $this->chk_channel($protocol) === true)
			$r = true;

		return($r);
	}

	function get_channel_by_protocol($protocol)
	{
		$r = false;

		$protocol = strtolower(strval($protocol));

		if($this->chk_channel_by_protocol($protocol) === true)
			$r = $this->_conf['channel'][$protocol];

		return($r);
	}

	function chk_channel_by_interface($interface)
	{
		$interface = strtolower(strval($interface));

		if(($pos = strpos($interface,'/')) !== false)
			$interface = substr($interface,0,$pos);

		if(xivo_issa('channel',$this->_conf) === false
		|| in_array($interface,$this->_conf['channel']) === false)
			return(false);

		return($interface);
	}

	function get_channel($channel)
	{
		$r = false;

		$channel = strtolower(strval($channel));

		if($this->chk_channel($channel) === true)
			$r = $this->_conf['channel'][$channel];

		return($r);
	}

	function mk_agent_interface($number,$group=false)
	{
		$number = strval($number);
		$group = (bool) $group;

		if(ctype_digit($number) === false)
			return(false);

		if($group === true)
			$number = ':'.$number;

		$r = $this->mk_interface($number,null,null,null,XIVO_SRE_IPBX_AST_CHAN_AGENT);

		return($r);
	}

	function mk_interface($name,$protocol='',$number='',$context='',$channel='')
	{
		$r = false;

		$name = strval($name);
		$protocol = strval($protocol);
		$number = strval($number);
		$context = strtolower(strval($context));
		$channel = strval($channel);

		if($number !== '')
		{
			$context = $context === '' ? 'default' : $context;

			if(($channel = $this->get_channel(XIVO_SRE_IPBX_AST_CHAN_LOCAL)) !== false)
				$r = $channel.'/'.$number.'@'.$context;

			return($r);
		}

		$chan = $channel;

		if($channel !== '')
		{
			if(($channel = $this->chk_channel_by_interface($chan)) === false
			&& ($channel = $this->get_channel($chan)) === false)
				return($r);
		}
		else if(($channel = $this->get_channel_by_protocol($protocol)) === false)
			return($r);

		$r = $channel.'/'.$name;

		return($r);
	}

	function clean_extension($str)
	{
		$str = preg_replace('/\[(\d)\]/','\\1',strval($str));

		if(isset($str{0}) === true && $str{0} === '_')
		{
			$substr = substr($str,1);

			if(preg_match('/[NXZ\-\[\]\!\.]/',$substr) === 0)
				$str = $substr;

			return($str);
		}
		else if(preg_match('/[NXZ\-\[\]\!\.]/',$str) !== 0)
			$str = '_'.$str;

		return($str);
	}

	function chk_extension($str)
	{
		$str = strval($str);

		if(isset($str{0}) === false)
			return(false);

		if(preg_match('/[\[\-\]]/',$str) === 0)
			return($this->_realexten_pattern($str));

		$split = preg_split('/\[(\d+(?:\-\d+)?)\]/',$str,-1,PREG_SPLIT_DELIM_CAPTURE);

		if(($nb = count($split)) === 0)
			return($this->_realexten_pattern($str));

		$r = '';

		for($i = 0;$i < $nb;$i+=2)
		{
			if(preg_match('/[\[\-\]]/',$split[$i]) === 1)
				return(false);
			else
				$r .= $this->_realexten_pattern($split[$i]);

			if(isset($split[$i+1]) === false)
				return($r);

			$nsplit = $split[$i+1];

			$arr = array();

			if(isset($nsplit{1}) === false)
			{
				$r .= $nsplit{0};
				continue;
			}

			if(preg_match('/^(\d*)(\d-\d)(\d*)$/',$nsplit,$match) === 1)
			{
				if($match[2]{0} >= $match[2]{2})
					return(false);

				$arr = array_flip(range($match[2]{0},$match[2]{2}));

				if(($len = strlen($match[1])) !== 0)
				{
					for($j = 0;$j < $len;$j++)
					{
						$num = $match[1]{$j};

						if(isset($arr[$num]) === true)
							return(false);

						$arr[$num] = 1;
					}
				}

				if(($len = strlen($match[3])) !== 0)
				{
					for($j = 0;$j < $len;$j++)
					{
						$num = $match[3]{$j};

						if(isset($arr[$num]) === true)
							return(false);

						$arr[$num] = 1;
					}
				}
			}
			else
			{
				$len = strlen($nsplit);

				for($j = 0;$j < $len;$j++)
				{
					$num = $nsplit{$j};

					if(isset($arr[$num]) === true)
						return(false);

					$arr[$num] = 1;
				}
			}
	
			ksort($arr);

			$r .= '['.implode('',array_keys($arr)).']';
		}

		return($r);
	}

	function _realexten_pattern($str)
	{
		$str = preg_replace('/\[(\d)\]/','\\1',(string) $str);
		$pat = array('_','X','Z','N');
		$rep = array('','[0123456789]','[123456879]','[23456789]');

		return(str_replace($pat,$rep,$str));
	}

	function _regexten_pattern($str)
	{
		$pat = array('_','*','X','Z','N','.','!');
		$rep = array('','\*','[0-9]','[1-9]','[2-9]','[0-9#\*]+','[0-9#\*]*');

		return(str_replace($pat,$rep,(string) $str));
	}

	function mk_callerid($number,$callerid='')
	{
		$r = '';

		if(preg_match('/^[0-9\*#]+$/',$number) !== 1)
			return($r);

		$r = preg_replace('/<'.preg_quote($number).'>$/','',$callerid);
		$r = trim(str_replace(array('<','>','"'),'',$r)).' <'.$number.'>';

		return(trim($r));
	}
}

?>
