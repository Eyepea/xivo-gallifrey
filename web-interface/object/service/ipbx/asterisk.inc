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
	}

	function add_user($protocol,$infos)
	{
		if(($module = &$this->get_protocol_module($protocol)) === false)
			return(false);

		return($module->add($infos));
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
		$gfeatures = &$this->get_module('queuefeatures');
		$queue = &$this->get_module('queue');
		$qmember = &$this->get_module('queuemember');

		if(($rs = $gfeatures->get_all($disable)) === false || ($nb = count($rs)) === 0)
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

			if(($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
				continue;

			$r[] = $info;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_members_list($protocols=array(),$disable=null)
	{
		$protocols = (array) $protocols;
		
		if(empty($protocols) === true)
			$protocols = array_keys($this->_conf['protocol']);
		else
			$protocols = array_values($protocols);

		$ufeatures = &$this->get_module('userfeatures');

		if(($rs = $ufeatures->get_all()) === false || ($nb = count($rs)) === 0)
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

			$interface = $this->mk_interface($info['protocol']['name'],
							 $info['ufeatures']['protocol'],
							 $info['ufeatures']['number'],
							 $info['protocol']['context']);

			$r[$interface] = trim($info['ufeatures']['firstname'].' '.$info['ufeatures']['lastname']);

			if($r[$interface] === '')
				$r[$interface] = '-';
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

			if(($info['dfeatures'] = $dfeatures->get_by_extenid($info['did']['id'])) === false
			|| ($info['extenumbers'] = $extenumbers->get_by_number_context($info['did']['exten'],$info['did']['context'])) === false
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

	function chk_interface($protocol)
	{
		return($this->_chk_exists_cfg('interface',$protocol));
	}

	function mk_interface($name,$protocol='',$number='',$context='')
	{
		$r = false;

		$name = (string) $name;
		$number = (string) $number;
		$context = (string) $context;

		if($number !== '')
		{
			$context = $context === '' ? 'local-extensions' : $context;

			$r = 'local/'.$number.'@'.$context;

			return($r);
		}

		if($this->chk_interface($protocol) === false)
			return($r);

		$r = $this->_conf['interface'][$protocol].'/'.$name;

		return($r);
	}
}

?>
