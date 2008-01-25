<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_callfilter extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_origin		= array();
	var $_status		= '';
	var $_callfilter	= null;
	var $_callfiltermember	= null;
	var $_userfeatures	= null;
	var $_dialstatus	= null;
	var $_param		= array('type'	=> '');

	function xivo_application_service_asterisk_callfilter(&$service,&$dso,$param=array(),$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if($this->set_params($param) === false
		|| isset($param['type']) === false
		|| ($this->_callfilter = &$this->_service->get_module('callfilter')) === false
		|| $this->_callfilter->chk_element_value('type',$param['type']) === false
		|| ($this->_userfeatures = &$this->_service->get_module('userfeatures')) === false)
			return(false);

		if((bool) $components === true)
			$this->load_components();

		$this->_param['type'] = $param['type'];

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_callfiltermember = &$this->_service->get_module('callfiltermember')) === false
		|| ($this->_dialstatus = &$this->get_abstract('dialstatus')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['callfilter'] = $this->_callfilter->get($id,$disable)) === false)
			return(false);

		$this->_origin['callfilter'] = $this->_callfilter->get_origin();

		if($this->_components === false)
			return($this->_info);

		$this->_get_callfiltermember();
		$this->_get_dialstatus();

		return($this->_info);
	}

	function _get_all_callfiltermember_user()
	{
		$this->_info['callfilteruser'] = false;
		$this->_origin['callfilteruser'] = false;
		
		if(($rs = $this->_dso->get_all_callfiltermember_user($this->_param['type'],
								     $this->_info['callfilter']['id'])) === false
		|| ($nb = $this->_dso->get_cnt()) === 0)
			return(false);

		$this->_info['callfilteruser'] = array();
		$this->_origin['callfilteruser'] = array();

		$this->_info['userfeatures'] = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];

			$user = array();
			$user['context'] = $ref['member_context'];
			$user['firstname'] = $ref['member_firstname'];
			$user['lastname'] = $ref['member_lastname'];
			$user['number'] = $ref['member_number'];
			$user['name'] = $ref['member_name'];

			unset($ref['member_context'],
			      $ref['member_firstname'],
			      $ref['member_lastname'],
			      $ref['member_number'],
			      $ref['member_name']);

			$user = $this->_userfeatures->mk_identity($user);

			$this->_info['userfeatures'][] = $user;
			$this->_origin['callfiltermember'][] = $ref;
			$this->_info['callfiltermember'][] = $this->_callfiltermember->_prepare($ref);
		}

		return(true);
	}

	function _get_callfiltermember()
	{
		switch($this->_param['type'])
		{
			case 'bosssecretary':
				return($this->_get_callfilterbs());
		}

		return(false);
	}

	function _get_callfilterbs()
	{
		$r = true;

		$this->_info['callfiltermember'] = array();
		$this->_origin['callfiltermember'] = array();

		$where = array();
		$where['type'] = 'user';
		$where['bstype'] = 'boss';
		$where['callfilterid'] = $this->_info['callfilter']['id'];

		if(($this->_info['callfiltermember']['boss'] = $this->_callfiltermember->get_where($where)) === false)
		{
			$r = false;
			$this->_info['callfiltermember']['boss'] = null;
			$this->_origin['callfiltermember'] = false;
		}
		else
			$this->_origin['callfiltermember'][] = $this->_callfiltermember->get_origin();

		$where['bstype'] = 'secretary';

		if(($this->_info['callfiltermember']['secretary'] = $this->_callfiltermember->get_all_where($where)) === false)
		{
			$this->_info['callfiltermember']['secretary'] = null;
			$this->_origin['callfiltermember'] = false;
			return(false);
		}
		else if($r === false)
			return($this->_info['callfiltermember']);

		$this->_origin['callfiltermember'] = array_merge($this->_origin['callfiltermember'],
								 $this->_callfiltermember->get_origin_list());

		return($this->_info['callfiltermember']);
	}

	function _get_dialstatus()
	{
		$this->_info['dialstatus'] = $this->_dialstatus->get('callfilter',$this->_info['callfilter']['id']);

		return($this->_info['dialstatus']);
	}

	function get_boss_users($disable=null)
	{
		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;

		if(($rs = $this->_dso->get_boss_users($disable,$order)) == false
		|| ($nb = count($rs)) === false)
			return(false);

		for($i = 0;$i < $nb;$i++)
			$rs[$i] = $this->_userfeatures->_prepare($rs[$i]);

		return($rs);
	}

	function get_free_boss_users()
	{
		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;

		if(($rs = $this->_dso->get_free_boss_users($order)) == false
		|| ($nb = count($rs)) === false)
			return(false);

		for($i = 0;$i < $nb;$i++)
			$rs[$i] = $this->_userfeatures->_prepare($rs[$i]);

		return($rs);
	}	

	function get_secretary_users($disable=null,$assoc=false)
	{
		$assoc = (bool) $assoc;

		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;

		if(($rs = $this->_dso->get_secretary_users($disable,$order)) == false
		|| ($nb = count($rs)) === false)
			return(false);

		$r = array();

		if($assoc === true)
		{
			for($i = 0;$i < $nb;$i++)
				$r[$rs[$i]['id']] = $this->_userfeatures->_prepare($rs[$i]);
		}
		else
		{
			for($i = 0;$i < $nb;$i++)
				$rs[$i] = $this->_userfeatures->_prepare($rs[$i]);
		}

		if(empty($r) === true)
			$r = false;

		return($r);
	}

	function get_elements()
	{
		$r = array();
		$r['callfilter'] = $this->_callfilter->get_element();
		$r['callfiltermember'] = $this->_callfiltermember->get_element();
		$r['userfeatures'] = $this->_userfeatures->get_element();
		$r['dialstatus'] = $this->_dialstatus->get_element();

		return($r);
	}

	function get_callfilters_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		$assoc = (bool) $assoc;

		$this->_cnt = $this->_dso->get_cnt();

		$rs = $this->_dso->get_all($this->_param['type'],$disable,$order,$limit);

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$r[$rs[$i]['id']] = $this->_callfilter->_prepare($rs[$i]);
			$ref = &$r[$rs[$i]['id']];

			if(xivo_empty($ref['member_id']) === true)
			{
				$ref['member_identity'] = '';
				$ref['linked'] = false;
			}
			else
			{
				if((bool) $ref['linked'] === false)
					$ref['linked'] = false;
				else
					$ref['linked'] = true;

				$member = array();
				$member['context'] = $ref['member_context'];
				$member['firstname'] = $ref['member_firstname'];
				$member['lastname'] = $ref['member_lastname'];
				$member['number'] = $ref['member_number'];
				$member['name'] = $ref['member_name'];
				$member = $this->_userfeatures->mk_identity($member);
				$ref['member_identity'] = $member['identity'];
			}
		}

		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_dialstatus_result()
	{
		return($this->_dialstatus->get_result_for_display());
	}

	function get_dialstatus_destination_list()
	{
		$r = $this->get_destination_list();
		$r['sounds'] = $this->_dialstatus->get_sound();

		return($r);
	}

	function set_add($arr)
	{
		return($this->_set('add',$arr));
	}

	function set_edit($arr)
	{
		if(empty($this->_info) === true)
			return(false);

		return($this->_set('edit',$arr));
	}

	function _set($action,$arr)
	{
		$this->_reset();

		if(($action !== 'add' && $action !== 'edit') === true
		|| xivo_issa('callfilter',$arr) === false)
			return(false);

		$this->set_callfilter($arr['callfilter']);
	
		switch($this->_param['type'])
		{
			case 'bosssecretary':
				if(xivo_issa('callfiltermember',$arr) === false)
					return(false);

				$this->set_callfiltermember($arr['callfiltermember']);
				break;
			default:
				return(false);
		}

		if(xivo_issa('dialstatus',$arr) === true)
			$this->set_dialstatus($arr['dialstatus']);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_callfilter($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('callfilter',null);
			$this->_set_error('callfilter','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_callfilter->chk_values($arr)) === false)
		{
			$this->_set_result('callfilter',$this->_callfilter->get_filter_result());
			$this->_set_error('callfilter',$this->_callfilter->get_filter_error());
			return(false);
		}
		else if($this->_param['type'] !== $rs['type']
		|| $this->_callfilter->chk_element_value($this->_param['type'],$rs[$this->_param['type']]) === false)
		{
			$rs[$this->_param['type']] = '';
			$this->_set_result('callfilter',$rs);
			$this->_set_error('callfilter','invalid '.$this->_param['type']);
			return(false);
		}

		$this->_set_result('callfilter',$rs);

		return(true);
	}

	function set_callfiltermember($arr)
	{
		switch($this->_param['type'])
		{
			case 'bosssecretary':
				return($this->_set_callfilterbs($arr));
		}

		return(false);
	}

	function _set_callfilterbs($list)
	{
		if(is_array($list) === false
		|| isset($list['boss'],$list['secretary']) === false
		|| is_array($list['boss']) === false
		|| isset($list['boss']['id']) === false
		|| is_array($list['secretary']) === false
		|| ($nb = count($list['secretary'])) === 0)
		{
			$this->_set_result('callfiltermember',null);
			$this->_set_error('callfiltermember','invalid data');
			return(false);
		}

		$secretarylist = array_values($list['secretary']);

		$tmp = $reslist = array();

		$info = array();
		$info['type'] = 'user';
		$info['bstype'] = 'boss';
		$info['callfilterid'] = 0;
		$info['priority'] = 0;
		$info['ringseconds'] = 0;
		$info['active'] = false;

		if(($boss = $this->_userfeatures->get_where(array('bsfilter' => 'boss'),true,null,$list['boss']['id'])) === false)
		{
			$this->_set_result('callfiltermember',null);
			$this->_set_error('callfiltermember','wrong boss');
			return(false);
		}

		$info['typeval'] = $boss['id'];
		
		if(isset($list['boss']['ringseconds']) === true)
			$info['ringseconds'] = $list['boss']['ringseconds'];

		if(($rs = $this->_callfiltermember->chk_values($info)) === false)
		{
			$this->_set_result('callfiltermember',$this->_callfiltermember->get_filter_result());
			$this->_set_error('callfiltermember',$this->_callfiltermember->get_filter_error());
			return(false);
		}

		$reslist['boss'] = $rs;
		$tmp[$boss['id']] = 1;

		$reslist['secretary'] = array();
		$info['bstype'] = 'secretary';
		$info['ringseconds'] = 0;

		$where = array();
		$where['bsfilter'] = 'secretary';

		for($i = 0;$i < $nb;$i++)
		{
			if(($secretary = $this->_userfeatures->get_where($where,true,null,$secretarylist[$i])) === false
			|| isset($tmp[$secretary['id']]) === true
			|| $boss['context'] !== $secretary['context'])
			{
				$this->_set_result('callfiltermember',$reslist);
				$this->_set_error('callfiltermember','wrong secretary');
				return(false);
			}

			$info['priority'] = $i;
			$info['typeval'] = $secretary['id'];

			if(($rs = $this->_callfiltermember->chk_values($info)) === false)
			{
				$this->_set_result('callfiltermember',$reslist);
				$this->_set_error('callfiltermember','wrong secretary');
				return(false);
			}

			$reslist['secretary'][] = $rs;
			$tmp[$secretary['id']] = 1;
		}

		if(isset($reslist['secretary'][0]) === false)
		{
			$this->_set_result('callfiltermember',false);
			return(false);
		}

		$this->_set_result('callfiltermember',$reslist);

		return(true);
	}

	function set_dialstatus($list)
	{
		if(isset($list['noanswer']) === true)
			$list = array('noanswer' => $list['noanswer']);
		else
			$list = array();

		if(($r = $this->_dialstatus->set($list,'callfilter')) === false)
			$this->_set_error('dialstatus',$this->_dialstatus->get_error());

		$this->_set_result('dialstatus',$this->_dialstatus->get_result());

		return($r);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$callfilterid = null;

		if($this->get_errnb() > 0
		|| ($callfilterid = $this->add_callfilter()) === false
		|| $this->add_callfiltermember($callfilterid) === false
		|| $this->add_dialstatus($callfilterid) === false)
		{
			$r = false;

			if($callfilterid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_callfilter()
	{
		if(($rs = $this->get_result('callfilter')) === null)
			return(false);

		$this->_return['callfilter'] = $this->_callfilter->add($rs);

		return($this->_return['callfilter']);
	}

	function add_callfiltermember($callfilterid)
	{
		switch($this->_param['type'])
		{
			case 'bosssecretary':
				return($this->_add_callfilterbs($callfilterid));
		}

		return(false);
	}

	function _add_callfilterbs($callfilterid)
	{
		if(($callfilterid = xivo_ulongint($callfilterid)) === 0
		|| ($rs = $this->get_result('callfiltermember')) === null
		|| $rs === false
		|| isset($rs['boss'],$rs['secretary']) === false
		|| ($nb = count($rs['secretary'])) === 0)
			return(false);

		$this->_return['callfiltermember'] = array();

		$where = array(); 
		$where['type'] = $rs['boss']['type'];
		$where['typeval'] = $rs['boss']['typeval'];
		$where['bstype'] = $rs['boss']['bstype'];

		$rs['boss']['callfilterid'] = $callfilterid;

		if($this->_callfiltermember->get_primary($where) !== false
		|| ($this->_return['callfiltermember'][] = $this->_callfiltermember->add($rs['boss'])) === false)
			return(false);

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs['secretary'][$i];
			$ref['callfilterid'] = $callfilterid;

			if(($this->_return['callfiltermember'][] = $this->_callfiltermember->add($ref)) === false)
				return(false);
		}

		if(isset($this->_return['callfiltermember'][1]) === false)
			$this->_return['callfiltermember'] = false;

		return(true);
	}

	function add_dialstatus($callfilterid)
	{
		$r = $this->_dialstatus->add($callfilterid);

		$this->_return['dialstatus'] = $this->_dialstatus->get_return();

		return($r);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'callfilter':
					$this->_callfilter->delete($val);
					break;
				case 'callfiltermember':
					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($i = 0;$i < $nb;$i++)
					{
						if($val[$i] !== false)
							$this->_callfiltermember->delete($val[$i]);
					}
					break;
				case 'dialstatus':
					$this->_dialstatus->reverse_add();
					break;
			}
		}

		return(true);
	}

	function edit()
	{
		$r = true;

		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_callfilter() === false
		|| $this->edit_callfiltermember() === false
		|| $this->edit_dialstatus() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_callfilter()
	{
		if(xivo_issa('callfilter',$this->_info) === false
		|| ($rs = $this->get_result('callfilter')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['callfilter']['commented'];

		$this->_return['callfilter'] = $this->_callfilter->edit($this->_info['callfilter']['id'],$rs);

		return($this->_return['callfilter']);
	}

	function edit_callfiltermember()
	{
		switch($this->_param['type'])
		{
			case 'bosssecretary':
				return($this->_edit_callfilterbs());
		}

		return(false);
	}

	function _edit_callfilterbs()
	{
		if(xivo_issa('callfilter',$this->_info) === false)
			return(false);

		$id = $this->_info['callfilter']['id'];

		if(xivo_issa('callfiltermember',$this->_info) === false)
			return($this->_add_callfilterbs($id));

		$this->_return['callfiltermember'] = $this->delete_callfiltermember();

		if(($rs = $this->get_result('callfiltermember')) === null
		|| $rs === false
		|| isset($rs['boss'],$rs['secretary']) === false
		|| ($nb = count($rs['secretary'])) === 0)
			return(false);

		$this->_return['callfiltermember'] = array();

		$where = array(); 
		$where['type'] = $rs['boss']['type'];
		$where['typeval'] = $rs['boss']['typeval'];
		$where['bstype'] = $rs['boss']['bstype'];

		$rs['boss']['callfilterid'] = $id;

		if(xivo_issa('boss',$this->_info['callfiltermember']) === true)
		{
			$cid = $this->_info['callfiltermember']['boss']['id'];
			$rs['boss']['active'] = $this->_info['callfiltermember']['boss']['active'];
		}
		else
			$cid = 0;

		if($this->_callfiltermember->get_primary($where) !== false
		|| ($this->_return['callfiltermember'][] = $this->_callfiltermember->add($rs['boss'],$cid)) === false)
			return(false);

		if(xivo_issa('secretary',$this->_info['callfiltermember']) === true)
		{
			$secretary = &$this->_info['callfiltermember']['secretary'];
			$cnt = count($secretary);
		}
		else
		{
			$secretary = null;
			$cnt = 0;
		}

		for($i = 0,$j = 1;$i < $nb;$i++,$j++)
		{
			$cid = 0;
			$ref = &$rs['secretary'][$i];

			for($k = 0;$k < $cnt;$k++)
			{
				if($secretary[$k]['type'] === $ref['type']
				&& (string) $secretary[$k]['typeval'] === (string) $ref['typeval'])
				{
					$ref['active'] = $secretary[$k]['active'];	
					$cid = $secretary[$k]['id'];
				}
			}

			if($cid === 0 && isset($secretary[$j]) === true)
				$cid = $secretary[$j]['id'];

			$ref['callfilterid'] = $id;

			if(($this->_return['callfiltermember'][] = $this->_callfiltermember->add($ref,$cid)) === false)
				return(false);
		}

		if(isset($this->_return['callfiltermember'][1]) === false)
			$this->_return['callfiltermember'] = false;

		return(true);
	}

	function edit_dialstatus()
	{
		if(xivo_issa('callfilter',$this->_info) === false)
			return(false);

		$r = $this->_dialstatus->edit($this->_status,'callfilter',$this->_info['callfilter']['id']);

		$this->_return['dialstatus'] = $this->_dialstatus->get_return();

		return($r);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'callfilter':
					$this->_callfilter->edit_origin($this->_origin['callfilter']);
					break;
				case 'callfiltermember':
					$this->delete_callfiltermember();

					$this->_callfiltermember->add_origin_list($this->_origin[$key]);
					break;
				case 'dialstatus':
					if(xivo_issa('callfilter',$this->_info) === true)
						$this->_dialstatus->reverse_edit($this->_status,'callfilter',
										 $this->_info['callfilter']['id']);
					break;
			}
		}

		return(true);
	}

	function delete()
	{
		$r = true;

		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_callfilter() === false
		|| $this->delete_callfiltermember() === false
		|| $this->delete_dialstatus() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}
		else $this->unlinks();

		$this->_status = '';

		return($r);
	}

	function delete_callfilter()
	{
		if(xivo_issa('callfilter',$this->_info) === false)
			return(false);

		$r = $this->_callfilter->delete($this->_info['callfilter']['id']);

		if($this->_status === 'delete')
			$this->_return['callfilter'] = $r;

		return($r);
	}

	function delete_callfiltermember()
	{
		switch($this->_param['type'])
		{
			case 'bosssecretary':
				return($this->_delete_callfilterbs());
		}

		return(false);
	}

	function _delete_callfilterbs()
	{
		if(xivo_issa('callfiltermember',$this->_info) === false
		|| (isset($this->_info['callfiltermember']['boss']) === false
		   && isset($this->_info['callfiltermember']['secretary']) === false) === true)
			return(null);
		else if(xivo_issa('callfilter',$this->_info) === false)
			return(false);

		$callfiltermember_where = array();
		$callfiltermember_where['type'] = 'user';
		$callfiltermember_where['callfilterid'] = $this->_info['callfilter']['id'];

		$r = $this->_callfiltermember->delete_where($callfiltermember_where);

		if($this->_status === 'delete')
			$this->_return['callfiltermember'] = $r;

		return($r);
	}

	function delete_dialstatus()
	{
		if(xivo_issa('callfilter',$this->_info) === false)
			$id = 0;
		else
			$id = $this->_info['callfilter']['id'];

		$r = $this->_dialstatus->delete($this->_status,'callfilter',$id);

		if($this->_status === 'delete')
			$this->_return['dialstatus'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'callfilter':
					$this->_callfilter->add_origin($this->_origin['callfilter']);
					break;
				case 'callfiltermember':
					$this->_callfiltermember->add_origin_list($this->_origin[$key]);
					break;
				case 'dialstatus':
					$this->_dialstatus->add_origin_list();
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('callfilter',$this->_info) === false)
			return(false);

		$this->_dialstatus->unlinked('callfilter',$this->_info['callfilter']['id']);
		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_callfilter() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;

		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_callfilter() === false)
			$r = false;

		$this->_status = '';

		return($r);
	}

	function enable_callfilter()
	{
		return($this->_enable_disable_callfilter(false));
	}

	function disable_callfilter()
	{
		return($this->_enable_disable_callfilter(true));
	}

	function _enable_disable_callfilter($disable=false)
	{
		$disable = (bool) $disable;

		if(xivo_issa('callfilter',$this->_info) === false)
			return(false);
		else if($disable === false)
			$r = $this->_callfilter->enable($this->_info['callfilter']['id']);
		else
			$r = $this->_callfilter->disable($this->_info['callfilter']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['callfilter'] = $r;

		return($r);
	}
}

?>
