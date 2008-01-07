<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_user_abstract_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'userfeatures'		=> '',
				'usersip'		=> '',
				'useriax'		=> '',
				'usercustom'		=> '',
				'callfilter'		=> '',
				'callfiltermember'	=> '');

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
	
		$select =	$t['userfeatures'].'.*,'.
				$t['usersip'].'.fullcontact,'.
				$t['usersip'].'.secret AS sipsecret,'.
				$t['usersip'].'.callerid AS sipcallerid,'.
				$t['useriax'].'.secret AS iaxsecret,'.
				$t['useriax'].'.callerid AS iaxcallerid';

		$r = $this->_dso->select_all($from,$select,$where);

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
			$select .= ','.$t['usersip'].'.fullcontact,'.
				   $t['usersip'].'.secret AS sipsecret,'.
				   $t['usersip'].'.callerid AS sipcallerid';

			$from .= ' '.$sipjoin.
				 ' AND '.$sipwhere.
				 ' AND '.$t['userfeatures'].'.protocol = '.$t['usersip'].'.protocol';

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
			$select .= ','.$t['useriax'].'.secret AS iaxsecret,'.
				   $t['useriax'].'.callerid AS iaxcallerid';

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

	function get_bsfilter($id,$context,$bsfilter)
	{
		if(($id = xivo_uint($id)) === 0)
			return(false);

		$t = &$this->_table;

		$userwhere = array();
		$userwhere['internal'] = 0;
		$userwhere['id'] = $id;

		$cfiltermjoin = $t['callfiltermember'].'.typeval = '.$t['userfeatures'].'.id';

		$cfiltermwhere = array();
		$cfiltermwhere['type'] = 'user';
		$cfiltermwhere['bstype'] = $bsfilter;

		if(($userwhere = $this->_dso->where($userwhere,$t['userfeatures'])) === false
		|| ($cfiltermjoin = $this->_dso->join($t['callfiltermember'],'INNER',$cfiltermjoin)) === false
		|| ($cfiltermwhere = $this->_dso->where($cfiltermwhere,$t['callfiltermember'])) === false)
			return(false);

		$from = $t['userfeatures'].
			' '.$cfiltermjoin.
			' AND '.$t['callfiltermember'].'.bstype = '.$t['userfeatures'].'.bsfilter'.
			' AND '.$cfiltermwhere;

		$callfilterids = $this->_dso->select_all($from,$t['callfiltermember'].'.callfilterid',$userwhere,null,true,true);

		if(isset($callfilterids[0]) === false)
			return(false);

		$userjoin = $t['userfeatures'].'.id = '.$t['callfiltermember'].'.typeval';

		$userwhere = array();
		$userwhere['internal'] = 0;
		$userwhere['context'] = $context;

		$cfilterjoin = $t['callfilter'].'.id = '.$t['callfiltermember'].'.callfilterid';

		$cfilterwhere = array();
		$cfilterwhere['type'] = 'bosssecretary';

		if(($userjoin = $this->_dso->join($t['userfeatures'],'INNER',$userjoin)) === false
		|| ($userwhere = $this->_dso->where($userwhere,$t['userfeatures'])) === false
		|| ($cfilterjoin = $this->_dso->join($t['callfilter'],'INNER',$cfilterjoin)) === false
		|| ($cfilterwhere = $this->_dso->where($cfilterwhere,$t['callfilter'])) === false)
			return(false);

		$this->_dso->new_where(array('type' => 'user'),false,$t['callfiltermember']);
		$this->_dso->add_where(array('bstype' => $bsfilter),true,$t['callfiltermember']);
		$this->_dso->add_where_in('id',$callfilterids,false,$t['callfilter']);

		$from = $t['callfiltermember'].
			' '.$userjoin.
			' AND '.$userwhere.
			' AND '.$t['userfeatures'].'.bsfilter = '.$t['callfiltermember'].'.bstype'.
			' AND IFNULL('.$t['userfeatures'].'.number,\'\') != \'\''.
			' '.$cfilterjoin.
			' AND '.$cfilterwhere.
			' AND '.$t['callfilter'].'.bosssecretary IS NOT NULL';

		$select = $t['callfilter'].'.id AS callfilterid,'.
			  $t['callfilter'].'.name AS callfiltername,'.
			  $t['userfeatures'].'.*';

		$r = $this->_dso->select_all($from,$select,$this->_dso->get_where());

		if(isset($r[0]) === false)
			$r = false;

		return($r);
	}

	function chk_bsfilter($id,$context,$bsfilter,$user2id)
	{
		if(($id = xivo_uint($id)) === 0
		|| ($user2id = xivo_uint($user2id)) === 0)
			return(false);

		$t = &$this->_table;
		$arr = $user1 = $user2 = $cfilterm1where = array();
		$user1where = $user2where = '';

		$arr['type'] = 'bosssecretary';

		$user1['id'] = $id;
		$user2['context'] = $context;
		$user2['id'] = $user2id;

		$user1['internal'] = $user2['internal'] = 0;

		$cfilterm1join = 'cfilterm1.callfilterid = '.$t['callfilter'].'.id';
		$cfilterm2join = 'cfilterm2.callfilterid = '.$t['callfilter'].'.id';
		$user1join = 'user1.id = cfilterm1.typeval';
		$user2join = 'user2.id = cfilterm2.typeval';

		$cfilterm1where['type'] = 'user';
		$cfilterm1where['bstype'] = $bsfilter;

		$cfilterm2where['type'] = 'user';

		if(($cfilterm1join = $this->_dso->join($t['callfiltermember'].' AS cfilterm1','INNER',$cfilterm1join)) === false
		|| ($cfilterm1where = $this->_dso->where($cfilterm1where,'cfilterm1')) === false
		|| ($user1join = $this->_dso->join($t['userfeatures'].' AS user1','INNER',$user1join)) === false
		|| ($user1where = $this->_dso->where($user1,'user1')) === false
		|| ($cfilterm2join = $this->_dso->join($t['callfiltermember'].' AS cfilterm2','INNER',$cfilterm2join)) === false
		|| ($cfilterm2where = $this->_dso->where($cfilterm2where,'cfilterm2')) === false
		|| ($user2join = $this->_dso->join($t['userfeatures'].' AS user2','INNER',$user2join)) === false
		|| ($user2where = $this->_dso->where($user2,'user2')) === false)
			return(false);

		$from =	$t['callfilter'].
			' '.$cfilterm1join.
			' AND '.$cfilterm1where.
			' '.$user1join.
			' AND '.$user1where.
			' AND IFNULL(user1.number,\'\') != \'\''.
			' AND user1.bsfilter = cfilterm1.bstype'.
			' '.$cfilterm2join.
			' AND '.$cfilterm2where.
			' '.$user2join.
			' AND '.$user2where.
			' AND IFNULL(user2.number,\'\') != \'\''.
			' AND user2.bsfilter = cfilterm2.bstype';

		$this->_dso->new_where(array('bstype' => $bsfilter),true,'cfilterm2');

		$where = $this->_dso->get_where().
			 ' AND '.$t['callfilter'].'.bosssecretary IS NOT NULL';

		$r = $this->_dso->select_one($from,'cfilterm2.id',$this->_dso->get_where());

		if(isset($r['id']) === false)
			$r = false;
		else
			$r = $r['id'];

		return($r);
	}
}

?>
