<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_context_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'userfeatures'		=> '',
				'usersip'		=> '',
				'useriax'		=> '',
				'voicemail'		=> '');

	function update_usersip_mailbox($context,$newcontext)
	{
		return($this->_update_user_mailbox(XIVO_SRE_IPBX_AST_PROTO_SIP,$context,$newcontext));
	}

	function update_useriax_mailbox($context,$newcontext)
	{
		return($this->_update_user_mailbox(XIVO_SRE_IPBX_AST_PROTO_IAX,$context,$newcontext));
	}

	function _update_user_mailbox($protocol,$context,$newcontext)
	{
		if(xivo_haslen($context) === false
		|| xivo_haslen($newcontext) === false)
			return(false);

		switch($protocol)
		{
			case XIVO_SRE_IPBX_AST_PROTO_SIP:
				$usertable = $this->_table['usersip'];
				break;
			case XIVO_SRE_IPBX_AST_PROTO_IAX:
				$usertable = $this->_table['useriax'];
				break;
			default:
				return(false);
		}

		$this->_dso->new_select($this->_table['voicemail'],null);

		$this->_dso->innerjoin($this->_table['userfeatures'],
				       $this->_table['voicemail'].'.uniqueid = '.$this->_table['userfeatures'].'.voicemailid',
				       null,
				       'protocolid');

		$this->_dso->andjoinwhere($this->_table['userfeatures'],
					  $this->_table['userfeatures'].'.protocol = %s',$protocol);

		$this->_dso->where(array('context' => $context),$this->_table['voicemail']);

		if(($where = $this->_dso->get_select()) === null)
			return(false);

		$concat = array();
		$concat[] = (object) ($this->_table['voicemail'].'.mailbox');
		$concat[] = '@'.$newcontext;

		if(($concat = $this->_dso->concat($concat)) === false)
			return(false);

		$this->_dso->new_select($this->_table['voicemail'],(object) $concat);

		$this->_dso->innerjoin($this->_table['userfeatures'],
				       $this->_table['voicemail'].'.uniqueid = '.$this->_table['userfeatures'].'.voicemailid');

		$this->_dso->andjoinwhere($this->_table['userfeatures'],
					  $this->_table['userfeatures'].'.protocol = %s',$protocol);

		$this->_dso->where(array('context' => $context),$this->_table['voicemail']);

		if(($set = $this->_dso->get_select()) === null)
			return(false);

		$set = (object) ('('.$set.')');
		$where = (object) ('('.$where.')');

		return($this->_dso->update($usertable,array('mailbox' => $set),array('id' => $where)));
	}
}

?>
