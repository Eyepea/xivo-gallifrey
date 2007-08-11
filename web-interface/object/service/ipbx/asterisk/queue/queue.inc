<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_queue extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'queue';
	var $_filter	= null;
	var $_origin	= null;

	function xivo_service_asterisk_queue(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id,$disable=null)
	{
		if(($r = $this->_dso->get($id,$disable)) === false)
			return($r);

		$this->_origin = $r;

		$r['commented'] = (bool) $r['commented'];

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
			return($this->add($this->_origin));
		else
			return($this->edit($this->_origin['name'],$this->_origin));
	}

	function add($arr)
	{
		if(is_array($arr) === false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(is_array($arr) === false)
			return(false);

		if($disable !== null)
			$arr['commented'] = (bool) $disable === false ? 0 : 1;

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		return($this->_dso->delete($id));
	}

	function get_all_where($arr=array(),$disable=null)
	{
		$arr = (array) $arr;

		if(($where = $this->get_elements_where($arr)) === false
		|| ($list = $this->_dso->get_all_where($where,$disable)) === false
		|| isset($list[0]) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $list[$i];

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
