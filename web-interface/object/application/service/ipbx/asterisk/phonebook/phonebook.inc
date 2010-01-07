<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

dwho::load_class('dwho_http');

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_phonebook extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_phonebook		= false;
	var $_phonebookaddress	= false;
	var $_phonebooknumber	= false;
	var $_conf		= array();

	function xivo_application_service_asterisk_phonebook(&$service,&$dso)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_phonebook = &$this->_service->get_module('phonebook')) === false
		|| ($this->_phonebookaddress = &$this->_service->get_module('phonebookaddress')) === false
		|| ($this->_phonebooknumber = &$this->_service->get_module('phonebooknumber')) === false)
			return(false);

		$this->_load_config(dirname(__FILE__));

		if(($size = $this->_verify_can_upload_file()) === false)
			die();

		$this->_conf['import']['file']['size'] = $size;

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
			if(isset($this->_info['phonebookaddress'][$info[$i]['type']]) === false)
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
			if(isset($this->_info['phonebooknumber'][$info[$i]['type']]) === false)
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
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$order,$limit);
				$this->_cnt = $this->_dso->get_cnt();
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
			if($action === 'search')
				$info['phonebook'] = $this->_phonebook->_prepare($rs[$i]);
			else
				$info['phonebook'] = $rs[$i];

			$info['phonebookaddress'] = $this->_phonebookaddress->get_all_type_by_phonebook($info['phonebook']['id']);
			$info['phonebooknumber'] = $this->_phonebooknumber->get_all_type_by_phonebook($info['phonebook']['id']);
			$r[$info['phonebook']['id']] = $info;
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function get_config_import_file()
	{
		return($this->_chk_exists_cfg('import','file',true));
	}

	function _get_config_file_maxsize()
	{
		$r = false;

		if(($import_file = $this->_chk_exists_cfg('import','file',true)) !== false
		&& isset($import_file['size']) === true)
			$r = dwho_get_memory($import_file['size'],false);

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
		|| dwho_issa('phonebook',$arr) === false)
			return(false);

		$this->set_phonebook($arr['phonebook']);

		if(dwho_issa('phonebookaddress',$arr) === true)
			$this->set_phonebookaddress($arr['phonebookaddress']);

		if(dwho_issa('phonebooknumber',$arr) === true)
			$this->set_phonebooknumber($arr['phonebooknumber']);

		return(($this->get_errnb() < 1));
	}

	function set_phonebook($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('phonebook',null);
			$this->_set_error('phonebook','invalid data');
			return(false);
		}
		else if(($rs = $this->_phonebook->chk_values($arr)) === false)
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
		|| is_array($list) === false)
			return(false);
		else if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		if(empty($list) === true)
			return(null);

		$info = $infotmp = array();
		$info['phonebookid'] = 0;

		$reslist = array();

		foreach($list as $key => $val)
		{
			if(($type === 'address'
			   && is_array($val) === false) === true
			|| ($type === 'number'
			   && is_scalar($val) === false) === true)
				continue;
			else if($type === 'number' && dwho_has_len($val) === false)
			{
				unset($list[$key]);
				continue;
			}

			if($type === 'address')
				$infotmp = array_merge($info,$val);
			else
			{
				$infotmp = $info;
				$infotmp['number'] = $val;
			}

			$infotmp['type'] = $key;

			if(($rs = $module->chk_values($infotmp)) !== false
			&& isset($reslist[$rs['type']]) === false)
				$reslist[$rs['type']] = $rs;
		}

		if($type === 'number' && empty($list) === true)
			return(null);
		else if(empty($reslist) === true)
		{
			$this->_set_result($typename,false);
			return(false);
		}

		$this->_set_result($typename,$reslist);

		return(true);
	}

	function add()
	{
		$r = true;
		$this->_status = 'add';
		$this->_return = array();
		$phonebookid = null;

		if($this->get_errnb() > 0
		|| ($phonebookid = $this->add_phonebook()) === false
		|| $this->add_phonebookaddress($phonebookid) === false
		|| $this->add_phonebooknumber($phonebookid) === false)
		{
			$r = false;
			if($phonebookid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
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
		|| ($id = dwho_ulongint($id)) === 0)
			return(false);
		else if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		if(($rs = $this->get_result($typename)) === null)
			return($rs);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return[$typename] = array();

		foreach($rs as $val)
		{
			$val['phonebookid'] = $id;

			if(($this->_return[$typename][$val['type']] = $module->add($val)) === false)
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
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			$module = null;

			switch($key)
			{
				case 'phonebook':
					$this->_phonebook->delete($val);
					break;
				case 'phonebookaddress':
					$module = &$this->_phonebookaddress;
				case 'phonebooknumber':
					if($module === null)
						$module = &$this->_phonebooknumber;

					if(is_array($val) === false)
						continue;

					foreach($val as $phoneval)
					{
						if($phoneval !== false)
							$module->delete($phoneval);
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
		|| $this->edit_phonebook() === false
		|| $this->edit_phonebookaddress() === false
		|| $this->edit_phonebooknumber() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_phonebook()
	{
		if(dwho_issa('phonebook',$this->_info) === false
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
		|| dwho_issa('phonebook',$this->_info) === false)
			return(false);
		else if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		$id = $this->_info['phonebook']['id'];

		if(dwho_issa($typename,$this->_info) === false)
			return($this->_add_phonebook_type($type,$id));

		$this->_return[$typename] = $this->_delete_phonebook_type($type);

		if(($rs = $this->get_result($typename)) === null)
			return($this->_return[$typename]);
		else if(is_array($rs) === false || empty($rs) === true)
			return(false);

		$this->_return[$typename] = array();

		$list = array_keys($this->_info[$typename]);

		$i = 0;

		foreach($rs as $val)
		{
			if(isset($list[$i]) === true)
				$pid = $this->_info[$typename][$list[$i++]]['id'];
			else
				$pid = 0;

			$val['phonebookid'] = $id;

			if(($this->_return[$typename][$val['type']] = $module->add($val,$pid)) === false)
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
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			$module = null;

			switch($key)
			{
				case 'phonebook':
					if(isset($this->_origin['phonebook']) === false
					|| $this->_origin['phonebook'] === false)
						$this->delete_phonebook();
					else
						$this->_phonebook->edit_origin($this->_origin['phonebook']);
					break;
				case 'phonebookaddress':
					$type = 'address';
					$module = &$this->_phonebookaddress;
				case 'phonebooknumber':
					if($module === null)
					{
						$type = 'number';
						$module = &$this->_phonebooknumber;
					}

					$this->_delete_phonebook_type($type);

					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$module->add_origin_list($this->_origin[$key]);
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

		if($this->delete_phonebook() === false
		|| $this->delete_phonebookaddress() === false
		|| $this->delete_phonebooknumber() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_phonebook()
	{
		if(dwho_issa('phonebook',$this->_info) === false)
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
		else if($type === 'address')
		{
			$typename = 'phonebookaddress';
			$module = &$this->_phonebookaddress;
		}
		else
		{
			$typename = 'phonebooknumber';
			$module = &$this->_phonebooknumber;
		}

		if(dwho_issa($typename,$this->_info) === false)
			return(null);
		else if(dwho_issa('phonebook',$this->_info) === false)
			return(false);

		$where = array();
		$where['phonebookid'] = $this->_info['phonebook']['id'];

		$r = $module->delete_where($where);

		if($this->_status === 'delete')
			$this->_return[$typename] = $r;

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

			$module = null;

			switch($key)
			{
				case 'phonebook':
					if(isset($this->_origin['phonebook']) === true
					&& $this->_origin['phonebook'] !== false)
						$this->_phonebook->add_origin($this->_origin['phonebook']);
					break;
				case 'phonebookaddress':
					$type = 'address';
					$module = &$this->_phonebookaddress;
				case 'phonebooknumber':
					if($module === null)
					{
						$type = 'number';
						$module = &$this->_phonebooknumber;
					}

					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$module->add_origin_list($this->_origin[$key]);
					break;
			}
		}

		return(true);
	}

	function import_csv()
	{
		if(dwho::load_class('dwho::file::csv') === false
		|| ($http_response = dwho_http::factory('response')) === false
		|| ($fileinfo = $http_response->upload_file('import',$this->_conf['import']['file'])) === false)
			return(false);

		$filecsv = new dwho_file_csv();

		if(($data = $filecsv->parse($fileinfo['tmp_name'],
					    0,
					    '|',
					    null,
					    true,
					    $this->_conf['import']['field'])) === false
		|| ($nb = count($data)) === 0)
		{
			dwho_file::rm($fileinfo['tmp_name']);
			return(false);
		}

		$r = true;

		for($i = 0;$i < $nb;$i++)
		{
			if($this->set_add($data[$i]) === false
			|| $this->add() === false)
				$r = false;
		}

		dwho_file::rm($fileinfo['tmp_name']);

		return($r);
	}

	function add_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_add($data) === false
		|| $this->add() === false)
			return(false);

		return(true);
	}

	function edit_from_json()
	{
		if(($data = $this->_get_data_from_json()) === false
		|| $this->set_edit($data) === false
		|| $this->edit() === false)
			return(false);

		return(true);
	}
}

?>
