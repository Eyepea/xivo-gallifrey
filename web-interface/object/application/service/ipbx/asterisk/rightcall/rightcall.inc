<?php

require_once(XIVO_PATH_OBJECT.XIVO_SEP_DIR.'application'.XIVO_SEP_DIR.'abstract.inc');

class xivo_application_service_asterisk_rightcall extends xivo_application_abstract
{
	var $_ipbx	= null;
	var $_origin	= array();

	function xivo_application_service_asterisk_rightcall(&$ipbx)
	{
		if(is_object($ipbx) === false || get_class($ipbx) !== 'xivo_service_asterisk')
			trigger_error('Invalid service in '.__CLASS__,E_USER_ERROR);

		$this->_ipbx = &$ipbx;
	}

	function get($id,$disable=null)
	{
		$this->_info = $this->_origin = array();

		if(($rightcall = &$this->_ipbx->get_module('rightcall')) === false
		|| ($this->_info['rightcall'] = $rightcall->get($id)) === false)
			return(false);

		$this->_origin['rightcall'] = $rightcall->get_origin();

		$this->_get_rightcalluser();
		$this->_get_rightcallgroup();
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

	function _get_rightcalloutcall()
	{
		return($this->_get_rightcall_member_type('outcall'));
	}

	function _get_rightcall_member_type($type)
	{
		if($type !== 'user'
		&& $type !== 'group'
		&& $type !== 'outcall')
			return(false);

		$membertype = 'rightcall'.$type;

		$this->_info[$membertype] = false;
		$this->_origin[$membertype] = false;

		$where = array();
		$where['type'] = $type;
		$where['rightcallid'] = $this->_info['rightcall']['id'];

		if(($rmember = &$this->_ipbx->get_module('rightcallmember')) === false
		|| ($this->_info[$membertype] = $rmember->get_all_where($where)) === false)
			return(false);

		$this->_origin[$membertype] = $rmember->get_origin_list();

		return($this->_info[$membertype]);
	}

	function _get_rightcallexten()
	{
		$this->_info['rightcallexten'] = false;
		$this->_origin['rightcallexten'] = false;

		$where = array();
		$where['rightcallid'] = $this->_info['rightcall']['id'];

		if(($rexten = &$this->_ipbx->get_module('rightcallexten')) === false
		|| ($this->_info['rightcallexten'] = $rexten->get_all_where($where)) === false)
			return(false);

		$this->_origin['rightcallexten'] = $rexten->get_origin_list();

		return($this->_info['rightcallexten']);
	}

	function get_element()
	{
		$r = array();

		if(($rightcall = &$this->_ipbx->get_module('rightcall')) !== false)
			$r['rightcall'] = $rightcall->get_element();
		else
			$r['rightcall'] = false;

		if(($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) !== false)
			$r['rightcallmember'] = $rightcallmember->get_element();
		else
			$r['rightcallmember'] = false;

		if(($rightcallexten = &$this->_ipbx->get_module('rightcallexten')) !== false)
			$r['rightcallexten'] = $rightcallexten->get_element();
		else
			$r['rightcallexten'] = false;

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

		if(xivo_issa('rightcalloutcall',$arr) === true)
			$this->set_rightcalloutcall($arr['rightcalloutcall']);

		if(xivo_issa('rightcallexten',$arr) === true)
			$this->set_rightcallexten($arr['rightcallexten']);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_rightcall($arr)
	{
		if(is_array($arr) === false
		|| ($rightcall = &$this->_ipbx->get_module('rightcall')) === false)
		{
			$this->_set_result('rightcall',null);
			$this->_set_error('rightcall','invalid');
			return(false);
		}

		if(($rs = $rightcall->chk_values($arr)) === false)
		{
			$this->_set_result('rightcall',$rightcall->get_filter_result());
			$this->_set_error('rightcall',$rightcall->get_filter_error());

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

	function set_rightcalloutcall($list)
	{
		return($this->_set_rightcall_member_type('outcall',$list));
	}

	function _set_rightcall_member_type($type,$list)
	{
		if(($type !== 'user'
		   && $type !== 'group'
		   && $type !== 'outcall') === true
		|| is_array($list) === false
		|| ($rightcallmember = &$this->_ipbx->get_module('rightcallmember')) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === false)
			return(null);
		
		$info = array();
		$info['rightcallid'] = 0;
		$info['type'] = $type;
		$info['typeval'] = 0;

		$key = 'rightcall'.$type;

		$tmp = $type_list = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($valid = $rightcallmember->is_valid($type,$list[$i])) === false
			|| isset($tmp[$valid['id']]) === true)
				continue;

			$info['typeval'] = $valid['id'];

			if(($rs = $rightcallmember->chk_values($info)) === false)
				continue;

			$tmp[$valid['id']] = 1;
			$type_list[] = $rs;
		}

		if(isset($type_list[0]) === false)
		{
			$this->_set_result($key,false);
			return(false);
		}

		$this->_set_result($key,$type_list);

		return(true);
	}

	function set_rightcallexten($list)
	{
		if(is_array($list) === false
		|| ($rightcallexten = &$this->_ipbx->get_module('rightcallexten')) === false)
			return(false);

		$list = array_values($list);

		if(($nb = count($list)) === false)
			return(null);

		$info = array();
		$info['rightcallid'] = 0;
		$info['exten'] = '';

		$tmp = $type_list = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(($exten = $this->_ipbx->chk_extension($list[$i])) === false)
			{
				$this->_set_error('rightcallexten','wrong extension');
				continue;
			}
			
			$extenhash = sha1($exten);
			$info['exten'] = $list[$i];

			if(($rs = $rightcallexten->chk_values($info)) === false
			|| isset($tmp[$extenhash]) === true)
				continue;

			$tmp[$extenhash] = 1;
			$type_list[] = $rs;
		}

		if(isset($type_list[0]) === false)
		{
			$this->_set_result('rightcallexten',false);
			return(false);
		}

		$this->_set_result('rightcallexten',$type_list);

		return(true);
	}

	function add()
	{
		$this->_return = array();
		$rightcallid = null;

		if($this->get_errnb() > 0
		|| ($rightcallid = $this->add_rightcall()) === false
		|| $this->add_rightcalluser($rightcallid) === false
		|| $this->add_rightcallgroup($rightcallid) === false
		|| $this->add_rightcalloutcall($rightcallid) === false
		|| $this->add_rightcallexten($rightcallid) === false)
		{
			if($rightcallid !== null)
				$this->_reverse_add();

			return(false);
		}

		return(true);
	}

	function add_rightcall()
	{
		if(($rs = $this->get_result('rightcall')) === null
		|| ($rightcall = &$this->_ipbx->get_module('rightcall')) === false)
			return(false);

		$this->_return['rightcall'] = $rightcall->add($rs);

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

	function add_rightcalloutcall($rightcallid)
	{
		return($this->_add_rightcall_member_type('outcall',$rightcallid));
	}

	function _add_rightcall_member_type($type,$rightcallid)
	{
		if(($type !== 'user'
		   && $type !== 'group'
		   && $type !== 'outcall') === true
		|| ($rightcallid = xivo_uint($rightcallid)) === 0)
			return(false);

		$key = 'rightcall'.$type;

		if(($rs = $this->get_result($key)) === null || $rs === false)
			return($rs);

		$rmember = &$this->_ipbx->get_module('rightcallmember');

		$this->_return[$key] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['rightcallid'] = $rightcallid;

			if(($this->_return[$key][] = $rmember->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return[$key][0]) === false)
			$this->_return[$key] = false;

		return(true);
	}

	function add_rightcallexten($rightcallid)
	{
		if(($rightcallid = xivo_uint($rightcallid)) === 0)
			return(false);

		if(($rs = $this->get_result('rightcallexten')) === null || $rs === false)
			return($rs);

		$rightcallexten = &$this->_ipbx->get_module('rightcallexten');

		$this->_return['rightcallexten'] = array();

		$nb = count($rs);

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['rightcallid'] = $rightcallid;

			if(($this->_return['rightcallexten'][] = $rightcallexten->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return['rightcallexten'][0]) === false)
			$this->_return['rightcallexten'] = false;

		return(true);
	}

	function _reverse_add()
	{
		if($this->get_errnb() > 0
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
					$rightcall = &$this->_ipbx->get_module('rightcall');
					$rightcall->delete($val);
					break;
				case 'rightcalluser':
				case 'rightcallgroup':
				case 'rightcalloutcall':
					$rmember = &$this->_ipbx->get_module('rightcallmember');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$rmember->delete($val[$j]);
					}
					break;
				case 'rightcallexten':
					$rightcallexten = &$this->_ipbx->get_module('rightcallexten');

					if(is_array($val) === false || ($nb = count($val)) === 0)
						continue;

					for($j = 0;$j < $nb;$j++)
					{
						if($val[$j] !== false)
							$rightcallexten->delete($val[$j]);
					}
					break;
			}
		}
		return(true);
	}

	function edit()
	{
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_rightcall() === false
		|| $this->edit_rightcalluser() === false
		|| $this->edit_rightcallgroup() === false
		|| $this->edit_rightcalloutcall() === false
		|| $this->edit_rightcallexten() === false)
		{
			$this->_reverse_edit();
			return(false);
		}

		return(true);
	}

	function edit_rightcall()
	{
		if(xivo_issa('rightcall',$this->_info) === false
		|| ($rs = $this->get_result('rightcall')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['rightcall']['commented'];

		$rightcall = &$this->_ipbx->get_module('rightcall');
		$this->_return['rightcall'] = $rightcall->edit($this->_info['rightcall']['id'],$rs);

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

	function edit_rightcalloutcall()
	{
		return($this->_edit_rightcall_member_type('outcall'));
	}

	function _edit_rightcall_member_type($type)
	{
		if(($type !== 'user'
		   && $type !== 'group'
		   && $type !== 'outcall') === true
		|| xivo_issa('rightcall',$this->_info) === false)
			return(false);

		$key = 'rightcall'.$type;

		if(xivo_issa($key,$this->_info) === false)
			return($this->_add_rightcall_member_type($type,$this->_info['rightcall']['id']));

		$this->_return[$key] = $this->_delete_rightcall_member_type($type);

		if(($rs = $this->get_result($key)) === null)
			return($this->_return[$key]);
		else if($rs === false)
			return(false);

		$rmember = &$this->_ipbx->get_module('rightcallmember');

		$this->_return[$key] = array();

		$i = 0;
		$nb = count($rs);

		if(xivo_issa($key,$this->_info) !== false
		&& ($cnt = count($this->_info[$key])) !== 0)
		{
			for($i;$i < $cnt;$i++)
			{
				if(isset($rs[$i]) === false)
				{
					$i = $nb;
					break;
				}

				$id = $this->_info[$key][$i]['id'];
				$rs[$i]['rightcallid'] = $this->_info['rightcall']['id'];

				if(($this->_return[$key][] = $rmember->add($rs[$i],$id)) === false)
					return(false);
			}
		}

		if($i >= $nb)
		{
			if(isset($this->_return[$key][0]) === false)
				$this->_return[$key] = false;

			return(true);
		}

		for($i;$i < $nb;$i++)
		{
			$rs[$i]['rightcallid'] = $this->_info['rightcall']['id'];

			if(($this->_return[$key][] = $rmember->add($rs[$i])) === false)
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

		if(xivo_issa('rightcallexten',$this->_info) === false)
			return($this->add_rightcallexten($this->_info['rightcall']['id']));

		$this->_return['rightcallexten'] = $this->delete_rightcallexten();

		if(($rs = $this->get_result('rightcallexten')) === null)
			return($this->_return['rightcallexten']);
		else if($rs === false)
			return(false);
	
		$rightcallexten = &$this->_ipbx->get_module('rightcallexten');

		$this->_return['rightcallexten'] = array();

		$i = 0;
		$nb = count($rs);

		if(xivo_issa('rightcallexten',$this->_info) !== false
		&& ($cnt = count($this->_info['rightcallexten'])) !== 0)
		{
			for($i;$i < $cnt;$i++)
			{
				if(isset($rs[$i]) === false)
				{
					$i = $nb;
					break;
				}

				$id = $this->_info['rightcallexten'][$i]['id'];
				$rs[$i]['rightcallid'] = $this->_info['rightcall']['id'];

				if(($this->_return['rightcallexten'][] = $rightcallexten->add($rs[$i],$id)) === false)
					return(false);
			}
		}

		if($i >= $nb)
		{
			if(isset($this->_return['rightcallexten'][0]) === false)
				$this->_return['rightcallexten'] = false;

			return(true);
		}

		for($i;$i < $nb;$i++)
		{
			$rs[$i]['rightcallid'] = $this->_info['rightcall']['id'];

			if(($this->_return['rightcallexten'][] = $rightcallexten->add($rs[$i])) === false)
				return(false);
		}

		if(isset($this->_return['rightcallexten'][0]) === false)
			$this->_return['rightcallexten'] = false;

		return(true);
	}

	function _reverse_edit()
	{
		if($this->get_errnb() > 0
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
					$rightcall = &$this->_ipbx->get_module('rightcall');
					$rightcall->set_origin($this->_origin['rightcall']);
					$rightcall->edit_origin();
					break;
				case 'rightcalluser':
				case 'rightcallgroup':
				case 'rightcalloutcall':
					$type = substr($key,9);
					$this->_delete_rightcall_member_type($type);

					if($this->_origin[$key] === false
					|| ($nb = count($this->_origin[$key])) === 0)
						continue;

					$rmember = &$this->_ipbx->get_module('rightcallmember');

					for($j = 0;$j < $nb;$j++)
					{
						$ref = &$this->_origin[$key][$j];
						$rmember->add($ref,$ref['id']);
					}
					break;
				case 'rightcallexten':
					$this->delete_rightcallexten();

					if($this->_origin['rightcallexten'] === false
					|| ($nb = count($this->_origin['rightcallexten'])) === 0)
						continue;

					$rightcallexten = &$this->_ipbx->get_module('rightcallexten');

					for($j = 0;$j < $nb;$j++)
					{
						$ref = &$this->_origin['rightcallexten'][$j];
						$rightcallexten->add($ref,$ref['id']);
					}
					break;
			}
		}
		return(true);
	}

	function delete_rightcalluser()
	{
		return($this->_delete_rightcall_member_type('user'));
	}

	function delete_rightcallgroup()
	{
		return($this->_delete_rightcall_member_type('group'));
	}

	function delete_rightcalloutcall()
	{
		return($this->_delete_rightcall_member_type('outcall'));
	}

	function _delete_rightcall_member_type($type)
	{
		if(($type !== 'user'
		   && $type !== 'group'
		   && $type !== 'outcall') === true
		|| xivo_issa('rightcall',$this->_info) === false
		|| ($rmember = &$this->_ipbx->get_module('rightcallmember')) === false)
			return(false);

		$rmember_where = array();
		$rmember_where['type'] = $type;
		$rmember_where['rightcallid'] = $this->_info['rightcall']['id'];

		return($rmember->delete_where($rmember_where));
	}

	function delete_rightcallexten()
	{
		if(xivo_issa('rightcall',$this->_info) === false
		|| ($rightcallexten = &$this->_ipbx->get_module('rightcallexten')) === false)
			return(false);

		$rightcallexten_where = array('rightcallid' => $this->_info['rightcall']['id']);

		return($rightcallexten->delete_where($rightcallexten_where));
	}
}

?>
