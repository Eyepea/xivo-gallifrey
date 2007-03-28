<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'abstract.inc');

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

	function _get_users($action,$protocols=array(),$disable=null,$opts=array())
	{
		$r = false;

		$action = (string) $action;
		$protocols = (array) $protocols;
		
		if(empty($protocols) === true)
			$protocols = array_keys($this->_conf['protocol']);
		else
			$protocols = array_values($protocols);

		$ufeatures = &$this->get_module('userfeatures');

		$r = $info = array();

		switch($action)
		{
			case 'search':
				$rs = $ufeatures->get_search($opts);
				break;
			case 'list':
			default:
				$rs = $ufeatures->get_list();
		}

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['ufeatures'] = $ufeatures->get($rs[$i])) === false
			|| in_array($info['ufeatures']['protocol'],$protocols) === false
			|| ($protocol = &$this->get_protocol_module($info['ufeatures']['protocol'])) === false
			|| ($info['protocol'] = $protocol->get($info['ufeatures']['protocolid'],$disable)) === false)
				continue;

			$r[] = $info;
		}
			
		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_groups_list($disable=null)
	{
		$r = false;

		$gfeatures = &$this->get_module('groupfeatures');
		$queue = &$this->get_module('queue');
		$qmember = &$this->get_module('queuemember');

		if(($rs = $gfeatures->get_list($disable)) === false || ($nb = count($rs)) === 0)
			return($r);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['gfeatures'] = $gfeatures->get($rs[$i],$disable)) === false
			|| ($info['queue'] = $queue->get($info['gfeatures']['name'],$disable)) === false
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
		$r = false;

		$meetme = &$this->get_module('meetme');
		$mfeatures = &$this->get_module('meetmefeatures');

		if(($rs = $meetme->get_list($disable)) === false || ($nb = count($rs)) === 0)
			return($r);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($info['meetme'] = $meetme->get($rs[$i],$disable)) === false
			|| ($info['mfeatures'] = $mfeatures->get_by_meetme($info['meetme']['id'])) === false)
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
/*
	function get_user_by_provisioning($id)
	{
		$r = false;

		if(($id = xivo_uint($id)) === 0 || ($arr = xivo_get_aks($this->_conf['provisioning_id'])) === false)
			return($r);

		$prefix = substr($id,0,1);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$k = &$arr['keys'][$i];
			$v = &$this->_conf['provisioning_id'][$k];

			if(isset($v['prefix'],$v['range']) === false || $v['prefix'] !== $prefix)
				continue;

			$min = $v['prefix'] * $v['range'];
			$max = $min + $v['range'];

			if($id < $min || $id > $max || $this->chk_protocol($v) === false || ($mod = &$this->get_module($k)) === false)
				return($r);

			$r = $mod->get_by_provisioning($id);
		}

		return($r);
	}
*/

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

	function chk_interface($name)
	{
		return($this->_chk_exists_cfg('interface',$name));
	}

	function mk_interface($name,$number)
	{
		$r = false;

		if($this->chk_interface($name) === false && ($number = xivo_uint($number)) !== 0)
			return($r);

		$r = $this->_conf['interface'][$name].'/'.$number;

		return($r);
	}
}

?>
