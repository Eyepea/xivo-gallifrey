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

xivo_file::required(array(XIVO_PATH_OBJECT,'application','service','ipbx','asterisk.inc'),true);

class xivo_application_service_asterisk_meetme extends xivo_application_asterisk_abstract
{
	var $_dso		= null;
	var $_appmeetme		= false;
	var $_meetmefeatures	= false;
	var $_meetmeroom	= false;
	var $_extensions	= false;
	var $_extenumbers	= false;
	var $_contextnummember	= false;
	var $_contextmember	= false;
	var $_musiconhold	= array();
	var $_contextlist	= array();

	function xivo_application_service_asterisk_meetme(&$service,&$dso,$param=null,$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_meetmefeatures = &$this->_service->get_module('meetmefeatures')) === false
		|| ($this->_appmeetme = &$this->_service->get_apprealstatic('meetme')) === false
		|| ($this->_meetmeroom = &$this->_appmeetme->get_module('room')) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_components()
	{
		$this->_components = true;

		if(($this->_extensions = &$this->_service->get_module('extensions')) === false
		|| ($this->_extenumbers = $this->get_abstract('extenumbers',array('type' => 'meetme'))) === false
		|| ($this->_contextnummember = $this->get_abstract('contextnummember',array('type' => 'meetme'))) === false
		|| ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'meetme'))) === false)
			return(false);

		return(true);
	}

	function get($id,$disable=null,$nocomponents=null)
	{
		$this->_info = $this->_origin = array();

		if(($this->_info['meetmefeatures'] = $this->_meetmefeatures->get($id)) === false
		|| ($this->_info['meetmeroom'] = $this->_meetmeroom->get_meetmeroom(
						$this->_info['meetmefeatures']['meetmeid'],
						$disable)) === false)
			return(false);

		$this->_origin['meetmefeatures'] = $this->_meetmefeatures->get_origin();
		$this->_origin['meetmeroom'] = $this->_meetmeroom->get_origin();

		if($this->_components === false)
			return($this->_info);

		if(xivo_ak('meetmemacro',$nocomponents,true) === false)
			$this->_get_meetmemacro();

		if(xivo_ak('extenumbers',$nocomponents,true) === false)
			$this->_get_extenumbers($this->_info['meetmefeatures']['id']);

		if(xivo_ak('contextnummember',$nocomponents,true) === false)
			$this->_get_contextnummember($this->_info['meetmefeatures']['id']);

		if(xivo_ak('contextmember',$nocomponents,true) === false)
			$this->_get_contextmember($this->_info['meetmefeatures']['id']);

		return($this->_info);
	}

	function get_nb($arr=null,$disable=null)
	{
		return($this->_meetmeroom->get_nb($arr,$disable));
	}

	function _get_meetmemacro()
	{
		$this->_info['meetmemacro'] = false;
		$this->_origin['meetmemacro'] = false;

		if(($this->_info['meetmemacro'] = $this->_extensions->get_exten($this->_info['meetmefeatures']['number'],
										$this->_info['meetmefeatures']['context'],
										'macro')) !== false)
			$this->_origin['meetmemacro'] = $this->_extensions->get_origin();

		return($this->_info['meetmemacro']);
	}

	function get_elements()
	{
		$r = array();

		$r['meetmeroom'] = $this->_meetmeroom->get_element();
		$r['meetmefeatures'] = $this->_meetmefeatures->get_element();

		return($r);
	}

	function get_meetme_search($search,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_meetme('search',$search,$disable,$order,$limit,$assoc));
	}

	function get_meetme_list($disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_meetme('list',null,$disable,$order,$limit,$assoc));
	}

	function _get_meetme($action,$search='',$disable=null,$order=null,$limit=null,$assoc=false)
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

			$identity['name'] = $ref['name'];
			$identity['number'] = $ref['number'];
			$identity['context'] = $ref['context'];

			$ref['identity'] = $this->_meetmefeatures->mk_identity($identity);
			$ref = xivo_service_asterisk_realstatic_meetme::parse_room($ref);
			$ref['commented'] = (bool) $ref['commented'];

			unset($ref['var_val']);
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

		return($r);
	}

	function _mk_meetmemacro_appdata($meetmefeaturesid)
	{
		$arr = array();
		$arr['appdata'][0] = XIVO_SRE_IPBX_AST_MEETME_MACRO;
		$arr['appdata'][1] = xivo_ulongint($meetmefeaturesid);

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
		|| xivo_issa('meetmeroom',$arr) === false
		|| xivo_issa('meetmefeatures',$arr) === false)
			return(false);

		$this->set_meetmeroom($arr['meetmeroom']);

		$number = (string) $this->get_result_var('meetmeroom','number');

		$arr['meetmefeatures']['meetmeid'] = 0;
		$arr['meetmefeatures']['number'] = $number;

		$this->set_meetmefeatures($arr['meetmefeatures']);

		$context = (string) $this->get_result_var('meetmefeatures','context');

		if(($rsmeetmemacro = $this->chk_macro('meetmemacro',
						      $number,
						      $this->_mk_meetmemacro_appdata(0),
						      $context)) !== false)
			$this->_set_result('meetmemacro',$rsmeetmemacro);

		$this->set_extenumbers($action,$number,$context);

		$this->set_contextnummember($context,$number);

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_meetmefeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('meetmefeatures',null);
			$this->_set_error('meetmefeatures','invalid data');
			return(false);
		}

		$this->get_musiconhold();

		if(isset($arr['musiconhold'],$this->_musiconhold[$arr['musiconhold']]) === false)
			$arr['musiconhold'] = '';

		if(($rs = $this->_meetmefeatures->chk_values($arr)) === false)
		{
			$this->_set_result('meetmefeatures',$this->_meetmefeatures->get_filter_result());
			$this->_set_error('meetmefeatures',$this->_meetmefeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('meetmefeatures',$rs);

		return(true);
	}

	function set_meetmeroom($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('meetmeroom',null);
			$this->_set_error('meetmeroom','invalid data');
			return(false);
		}
		else if(($r = $this->_meetmeroom->set_meetmeroom($arr)) === false)
			$this->_set_error('meetmeroom',$this->_meetmeroom->get_error('meetmeroom'));

		$this->_set_result('meetmeroom',$this->_meetmeroom->get_result('meetmeroom'));

		return($r);
	}

	function _set_meetmemacro($action,$meetmefeaturesid)
	{
		$rs = false;

		if(($meetmefeaturesid = xivo_ulongint($meetmefeaturesid)) === 0
		|| ($rs = $this->get_result('meetmemacro')) === null
		|| $rs === false)
			return($rs);
		else if(($rsmacro = $this->set_macro($action,
						     'meetmemacro',
						     $rs['exten'],
						     $this->_mk_meetmemacro_appdata($meetmefeaturesid),
						     $rs['context'])) === false)
			return(false);

		$this->_set_result('meetmemacro',$rsmacro);

		return(true);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['meetmefeatures-exitcontext']['context'] = (string) $this->get_result_var('meetmefeatures','exitcontext');
		$rs['meetmefeatures-exitcontext']['typeval'] = 0;
		$rs['meetmefeatures-exitcontext']['varname'] = 'exitcontext';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$meetmeroomid = null;

		if($this->get_errnb() > 0
		|| ($meetmeroomid = $this->add_meetmeroom()) === false
		|| ($meetmefeaturesid = $this->add_meetmefeatures($meetmeroomid)) === false
		|| $this->add_extenumbers($meetmefeaturesid) === false
		|| $this->add_meetmemacro($meetmefeaturesid) === false
		|| $this->add_contextnummember($meetmefeaturesid) === false
		|| $this->add_contextmember($meetmefeaturesid) === false)
		{
			$r = false;
			if($meetmeroomid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_meetmeroom()
	{
		if(($rs = $this->_meetmeroom->add_meetmeroom()) === null
		|| $rs === false)
			return(false);

		return(($this->_return['meetmeroom'] = $this->_meetmeroom->get_return()));
	}

	function add_meetmefeatures($meetmeid)
	{
		if(($meetmeid = xivo_ulongint($meetmeid)) === 0
		|| ($rs = $this->get_result('meetmefeatures')) === null
		|| $rs === false)
			return(false);

		$rs['meetmeid'] = $meetmeid;

		return(($this->_return['meetmefeatures'] = $this->_meetmefeatures->add($rs)));
	}

	function add_meetmemacro($meetmefeaturesid)
	{
		if(($rs = $this->_set_meetmemacro('add',$meetmefeaturesid)) !== true)
			return($rs);

		return(parent::add_macro('meetmemacro'));
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
				case 'meetmeroom':
					$this->_meetmeroom->delete_meetmeroom($this->_status,$val);
					break;
				case 'meetmefeatures':
					$this->_meetmefeatures->delete($val);
					break;
				case 'meetmemacro':
					$this->_extensions->delete_exten($val);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_add();
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_add();
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
		|| $this->edit_meetmeroom() === false
		|| $this->edit_meetmefeatures() === false
		|| $this->edit_extenumbers() === false
		|| $this->edit_meetmemacro($this->_info['meetmefeatures']['id']) === false
		|| $this->edit_contextnummember() === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_meetmeroom()
	{
		if(xivo_issa('meetmeroom',$this->_info) === false
		|| ($rs = $this->_meetmeroom->edit_meetmeroom($this->_info['meetmeroom']['id'])) === null
		|| $rs === false)
			return(false);

		return(($this->_return['meetmeroom'] = $this->_meetmeroom->get_return()));
	}

	function edit_meetmefeatures()
	{
		if(xivo_issa('meetmefeatures',$this->_info) === false
		|| ($rs = $this->get_result('meetmefeatures')) === null
		|| $rs === false)
			return(false);

		$rs['meetmeid'] = $this->_info['meetmefeatures']['meetmeid'];

		return(($this->_return['meetmefeatures'] = $this->_meetmefeatures->edit($this->_info['meetmefeatures']['id'],$rs)));
	}

	function edit_meetmemacro($meetmefeaturesid)
	{
		if(($rs = $this->_set_meetmemacro('edit',$meetmefeaturesid)) !== true)
			return($rs);

		return(parent::edit_macro('meetmemacro'));
	}

	function edit_contextnummember()
	{
		if(xivo_issa('meetmefeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextnummember($this->_info['meetmefeatures']['id']));
	}

	function edit_contextmember()
	{
		if(xivo_issa('meetmefeatures',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['meetmefeatures']['id']));
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
				case 'meetmeroom':
					if(isset($this->_origin['meetmeroom']) === false
					|| $this->_origin['meetmeroom'] === false)
						$this->delete_meetmeroom();
					else
						$this->_meetmeroom->edit_origin($this->_origin['meetmeroom']);
					break;
				case 'meetmefeatures':
					if(isset($this->_origin['meetmefeatures']) === false
					|| $this->_origin['meetmefeatures'] === false)
						$this->delete_meetmefeatures();
					else
						$this->_meetmefeatures->edit_origin($this->_origin['meetmefeatures']);
					break;
				case 'meetmemacro':
					if(isset($this->_origin['meetmemacro']) === false
					|| $this->_origin['meetmemacro'] === false)
						$this->delete_macro('meetmemacro');
					else
						$this->_extensions->edit_origin($this->_origin['meetmemacro']);
					break;
				case 'extenumbers':
					$this->_extenumbers->reverse_edit($this->_status);
					break;
				case 'contextnummember':
					$this->_contextnummember->reverse_edit($this->_status);
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

		if($this->delete_meetmeroom() === false
		|| $this->delete_meetmefeatures() === false
		|| $this->delete_extenumbers() === false
		|| $this->delete_macro('meetmemacro') === false
		|| $this->delete_contextnummember() === false
		|| $this->delete_contextmember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}
		else
			$this->unlinks();

		$this->_status = '';

		return($r);
	}

	function delete_meetmeroom()
	{
		if(xivo_issa('meetmeroom',$this->_info) === false
		|| ($r = $this->_meetmeroom->delete_meetmeroom($this->_status,
							       $this->_info['meetmeroom']['id'])) === null
		|| $r === false)
			return(false);

		if($this->_status === 'delete')
			$this->_return['meetmeroom'] = $r;

		return($r);
	}

	function delete_meetmefeatures()
	{
		if(xivo_issa('meetmefeatures',$this->_info) === false)
			return(false);

		$r = $this->_meetmefeatures->delete($this->_info['meetmefeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['meetmefeatures'] = $r;

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
				case 'meetmeroom':
					if(isset($this->_origin['meetmeroom']) === true
					&& $this->_origin['meetmeroom'] !== false)
						$this->_meetmeroom->add_origin($this->_origin['meetmeroom']);
					break;
				case 'meetmefeatures':
					if(isset($this->_origin['meetmefeatures']) === true
					&& $this->_origin['meetmefeatures'] !== false)
						$this->_meetmefeatures->add_origin($this->_origin['meetmefeatures']);
					break;
				case 'meetmemacro':
					if(isset($this->_origin['meetmemacro']) === true
					&& $this->_origin['meetmemacro'] !== false)
						$this->_extensions->add_origin($this->_origin['meetmemacro']);
					break;
				case 'extenumbers':
					if(isset($this->_origin['extenumbers']) === true
					&& $this->_origin['extenumbers'] !== false)
						$this->_extenumbers->add_origin($this->_origin['extenumbers']);
					break;
				case 'contextnummember':
					$this->_contextnummember->add_origin();
					break;
				case 'contextmember':
					$this->_contextmember->add_origin_list();
					break;
			}
		}

		return(true);
	}

	function unlinks()
	{
		if(xivo_issa('meetmefeatures',$this->_info) === false)
			return(false);
		else if(($dialaction = &$this->_service->get_module('dialaction')) !== false)
		{
			$where = array();
			$where['type'] = 'meetme';
			$where['typeval'] = $this->_info['meetmefeatures']['id'];

			$dialaction->unlinked_where($where);
		}

		return(true);
	}

	function enable()
	{
		$r = true;

		$this->_status = 'enable';
		$this->_return = array();

		if(is_object($this->_meetmeroom) === false)
			$r = false;
		else if($this->enable_meetmeroom() === false)
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

		if(is_object($this->_meetmeroom) === false)
			$r = false;
		else if($this->disable_meetmeroom() === false)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return(true);
	}

	function enable_meetmeroom()
	{
		return($this->_enable_disable_meetmeroom(false));
	}

	function disable_meetmeroom()
	{
		return($this->_enable_disable_meetmeroom(true));
	}

	function _enable_disable_meetmeroom($disable=false)
	{
		if(xivo_issa('meetmeroom',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_meetmeroom->enable($this->_info['meetmeroom']['id']);
		else
			$r = $this->_meetmeroom->disable($this->_info['meetmeroom']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['meetmeroom'] = $r;

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
				case 'meetmeroom':
					$this->_enable_disable_meetmeroom($disable);
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
