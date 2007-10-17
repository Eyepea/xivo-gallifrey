<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'application'.XIVO_SEP_DIR.'abstract.inc');

class xivo_application_asterisk_abstract extends xivo_application_abstract
{
	var $_service		= null;
	var $_abstract		= array();
	var $_service_type	= 'ipbx';
	var $_service_name	= 'asterisk';

	function xivo_application_asterisk_abstract(&$service)
	{
		if(is_object($service) === false || get_class($service) !== 'xivo_service_asterisk')
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_service = &$service;
	}

	function mk_interface($name,$chantype,$context='',$number='',$protocol='')
	{
		$chantype = strval($chantype);
		$number = strval($number);

		$r = array();

		if($protocol === XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
		{
			$r['interface'] = $this->_service->mk_interface($name,$protocol);
			$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_UNKNOWN;

			if($r['interface'] === false)
				return(false);

			return($r);
		}

		switch($chantype)
		{
			case XIVO_SRE_IPBX_AST_CHAN_LOCAL:
				$r['interface'] = $this->_service->mk_interface($name,null,$number,$context);

				if($number === '' || $r['interface'] === false)
					return(false);

				$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
				break;
			default:
				$r['interface'] = $this->_service->mk_interface($name,$protocol);
				$r['channel'] = $this->_service->get_channel_by_protocol($protocol);

				if($r['interface'] === false || $r['channel'] === false)
					return(false);
		}

		return($r);
	}

	function mk_agent_interface($number,$group=false)
	{
		$r = array();
		$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;

		if(($r['interface'] = $this->_service->mk_agent_interface($number,$group)) === false)
			return(false);

		return($r);
	}

	function _get_extenumbers($type,$typeval)
	{
		$this->_info['extenumbers'] = $this->_extenumbers->get($type,$typeval);
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		return($this->_info['extenumbers']);
	}

	function set_extenumbers($action,$type,$number,$context,$typeval='')
	{
		if(($r = $this->_extenumbers->set($action,$type,$number,$context,$typeval)) === false)
			$this->_set_error('extenumbers',$this->_extenumbers->get_error());

		$this->_set_result('extenumbers',$this->_extenumbers->get_result());

		return($r);
	}

	function set_macro($action,$name,$exten,$arr,$context)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_scalar($name) === false
		|| ($extensions = &$this->_service->get_module('extensions')) === false)
			return(false);

		if($action === 'edit' && xivo_issa($name,$this->_info) !== false)
			$rs = $extensions->chk_exten('macro',$arr,$exten,$context);
		else
			$rs = $extensions->new_exten('macro',$arr,$exten,$context);

		return($rs);
	}

	function add_extenumbers($typeval='')
	{
		if(($rs = $this->_extenumbers->add($typeval)) === null || $rs === false)
			return($rs);

		$this->_return['extenumbers'] = $this->_extenumbers->get_return();

		return($this->_return['extenumbers']);
	}

	function add_macro($name)
	{
		if(($rs = $this->get_result($name)) === null || $rs === false)
			return($rs);

		$extensions = &$this->_service->get_module('extensions');

		$this->_return[$name] = $extensions->add_exten($rs);

		return($this->_return[$name]);
	}

	function edit_extenumbers($id=0)
	{
		if(($rs = $this->_extenumbers->edit($id)) === null || $rs === false)
			return($rs);

		$this->_return['extenumbers'] = $this->_extenumbers->get_return();

		return($this->_return['extenumbers']);
	}

	function edit_macro($name)
	{
		$rs = false;

		if(xivo_issa($name,$this->_info) === false
		|| ($rs = $this->get_result($name)) === null
		|| $rs === false)
			return($rs);

		$extensions = &$this->_service->get_module('extensions');
		$this->_return[$name] = $extensions->edit($this->_info[$name]['id'],$rs);

		return($this->_return[$name]);
	}

	function save_extenumbers($typeval=null)
	{
		if(($rs = $this->_extenumbers->save($this->_status,$typeval)) === null || $rs === false)
			return($rs);

		$this->_return['extenumbers'] = $this->_extenumbers->get_return();

		return($this->_return['extenumbers']);
	}

	function delete_extenumbers($id=0)
	{
		if(($rs = $this->_extenumbers->delete($this->_status,$id)) === null
		|| $rs === false)
			return($rs);

		$this->_return['extenumbers'] = $this->_extenumbers->get_return();

		return($this->_return['extenumbers']);
	}

	function delete_macro($name)
	{
		if(xivo_issa($name,$this->_info) === false)
			return(null);

		if(($extensions = &$this->_service->get_module('extensions')) === false)
			return(false);

		$r = $extensions->delete($this->_info[$name]['id']);

		if($this->_status === 'delete')
			$this->_return[$name] = $r;

		return($r);
	}

	function get_destination_list($type='',$id=0)
	{
		$r = array();

		xivo::load_class('xivo_sort');
		$sortidentity = new xivo_sort(array('key' => 'identity'));
		$sortname = new xivo_sort(array('key' => 'name'));

		$userid = $groupid = 0;

		if($type === 'user')
			$userid = xivo_uint($id);
		else if($type === 'group')
			$groupid = xivo_uint($id);

		if(($ufeatures = &$this->_service->get_module('userfeatures')) === false
		|| ($r['users'] = $ufeatures->get_all_number($userid)) === false)
			$r['users'] = false;
		else
			usort($r['users'],array(&$sortidentity,'str_usort'));

		if(($gfeatures = &$this->_service->get_module('groupfeatures')) === false
		|| ($r['groups'] = $gfeatures->get_all_number($groupid)) === false)
			$r['groups'] = false;
		else
			usort($r['groups'],array(&$sortidentity,'str_usort'));

		if(($qfeatures = &$this->_service->get_module('queuefeatures')) === false
		|| ($r['queues'] = $qfeatures->get_all_number()) === false)
			$r['queues'] = false;
		else
			usort($r['queues'],array(&$sortidentity,'str_usort'));

		if(($mfeatures = &$this->_service->get_module('meetmefeatures')) === false
		|| ($r['meetme'] = $mfeatures->get_all_number()) === false)
			$r['meetme'] = false;
		else
			usort($r['meetme'],array(&$sortidentity,'str_usort'));

		if(($schedule = &$this->_service->get_module('schedule')) === false
		|| ($r['schedule'] = $schedule->get_all()) === false)
			$r['schedule'] = false;
		else
			usort($r['schedule'],array(&$sortname,'str_usort'));

		return($r);
	}
}

?>
