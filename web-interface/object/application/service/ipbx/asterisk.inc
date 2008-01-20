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
		if(xivo_instanceof($service,'xivo_service_asterisk') === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_service = &$service;
	}

	function get_origin()
	{
		return($this->_origin);
	}

	function get_origin_var($var)
	{
		$var = (string) $var;

		if(is_array($this->_origin) === false
		|| isset($this->_origin[$var]) === false)
			return(null);

		return($this->_origin[$var]);
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

	function get_musiconhold()
	{
		if(isset($this->_musiconhold) === false)
			return(false);

		if($this->_musiconhold !== false)
			return($this->_musiconhold);

		if(($musiconhold = &$this->_service->get_module('musiconhold')) === false)
			return(false);

		if(($this->_musiconhold = $musiconhold->get_all_category(null,false)) !== false)
			ksort($this->_musiconhold);

		return($this->_musiconhold);
	}

	function _get_extenumbers($type,$typeval)
	{
		$this->_info['extenumbers'] = $this->_extenumbers->get($type,$typeval);
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		return($this->_info['extenumbers']);
	}

	function set_extenumbers($action,$type,$number,$context='',$typeval='')
	{
		if(($r = $this->_extenumbers->set($action,$type,$number,$context,$typeval)) === false)
			$this->_set_error('extenumbers',$this->_extenumbers->get_error());

		$this->_set_result('extenumbers',$this->_extenumbers->get_result());

		return($r);
	}

	function set_macro($action,$name,$exten,$arr,$context)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_scalar($name) === false)
			return(false);

		if($action === 'edit' && xivo_issa($name,$this->_info) !== false)
			$r = $this->_extensions->chk_exten('macro',$arr,$exten,$context);
		else
			$r = $this->_extensions->new_exten('macro',$arr,$exten,$context);

		if($r === false)
		{
			if($this->_extensions->get_filter_errnb() > 0)
				$this->_set_error($name,$this->_extensions->get_filter_error());
			else
				$this->_set_error($name,'invalid macro');
		}

		return($r);
	}

	function add_extenumbers($typeval='',$id=0)
	{
		if(($rs = $this->_extenumbers->add($typeval,$id)) === null || $rs === false)
			return($rs);

		$this->_return['extenumbers'] = $this->_extenumbers->get_return();

		return($this->_return['extenumbers']);
	}

	function add_macro($name)
	{
		if(($rs = $this->get_result($name)) === null || $rs === false)
			return($rs);

		$this->_return[$name] = $this->_extensions->add_exten($rs);

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

		$this->_return[$name] = $this->_extensions->edit($this->_info[$name]['id'],$rs);

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

		if($this->_status === 'delete')
			$this->_return['extenumbers'] = $rs;

		return($rs);
	}

	function delete_macro($name)
	{
		if(xivo_issa($name,$this->_info) === false)
			return(null);

		$r = $this->_extensions->delete($this->_info[$name]['id']);

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

		if(($ufeatures = &$this->_service->get_module('userfeatures')) !== false
		&& ($r['users'] = $ufeatures->get_all_number($userid)) !== false)
			usort($r['users'],array(&$sortidentity,'str_usort'));
		else
			$r['users'] = false;

		if(($gfeatures = &$this->_service->get_module('groupfeatures')) !== false
		&& ($r['groups'] = $gfeatures->get_all_number($groupid)) !== false)
			usort($r['groups'],array(&$sortidentity,'str_usort'));
		else
			$r['groups'] = false;

		if(($qfeatures = &$this->_service->get_module('queuefeatures')) !== false
		&& ($r['queues'] = $qfeatures->get_all_number()) !== false)
			usort($r['queues'],array(&$sortidentity,'str_usort'));
		else
			$r['queues'] = false;

		if(($mfeatures = &$this->_service->get_module('meetmefeatures')) !== false
		&& ($r['meetme'] = $mfeatures->get_all_number()) !== false)
			usort($r['meetme'],array(&$sortidentity,'str_usort'));
		else
			$r['meetme'] = false;

		if(($schedule = &$this->_service->get_module('schedule')) !== false
		&& ($r['schedule'] = $schedule->get_all()) !== false)
			usort($r['schedule'],array(&$sortname,'str_usort'));
		else
			$r['schedule'] = false;

		return($r);
	}

	function get_destination_result_for_display($type)
	{
		$r = array();
		$r['endcall'] = '';
		$r['user'] = '';
		$r['group'] = '';
		$r['queue'] = '';
		$r['meetme'] = '';
		$r['schedule'] = '';
		$r['application'] = '';
		$r['sound'] = '';
		$r['custom'] = ''; 

		if(xivo_issa($type,$this->_result) === true)
		{
			$r[$this->_result[$type]['type']] = $this->_result[$type]['typeval'];
			$r = array_merge($r,$this->_result[$type]);
		}

		return($r);
	}
}

?>
