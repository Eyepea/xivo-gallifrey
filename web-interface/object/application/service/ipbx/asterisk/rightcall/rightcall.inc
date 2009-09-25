<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

dwho_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_rightcall extends xivo_application_asterisk_abstract
{
	var $_rightcall		= false;
	var $_rightcallmember	= false;
	var $_rightcallexten	= false;
	var $_contextmember	= false;
	var $_contextlist	= array();

	function xivo_application_service_asterisk_rightcall(&$service,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(($this->_rightcall = &$this->_service->get_module('rightcall')) === false
		|| ((bool) $components === true && $this->load_components() === false) === true)
			return(false);

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_rightcallmember = &$this->_service->get_module('rightcallmember')) === false
		|| ($this->_rightcallexten = &$this->_service->get_module('rightcallexten')) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'rightcall'))) === false)
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
		$this->_get_contextmember($this->_info['rightcall']['id']);

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

	function get_rightcalls_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		if(($rs = $this->_rightcall->get_all($disable,true,$order,$limit)) === false
		|| ($nb = count($rs)) === 0)
		{
			$this->_cnt = $this->_rightcall->get_cnt();
			return(false);
		}

		$this->_cnt = $this->_rightcall->get_cnt();

		$r = array();

		if((bool) $assoc === false)
			$r = &$rs;
		else
		{
			for($i = 0;$i < $nb;$i++)
				$r[$rs[$i]['id']] = $rs[$i];
		}

		if(empty($r) === true)
			return(false);

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
		|| dwho_issa('rightcall',$arr) === false)
			return(false);

		$this->set_rightcall($arr['rightcall']);

		if(dwho_issa('rightcalluser',$arr) === true)
			$this->set_rightcalluser($arr['rightcalluser']);

		if(dwho_issa('rightcallgroup',$arr) === true)
			$this->set_rightcallgroup($arr['rightcallgroup']);

		if(dwho_issa('rightcallincall',$arr) === true)
			$this->set_rightcallincall($arr['rightcallincall']);

		if(dwho_issa('rightcalloutcall',$arr) === true)
			$this->set_rightcalloutcall($arr['rightcalloutcall']);

		if(dwho_issa('rightcallexten',$arr) === true)
			$this->set_rightcallexten($arr['rightcallexten']);
		else
		{
			$this->_set_result('rightcallexten',null);
			$this->_set_error('rightcallexten','invalid data');
		}

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
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

	function set_contextmember()
	{
		$rs = array();
		$rs['rightcall-context']['context'] = (string) $this->get_result_var('rightcall','context');
		$rs['rightcall-context']['typeval'] = 0;
		$rs['rightcall-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
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
		|| $this->add_rightcallexten($rightcallid) === false
		|| $this->add_contextmember($rightcallid) === false)
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
		|| ($rightcallid = dwho_ulongint($rightcallid)) === 0)
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
		if(($rightcallid = dwho_ulongint($rightcallid)) === 0)
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
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
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
					if(is_array($val) === false)
						continue;

					foreach($val as $memberval)
					{
						if($memberval !== false)
							$this->_rightcallmember->delete($memberval);
					}
					break;
				case 'rightcallexten':
					if(is_array($val) === false)
						continue;

					foreach($val as $extenval)
					{
						if($extenval !== false)
							$this->_rightcallexten->delete($extenval);
					}
					break;
				case 'contextmember':
					$this->_contextmember->reverse_add();
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
		|| $this->edit_rightcallexten() === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_rightcall()
	{
		if(dwho_issa('rightcall',$this->_info) === false
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
		|| dwho_issa('rightcall',$this->_info) === false)
			return(false);

		$id = $this->_info['rightcall']['id'];

		$key = 'rightcall'.$type;

		if(dwho_issa($key,$this->_info) === false)
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
		if(dwho_issa('rightcall',$this->_info) === false)
			return(false);

		$id = $this->_info['rightcall']['id'];

		if(dwho_issa('rightcallexten',$this->_info) === false)
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

	function edit_contextmember()
	{
		if(dwho_issa('rightcall',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['rightcall']['id']));
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
				case 'rightcall':
					if(isset($this->_origin['rightcall']) === false
					|| $this->_origin['rightcall'] === false)
						$this->delete_rightcall();
					else
						$this->_rightcall->edit_origin($this->_origin['rightcall']);
					break;
				case 'rightcalluser':
				case 'rightcallgroup':
				case 'rightcallincall':
				case 'rightcalloutcall':
					$this->_delete_rightcall_member_type(substr($key,9));

					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$this->_rightcallmember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcallexten':
					$this->delete_rightcallexten();

					if(isset($this->_origin['rightcallexten']) === true
					&& $this->_origin['rightcallexten'] !== false)
						$this->_rightcallexten->add_origin_list($this->_origin['rightcallexten']);
					break;
				case 'contextmember':
					$this->_contextmember->reverse_edit($this->_status);
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
		|| $this->delete_rightcallexten() === false
		|| $this->delete_contextmember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_rightcall()
	{
		if(dwho_issa('rightcall',$this->_info) === false)
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

		if(dwho_issa($key,$this->_info) === false)
			return(null);
		else if(dwho_issa('rightcall',$this->_info) === false)
			return(false);

		$where = array();
		$where['type'] = $type;
		$where['rightcallid'] = $this->_info['rightcall']['id'];

		$r = $this->_rightcallmember->delete_where($where);

		if($this->_status === 'delete')
			$this->_return[$key] = $r;

		return($r);
	}

	function delete_rightcallexten()
	{
		if(dwho_issa('rightcallexten',$this->_info) === false)
			return(null);
		else if(dwho_issa('rightcall',$this->_info) === false)
			return(false);

		$where = array('rightcallid' => $this->_info['rightcall']['id']);

		$r = $this->_rightcallexten->delete_where($where);

		if($this->_status === 'delete')
			$this->_return['rightcallexten'] = $r;

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
				case 'rightcall':
					if(isset($this->_origin['rightcall']) === true
					&& $this->_origin['rightcall'] !== false)
						$this->_rightcall->add_origin($this->_origin['rightcall']);
					break;
				case 'rightcalluser':
				case 'rightcallgroup':
				case 'rightcallincall':
				case 'rightcalloutcall':
					if(isset($this->_origin[$key]) === true
					&& $this->_origin[$key] !== false)
						$this->_rightcallmember->add_origin_list($this->_origin[$key]);
					break;
				case 'rightcallexten':
					if(isset($this->_origin['rightcallexten']) === true
					&& $this->_origin['rightcallexten'] !== false)
						$this->_rightcallexten->add_origin_list($this->_origin['rightcallexten']);
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
			}
		}

		return(true);
	}
}

?>
