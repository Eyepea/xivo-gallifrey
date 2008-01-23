<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_rightcall extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_rightcall		= null;
	var $_rightcallmember	= null;
	var $_rightcallexten	= null;

	function xivo_application_service_asterisk_rightcall(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_rightcall = &$this->_service->get_module('rightcall')) === false
		|| ($this->_rightcallmember = &$this->_service->get_module('rightcallmember')) === false
		|| ($this->_rightcallexten = &$this->_service->get_module('rightcallexten')) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['rightcall'] = $this->_rightcall->get($id)) === false)
			return(false);

		$this->_origin['rightcall'] = $this->_rightcall->get_origin();

		$this->_get_rightcalluser();
		$this->_get_rightcallgroup();
		$this->_get_rightcallincall();
		$this->_get_rightcalloutcall();
		$this->_get_rightcallexten();

		return($this->_info);
	}

	function _get_rightcalluser()
	{
		return($this->_get_rightcall_member_type('user'));
	}

	function _get_rightcallgroup()
	{
		return($this->_get_rightcall_member_type('group'));
	}

	function _get_rightcallincall()
	{
		return($this->_get_rightcall_member_type('incall'));
	}

	function _get_rightcalloutcall()
	{
		return($this->_get_rightcall_member_type('outcall'));
	}

	function _get_rightcall_member_type($type)
	{
		if($this->_rightcallmember->chk_element_value('type',$type) === false)
			return(false);

		$membertype = 'rightcall'.$type;

		$this->_info[$membertype] = false;
		$this->_origin[$membertype] = false;

		$where = array();
		$where['type'] = $type;
		$where['rightcallid'] = $this->_info['rightcall']['id'];

		if(($this->_info[$membertype] = $this->_rightcallmember->get_all_where($where)) === false)
			return(false);

		$this->_origin[$membertype] = $this->_rightcallmember->get_origin_list();

		return($this->_info[$membertype]);
	}

	function _get_rightcallexten()
	{
		$this->_info['rightcallexten'] = false;
		$this->_origin['rightcallexten'] = false;

		$where = array();
		$where['rightcallid'] = $this->_info['rightcall']['id'];

		if(($this->_info['rightcallexten'] = $this->_rightcallexten->get_all_where($where)) === false)
			return(false);

		$this->_origin['rightcallexten'] = $this->_rightcallexten->get_origin_list();

		return($this->_info['rightcallexten']);
	}

	function get_elements()
	{
		$r = array();

		$r['rightcall'] = $this->_rightcall->get_element();
		$r['rightcallmember'] = $this->_rightcallmember->get_element();
		$r['rightcallexten'] = $this->_rightcallexten->get_element();

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
		|| xivo_issa('rightcall',$arr) === false)
			return(false);

		$this->set_rightcall($arr['rightcall']);

		if(xivo_issa('rightcalluser',$arr) === true)
			$this->set_rightcalluser($arr['rightcalluser']);

		if(xivo_issa('rightcallgroup',$arr) === true)
			$this->set_rightcallgroup($arr['rightcallgroup']);

		if(xivo_issa('rightcallincall',$arr) === true)
			$this->set_rightcallincall($arr['rightcallincall']);

		if(xivo_issa('rightcalloutcall',$arr) === true)
			$this->set_rightcalloutcall($arr['rightcalloutcall']);

		if(xivo_issa('rightcallexten',$arr) === true)
			$this->set_rightcallexten($arr['rightcallexten']);
		else
		{
			$this->_set_result('rightcallexten',null);
			$this->_set_error('rightcallexten','invalid data');
		}

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_rightcall($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('rightcall',null);
			$this->_set_error('rightcall','invalid data');
			return(false);
		}
		else if(($rs = $this->_rightcall->chk_values($arr)) === false)
		{
			$this->_set_result('rightcall',$this->_rightcall->get_filter_result());
			$this->_set_error('rightcall',$this->_rightcall->get_filter_error());
			return(false);
		}

		$this->_set_result('rightcall',$rs);

		return(true);
	}

	function set_rightcalluser($list)
	{
		return($this->_set_rightcall_member_type('user',$list));
	}

	function set_rightcallgroup($list)
	{
		return($this->_set_rightcall_member_type('group',$list));
	}

	function set_rightcallincall($list)
	{
		return($this->_set_rightcall_member_type('incall',$list));
	}

	function set_rightcalloutcall($list)
	{
		return($this->_set_rightcall_member_type('outcall',$list));
	}

	function _set_rightcall_member_type($type,$list)
	{
		if($this->_rightcallmember->chk_element_value('type',$type) === false
		|| is_array($list) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === 0)
			return(null);
		
		$info = array();
		$info['rightcallid'] = 0;
		$info['type'] = $type;
		$info['typeval'] = 0;

		$key = 'rightcall'.$type;

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($valid = $this->_rightcallmember->is_valid($type,$list[$i])) === false
			|| isset($tmp[$valid['id']]) === true)
				continue;

			$info['typeval'] = $valid['id'];

			if(($rs = $this->_rightcallmember->chk_values($info)) === false)
				continue;

			$tmp[$valid['id']] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result($key,false);
			return(false);
		}

		$this->_set_result($key,$reslist);

		return(true);
	}

	function set_rightcallexten($list)
	{
		if(is_array($list) === false
		|| ($list = array_values($list)) === false
		|| ($nb = count($list)) === 0)
		{
			$this->_set_result('rightcallexten',null);
			$this->_set_error('rightcallexten','invalid data');
			return(false);
		}

		$info = array();
		$info['rightcallid'] = 0;
		$info['exten'] = '';

		$tmp = $reslist = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($exten = $this->_service->chk_extension($list[$i])) === false)
			{
				$this->_set_error('rightcallexten','wrong extension');
				continue;
			}
			
			$extenhash = sha1($exten);
			$info['exten'] = $list[$i];

			if(($rs = $this->_rightcallexten->chk_values($info)) === false
			|| isset($tmp[$extenhash]) === true)
				continue;

			$tmp[$extenhash] = 1;
			$reslist[] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('rightcallexten',false);
			$this->_set_error('rightcallexten','empty');
			return(false);
		}

		$this->_set_result('rightcallexten',$reslist);

		return(true);
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$rightcallid = null;

		if($this->get_errnb() > 0
		|| ($rightcallid = $this->add_rightcall()) === false
		|| $this->add_rightcalluser($rightcallid) === false
		|| $this->add_rightcallgroup($rightcallid) === false
		|| $this->add_rightcallincall($rightcallid) === false
		|| $this->add_rightcalloutcall($rightcallid) === false
		|| $this->add_rightcallexten($rightcallid) === false)
		{
			$r = false;
			if($rightcallid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_rightcall()
	{
		if(($rs = $this->get_result('rightcall')) === null)
			return(false);

		$this->_return['rightcall'] = $this->_rightcall->add($rs);

		return($this->_return['rightcall']);
	}

	function add_rightcalluser($rightcallid)
	{
		return($this->_add_rightcall_member_type('user',$rightcallid));
	}

	function add_rightcallgroup($rightcallid)
	{
		return($this->_add_rightcall_member_type('group',$rightcallid));
	}

	function add_rightcallincall($rightcallid)
	{
		return($this->_add_rightcall_member_type('incall',$rightcallid));
	}

	function add_rightcalloutcall($rightcallid)
	{
		return($this->_add_rightcall_member_type('outcall',$rightcallid));
	}

	function _add_rightcall_member_type($type,$rightcallid)
	{
		if($this->_rightcallmember->chk_element_value('type',$type) === false
		|| ($rightcallid = xivo_ulongint($rightcallid)) === 0)
			return(false);

		$key = 'rightcall'.$type;

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);

		$this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['rightcallid'] = $rightcallid;

			if(($this->_return[$key][] = $this->_rightcallmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function add_rightcallexten($rightcallid)
	{
		if(($rightcallid = xivo_ulongint($rightcallid)) === 0)
			return(false);
		else if(($rs = $this->get_result('rightcallexten')) === null || $rs === false)
			return($rs);

		$this->_return['rightcallexten'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['rightcallid'] = $rightcallid;

			if(($this->_return['rightcallexten'][] = $this->_rightcallexten->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return['rightcallexten'][0]) === false)
			$this->_return['rightcallexten'] = false;

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'rightcall':
					$this->_rightcall->delete($val);
					break;
				case 'rightcalluser':
				case 'rightcallgroup':
				case 'rightcallincall':
				case 'rightcalloutcall':
					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$this->_rightcallmember->delete($val[$j]);
					}
					break;
				case 'rightcallexten':
					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$this->_rightcallexten->delete($val[$j]);
					}
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
		|| $this->edit_rightcall() === false
		|| $this->edit_rightcalluser() === false
		|| $this->edit_rightcallgroup() === false
		|| $this->edit_rightcallincall() === false
		|| $this->edit_rightcalloutcall() === false
		|| $this->edit_rightcallexten() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_rightcall()
	{
		if(xivo_issa('rightcall',$this->_info) === false
		|| ($rs = $this->get_result('rightcall')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['rightcall']['commented'];

		$this->_return['rightcall'] = $this->_rightcall->edit($this->_info['rightcall']['id'],$rs);

		return($this->_return['rightcall']);
	}

	function edit_rightcalluser()
	{
		return($this->_edit_rightcall_member_type('user'));
	}

	function edit_rightcallgroup()
	{
		return($this->_edit_rightcall_member_type('group'));
	}

	function edit_rightcallincall()
	{
		return($this->_edit_rightcall_member_type('incall'));
	}

	function edit_rightcalloutcall()
	{
		return($this->_edit_rightcall_member_type('outcall'));
	}

	function _edit_rightcall_member_type($type)
	{
		if($this->_rightcallmember->chk_element_value('type',$type) === false
		|| xivo_issa('rightcall',$this->_info) === false)
			return(false);

		$id = $this->_info['rightcall']['id'];

		$key = 'rightcall'.$type;

		if(xivo_issa($key,$this->_info) === false)
			return($this->_add_rightcall_member_type($type,$id));

		$this->_return[$key] = $this->_delete_rightcall_member_type($type);

		if(($rs = $this->get_result($key)) === null)
			return($this->_return[$key]);
		else if($rs === false)
			return(false);

		$this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info[$key][$i]) === true)
				$rid = $this->_info[$key][$i]['id'];
			else
				$rid = 0;

			$rs[$i]['rightcallid'] = $id;

			if(($this->_return[$key][] = $this->_rightcallmember->add($rs[$i],$rid)) === false)
				return(false);
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function edit_rightcallexten()
	{
		if(xivo_issa('rightcall',$this->_info) === false)
			return(false);

		$id = $this->_info['rightcall']['id'];

		if(xivo_issa('rightcallexten',$this->_info) === false)
			return($this->add_rightcallexten($id));

		$this->_return['rightcallexten'] = $this->delete_rightcallexten();

		if(($rs = $this->get_result('rightcallexten')) === null)
			return($this->_return['rightcallexten']);
		else if($rs === false)
			return(false);
	
		$this->_return['rightcallexten'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info['rightcallexten'][$i]) === true)
				$rid = $this->_info['rightcallexten'][$i]['id'];
			else
				$rid = 0;

			$rs[$i]['rightcallid'] = $id;

			if(($this->_return['rightcallexten'][] = $this->_rightcallexten->add($rs[$i],$rid)) === false)
				return(false);
		}

		if(isset($this->_return['rightcallexten'][0]) === false)
			$this->_return['rightcallexten'] = false;

		return(true);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'edit'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$this->_return[$key];

			if($val === false)
				continue;

			switch($key)
			{
				case 'rightcall':
					$this->_rightcall->edit_origin($this->_origin['rightcall']);
					break;
				case 'rightcalluser':
				case 'rightcallgroup':
				case 'rightcallincall':
				case 'rightcalloutcall':
					$this->_delete_rightcall_member_type(substr($key,9));

					$this->_rightcallmember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcallexten':
					$this->delete_rightcallexten();

					$this->_rightcallexten->add_origin_list($this->_origin['rightcallexten']);
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

		if($this->delete_rightcall() === false
		|| $this->delete_rightcalluser() === false
		|| $this->delete_rightcallgroup() === false
		|| $this->delete_rightcallincall() === false
		|| $this->delete_rightcalloutcall() === false
		|| $this->delete_rightcallexten() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_rightcall()
	{
		if(xivo_issa('rightcall',$this->_info) === false)
			return(false);

		$r = $this->_rightcall->delete($this->_info['rightcall']['id']);

		if($this->_status === 'delete')
			$this->_return['rightcall'] = $r;

		return($r);
	}

	function delete_rightcalluser()
	{
		return($this->_delete_rightcall_member_type('user'));
	}

	function delete_rightcallgroup()
	{
		return($this->_delete_rightcall_member_type('group'));
	}

	function delete_rightcallincall()
	{
		return($this->_delete_rightcall_member_type('incall'));
	}

	function delete_rightcalloutcall()
	{
		return($this->_delete_rightcall_member_type('outcall'));
	}

	function _delete_rightcall_member_type($type)
	{
		if($this->_rightcallmember->chk_element_value('type',$type) === false)
			return(false);

		$key = 'rightcall'.$type;

		if(xivo_issa($key,$this->_info) === false)
			return(null);

		if(xivo_issa('rightcall',$this->_info) === false)
			return(false);

		$rightcallmember_where = array();
		$rightcallmember_where['type'] = $type;
		$rightcallmember_where['rightcallid'] = $this->_info['rightcall']['id'];

		$r = $this->_rightcallmember->delete_where($rightcallmember_where);

		if($this->_status === 'delete')
			$this->_return[$key] = $r;

		return($r);
	}

	function delete_rightcallexten()
	{
		if(xivo_issa('rightcallexten',$this->_info) === false)
			return(null);

		if(xivo_issa('rightcall',$this->_info) === false)
			return(false);

		$rightcallexten_where = array('rightcallid' => $this->_info['rightcall']['id']);

		$r = $this->_rightcallexten->delete_where($rightcallexten_where);

		if($this->_status === 'delete')
			$this->_return['rightcallexten'] = $r;

		return($r);
	}

	function _reverse_delete()
	{
		if($this->_status !== 'delete'
		|| ($arr = xivo_get_aks($this->_return)) === false)
			return(false);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];

			if($this->_return[$key] === false)
				continue;

			switch($key)
			{
				case 'rightcall':
					$this->_rightcall->add_origin($this->_origin['rightcall']);
					break;
				case 'rightcalluser':
				case 'rightcallgroup':
				case 'rightcallincall':
				case 'rightcalloutcall':
					$this->_rightcallmember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcallexten':
					$this->_rightcallexten->add_origin_list($this->_origin['rightcallexten']);
					break;
			}
		}
		return(true);
	}
}

?>
