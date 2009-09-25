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

class xivo_application_service_asterisk_outcall extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_outcall		= false;
	var $_extenumbers	= false;
	var $_outcalltrunk	= false;
	var $_extensions	= false;
	var $_rightcall		= false;
	var $_contextmember	= false;
	var $_contextlist	= array();

	function xivo_application_service_asterisk_outcall(&$service,&$dso,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_outcall = &$this->_service->get_module('outcall')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'outcall'))) === false
		|| ($this->_extensions = &$this->_service->get_module('extensions')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_outcalltrunk = &$this->_service->get_module('outcalltrunk')) === false
		|| ($this->_rightcall = $this->get_abstract('rightcall',array('type' => 'outcall'))) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'outcall'))) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null,$nocomponents=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['outcall'] = $this->_outcall->get($id,$disable)) === false
		|| ($this->_info['extenumbers'] = $this->_get_extenumbers($this->_info['outcall']['id'])) === false
		|| $this->_get_outcallmacro() === false)
			return(false);

		$this->_origin['outcall'] = $this->_outcall->get_origin();
		$this->_origin['extenumbers'] = $this->_extenumbers->get_origin();

		if($this->_components === false)
			return($this->_info);

		if(dwho_ak('outcalltrunk',$nocomponents,true) === false)
			$this->_get_outcalltrunk();

		if(dwho_ak('rightcall',$nocomponents,true) === false)
			$this->_get_rightcall($this->_info['outcall']['id']);

		if(dwho_ak('contextmember',$nocomponents,true) === false)
			$this->_get_contextmember($this->_info['outcall']['id']);

		return($this->_info);
	}

	function _get_outcalltrunk()
	{
		$this->_info['outcalltrunk'] = false;
		$this->_origin['outcalltrunk'] = false;

		$where = array();
		$where['outcallid'] = $this->_info['outcall']['id'];

		if(($this->_info['outcalltrunk'] = $this->_outcalltrunk->get_all_where($where)) !== false)
			$this->_origin['outcalltrunk'] = $this->_outcalltrunk->get_origin_list();

		return($this->_info['outcalltrunk']);
	}

	function _get_outcallmacro()
	{
		$this->_info['outcallmacro'] = false;
		$this->_origin['outcallmacro'] = false;

		if(($this->_info['outcallmacro'] = $this->_extensions->get_exten($this->_info['outcall']['exten'],
										 $this->_info['outcall']['context'],
										 'macro')) !== false)
			$this->_origin['outcallmacro'] = $this->_extensions->get_origin();

		return($this->_info['outcallmacro']);
	}

	function get_elements()
	{
		$r = array();
		$r['outcall'] = $this->_outcall->get_element();
		$r['extenumbers'] = $this->_extenumbers->get_element();

		return($r);
	}

	function get_outcalls_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_outcalls('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_outcalls_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_outcalls('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_outcalls($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$search = strval($search);

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$disable,$order,$limit);
				break;
			case 'list':
			default:
				$rs = $this->_dso->get_all($disable,$order,$limit);
		}

		$this->_cnt = $this->_dso->get_cnt();

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		$r = $identity = array();

		for($i = 0;$i < $nb;$i++)
		{
			$r[$rs[$i]['id']] = $rs[$i];

			$ref = &$r[$rs[$i]['id']];
			$ref['exten'] = $this->_service->display_extension($ref['exten']);

			$identity['name'] = $ref['name'];
			$identity['context'] = $ref['context'];

			$ref['identity'] = $this->_outcall->mk_identity($identity);
			$ref['commented'] = (bool) $ref['commented'];
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function _mk_outcallmacro_appdata($outcallid)
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_OUTCALL_MACRO;
		$arr['appdata'][1] = dwho_ulongint($outcallid);

		return($arr);
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
		|| dwho_issa('outcall',$arr) === false)
			return(false);

		$this->set_outcall($arr['outcall']);

		$number = (string) $this->get_result_var('outcall','exten');
		$context = (string) $this->get_result_var('outcall','context');

		if(($rsoutcallmacro = $this->chk_macro('outcallmacro',
						       $number,
						       $this->_mk_outcallmacro_appdata(0),
						       $context)) !== false)
			$this->_set_result('outcallmacro',$rsoutcallmacro);

		$this->set_extenumbers($action,$number,$context);

		if(dwho_issa('outcalltrunk',$arr) === true)
			$this->set_outcalltrunk($arr['outcalltrunk']);
		else
			$this->_set_error('outcalltrunk','invalid data');

		if(dwho_issa('rightcall',$arr) === true)
			$this->set_rightcall($arr['rightcall']);

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_outcall($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('outcall',null);
			$this->_set_error('outcall','invalid data');
			return(false);
		}

		$arr['commented'] = false;

		if(($rs = $this->_outcall->chk_values($arr)) === false)
		{
			$this->_set_result('outcall',$this->_outcall->get_filter_result());
			$this->_set_error('outcall',$this->_outcall->get_filter_error());
			return(false);
		}

		$this->_set_result('outcall',$rs);

		return(true);
	}

	function _set_outcallmacro($action,$outcallid)
	{
		if(($outcallid = dwho_ulongint($outcallid)) === 0
		|| ($rs = $this->get_result('outcallmacro')) === null
		|| $rs === false
		|| ($rsmacro = $this->set_macro($action,
						'outcallmacro',
						$rs['exten'],
						$this->_mk_outcallmacro_appdata($outcallid),
						$rs['context'])) === false)
			return(false);

		$this->_set_result('outcallmacro',$rsmacro);

		return(true);
	}

	function set_outcalltrunk($arr)
	{
		if(is_array($arr) === false || ($nb = count($arr)) === 0)
		{
			$this->_set_result('outcalltrunk',null);
			$this->_set_error('outcalltrunk','invalid data');
			return(false);
		}
		else if(($trunkfeatures = &$this->_service->get_module('trunkfeatures')) === false)
		{
			$this->_set_result('outcalltrunk',false);
			return(false);
		}

		$arr = array_values($arr);

		$info = array();
		$info['outcallid'] = 0;

		$reslist = $tmp = array();

		for($i = 0,$j = 0;$i < $nb;$i++)
		{
			$info['priority'] = $j;

			if(($info['trunkfeaturesid'] = $trunkfeatures->get_primary($arr[$i])) === false
			|| ($rs = $this->_outcalltrunk->chk_values($info)) === false
			|| isset($tmp[$rs['trunkfeaturesid']]) === true)
				continue;

			$tmp[$rs['trunkfeaturesid']] = 1;
			$reslist[$j++] = $rs;
		}

		if(isset($reslist[0]) === false)
		{
			$this->_set_result('outcalltrunk',false);
			return(false);
		}

		$this->_set_result('outcalltrunk',$reslist);
		return(true);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['outcall-context']['context'] = (string) $this->get_result_var('outcall','context');
		$rs['outcall-context']['typeval'] = 0;
		$rs['outcall-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$outcallid = null;

		if($this->get_errnb() > 0
		|| ($outcallid = $this->add_outcall()) === false
		|| $this->add_extenumbers($outcallid) === false
		|| $this->add_outcallmacro($outcallid) === false
		|| $this->add_outcalltrunk($outcallid) === false
		|| $this->add_rightcall($outcallid) === false
		|| $this->add_contextmember($outcallid) === false)
		{
			$r = false;
			if($outcallid !== null)
				$this->_reverse_add($outcallid);
		}

		$this->_status = '';

		return($r);
	}

	function add_outcall()
	{
		if(($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['outcall'] = $this->_outcall->add($rs)));
	}

	function add_outcallmacro($outcallid)
	{
		if($this->_set_outcallmacro('add',$outcallid) !== true)
			return(false);

		return(parent::add_macro('outcallmacro'));
	}

	function add_outcalltrunk($outcallid)
	{
		if(($outcallid = dwho_ulongint($outcallid)) === 0
		|| ($rs = $this->get_result('outcalltrunk')) === null
		|| $rs === false)
			return(false);

		$id = array();

		$nb = count($rs);

		$this->_return['outcalltrunk'] = array();

		for($i = 0;$i < $nb;$i++)
		{
			$rs[$i]['outcallid'] = $outcallid;

			$id['outcallid'] = $outcallid;

			if($this->_outcalltrunk->add($rs[$i]) === false)
				return(false);

			$this->_return['outcalltrunk'][] = $id;
		}

		if(isset($this->_return['outcalltrunk'][0]) === false)
		{
			$this->_return['outcalltrunk'] = false;
			return(false);
		}

		return(true);
	}

	function _reverse_add($outcallid)
	{
		if($this->get_errnb() > 0
		|| $this->_status !== 'add'
		|| ($outcallid = dwho_ulongint($outcallid)) === 0
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'outcall':
					$this->_outcall->delete($outcallid);
					break;
				case 'outcallmacro':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add();
					break;
				case 'outcalltrunk':
					$this->_outcalltrunk->delete_where(array('outcallid' => $outcallid));
					break;
				case 'rightcall':
					$this->_rightcall->reverse_add();
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
		|| $this->edit_outcall() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_outcallmacro() === false
		|| $this->edit_outcalltrunk() === false
		|| $this->edit_rightcall() === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_outcall()
	{
		if(dwho_issa('outcall',$this->_info) === false
		|| ($rs = $this->get_result('outcall')) === null
		|| $rs === false)
			return(false);

		$rs['commented'] = $this->_info['outcall']['commented'];

		return(($this->_return['outcall'] = $this->_outcall->edit($this->_info['outcall']['id'],$rs)));
	}

	function edit_outcallmacro()
	{
		if(dwho_issa('outcall',$this->_info) === false
		|| $this->_set_outcallmacro('edit',$this->_info['outcall']['id']) !== true)
			return(false);

		return(parent::edit_macro('outcallmacro'));
	}

	function edit_outcalltrunk()
	{
		if(dwho_issa('outcall',$this->_info) === false)
			return(false);

		$this->_return['outcalltrunk'] = $this->delete_outcalltrunk();

		return($this->add_outcalltrunk($this->_info['outcall']['id']));
	}

	function edit_rightcall()
	{
		if(dwho_issa('outcall',$this->_info) === false)
			return(false);

		return(parent::edit_rightcall($this->_info['outcall']['id']));
	}

	function edit_contextmember()
	{
		if(dwho_issa('outcall',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['outcall']['id']));
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
				case 'outcall':
					if(isset($this->_origin['outcall']) === false
					|| $this->_origin['outcall'] === false)
						$this->delete_outcall();
					else
						$this->_outcall->edit_origin($this->_origin['outcall']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'outcallmacro':
					if(isset($this->_origin['outcallmacro']) === false
					|| $this->_origin['outcallmacro'] === false)
						$this->delete_macro('outcallmacro');
					else
						$this->_extensions->edit_origin($this->_origin['outcallmacro']);
					break;
				case 'outcalltrunk':
					$this->delete_outcalltrunk();

					if(isset($this->_origin['outcalltrunk']) === true
					&& $this->_origin['outcalltrunk'] !== false)
						$this->_outcalltrunk->add_origin_list($this->_origin['outcalltrunk']);
					break;
				case 'rightcall':
					$this->_rightcall->reverse_edit($this->_status);
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

		if($this->delete_outcall() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_macro('outcallmacro') === false
		|| $this->delete_outcalltrunk() === false
		|| $this->delete_rightcall() === false
		|| $this->delete_contextmember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_outcall()
	{
		if(dwho_issa('outcall',$this->_info) === false)
			return(false);

		$r = $this->_outcall->delete($this->_info['outcall']['id']);

		if($this->_status === 'delete')
			$this->_return['outcall'] = $r;

		return($r);
	}

	function delete_outcalltrunk()
	{
		if(dwho_issa('outcalltrunk',$this->_info) === false)
			return(null);
		else if(dwho_issa('outcall',$this->_info) === false)
			return(false);

		$r = $this->_outcalltrunk->delete_where(array('outcallid' => $this->_info['outcall']['id']));

		if($this->_status === 'delete')
			$this->_return['outcalltrunk'] = $r;

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
				case 'outcall':
					if(isset($this->_origin['outcall']) === true
					&& $this->_origin['outcall'] !== false)
						$this->_outcall->add_origin($this->_origin['outcall']);
					break;
				case 'outcallmacro':
					if(isset($this->_origin['outcallmacro']) === true
					&& $this->_origin['outcallmacro'] !== false)
						$this->_extensions->add_origin($this->_origin['outcallmacro']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'outcalltrunk':
					if(isset($this->_origin['outcalltrunk']) === true
					&& $this->_origin['outcalltrunk'] !== false)
						$this->_outcalltrunk->add_origin_list($this->_origin['outcalltrunk']);
					break;
				case 'rightcall':
					$this->_rightcall->add_origin_list();
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
			}
		}

		return(true);
	}

	function enable()
	{
		$r = true;
		$this->_status = 'enable';
		$this->_return = array();

		if($this->enable_outcall() === false
		|| $this->enable_outcallmacro() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
	}

	function disable()
	{
		$r = true;
		$this->_status = 'disable';
		$this->_return = array();

		if($this->disable_outcall() === false
		|| $this->disable_outcallmacro() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
	}

	function enable_outcall()
	{
		return($this->_enable_disable_outcall(false));
	}

	function disable_outcall()
	{
		return($this->_enable_disable_outcall(true));
	}

	function _enable_disable_outcall($disable=false)
	{
		if(dwho_issa('outcall',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_outcall->enable($this->_info['outcall']['id']);
		else
			$r = $this->_outcall->disable($this->_info['outcall']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['outcall'] = $r;

		return($r);
	}

	function enable_outcallmacro()
	{
		return($this->_enable_disable_outcallmacro(false));
	}

	function disable_outcallmacro()
	{
		return($this->_enable_disable_outcallmacro(true));
	}

	function _enable_disable_outcallmacro($disable=false)
	{
		if(dwho_issa('outcall',$this->_info) === false)
			return(false);

		$where = array();
		$where['exten'] = $this->_info['outcall']['exten'];
		$where['context'] = $this->_info['outcall']['context'];

		if((bool) $disable === false)
			$r = $this->_extensions->enable_where($where);
		else
			$r = $this->_extensions->disable_where($where);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['outcallmacro'] = $r;

		return($r);
	}

	function _reverse_enable_disable()
	{
		if(($this->_status !== 'enable' && $this->_status !== 'disable') === true
		|| is_array($this->_return) === false
		|| empty($this->_return) === true)
			return(false);

		$disable = $this->_status === 'enable';

		$this->_status = '';

		foreach($this->_return as $key => $val)
		{
			if($val === false)
				continue;

			switch($key)
			{
				case 'outcall':
					$this->_enable_disable_outcall($disable);
					break;
				case 'outcallmacro':
					$this->_enable_disable_outcallmacro($disable);
					break;
			}
		}

		return(true);
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
