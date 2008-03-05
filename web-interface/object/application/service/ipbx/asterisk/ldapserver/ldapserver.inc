<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_ldapserver extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_ldapserver	= null;
	var $_xldapserver	= null;

	function xivo_application_service_asterisk_ldapserver(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_ldapserver = &$this->_service->get_module('ldapserver')) === false
		|| xivo::load_class('xivo_ldapserver',XIVO_PATH_OBJECT,null,false) === false
		|| ($this->_xldapserver = new xivo_ldapserver()) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['ldapserver'] = $this->_ldapserver->get($id,$disable)) === false)
			return(false);

		$this->_origin['ldapserver'] = $this->_ldapserver->get_origin();

		$this->_get_xldapserver($disable);

		return($this->_info);
	}

	function get_primary($arr,$disable=null)
	{
		return($this->_ldapserver->get_primary($arr,$disable));
	}

	function _get_xldapserver($disable)
	{
		$this->_origin['xldapserver'] = false;

		if(($this->_info['xldapserver'] = $this->_xldapserver->get($this->_info['ldapserver']['ldapserverid'],
									   $disable)) === false)
			return(false);

		$this->_origin['xldapserver'] = $this->_xldapserver->get_origin();

		return($this->_info['xldapserver']);
	}

	function get_elements()
	{
		$r = array();
		$r['ldapserver'] = $this->_ldapserver->get_element();
		$r['xldapserver'] = $this->_xldapserver->get_element();

		return($r);
	}

	function get_ldapservers_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_ldapserver->get_all($disable,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_ldapserver->get_cnt();
			return(false);
		}

		$r = $info = array();

		$this->_cnt = $this->_ldapserver->get_cnt();

		for($i = 0;$i < $nb;$i++)
		{
			$info['ldapserver'] = $rs[$i];
			$info['identity'] = $info['ldapserver']['name'];

			if(($info['xldapserver'] = $this->_xldapserver->get($info['ldapserver']['ldapserverid'])) !== false)
				$info['identity'] .= ' ('.$info['xldapserver']['host'].')';

			$r[$info['ldapserver']['id']] = $info;
		}

		if(empty($r) === true)
			$r = false;
		else if((bool) $assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_xldapservers_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_xldapserver->get_all($disable,true,$order,$limit,$assoc));
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
		|| xivo_issa('ldapserver',$arr) === false)
			return(false);

		$this->set_ldapserver($arr['ldapserver']);
	
		return(($this->get_errnb() < 1));
	}

	function set_ldapserver($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('ldapserver',null);
			$this->_set_error('ldapserver','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_ldapserver->chk_values($arr)) === false)
		{
			$this->_set_result('ldapserver',$this->_ldapserver->get_filter_result());
			$this->_set_error('ldapserver',$this->_ldapserver->get_filter_error());
			return(false);
		}
		else if($this->_xldapserver->get_primary($rs['ldapserverid']) === false)
		{
			$this->_set_result('ldapserver',$this->_ldapserver->get_filter_result());
			$this->_set_error('ldapserver','wrong xivo_ldapserver');
			return(false);
		}

		$this->_set_result('ldapserver',$rs);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->add_ldapserver() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function add_ldapserver()
	{
		if(($rs = $this->get_result('ldapserver')) === null
		|| $rs === false)
			return(false);

		$this->_return['ldapserver'] = $this->_ldapserver->add($rs);

		return($this->_return['ldapserver']);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_ldapserver() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function edit_ldapserver()
	{
		if(xivo_issa('ldapserver',$this->_info) === false
		|| ($rs = $this->get_result('ldapserver')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['ldapserver']['commented'];

		$this->_return['ldapserver'] = $this->_ldapserver->edit($this->_info['ldapserver']['id'],$rs);

		return($this->_return['ldapserver']);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_ldapserver() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function delete_ldapserver()
	{
		if(xivo_issa('ldapserver',$this->_info) === false)
			return(false);

		$r = $this->_ldapserver->delete($this->_info['ldapserver']['id']);

		if($this->_status === 'delete')
			$this->_return['ldapserver'] = $r;

		return($r);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_ldapserver() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_ldapserver() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_ldapserver()
	{
		return($this->_enable_disable_ldapserver(false));
	}

	function disable_ldapserver()
	{
		return($this->_enable_disable_ldapserver(true));
	}

	function _enable_disable_ldapserver($disable=false)
	{
		if(xivo_issa('ldapserver',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_ldapserver->enable($this->_info['ldapserver']['id']);
		else
			$r = $this->_ldapserver->disable($this->_info['ldapserver']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['ldapserver'] = $r;

		return($r);
	}
}

?>
