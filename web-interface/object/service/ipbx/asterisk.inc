<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk extends xivo_service_abstract
{
	var $_ini		= array();
	var $_type		= 'ipbx';
	var $_name		= 'asterisk';
	var $_realstatic	= array();
	var $_apprealstatic	= array();
	var $_discuss		= null;

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

	function discuss($cmd,$recv=false)
	{
		if($this->_discuss === false
		|| (is_object($this->_discuss) === false
		   && ($this->_discuss = &$this->get_module('ami')) === false) === true)
		{
			$this->_discuss = false;
			return(false);
		}

		return($this->_discuss->cmd($cmd,$recv));
	}

	function _load_realstatic_class($name)
	{
		$name = (string) $name;

		$file = $name.'.inc';

		$dir = array(XIVO_PATH_OBJECT,'service',$this->_type,$this->_name);
		$class = 'xivo_service_'.$this->_name.'_realstatic';

		$dirapi = $dir;
		$dirapi[] = $name;
		$classapi = $class.'_'.$name;
		
		if(xivo_file::load_file($file,$dirapi,true) !== false && class_exists($classapi) === true)
			return($classapi);

		$file = 'realstatic.inc';
		$dir[] = 'realstatic';

		if(xivo_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function chk_realstatic_enable($name)
	{
		return($this->_chk_exists_cfg('realstatic',$name));
	}

	function _load_realstatic($name,$param=null)
	{
		$name = (string) $name;

		if($this->_chk_exists_cfg('realstatic',$name) === false)
		{
			trigger_error('Invalid realtime static in '.$name.' '.$this->_name,E_USER_ERROR);
			return(false);
		}

		$ref = &$this->_conf['realstatic'][$name];

		if(isset($this->_realstatic[$name]) === true)
		{
			if(is_array($param) === true)
				$this->_realstatic[$name]->load_init($param);

			return(true);
		}
		else if($this->chk_realstatic_enable($name) === false)
			return(false);
		else if(($class = $this->_load_realstatic_class($name)) === false)
		{
			trigger_error('Realtime Static '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}
		else if(isset($ref['datastorage']) === false || (bool) $ref['datastorage'] === false)
			$this->_realstatic[$name] = new $class($this,$param);
		else if(is_string($ref['datastorage']) === true && $this->chk_realstatic_enable($ref['datastorage']) === true)
			$this->_realstatic[$name] = new $class($this,$this->_load_dso($ref['datastorage']),$param);
		else
			$this->_realstatic[$name] = new $class($this,$this->_load_dso($name),$param);

		return(true);
	}

	function &get_realstatic($name,$param=null)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_realstatic($name,$param) === true)
			$r = &$this->_realstatic[$name];

		return(($ref = &$r));
	}

	function _load_apprealstatic_class($application)
	{
		$application = (string) $application;

		$file = $application.'.inc';

		$dir = array(XIVO_PATH_OBJECT,'application','service',$this->_type,$this->_name,'realstatic',$application);

		$class = 'xivo_application_service_'.$this->_name.'_realstatic_'.$application;

		if(xivo_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function &call_apprealstatic($name,$func,$args=null)
	{
		$name = (string) $name;
		$func = (string) $func;

		if($this->_load_apprealstatic($name) === false)
			$r = false;
		else if($args === null)
			$r = call_user_func(array(&$this->_apprealstatic[$name],$func));
		else
		{
			$args = (array) $args;
			$r = call_user_func_array(array(&$this->_apprealstatic[$name],$func),$args);
		}

		return(($ref = &$r));
	}

	function &get_apprealstatic($name)
	{
		$r = false;

		$name = (string) $name;

		if($this->_load_apprealstatic($name) === true)
			$r = &$this->_apprealstatic[$name];

		return(($ref = &$r));
	}

	function _load_apprealstatic($name)
	{
		$name = (string) $name;

		if(isset($this->_apprealstatic[$name]) === true)
			return(true);
		else if(($class = $this->_load_apprealstatic_class($name)) === false)
		{
			trigger_error('RealStatic Application '.$name.' does not exist in '.$this->_name,E_USER_ERROR);
			return(false);
		}

		$this->_apprealstatic[$name] = new $class($this);

		return(true);
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

		if(xivo_issa('info',$sess_sre) === true
		&& isset($sess_sre['info']['label'],$sess_sre['info']['version']) === true)
		{
			$label = (string) $sess_sre['info']['label'];
			$version = (string) $sess_sre['info']['version'];
		}
		else if(($nversion = $this->_get_service_info_from_discuss()) !== false)
		{
			if(isset($nversion['label']) === true)
				$label = $sess_sre['info']['label'] = $nversion['label'];

			if(isset($nversion['version']) === true)
				$version = $sess_sre['info']['version'] = $nversion['version'];
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

	function _get_service_info_from_discuss()
	{
		if((defined('XIVO_SESS_ENABLE') === true
		&& (bool) XIVO_SESS_ENABLE === false) === true
		|| ($info = $this->discuss('show version',true)) === false
		|| is_array($info) === false
		|| isset($info[0]) === false)
			return(false);

		$info = explode(' ',$info[0],3);

		$r = array();

		if(isset($info[0]) === true)
		{
			$r['label'] = $info[0];

			if(isset($info[1]) === true)
				$r['version'] = $info[1];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_trunks_list($disable=null,$trunk=true,$assoc=true)
	{
		$trunk = (bool) $trunk;

		$tfeatures = &$this->get_module('trunkfeatures');

		if(($rs = $tfeatures->get_all(null,true,array('trunk' => SORT_ASC))) === false
		|| ($nb = count($rs)) === 0)
			return(false);

		$r = $info = $trunks = array();

		xivo::load_class('xivo_sort');
		$sort = new xivo_sort(array('browse' => 'trunk','key' => 'identity'));

		for($i = 0;$i < $nb;$i++)
		{
			$info['tfeatures'] = $rs[$i];

			if(isset($trunks[$info['tfeatures']['trunk']]) === false)
				$trunks[$info['tfeatures']['trunk']] = &$this->get_module('trunk'.$info['tfeatures']['trunk']);

			$trunkmodule = &$trunks[$info['tfeatures']['trunk']];

			if($trunkmodule === false)
				continue;
			else if($trunk === true)
			{
				if(isset($r[$info['tfeatures']['trunk']]) === false)
					$r[$info['tfeatures']['trunk']] = array();

				$ret = &$r[$info['tfeatures']['trunk']];
			}

			if(($info['trunk'] = $trunkmodule->get($info['tfeatures']['trunkid'],$disable)) === false)
			{
				if($trunk === true && empty($ret) === true)
					unset($r[$info['tfeatures']['trunk']]);
				continue;
			}

			$info['trunk']['trunkfeaturesid'] = $info['tfeatures']['id'];

			if($trunk === true)
			{
				$ret[$info['tfeatures']['id']] = $info;
				usort($ret,array(&$sort,'str_usort'));
			}
			else
			{
				$r[$info['tfeatures']['id']] = $info;
				uasort($r,array(&$sort,'str_usort'));
			}
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_agents_list($group=0,$disable=null,$assoc=false)
	{
		$group = xivo_ulongint($group);

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

			$r[$info['afeatures']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_agent_groups_list($disable=null,$assoc=false)
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

			$r[$info['agroup']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

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
		$userid = xivo_ulongint($userid);
	
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
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_group_agent($userid=0,$disable=null)
	{
		$userid = xivo_ulongint($userid);
	
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

	function get_protocol()
	{
		return($this->_conf['protocol']);
	}

	function chk_protocol($name)
	{
		$name = strval($name);

		return($this->_chk_exists_cfg('protocol',strval($name)));
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
		$modules = (array) $modules;
		
		if(isset($modules[0]) === false)
			$modules = array_keys($this->_conf['module']);
			
		if(empty($modules) === true)
			return(false);

		$r = array();

		foreach($modules as $v)
		{
			if(($module = &$this->get_module($v)) !== false && ($list = $module->get_element()) !== false)
				$r[$v] = &$list;
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_protocol_element($protocols=array(),$module=false)
	{
		$protocols = (array) $protocols;
		$module = (bool) $module;

		if(isset($protocols[0]) === false)
			$protocols = $this->_conf['protocol'];			

		if(empty($protocols) === true)
			return(false);

		$r = array();

		foreach($protocols as $k => $v)
		{
			if(isset($v['module']) === false || ($element = $this->get_element($v['module'])) === false)
				continue;
			else if($module === true)
				$r[$k] = $element;
			else if(is_array($element) === true)
			{
				foreach($element as $value)
					$r[$k] = $value;
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

		if(ctype_digit($number) === false)
			return(false);
		else if((bool) $group === true)
			$number = ':'.$number;

		return($this->mk_interface($number,null,null,null,XIVO_SRE_IPBX_AST_CHAN_AGENT));
	}

	function mk_interface($name,$protocol='',$number='',$context='',$channel='')
	{
		$r = false;

		$name = strval($name);
		$protocol = strval($protocol);
		$number = strval($number);
		$context = strtolower(strval($context));
		$channel = strval($channel);

		if($protocol === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
			return($name);

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

	function display_extension($str)
	{
		if(isset($str{0}) === true && $str{0} === '_')
			$str = substr($str,1);

		return($str);
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
		else if(preg_match('/[\[\-\]]/',$str) === 0)
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
		$str = preg_replace('/\[(\d)\]/','\\1',$str);
		$pat = array('_','X','Z','N');
		$rep = array('','[0123456789]','[123456879]','[23456789]');

		return(str_replace($pat,$rep,$str));
	}

	function _regexten_pattern($str)
	{
		$pat = array('_','*','X','Z','N','.','!');
		$rep = array('','\*','[0-9]','[1-9]','[2-9]','[0-9#\*]+','[0-9#\*]*');

		return(str_replace($pat,$rep,$str));
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

	function get_contexts_list($arr=null)
	{
		$r = array();
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_EXTRAFEATURES;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_FEATURES;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_GENERAL;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_GLOBAL;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_GLOBALS;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_HINT;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_HINTS;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_INITCONFIG;
		$r[] = XIVO_SRE_IPBX_AST_CONTEXT_PARKEDCALLS;

		$r[] = XIVO_SRE_IPBX_AST_INCALL_CONTEXT;
		$r[] = XIVO_SRE_IPBX_AST_HANDYNUMBERS_CONTEXT;

		if(is_array($arr) === true)
			$r = array_merge($arr,$r);

		return($r);
	}

	function chk_context($context,$arr=null)
	{
		if(is_scalar($context) === false
		|| in_array($context,xivo_service_asterisk::get_contexts_list($arr)) === true
		|| preg_match('/^(?!macro-)[a-z0-9_-]{1,39}$/',$context) !== 1)
			return(false);

		return($context);
	}

	function chk_passwd_context($args,$arr=null)
	{
		if(is_scalar($args) === false
		|| preg_match('/^([0-9]{4,6}|no-password)\|((?!macro-)[a-z0-9_-]{1,39})$/',$args,$match) !== 1
		|| in_array($match[2],xivo_service_asterisk::get_contexts_list($arr)) === true)
			return(false);

		return($match[1].'|'.$match[2]);
	}
}

?>
