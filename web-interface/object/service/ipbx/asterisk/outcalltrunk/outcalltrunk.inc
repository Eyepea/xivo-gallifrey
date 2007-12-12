<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_outcalltrunk extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'outcalltrunk';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_outcalltrunk(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function get($id)
	{
		$this->_origin = false;

		if(is_array($id) === false
		|| isset($id['outcallid'],$id['trunkid']) === false
		|| ($id['outcallid'] = xivo_uint($id['outcallid'])) === 0
		|| ($id['trunkid'] = xivo_uint($id['trunkid'])) === 0
		|| ($r = $this->_dso->get($id)) === false)
			return(false);

		$this->_origin = $r;

		$r = $this->_prepare($r);

		return($r);
	}

	function _origin($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false)
			return(false);

		if($type === 'add')
			$r = $this->add($orig);
		else
			$r = $this->edit(array($orig['outcallid'],$orig['trunkid']),$orig);

		return($r);
	}

	function _origin_list($type,$val=null)
	{
		if($val !== null)
			$orig = $val;
		else
			$orig = $this->_origin_list;

		if(($type !== 'add' && $type !== 'edit') === true
		|| is_array($orig) === false
		|| ($nb = count($orig)) === 0)
			return(false);

		if($type === 'add')
		{
			for($i = 0;$i < $nb;$i++)
				$this->add($orig[$i]);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$this->edit(array($orig[$i]['outcallid'],$orig[$i]['trunkid']),$orig[$i]);
		}

		return(true);
	}

	function add($arr)
	{
		if(is_array($arr) === false
		|| $this->get_id($arr['outcallid'],$arr['trunkid']) !== false)
			return(false);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(is_array($id) === false
		|| is_array($arr) === false
		|| isset($id['outcallid'],$id['trunkid']) === false
		|| ($id['outcallid'] = xivo_uint($id['outcallid'])) === 0
		|| ($id['trunkid'] = xivo_uint($id['trunkid'])) === 0)
			return(false);

		return($this->_dso->edit($id,$arr));
	}

	function delete($id)
	{
		if(is_array($id) === false
		|| isset($id['outcallid'],$id['trunkid']) === false
		|| ($id['outcallid'] = xivo_uint($id['outcallid'])) === 0
		|| ($id['trunkid'] = xivo_uint($id['trunkid'])) === 0)
			return(false);

		return($this->_dso->delete($id));
	}
}

?>
