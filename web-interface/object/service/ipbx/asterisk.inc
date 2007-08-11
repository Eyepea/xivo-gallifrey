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
				$label = (string) $infos[0];
				$sess_sre['infos']['label'] = $label;
			}

			if(isset($infos[1]) === true)
			{
				$version = (string) $infos[1];
				$sess_sre['infos']['version'] = $version;
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

	function get_users_search($name,$protocols=array(),$disable=null)
	{
		return($this->_get_users('search',$protocols,$disable,$name));
	}

	function get_users_list($protocols=array(),$disable=null)
	{
		return($this->_get_users('list',$protocols,$disable));
	}

	function get_users_context($name,$protocols=array(),$disable=null)
	{
		$name = strval($name);

		if($name === '#main')
			$name = '';

		return($this->_get_users('context',$protocols,$disable,$name));
	}

	function _get_users($action,$protocols=array(),$disable=null,$opts=array())
	{
		$action = (string) $action;
		$protocols = (array) $protocols;
		
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

			$info['sort'] = array();
			$info['sort']['name'] = trim($info['ufeatures']['firstname'].' '.$info['ufeatures']['lastname']);

			if($info['sort']['name'] === '')
				$info['sort']['name'] = '-';

			$r[] = $info;
		}
			
		if(isset($r[0]) === false)
			$r = false;

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

	function get_groups_list($disable=null)
	{
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
			|| ($info['nb_qmember'] = $qmember->get_nb_by_name($info['queue']['name'],$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_queues_list($disable=null)
	{
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
			|| ($info['nb_qmember'] = $qmember->get_nb_by_name($info['queue']['name'],$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

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
			$info['sort']['name'] = trim($info['afeatures']['firstname'].' '.$info['afeatures']['lastname']);

			if($info['sort']['name'] === '')
				$info['sort']['name'] = '-';

			$info['sort']['fullname-number'] = $info['sort']['name'].' ('.$info['afeatures']['number'].')';
			$info['sort']['id'] = $info['afeatures']['id'];
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
			|| ($info['nb_agent'] = $afeatures->get_nb_by_group($info['agroup']['id'],$disable)) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_queue_user($id=0,$disable=null)
	{
		return($this->_get_queue_member('user',$id,$disable));
	}

	function get_queue_agent($id=0,$disable=null)
	{
		return($this->_get_queue_member('agent',$id,$disable));
	}

	function _get_queue_member($usertype='',$userid=0,$disable=null)
	{
		$usertype = (string) $usertype;
		$userid = xivo_uint($userid);
	
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

					$r[] = $info;
					continue;
				}
			}

			$info['member'] = $qmember->get_all_where($where);

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_group_user($userid=0,$disable=null)
	{
		$userid = xivo_uint($userid);
	
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

				$r[] = $info;
				continue;
			}

			$info['member'] = $qmember->get_all_where($where);

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

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

			if(($info['mfeatures'] = $mfeatures->get(array('meetmeid' => $info['meetme']['id']))) === false)
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
				$info['protocol']['context'] = 'local-extensions';
			
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

			$name = trim(trim($info['ufeatures']['firstname']).' '.trim($info['ufeatures']['lastname']));
			$channel = $this->get_channel_by_protocol($info['ufeatures']['protocol']);

			if($name === '')
				$name = '- ('.$channel.'/'.$info['protocol']['name'].')';

			$r[$info['ufeatures']['id']] = array('interface' => $interface,
							     'fullname' => $name,
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

			$name = trim(trim($info['afeatures']['firstname']).' '.trim($info['afeatures']['lastname']));

			if($name === '')
				$name = '-';

			$name .= ' ('.$info['afeatures']['number'].')';

			$r[$info['afeatures']['id']] = array('interface' => $interface,
							     'fullname' => $name,
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

			$name = trim($info['agroup']['name']);

			if($name === '')
				$name = '-';

			$r[$info['agroup']['id']] = array('interface' => $interface,
							  'fullname' => $name,
							  'channel' => XIVO_SRE_IPBX_AST_CHAN_AGENT);
		}
			
		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_did_list($disable=null)
	{
		$r = false;

		$extensions = &$this->get_module('extensions');
		$dfeatures = &$this->get_module('didfeatures');
		$extenumbers = &$this->get_module('extenumbers');

		$where = array();
		$where['context'] = 'did-extensions';
		$where['app'] = 'Macro';
		$where['appdata'] = 'superdid';

		if(($rs = $extensions->get_all_where($where,$disable)) === false || ($nb = count($rs)) === 0)
			return($r);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['did'] = $rs[$i];
			$info['tyfeatures'] = false;

			if(($info['dfeatures'] = $dfeatures->get(array('extenid' => $info['did']['id']))) === false
			|| ($info['extenumbers'] = $extenumbers->get(array(
									'number' => $info['did']['exten'],
									'context' => $info['did']['context']))) === false
			|| ($info['dfeatures']['type'] !== 'custom' && $info['dfeatures']['commented'] === false
			   && (($tyfeatures = &$this->get_module($info['dfeatures']['type'].'features')) === false
			   || ($info['tyfeatures'] = $tyfeatures->get($info['dfeatures']['typeid'])) === false
			   || xivo_empty($info['tyfeatures']['number']) === true) === true) === true)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_protocol()
	{
		return($this->_conf['protocol']);
	}

	function chk_protocol($name)
	{
		$name = (string) $name;

		return($this->_chk_exists_cfg('protocol',$name));
	}

	function get_protocol_name($name)
	{
		$r = false;

		$name = (string) $name;

		if($this->chk_protocol($name) === true)
			$r = xivo_ak('name',$this->_conf['protocol'][$name],true);

		return($r);
	}

	function get_protocol_by_protocol_name($name)
	{
		$r = false;

		$name = (string) $name;

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

		$name = (string) $name;

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
		$channel = strtolower((string) $channel);

		return($this->_chk_exists_cfg('channel',$channel));
	}

	function chk_channel_by_protocol($protocol)
	{
		$r = false;

		$protocol = strtolower((string) $protocol);

		if($this->chk_protocol($protocol) === true && $this->chk_channel($protocol) === true)
			$r = true;

		return($r);
	}

	function get_channel_by_protocol($protocol)
	{
		$r = false;

		$protocol = strtolower((string) $protocol);

		if($this->chk_channel_by_protocol($protocol) === true)
			$r = $this->_conf['channel'][$protocol];

		return($r);
	}

	function chk_channel_by_interface($interface)
	{
		$interface = strtolower((string) $interface);

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

		$channel = strtolower((string) $channel);

		if($this->chk_channel($channel) === true)
			$r = $this->_conf['channel'][$channel];

		return($r);
	}

	function mk_agent_interface($number,$group=false)
	{
		$number = (string) $number;
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

		$name = (string) $name;
		$protocol = (string) $protocol;
		$number = (string) $number;
		$context = (string) $context;
		$channel = (string) $channel;

		if($number !== '')
		{
			$context = $context === '' ? 'local-extensions' : $context;

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
}

?>
