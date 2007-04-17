<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_queuemember extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queuemember';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_queuemember(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($ids,$disable=null)
	{
		if(is_array($ids) === false || isset($ids['queue_name'],$ids['interface']) === false
		|| ($r = $this->_dso->get($ids,$disable)) === false)
			return(false);

		$this->_origin = $r;

		return($r);
	}

	function add_origin()
	{
		return($this->_origin('add'));
	}

	function edit_origin()
	{
		return($this->_origin('edit'));
	}

	function _origin($type)
	{
		if(($type !== 'add' && $type !== 'edit') === true || is_array($this->_origin) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($this->_origin,array($this->_origin['queue_name'],$this->_origin['interface']));
		else
			$r = $this->edit(array($this->_origin['queue_name'],$this->_origin['interface']),$this->_origin);

		return($r);
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

		return($this->_dso->add($arr));
	}

	function edit($ids,$arr)
	{
		if(is_array($ids) === false || is_array($arr) === false
		|| isset($ids['queue_name'],$ids['interface']) === false)
			return(false);

		if(isset($arr['commented']) === true)
			$arr['commented'] = (bool) $arr['commented'];

		return($this->_dso->edit($ids,$arr));
	}

	function edit_where($arr,$values)
	{
		$arr = (array) $arr;

		if(is_array($values) === false
		|| ($where = $this->get_elements_where($arr)) === false
		|| ($values = $this->chk_set_values($values,true,true)) === false)
			return(false);

		return($this->_dso->edit_where($where,$values));
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
