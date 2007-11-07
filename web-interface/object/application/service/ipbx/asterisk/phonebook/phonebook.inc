<?php

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_phonebook extends xivo_application_asterisk_abstract
{
	var $_origin		= array();
	var $_status		= '';
	var $_cnt		= 0;
	var $_phonebook		= null;
	var $_phonebookaddress	= null;
	var $_phonebooknumber	= null;

	function xivo_application_service_asterisk_phonebook(&$service)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_phonebook = &$this->_service->get_module('phonebook')) === false
		|| ($this->_phonebookaddress = &$this->_service->get_module('phonebookaddress')) === false
		|| ($this->_phonebooknumber = &$this->_service->get_module('phonebooknumber')) === false)
			return(false);

		return(true);
	}

	function get($id)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['phonebook'] = $this->_phonebook->get($id)) === false)
			return(false);

		$this->_origin['phonebook'] = $this->_phonebook->get_origin();

		$this->_get_phonebookaddress();
		$this->_get_phonebooknumber();

		return($this->_info);
	}

	function get_cnt()
	{
		return(xivo_uint($this->_cnt));
	}

	function _get_phonebookaddress()
	{
		$this->_info['phonebookaddress'] = false;
		$this->_origin['phonebookaddress'] = false;

		$where = array();
		$where['phonebookid'] = $this->_info['phonebook']['id'];

		if(($info = $this->_phonebookaddress->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info['phonebookaddress'] = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info['phonebookaddress'][$info[$i]['type']]) === true)
				continue;

			$this->_info['phonebookaddress'][$info[$i]['type']] = $info[$i];
		}

		$this->_origin['phonebookaddress'] = $this->_phonebookaddress->get_origin_list();

		return($this->_info['phonebookaddress']);
	}

	function _get_phonebooknumber()
	{
		$this->_info['phonebooknumber'] = false;
		$this->_origin['phonebooknumber'] = false;

		$where = array();
		$where['phonebookid'] = $this->_info['phonebook']['id'];

		if(($info = $this->_phonebooknumber->get_all_where($where)) === false
		|| ($nb = count($info)) === 0)
			return(false);

		$this->_info['phonebooknumber'] = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($this->_info['phonebooknumber'][$info[$i]['type']]) === true)
				continue;

			$this->_info['phonebooknumber'][$info[$i]['type']] = $info[$i];
		}

		$this->_origin['phonebooknumber'] = $this->_phonebooknumber->get_origin_list();

		return($this->_info['phonebooknumber']);
	}

	function get_phonebook_search($search,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_phonebook('search',$search,$order,$limit,$assoc));
	}

	function get_phonebook_list($order=null,$limit=null,$assoc=false)
	{
		return($this->_get_phonebook('list',null,$order,$limit,$assoc));
	}

	function _get_phonebook($action,$search=null,$order=null,$limit=null,$assoc=false)
	{
		$action = (string) $action;
		$assoc = (bool) $assoc;

		$this->_cnt = 0;

		switch($action)
		{
			case 'search':
				$max = null;
				$list = array();

				if(isset($limit[1]) === true)
					$max = array(0,$limit[1]);

				if(($rsa = $this->_phonebookaddress->get_search($search,null,null,$max)) !== false
				&& ($nb = count($rsa)) !== 0)
				{
					for($i = 0;$i < $nb;$i++)
						$list[] = $rsa[$i]['phonebookid'];
				}

				if(($rsn = $this->_phonebooknumber->get_search($search,$list,null,$max)) !== false
				&& ($nb = count($rsn)) !== 0)
				{
					for($i = 0;$i < $nb;$i++)
						$list[] = $rsn[$i]['phonebookid'];
				}

				if(($rs = $this->_phonebook->get_search($search,$list,$order,$limit)) !== false)
					$this->_cnt = $this->_phonebook->get_cnt();
				break;
			case 'list':
			default:
				$rs = $this->_phonebook->get_all(null,true,$order,$limit);
				$this->_cnt = $this->_phonebook->get_cnt();
		}

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $info = array();

		for($i = 0;$i < $nb;$i++)
		{
			$info['phonebook'] = $rs[$i];
			$info['phonebookaddress'] = $this->_phonebookaddress->get_all_type_by_phonebook($info['phonebook']['id']); 
			$info['phonebooknumber'] = $this->_phonebooknumber->get_all_type_by_phonebook($info['phonebook']['id']); 
			$r[$info['phonebook']['id']] = $info;
		}
		
		if(empty($r) === true)
			$r = false;
		else if($assoc === false)
			$r = array_values($r);

		return($r);
	}

	function get_elements()
	{
		$r = array();

		$r['phonebook'] = $this->_phonebook->get_element();
		$r['phonebookaddress'] = $this->_phonebookaddress->get_element();
		$r['phonebooknumber'] = $this->_phonebooknumber->get_element();

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
		|| xivo_issa('phonebook',$arr) === false)
			return(false);

		$this->set_phonebook($arr['phonebook']);

		if(xivo_issa('phonebookaddress',$arr) === true)
			$this->set_phonebookaddress($arr['phonebookaddress']);

		if(xivo_issa('phonebooknumber',$arr) === true)
			$this->set_phonebooknumber($arr['phonebooknumber']);

		if($this->get_errnb() > 0)
			$r = false;
		else
			$r = true;

		return($r);
	}

	function set_phonebook($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('phonebook',null);
			$this->_set_error('phonebook','invalid data');
			return(false);
		}

		if(($rs = $this->_phonebook->chk_values($arr)) === false)
		{
			$this->_set_result('phonebook',$this->_phonebook->get_filter_result());
			$this->_set_error('phonebook',$this->_phonebook->get_filter_error());
			return(false);
		}

		$this->_set_result('phonebook',$rs);

		return(true);
	}

	function set_phonebookaddress($list)
	{
		return($this->_set_phonebook_type('address',$list));
	}

	function set_phonebooknumber($list)
	{
		return($this->_set_phonebook_type('number',$list));
	}

	function _set_phonebook_type($type,$list)
	{
		if(($type !== 'address' && $type !== 'number') === true
		|| is_array($list) === false
		|| ($arr = xivo_get_aks($list,true)) === false)
			return(false);

		if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		if($arr['cnt'] === 0)
			return(null);

		$info = array();
		$info['phonebookid'] = 0;

		$reslist = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$val = &$list[$key];

			if(($type === 'address'
			   && is_array($val) === false) === true
			|| ($type === 'number'
			   && is_scalar($val) === false) === true)
				continue;

			$info['type'] = $key;

			if($type === 'address')
				$info = array_merge($info,$val);
			else
				$info['number'] = $val;

			if(($rs = $module->chk_values($info)) === false
			|| isset($tmp[$rs['type']]) === true)
				continue;

			$reslist[$rs['type']] = $rs;
		}

		if(empty($reslist) === true)
		{
			$this->_set_result($typename,false);
			return(false);
		}

		$this->_set_result($typename,$reslist);
		
		return(true);
	}

	function add()
	{
		$this->_status = 'add';
		$this->_return = array();
		$phonebookid = null;

		if($this->get_errnb() > 0
		|| ($phonebookid = $this->add_phonebook()) === false
		|| $this->add_phonebookaddress($phonebookid) === false
		|| $this->add_phonebooknumber($phonebookid) === false)
		{
			if($phonebookid !== null)
				$this->_reverse_add();

			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function add_phonebook()
	{
		if(($rs = $this->get_result('phonebook')) === null
		|| $rs === false)
			return($rs);

		$this->_return['phonebook'] = $this->_phonebook->add($rs);

		return($this->_return['phonebook']);
	}

	function add_phonebookaddress($id)
	{
		return($this->_add_phonebook_type('address',$id));
	}

	function add_phonebooknumber($id)
	{
		return($this->_add_phonebook_type('number',$id));
	}

	function _add_phonebook_type($type,$id)
	{
		if(($type !== 'address' && $type !== 'number') === true
		|| ($id = xivo_uint($id)) === 0)
			return(false);

		if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		if(($rs = $this->get_result($typename)) === null || $rs === false)
			return($rs);

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$this->_return[$typename] = array();

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			$key = &$arr['keys'][$i];
			$ref = &$rs[$key];
			$ref['phonebookid'] = $id;

			if(($this->_return[$typename][$ref['type']] = $module->add($ref)) === false)
				return(false);
		}

		if(empty($this->_return[$typename]) === true)
			$this->_return[$typename] = false;

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
				case 'phonebook':
					$this->_phonebook->delete($val);
					break;
				case 'phonebookaddress':
					$module = &$this->_phonebookaddress;
				case 'phonebooknumber':
					if(isset($module) === false)
						$module = &$this->_phonebooknumber;

					if(($arrval = xivo_get_aks($val)) === false)
						continue;

					for($j = 0;$j < $arrval['cnt'];$j++)
					{
						if($val[$arrval['keys'][$j]] !== false)
							$module->delete($val[$arrval['keys'][$j]]);
					}
					break;
			}
		}
		return(true);
	}

	function edit()
	{
		$this->_status = 'edit';
		$this->_return = array();

		if($this->get_errnb() > 0
		|| $this->edit_phonebook() === false
		|| $this->edit_phonebookaddress() === false
		|| $this->edit_phonebooknumber() === false)
		{
			$this->_reverse_edit();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function edit_phonebook()
	{
		if(xivo_issa('phonebook',$this->_info) === false
		|| ($rs = $this->get_result('phonebook')) === null
		|| $rs === false)
			return(false);

		$this->_return['phonebook'] = $this->_phonebook->edit($this->_info['phonebook']['id'],$rs);

		return($this->_return['phonebook']);
	}

	function edit_phonebookaddress()
	{
		return($this->_edit_phonebook_type('address'));
	}

	function edit_phonebooknumber()
	{
		return($this->_edit_phonebook_type('number'));
	}

	function _edit_phonebook_type($type)
	{
		if(($type !== 'address' && $type !== 'number') === true
		|| xivo_issa('phonebook',$this->_info) === false)
			return(false);

		$id = $this->_info['phonebook']['id'];

		if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		if(xivo_issa($typename,$this->_info) === false)
			return($this->_add_phonebook_type($type,$id));

		$this->_return[$typename] = $this->_delete_phonebook_type($type);

		if(($rs = $this->get_result($typename)) === null)
			return($this->_return[$typename]);

		if(($arr = xivo_get_aks($rs)) === false)
			return(false);

		$this->_return[$typename] = array();

		$list = xivo_get_aks($this->_info[$typename]);

		for($i = 0;$i < $arr['cnt'];$i++)
		{
			if($list !== false && isset($list['keys'][$i]) === true)
				$pid = $this->_info[$typename][$list['keys'][$i]]['id'];
			else
				$pid = 0;

			$ref = &$rs[$arr['keys'][$i]];
			$ref['phonebookid'] = $id;

			if(($this->_return[$typename][$ref['type']] = $module->add($ref,$pid)) === false)
				return(false);
		}

		if(empty($this->_return[$typename]) === true)
			$this->_return[$typename] = false;

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
				case 'phonebook':
					if(isset($this->_origin['phonebook']) === false)
						continue;

					if($this->_origin['phonebook'] === false)
						$this->_phonebook->delete($val);
					else
						$this->_phonebook->edit_origin($this->_origin['phonebook']);
					break;
				case 'phonebookaddress':
					$type = 'address';
					$module = &$this->_phonebookaddress;
				case 'phonebooknumber':
					if(isset($module) === false)
					{
						$type = 'number';
						$module = &$this->_phonebooknumber;
					}

					if(isset($this->_origin[$key]) === false)
						continue;
						
					$this->_delete_phonebook_type($type);

					$module->add_origin_list($this->_origin[$key]);
					break;
			}
		}
		return(true);
	}

	function delete()
	{
		$this->_status = 'delete';
		$this->_return = array();

		if($this->delete_phonebook() === false
		|| $this->delete_phonebookaddress() === false
		|| $this->delete_phonebooknumber() === false)
		{
			$this->_reverse_delete();
			$this->_status = '';
			return(false);
		}

		$this->_status = '';

		return(true);
	}

	function delete_phonebook()
	{
		if(xivo_issa('phonebook',$this->_info) === false)
			return(false);

		$r = $this->_phonebook->delete($this->_info['phonebook']['id']);

		if($this->_status === 'delete')
			$this->_return['phonebook'] = $r;

		return($r);
	}

	function delete_phonebookaddress()
	{
		return($this->_delete_phonebook_type('address'));
	}

	function delete_phonebooknumber()
	{
		return($this->_delete_phonebook_type('number'));
	}

	function _delete_phonebook_type($type)
	{
		if(($type !== 'address' && $type !== 'number') === true)
			return(false);

		if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		if(xivo_issa($typename,$this->_info) === false)
			return(null);

		if(xivo_issa('phonebook',$this->_info) === false)
			return(false);

		$phonebooktype_where = array();
		$phonebooktype_where['phonebookid'] = $this->_info['phonebook']['id'];

		$r = $module->delete_where($phonebooktype_where);

		if($this->_status === 'delete')
			$this->_return[$typename] = $r;

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
				case 'phonebook':
					if(isset($this->_origin['phonebook']) === false)
						continue;

					$this->_phonebook->add_origin($this->_origin['phonebook']);
					break;
				case 'phonebookaddress':
					$type = 'address';
					$module = &$this->_phonebookaddress;
				case 'phonebooknumber':
					if(isset($module) === false)
					{
						$type = 'number';
						$module = &$this->_phonebooknumber;
					}

					if(isset($this->_origin[$key]) === false)
						continue;

					$module->add_origin_list($this->_origin[$key]);
					break;
			}
		}
		return(true);
	}
}

?>
