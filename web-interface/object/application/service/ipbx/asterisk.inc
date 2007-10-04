<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'application'.XIVO_SEP_DIR.'abstract.inc');

class xivo_application_asterisk_abstract extends xivo_application_abstract
{
	var $_ipbx	= null;

	function xivo_application_asterisk_abstract(&$ipbx)
	{
		if(is_object($ipbx) === false || get_class($ipbx) !== 'xivo_service_asterisk')
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_ipbx = &$ipbx;
	}

	function mk_interface($name,$chantype,$context='',$number='',$protocol='')
	{
		$chantype = strval($chantype);
		$number = strval($number);

		$r = array();

		switch($chantype)
		{
			case XIVO_SRE_IPBX_AST_CHAN_LOCAL:
				$r['interface'] = $this->_ipbx->mk_interface($name,null,$number,$context);

				if($number === '' || $r['interface'] === false)
					return(false);

				$r['channel'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
				break;
			default:
				$r['interface'] = $this->_ipbx->mk_interface($name,$protocol);
				$r['channel'] = $this->_ipbx->get_channel_by_protocol($protocol);

				if($r['interface'] === false || $r['channel'] === false)
					return(false);
		}

		return($r);
	}

	function set_extenumbers($action,$number,$context)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| ($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false)
			return(false);

		if($action === 'edit' && xivo_issa('extenumbers',$this->_info) === true)
			$id = $this->_info['extenumbers']['id'];
		else
			$id = null;

		$extenum = array();
		$extenum['exten'] = $number;
		$extenum['context'] = $context;

		$exists = false;

		if(($rs = $extenumbers->chk_values($extenum)) === false
		|| ($exists = $extenumbers->exists($rs,$id)) !== false)
		{
			$this->_set_result('extenumbers',$extenumbers->get_filter_result());

			if($exists === false)
				$this->_set_error('extenumbers',$extenumbers->get_filter_error());
			else
				$this->_set_error('extenumbers','exists');

			return(false);
		}

		$rs = $this->_set_result('extenumbers',$rs);

		return(true);
	}

	function set_macro($action,$name,$exten,$arr,$context)
	{
		if(($action !== 'add' && $action !== 'edit') === true
		|| is_scalar($name) === false
		|| ($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		if($action === 'edit' && xivo_issa($name,$this->_info) !== false)
			$rs = $extensions->chk_exten('macro',$arr,$exten,$context);
		else
			$rs = $extensions->new_exten('macro',$arr,$exten,$context);

		return($rs);
	}

	function add_extenumbers()
	{
		if(($rs = $this->get_result('extenumbers')) === null || $rs === false)
			return($rs);

		$extenumbers = &$this->_ipbx->get_module('extenumbers');
		$this->_return['extenumbers'] = $extenumbers->add($rs);

		return($this->_return['extenumbers']);
	}

	function add_macro($name)
	{
		if(($rs = $this->get_result($name)) === null || $rs === false)
			return($rs);

		$extensions = &$this->_ipbx->get_module('extensions');

		$this->_return[$name] = $extensions->add_exten($rs);

		return($this->_return[$name]);
	}

	function edit_extenumbers()
	{
		$rs = false;

		if(xivo_issa('extenumbers',$this->_info) === false
		|| ($rs = $this->get_result('extenumbers')) === null
		|| $rs === false)
			return($rs);

		$extenumbers = &$this->_ipbx->get_module('extenumbers');
		$this->_return['extenumbers'] = $extenumbers->edit($this->_info['extenumbers']['id'],$rs);

		return($this->_return['extenumbers']);
	}

	function edit_macro($name)
	{
		$rs = false;

		if(xivo_issa($name,$this->_info) === false
		|| ($rs = $this->get_result($name)) === null
		|| $rs === false)
			return($rs);

		$extensions = &$this->_ipbx->get_module('extensions');
		$this->_return[$name] = $extensions->edit($this->_info[$name]['id'],$rs);

		return($this->_return[$name]);
	}

	function delete_extenumbers()
	{
		if(xivo_issa('extenumbers',$this->_info) === false)
			return(null);

		if(($extenumbers = &$this->_ipbx->get_module('extenumbers')) === false)
			return(false);

		$r = $extenumbers->delete($this->_info['extenumbers']['id']);

		if($this->_status === 'delete')
			$this->_return['extenumbers'] = $r;

		return($r);
	}

	function delete_macro($name)
	{
		if(xivo_issa($name,$this->_info) === false)
			return(null);

		if(($extensions = &$this->_ipbx->get_module('extensions')) === false)
			return(false);

		$r = $extensions->delete($this->_info[$name]['id']);

		if($this->_status === 'delete')
			$this->_return[$name] = $r;

		return($r);
	}
}

?>
