<?php

xivo_service_asterisk::required(array('abstract','abstract.inc'),true);

class xivo_service_asterisk_dialstatus extends xivo_service_asterisk_abstract
{
	var $_dso		= null;
	var $_name		= 'dialstatus';
	var $_filter		= false;
	var $_origin		= false;
	var $_origin_list	= false;

	function xivo_service_asterisk_dialstatus(&$sre,&$dso)
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
		$data['voicemail'] = '';
		$data['schedule'] = '';
		$data['application'] = '';
		$data['sound'] = '';
		$data['custom'] = ''; 

		if(isset($data[$data['type']]) === true)
			$data[$data['type']] = $data['typeval'];

		$data['linked'] = (bool) $data['linked'];

		return($data);
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);
		else if($arr['type'] !== 'application')
			$arr['applicationval'] = '';

		return(parent::add($arr,$id));
	}

	function edit($id,$arr)
	{
		if(is_array($arr) === false)
			return(false);
		else if(isset($arr['type']) === true && $arr['type'] !== 'application')
			$arr['applicationval'] = '';

		return(parent::edit($id,$arr));
	}

	function linked($id)
	{
		return($this->edit($id,array('linked' => true)));
	}

	function unlinked($id)
	{
		return($this->edit($id,array('linked' => false)));
	}

	function linked_where($id)
	{
		return($this->edit_where($id,array('linked' => true)));
	}

	function unlinked_where($id)
	{
		return($this->edit_where($id,array('linked' => false)));
	}

	function is_valid($category,$categoryval,$type,$typeval,$appval='',$disable=null)
	{
		if($this->chk_element_value('category',$category) === false
		|| $this->chk_element_value('type',$type) === false)
			return(false);
		else if($category === $type)
			$id = $categoryval;
		else
			$id = null;

		switch($type)
		{
			case 'endcall':
				return($this->_chk_dest_value($type,$typeval));
			case 'user':
				return($this->chk_dest_user($typeval,$id,$disable));
			case 'group':
				return($this->chk_dest_group($typeval,$id,$disable));
			case 'queue':
				return($this->chk_dest_queue($typeval,null,$disable));
			case 'meetme':
				return($this->chk_dest_meetme($typeval,null,$disable));
			case 'voicemail':
				return($this->chk_dest_voicemail($typeval,null,$disable));
			case 'schedule':
				return($this->chk_dest_schedule($typeval,null,$disable));
			case 'application':
				return($this->chk_dest_application($typeval,$appval));
			case 'sound':
				return($this->chk_dest_sound('playback',$typeval));
			case 'custom':
				return($this->chk_dest_custom($typeval));
		}

		return(false);
	}
}

?>
