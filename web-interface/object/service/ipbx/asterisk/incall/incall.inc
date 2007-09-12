<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_incall extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'incall';
	var $_filter	= null;
	var $_origin	= false;

	function xivo_service_asterisk_incall(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;

		$this->_load_config();
	}

	function _prepare($data)
	{
		$data['endcall'] = '';
		$data['user'] = '';
		$data['group'] = '';
		$data['queue'] = '';
		$data['meetme'] = '';
		$data['schedule'] = '';
		$data['application'] = '';
		$data['custom'] = '';

		if(isset($data[$data['type']]) === true)
			$data[$data['type']] = $data['typeval'];

		$data['linked'] = (bool) $data['linked'];
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function linked($id,$disable=null)
	{
		return($this->edit($id,array('linked' => true),$disable));
	}

	function unlinked($id,$disable=null)
	{
		return($this->edit($id,array('linked' => false),$disable));
	}

	function linked_where($id,$disable=null)
	{
		return($this->edit_where($id,array('linked' => true),$disable));
	}

	function unlinked_where($id,$disable=null)
	{
		return($this->edit_where($id,array('linked' => false),$disable));
	}

	function is_valid($type,$typeval,$id=null,$disable=null)
	{
		$type = strval($type);
		$id = $id !== null ? xivo_uint($id) : null;
		$module = $number = false;

		switch($type)
		{
			case 'endcall':
			case 'application':
				if(is_scalar($typeval) === false
				|| ($endcall = $this->get_element_value($type)) === false
				|| in_array($typeval,$endcall,true) === false)
					return(false);

				return(true);
			case 'custom':
				if(preg_match('/^[a-zA-Z]{3,}\((.*)\)$/',$typeval,$match) === 1
				&& ($match[1] === ''
				   || (ctype_print($match[1]) === true
				      && trim($match[1]) === $match[1]) === true))
					return(true);

				return(false);
			case 'schedule':
				$typeval = (int) $typeval;

				$module = $this->_sre->get_module('schedule');
				break;
			case 'user':
			case 'group':
			case 'queue':
			case 'meetme':
				$number = true;
				$typeval = (int) $typeval;

				$module = $this->_sre->get_module($type.'features');
				break;
			default:
				return(false);
		}

		if($module === false
		|| ($info = $module->get($typeval,$disable)) === false
		|| ($number === true && xivo_empty($info['number']) === true) === true)
			return(false);

		return($info);
	}
}

?>
