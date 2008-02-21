<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_serverldap extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_serverldap	= null;

	function xivo_application_service_asterisk_serverldap(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_serverldap = &$this->_service->get_module('serverldap')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['serverldap'] = $this->_serverldap->get($id)) === false)
			return(false);

		$this->_origin['serverldap'] = $this->_serverldap->get_origin();

		return($this->_info);
	}

	function get_elements()
	{
		$r = array();
		$r['serverldap'] = $this->_serverldap->get_element();

		return($r);
	}

	function get_serverldaps_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_serverldap->get_all($disable,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_serverldap->get_cnt();
			return(false);
		}

		$this->_cnt = $this->_serverldap->get_cnt();

		if((bool) $assoc === false)
			$r = &$rs;
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[$rs[$i]['id']] = $rs[$i];
		}

		if(empty($r) === true)
			$r = false;

		return($r);
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
		|| xivo_issa('serverldap',$arr) === false)
			return(false);

		$this->set_serverldap($arr['serverldap']);
	
		return(($this->get_errnb() < 1));
	}

	function set_serverldap($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('serverldap',null);
			$this->_set_error('serverldap','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_serverldap->chk_values($arr)) === false)
		{
			$this->_set_result('serverldap',$this->_serverldap->get_filter_result());
			$this->_set_error('serverldap',$this->_serverldap->get_filter_error());
			return(false);
		}

		$this->_set_result('serverldap',$rs);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->add_serverldap() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function add_serverldap()
	{
		if(($rs = $this->get_result('serverldap')) === null
		|| $rs === false)
			return(false);

		$this->_return['serverldap'] = $this->_serverldap->add($rs);

		return($this->_return['serverldap']);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_serverldap() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function edit_serverldap()
	{
		if(xivo_issa('serverldap',$this->_info) === false
		|| ($rs = $this->get_result('serverldap')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['serverldap']['commented'];

		$this->_return['serverldap'] = $this->_serverldap->edit($this->_info['serverldap']['id'],$rs);

		return($this->_return['serverldap']);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_serverldap() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function delete_serverldap()
	{
		if(xivo_issa('serverldap',$this->_info) === false)
			return(false);

		$r = $this->_serverldap->delete($this->_info['serverldap']['id']);

		if($this->_status === 'delete')
			$this->_return['serverldap'] = $r;

		return($r);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_serverldap() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_serverldap() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_serverldap()
	{
		return($this->_enable_disable_serverldap(false));
	}

	function disable_serverldap()
	{
		return($this->_enable_disable_serverldap(true));
	}

	function _enable_disable_serverldap($disable=false)
	{
		if(xivo_issa('serverldap',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_serverldap->enable($this->_info['serverldap']['id']);
		else
			$r = $this->_serverldap->disable($this->_info['serverldap']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['serverldap'] = $r;

		return($r);
	}
}

?>
