<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_user_abstract_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'userfeatures'	=> '',
				'usersip'	=> '',
				'useriax'	=> '',
				'usercustom'	=> '');

	function get_nb($arr=null,$disable=null,$initialized=null)
	{
		if(is_array($arr) === false)
			$arr = array();

		$t = &$this->_table;

		$where = '';

		$sip = $iax = $custom = array();
		$sip['category'] = $iax['category'] = $custom['category'] = 'user';

		if($disable !== null)
		{
			$disable = intval((bool) $disable);
			$arr['commented'] = $disable;
			$sip['commented'] = $disable;
			$iax['commented'] = $disable;
			$custom['commented'] = $disable;
		}

		$arr['internal'] = 0;
		$protocolid = $t['userfeatures'].'.protocolid';

		if((empty($arr) === false && ($where = $this->_dso->where($arr,$t['userfeatures'])) === false) === true
		|| ($sipjoin = $this->_dso->join($t['usersip'],'LEFT',$t['usersip'].'.id = '.$protocolid)) === false
		|| ($sipwhere = $this->_dso->where($sip,$t['usersip'])) === false
		|| ($iaxjoin = $this->_dso->join($t['useriax'],'LEFT',$t['useriax'].'.id = '.$protocolid)) === false
		|| ($iaxwhere = $this->_dso->where($iax,$t['useriax'])) === false
		|| ($customjoin = $this->_dso->join($t['usercustom'],'LEFT',$t['usercustom'].'.id = '.$protocolid)) === false
		|| ($customwhere = $this->_dso->where($custom,$t['usercustom'])) === false)
			return(false);

		if($initialized !== null)
		{
			if((bool) $initialized === false)
				$sipwhere .= ' AND IFNULL('.$t['usersip'].'.fullcontact,\'\') = \'\'';
			else
				$sipwhere .= ' AND IFNULL('.$t['usersip'].'.fullcontact,\'\') != \'\'';
		}

		$sipwhere .= ' AND '.$t['userfeatures'].'.protocol = '.$t['usersip'].'.protocol';
		$iaxwhere .= ' AND '.$t['userfeatures'].'.protocol = '.$t['useriax'].'.protocol';
		$customwhere .= ' AND '.$t['userfeatures'].'.protocol = '.$t['usercustom'].'.protocol';

		$from = $t['userfeatures'].
			' '.$sipjoin.' AND '.$sipwhere.
			' '.$iaxjoin.' AND '.$iaxwhere.
			' '.$customjoin.' AND '.$customwhere;

		$where .= ' AND ('.$t['usersip'].'.id IS NOT NULL'.
				' OR '.$t['useriax'].'.id IS NOT NULL'.
				' OR '.$t['usercustom'].'.id IS NOT NULL)';

		$r = $this->_dso->select_count($from,$t['userfeatures'].'.id',$where);

		return($r);
	}

	function search($value,$disable=null,$initialized=null,$order=null,$limit=null,$type=0)
	{
		$t = &$this->_table;
		$arr = $search = array();
		$type = xivo_uint($type);

		if($type === 2)
		{
			if(($rs = $this->_dso->search_exact('context',$value,$t['userfeatures'])) !== false)
				$search[] = $rs;
		}
		else
		{
			if(($rs = $this->_dso->search_contain('firstname',$value,$t['userfeatures'])) !== false)
				$search[] = $rs;

			if(($rs = $this->_dso->search_contain('lastname',$value,$t['userfeatures'])) !== false)
				$search[] = $rs;

			if(($rs = $this->_dso->search_contain('number',$value,$t['userfeatures'])) !== false)
				$search[] = $rs;

			if(($rs = $this->_dso->search_contain('name',$value,$t['userfeatures'])) !== false)
				$search[] = $rs;

			if($type === 0 && ($rs = $this->_dso->search_contain('context',$value,$t['userfeatures'])) !== false)
				$search[] = $rs;
		}

		if(($search = implode(' OR ',$search)) === '')
			return(false);

		$where = '('.$search.')';

		$sip = $iax = $custom = array();
		$sip['category'] = $iax['category'] = $custom['category'] = 'user';

		if($disable !== null)
		{
			$disable = intval((bool) $disable);
			$arr['commented'] = $disable;
			$sip['commented'] = $disable;
			$iax['commented'] = $disable;
			$custom['commented'] = $disable;
		}

		$arr['internal'] = 0;
		$protocolid = $t['userfeatures'].'.protocolid';

		$uwhere = '';

		if((empty($arr) === false && ($uwhere = $this->_dso->where($arr,$t['userfeatures'])) === false) === true
		|| ($sipjoin = $this->_dso->join($t['usersip'],'LEFT',$t['usersip'].'.id = '.$protocolid)) === false
		|| ($sipwhere = $this->_dso->where($sip,$t['usersip'])) === false
		|| ($iaxjoin = $this->_dso->join($t['useriax'],'LEFT',$t['useriax'].'.id = '.$protocolid)) === false
		|| ($iaxwhere = $this->_dso->where($iax,$t['useriax'])) === false
		|| ($customjoin = $this->_dso->join($t['usercustom'],'LEFT',$t['usercustom'].'.id = '.$protocolid)) === false
		|| ($customwhere = $this->_dso->where($custom,$t['usercustom'])) === false)
			return(false);

		if($uwhere !== '')
			$where .= ' AND '.$uwhere;

		if($initialized !== null)
		{
			if((bool) $initialized === false)
				$sipwhere .= ' AND IFNULL('.$t['usersip'].'.fullcontact,\'\') = \'\'';
			else
				$sipwhere .= ' AND IFNULL('.$t['usersip'].'.fullcontact,\'\') != \'\'';
		}

		$sipwhere .= ' AND '.$t['userfeatures'].'.protocol = '.$t['usersip'].'.protocol';
		$iaxwhere .= ' AND '.$t['userfeatures'].'.protocol = '.$t['useriax'].'.protocol';
		$customwhere .= ' AND '.$t['userfeatures'].'.protocol = '.$t['usercustom'].'.protocol';

		$from = $t['userfeatures'].
			' '.$sipjoin.' AND '.$sipwhere.
			' '.$iaxjoin.' AND '.$iaxwhere.
			' '.$customjoin.' AND '.$customwhere;

		if($type === 1)
			$where .= ' AND IFNULL('.$t['userfeatures'].'.number,\'\') != \'\'';

		$where .= ' AND ('.$t['usersip'].'.id IS NOT NULL'.
				' OR '.$t['useriax'].'.id IS NOT NULL'.
				' OR '.$t['usercustom'].'.id IS NOT NULL)';

		$this->_dso->new_order($order,null,$t['userfeatures']);
		$this->_dso->new_limit($limit);

		$r = $this->_dso->select_all($from,$t['userfeatures'].'.*,'.$t['usersip'].'.fullcontact',$where);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count($from,$t['userfeatures'].'.id',$where)) === false
		|| empty($r) === true)
			$r = false;

		return($r);
	}

	function get_all($protocols,$disable=null,$initialized=null,$order=null,$limit=null)
	{
		$t = &$this->_table;
		$arr = array();

		$where = '';
		
		if(is_array($protocols) === false)
			$use_sip = $use_iax = $use_custom = true;
		else if(($where = $this->_dso->in('protocol',$protocols,false,$t['userfeatures']))  === false)
			return(false);
		else
		{
			$use_sip = in_array(XIVO_SRE_IPBX_AST_PROTO_SIP,$protocols);
			$use_iax = in_array(XIVO_SRE_IPBX_AST_PROTO_IAX,$protocols);
			$use_custom = in_array(XIVO_SRE_IPBX_AST_PROTO_CUSTOM,$protocols);
		}

		$sip = $iax = $custom = array();
		$sip['category'] = $iax['category'] = $custom['category'] = 'user';

		if($disable !== null)
		{
			$disable = intval((bool) $disable);
			$arr['commented'] = $disable;
			$sip['commented'] = $disable;
			$iax['commented'] = $disable;
			$custom['commented'] = $disable;
		}

		$arr['internal'] = 0;
		$protocolid = $t['userfeatures'].'.protocolid';

		$uwhere = '';

		if((empty($arr) === false && ($uwhere = $this->_dso->where($arr,$t['userfeatures'])) === false) === true)
			return(false);

		if((empty($arr) === false
		   && ($uwhere = $this->_dso->where($arr,$t['userfeatures'])) === false) === true
		|| ($use_sip === true
		   && (($sipjoin = $this->_dso->join($t['usersip'],'LEFT',$t['usersip'].'.id = '.$protocolid)) === false
		      || ($sipwhere = $this->_dso->where($sip,$t['usersip'])) === false) === true) === true
		|| ($use_iax === true
  		   && (($iaxjoin = $this->_dso->join($t['useriax'],'LEFT',$t['useriax'].'.id = '.$protocolid)) === false
		      || ($iaxwhere = $this->_dso->where($iax,$t['useriax'])) === false) === true) === true
		|| ($use_custom === true
  		   && (($customjoin = $this->_dso->join($t['usercustom'],'LEFT',$t['usercustom'].'.id = '.$protocolid)) === false
		      || ($customwhere = $this->_dso->where($custom,$t['usercustom'])) === false) === true) === true)
			return(false);

		if($uwhere !== '')
		{
			if($where !== '')
				$where .= ' AND ';

			$where .= $uwhere;
		}

		$from = $t['userfeatures'];
		$select = $t['userfeatures'].'.*';

		$notnull = array();

		if($use_sip === true)
		{
			$from .= ' '.$sipjoin.
				 ' AND '.$sipwhere.
				 ' AND '.$t['userfeatures'].'.protocol = '.$t['usersip'].'.protocol';

			$select .= ','.$t['usersip'].'.fullcontact';

			if($initialized !== null)
			{
				if((bool) $initialized === false)
					$from .= ' AND IFNULL('.$t['usersip'].'.fullcontact,\'\') = \'\'';
				else
					$from .= ' AND IFNULL('.$t['usersip'].'.fullcontact,\'\') != \'\'';
			}

			$notnull[] = $t['usersip'].'.id IS NOT NULL';
		}

		if($use_iax === true)
		{
			$from .= ' '.$iaxjoin.
				 ' AND '.$iaxwhere.
				 ' AND '.$t['userfeatures'].'.protocol = '.$t['useriax'].'.protocol';

			$notnull[] = $t['useriax'].'.id IS NOT NULL';
		}

		if($use_custom === true)
		{
			$from .= ' '.$customjoin.
				 ' AND '.$customwhere.
				 ' AND '.$t['userfeatures'].'.protocol = '.$t['usercustom'].'.protocol';

			$notnull[] = $t['usercustom'].'.id IS NOT NULL';
		}

		if(isset($notnull[0]) === true)
		{
			if($where !== '')
				$where .= ' AND ';

			$where .= '('.implode(' OR ',$notnull).')';
		}

		$this->_dso->new_order($order,null,$t['userfeatures']);
		$this->_dso->new_limit($limit);

		$r = $this->_dso->select_all($from,$select,$where);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count($from,$t['userfeatures'].'.id',$where)) === false
		|| empty($r) === true)
			$r = false;

		return($r);
	}
}

?>
