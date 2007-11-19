<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_incall extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'incall';
	var $_filter	= false;
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
		$data['sound'] = '';
		$data['custom'] = '';

		if(isset($data[$data['type']]) === true)
			$data[$data['type']] = $data['typeval'];

		$data['exten'] = $this->_sre->display_extension($data['exten']);
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

		if(isset($arr['exten']) === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return($this->_dso->add($arr));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		if(isset($arr['exten']) === true)
			$arr['exten'] = $this->_sre->clean_extension($arr['exten']);

		return($this->_dso->edit($id,$arr));
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

	function is_valid($type,$typeval,$appval='',$disable=null)
	{
		switch($type)
		{
			case 'endcall':
				return($this->_chk_dest_value($type,$typeval));
			case 'application':
				return($this->chk_dest_application($typeval,$appval));
			case 'custom':
				return($this->chk_dest_custom($typeval));
			case 'schedule':
				return($this->chk_dest_schedule($typeval,null,$disable));
			case 'user':
				return($this->chk_dest_user($typeval,null,$disable));
			case 'group':
				return($this->chk_dest_group($typeval,null,$disable));
			case 'queue':
				return($this->chk_dest_queue($typeval,null,$disable));
			case 'meetme':
				return($this->chk_dest_meetme($typeval,null,$disable));
			case 'sound':
				return($this->chk_dest_sound('playback',$typeval));
		}

		return(false);
	}

	function get_search($exten,$disable=null,$order=null,$limit=null)
	{
		if(($list = $this->_dso->get_search($exten,$disable,$order,$limit)) === false)
			return(false);

		$nb = count($list);

		$r = array();

		for($i = 0;$i < $nb;$i++)
			$r[] = $this->_prepare($list[$i]);

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}
}

?>
