<?php

require_once(XIVO_PATH_LIBS.XIVO_SEP_DIR.'service'.XIVO_SEP_DIR.'ipbx'.XIVO_SEP_DIR.'asterisk'.XIVO_SEP_DIR.'abstract.inc');

class xivo_service_asterisk_uservoicemail extends xivo_service_asterisk_abstract
{
	var $_dso	= null;
	var $_name	= 'uservoicemail';

	function xivo_service_asterisk_uservoicemail(&$sre,&$dso)
	{
		if(is_object($sre) === false)
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		if(is_object($dso) === false)
			trigger_error('Invalid datastorage in '.__CLASS__,E_USER_ERROR);

		$this->_sre = &$sre;
		$this->_dso = &$dso;
	}

	function get($id,$disable=null)
	{
		if(($id = xivo_uint($id)) === 0 || ($r = $this->_dso->get($id,$disable)) === false)
			return(false);

		$r['commented'] = (bool) $r['commented'];

		return($r);
	}

	function get_by_mailbox($mailbox,$disable=null)
	{
		if(($mailbox = xivo_uint($mailbox)) === 0 || ($id = $this->_dso->get_by_mailbox($mailbox)) === false)
			return(false);

		return($this->get($id,$disable));
	}

	function add($arr,$id=0)
	{
		if(is_array($arr) === false)
			return(false);

		$r = array();

		if(($id = xivo_uint($id)) !== 0)
			$r['id'] = $id;

		$r['fullname'] = $arr['fullname'];
		$r['password'] = $arr['password'];
		$r['context'] = 'default';
		$r['mailbox'] = xivo_uint($arr['mailbox']);
		$r['email'] = $arr['email'];
		$r['attach'] = xivo_bool(xivo_ak('attach',$arr,true)) === false ? 'no' : 'yes';
		$r['delete'] = xivo_bool(xivo_ak('delete',$arr,true)) === true ? 'yes' : 'no';

		return($this->_dso->add($r));
	}

	function edit($id,$arr)
	{
		if(($id = xivo_uint($id)) === 0 || is_array($arr) === false)
			return(false);

		$r = array();
		$r['fullname'] = $arr['fullname'];
		$r['password'] = $arr['password'];
		$r['context'] = 'default';
		$r['mailbox'] = isset($arr['mailbox']) === true ? xivo_uint($arr['mailbox']) : 0;
		$r['email'] = isset($arr['email']) === true ? $arr['email'] : '';
		$r['attach'] = xivo_bool(xivo_ak('attach',$arr,true)) === false ? 'no' : 'yes';
		$r['delete'] = xivo_bool(xivo_ak('delete',$arr,true)) === true ? 'yes' : 'no';
		$r['commented'] = xivo_ak('commented',$arr,true) === 1 ? 1 : 0;

		return($this->_dso->edit($id,$r));
	}

	function delete($id)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		return($this->_dso->delete($id));
	}
}

?>
