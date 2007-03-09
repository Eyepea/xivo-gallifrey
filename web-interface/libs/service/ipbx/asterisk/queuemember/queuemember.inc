<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_queuemember extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queuemember';

	function xivo_service_asterisk_queuemember(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;
	}

	function get($ids,$disable=null)
	{
		if(is_array($ids) === false || isset($ids['queue_name'],$ids['interface']) === false)
			return(false);

		return($this->_dso->get($ids,$disable));
	}

	function get_list_by_interface($interface,$disable=null)
	{
		$r = false;

		$interface = (string) $interface;

		if(($list = $this->_dso->get_list_by_interface($interface)) === false
		|| isset($list[0]) === false)
			return($r);

		$r = array();
	
		$nb = count($list);

		for($i = 0;$i < $nb;$i++)
		{
			if(($rs = $this->get($list[$i],$disable)) === false)
				continue;

			$r[] = $rs;
		}

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function get_nb_by_name($name,$disable=null)
	{
		$name = (string) $name;

		return($this->_dso->get_nb_by_name($name,$disable));
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();
		$r['queue_name'] = $arr['queue_name'];
		$r['interface'] = $arr['interface'];
		$r['call_limit'] = xivo_uint($arr['call_limit']);

		return($this->_dso->add($r));
	}

	function add_list($arr)
	{
		$r = false;

		if(is_array($arr) === false)
			return($r);

		$values = array_values($arr);

		if(($nb = count($values)) === 0)
			return($r);

		for($i = 0;$i < $nb;$i++)
			$this->add($arr[$i]);

		return(true);
	}

	function edit($ids,$arr)
	{
		if(is_array($ids) === false || is_array($arr) === false || isset($ids['queue_name'],$ids['interface']) === false)
			return(false);

		$r = array();
		$r['queue_name'] = $arr['queue_name'];
		$r['interface'] = $arr['interface'];
		$r['call_limit'] = xivo_uint($arr['call_limit']);
		$r['commented'] = 0;

		return($this->_dso->edit($ids,$r));
	}

	function update_by_name($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(isset($arr['queue_name']) === true)
			$r['queue_name'] = $arr['queue_name'];

		if(isset($arr['interface']) === true)
			$r['interface'] = $arr['interface'];

		if(isset($arr['call_limit']) === true)
			$r['call_limit'] = xivo_uint($arr['call_limit']);

		return($this->_dso->update_by_name($id,$r));
	}

	function delete($ids)
	{
		if(is_array($ids) === false || isset($ids['queue_name'],$ids['interface']) === false)
			return(false);

		return($this->_dso->delete($ids));
	}

	function delete_by_name($name)
	{
		$name = (string) $name;

		return($this->_dso->delete_by_name($name));
	}

	function delete_by_interface($interface)
	{
		$interface = (string) $interface;

		return($this->_dso->delete_by_interface($interface));
	}
}

?>
