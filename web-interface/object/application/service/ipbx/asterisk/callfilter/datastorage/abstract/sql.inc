<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk','abstract','datastorage','abstract','sql.inc'),true);

class xivo_application_service_asterisk_callfilter_abstract_sql extends xivo_application_service_asterisk_abstract_sql
{
	var $_table	= array(
				'callfilter'		=> '',
				'callfiltermember'	=> '',
				'userfeatures'		=> '');

	function get_all($type,$disable=null,$order=null,$limit=null)
	{
		$t = &$this->_table;
		$arr = $user1 = $user2 = $cfilterm1where = array();
		$where = $user1where = $user2where = '';

		$arr['type'] = $type;

		if($disable !== null)
			$arr['commented'] = intval((bool) $disable);

		$user1['internal'] = $user2['internal'] = 0;

		$cfilterm1join = 'cfilterm1.callfilterid = '.$t['callfilter'].'.id';
		$cfilterm2join = 'cfilterm2.callfilterid = '.$t['callfilter'].'.id';
		$user1join = 'user1.id = cfilterm1.typeval';
		$user2join = 'user2.id = cfilterm2.typeval';

		if($type === 'bosssecretary')
		{
			$cfilterm1where['type'] = 'user';
			$cfilterm1where['bstype'] = 'boss';
			$user1['bsfilter'] = 'boss';

			$cfilterm2where['type'] = 'user';
			$cfilterm2where['bstype'] = 'secretary';
			$user2['bsfilter'] = 'secretary';
			$user2join .= ' AND user1.context = user2.context';
		}

		if(($where = $this->_dso->where($arr,$t['callfilter'])) === false
		|| ($cfilterm1join = $this->_dso->join($t['callfiltermember'].' AS cfilterm1','LEFT',$cfilterm1join)) === false
		|| (empty($cfilterm1where) === false
		   && ($cfilterm1where = $this->_dso->where($cfilterm1where,'cfilterm1')) === false) === true
		|| ($user1join = $this->_dso->join($t['userfeatures'].' AS user1','LEFT',$user1join)) === false
		|| ($user1where = $this->_dso->where($user1,'user1')) === false
		|| ($cfilterm2join = $this->_dso->join($t['callfiltermember'].' AS cfilterm2','LEFT',$cfilterm2join)) === false
		|| (empty($cfilterm2where) === false
		   && ($cfilterm2where = $this->_dso->where($cfilterm2where,'cfilterm2')) === false) === true
		|| ($user2join = $this->_dso->join($t['userfeatures'].' AS user2','LEFT',$user2join)) === false
		|| ($user2where = $this->_dso->where($user2,'user2')) === false)
			return(false);

		if($type === 'bosssecretary')
		{
			$user1where .= ' AND IFNULL(user1.number,\'\') != \'\'';
			$user2where .= ' AND IFNULL(user2.number,\'\') != \'\'';
		}

		$from =	$t['callfilter'].
			' '.$cfilterm1join.
			(empty($cfilterm1where) === false ? ' AND '.$cfilterm1where : '').
			' '.$user1join.
			' AND '.$user1where.
			' '.$cfilterm2join.
			(empty($cfilterm2where) === false ? ' AND '.$cfilterm2where : '').
			' '.$user2join.
			' AND '.$user2where;

		$select =	$t['callfilter'].'.*,'.
				'user1.id AS member_id,'.
				'user1.context AS member_context,'.
				'user1.firstname AS member_firstname,'.
				'user1.lastname AS member_lastname,'.
				'user1.number AS member_number,'.
				'user1.name AS member_name,'.
				'IFNULL(user2.id,0) AS linked';

		$this->_dso->new_order($order,null,$t['callfilter']);
		$this->_dso->new_limit($limit);

		$r = $this->_dso->select_all($from,$select,$where);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		if(($this->_cnt = $this->_dso->select_count($from,$t['callfilter'].'.id',$where)) === false
		|| empty($r) === true)
			$r = false;

		return($r);
	}

	function get_all_callfiltermember_user($type,$id)
	{
		$t = &$this->_table;

		$this->_dso->new_where(array('internal' => 0),false,$t['userfeatures']);

		if($type === 'bosssecretary')
			$this->_dso->add_where_in('bsfilter',array('boss','secretary'),false,false,$t['userfeatures']);

		$where = array();
		$where['callfilterid'] = $id;
		$where['type'] = 'user';

		$userjoin = $t['userfeatures'].'.id = '.$t['callfiltermember'].'.typeval';

		if(($where = $this->_dso->where($where,$t['callfiltermember'])) === false
		|| ($userjoin = $this->_dso->join($t['userfeatures'],'INNER',$userjoin)) === false)
			return(false);

		$from = $t['callfiltermember'].
			' '.$userjoin;

		if(($userwhere = $this->_dso->get_where()) !== '')
			$from .= ' AND '.$userwhere;

		$select = $t['callfiltermember'].'.*,'.
			  $t['userfeatures'].'.context AS member_context,'.
			  $t['userfeatures'].'.firstname AS member_firstname,'.
			  $t['userfeatures'].'.lastname AS member_lastname,'.
			  $t['userfeatures'].'.number AS member_number,'.
			  $t['userfeatures'].'.name AS member_name';

		$this->_cnt = 0;

		if(($r = $this->_dso->select_all($from,$select,$where,null,false)) === false)
			return(false);

		$this->_cnt = $this->_dso->num_rows();
		$this->_dso->free();

		return($r);
	}

	function get_boss_users($disable=null,$order=null,$limit=null)
	{
		return($this->_get_boss_secretary_users(true,$disable,$order,$limit));
	}

	function get_secretary_users($disable=null,$order=null,$limit=null)
	{
		return($this->_get_boss_secretary_users(false,$disable,$order,$limit));
	}

	function _get_boss_secretary_users($boss=true,$disable=null,$order=null,$limit=null)
	{
		$t = &$this->_table;

		$where = array();
		$where['internal'] = 0;
		$where['bsfilter'] = (bool) $boss === true ? 'boss' : 'secretary';

		if($disable !== null)
			$where['disable'] = intval((bool) $disable);

		if(($where = $this->_dso->where($where,$t['userfeatures'])) === false)
			return(false);

		$where .= ' AND IFNULL('.$t['userfeatures'].'.number,\'\') != \'\'';

		$this->_dso->new_order($order,null,$t['userfeatures']);
		$this->_dso->new_limit($limit);

		$r = $this->_dso->select_all($t['userfeatures'],null,$where);

		$this->_dso->reset_order();
		$this->_dso->reset_limit();

		return($r);
	}

	function get_free_boss_users($order=null,$limit=null)
	{
		$t = &$this->_table;

		$userwhere = array();
		$userwhere['internal'] = 0;
		$userwhere['bsfilter'] = 'boss';

		$cfiltermjoin = $t['callfiltermember'].'.typeval = '.$t['userfeatures'].'.id';

		$cfiltermwhere = array();
		$cfiltermwhere['type'] = 'user';
		$cfiltermwhere['bstype'] = 'boss';

		if(($where = $this->_dso->where($userwhere,$t['userfeatures'])) === false
		|| ($cfiltermjoin = $this->_dso->join($t['callfiltermember'],'LEFT',$cfiltermjoin)) === false
		|| ($cfiltermwhere = $this->_dso->where($cfiltermwhere,$t['callfiltermember'])) === false)
			return(false);

		$from = $t['userfeatures'].
			' '.$cfiltermjoin.
			' AND '.$cfiltermwhere;

		$where .= ' AND IFNULL('.$t['userfeatures'].'.number,\'\') != \'\''.
			  ' AND '.$t['callfiltermember'].'.id IS NULL';

		$r = $this->_dso->select_all($from,$t['userfeatures'].'.*',$where);

		return($r);
	}
}

?>
