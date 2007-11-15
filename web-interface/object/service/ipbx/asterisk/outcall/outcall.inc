<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_outcall extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'outcall';
	var $_filter	= false;
	var $_origin	= false;

	function xivo_service_asterisk_outcall(&$sre,&$dso)
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
		if(isset($data['exten']) === true)
			$data['exten'] = $this->_sre->display_extension($data['exten']);

		$data['setcallerid'] = (bool) $data['setcallerid'];
		$data['useenum'] = (bool) $data['useenum'];
		$data['internal'] = (bool) $data['internal'];
		$data['linked'] = (bool) $data['linked'];
		$data['commented'] = (bool) $data['commented'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		if(($id = xivo_uint($id)) !== 0)
			$arr['id'] = $id;

		if(isset($arr['name']) === true)
		{
			$where = array('name' => strtolower($arr['name']));

			if($this->get_id($where) !== false)
				return(false);
		}

		if(isset($arr['exten']) === true)
			$this->_sre->clean_extension($arr['exten']);

		if(isset($arr['exten'],$arr['context']) === true)
		{
			$where = array();
			$where['exten'] = $arr['exten'];
			$where['context'] = $arr['context'];

			if($this->get_id($where) !== false)
				return(false);
		}

		return($this->_dso->add($arr));
	}

	function edit($id,$arr,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['name']) === true)
		{
			$where = array('name' => strtolower($arr['name']));

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);
		}

		if(isset($arr['exten']) === true)
			$this->_sre->clean_extension($arr['exten']);

		if(isset($arr['exten'],$arr['context']) === true)
		{
			$where = array();
			$where['exten'] = $arr['exten'];
			$where['context'] = $arr['context'];

			if(($whereid = $this->get_id($where)) !== false && $whereid !== $id)
				return(false);
		}

		return($this->_dso->edit($id,$arr,$disable));
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
}

?>
