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

class xivo_application_service_asterisk_trunk extends xivo_application_asterisk_abstract
{
	var $_protocol		= false;
	var $_trunkfeatures	= false;
	var $_register		= false;
	var $_contextmember	= false;
	var $_param		= array('protocol'	=> '');
	var $_contextlist	= array();
	var $_timezones		= array();

	function xivo_application_service_asterisk_trunk(&$service,&$dso,$param=array(),$components=true)
	{
		$this->xivo_application_asterisk_abstract(&$service);

		if(is_object($dso) === false)
			trigger_error('Invalid Datastorage object',E_USER_ERROR);

		$this->_dso = &$dso;

		if(($this->_trunkfeatures = &$this->_service->get_module('trunkfeatures')) === false
		|| $this->load_init($param) === false)
			return(false);
		else if((bool) $components === true)
			$this->load_components();

		return(true);
	}

	function load_init($param=array())
	{
		if(is_array($param) === false || isset($param['protocol']) === false)
		{
			$this->_protocol = false;
			$this->_param['protocol'] = '';
			return(null);
		}
		else if($this->set_params($param) === true
		&& is_object($this->_trunkfeatures) === true
		&& $this->_trunkfeatures->chk_element_value('protocol',$param['protocol']) !== false)
		{
			if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_SIP
			&& ($this->_protocol = $this->_service->get_module('trunksip')) !== false)
				return(true);
			else if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_IAX
			&& ($this->_protocol = $this->_service->get_module('trunkiax')) !== false)
				return(true);
			else if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_CUSTOM
			&& ($this->_protocol = $this->_service->get_module('trunkcustom')) !== false)
				return(true);
		}

		$this->_protocol = false;
		$this->_param['protocol'] = '';

		return(false);
	}

	function load_components()
	{
		$this->_components = true;

		if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_SIP
		&& ($this->_appsip = &$this->_service->get_apprealstatic('sip')) !== false
		&& ($this->_register = $this->_appsip->get_module('register')) !== false
		&& ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'trunksip'))) !== false)
			return(true);
		else if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_IAX
		&& ($this->_appiax = &$this->_service->get_apprealstatic('iax')) !== false
		&& ($this->_register = $this->_appiax->get_module('register')) !== false
		&& ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'trunkiax'))) !== false)
			return(true);
		else if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_CUSTOM
		&& ($this->_contextmember = $this->get_abstract('contextmember',array('type' => 'trunkcustom'))) !== false)
		{
			$this->_register = false;
			return(true);
		}

		$this->_protocol = false;
		$this->_param['protocol'] = '';

		return(false);
	}

	function get($id,$disable=null)
	{
		$this->_info = false;
		$this->_origin = false;

		if(($this->_info['trunkfeatures'] = $this->_trunkfeatures->get($id,$disable)) === false
		|| $this->_info['trunkfeatures']['protocol'] !== $this->_param['protocol']
		|| is_object($this->_protocol) === false
		|| ($this->_info['protocol'] = $this->_protocol->get($this->_info['trunkfeatures']['protocolid'])) === false
		|| $this->_get_register() === false)
		{
			$this->_info = $this->_origin = false;
			return(false);
		}

		$this->_origin['trunkfeatures'] = $this->_trunkfeatures->get_origin();
		$this->_origin['protocol'] = $this->_protocol->get_origin();
		$this->_get_contextmember($this->_info['protocol']['id']);

		return($this->_info);
	}

	function _get_register()
	{
		$this->_info['register'] = false;
		$this->_origin['register'] = false;

		if(isset($this->_info['trunkfeatures']['registerid']) === false
		|| is_object($this->_register) === false
		|| ($registerid = xivo_ulongint($this->_info['trunkfeatures']['registerid'])) === 0)
			return(null);
		else if(($this->_info['register'] = $this->_register->get_register($registerid)) === false)
			return(false);

		$this->_origin['register'] = $this->_register->get_origin();

		return(true);
	}

	function get_timezones()
	{
		if(empty($this->_timezones) === true
		&& ($timezones = xivo_i18n::get_timezone_list()) !== false)
			$this->_timezones = $timezones;

		return($this->_timezones);
	}

	function get_elements()
	{
		$r = array();

		$r['trunkfeatures'] = $this->_trunkfeatures->get_element();

		if(is_object($this->_protocol) === true)
			$r['protocol'] = $this->_protocol->get_element();
		else
			$r['protocol'] = null;

		if(is_object($this->_register) === true)
			$r['register'] = $this->_register->get_element();
		else
			$r['register'] = null;

		return($r);
	}

	function get_trunks_search($search,$protocols=null,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_trunks('search',$search,$protocols,$disable,$order,$limit,$assoc));
	}

	function get_trunks_list($protocols=null,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		return($this->_get_trunks('list',null,$protocols,$disable,$order,$limit,$assoc));
	}

	function _get_trunks($action,$search='',$protocols=null,$disable=null,$order=null,$limit=null,$assoc=false)
	{
		$action = (string) $action;
		$search = strval($search);

		if(is_array($protocols) === true && empty($protocols) === false)
			$protocols = array_values($protocols);
		else if($protocols === true && xivo_haslen($this->_param['protocol']) === true)
			$protocols = array($this->_param['protocol']);
		else
			$protocols = null;

		switch($action)
		{
			case 'search':
				$rs = $this->_dso->search($search,$protocols,$disable,$order,$limit);
				break;
			default:
				$rs = $this->_dso->get_all($protocols,$disable,$order,$limit);
		}

		$this->_cnt = $this->_dso->get_cnt();

		$sipidentity = $staticsipclass = null;
		$iaxidentity = $staticiaxclass = null;
		$customidentity = null;

		if($rs === false || ($nb = count($rs)) === 0)
			return(false);

		if($protocols === null
		|| in_array(XIVO_SRE_IPBX_AST_PROTO_SIP,$protocols) === true)
		{
			if(($sipidentity = $this->_service->get_identity('trunksip')) === false)
				return(false);
			else if(($staticsipclass = &$this->_service->get_realstatic('staticsip')) === false)
				return(false);
		}

		if($protocols === null
		|| in_array(XIVO_SRE_IPBX_AST_PROTO_IAX,$protocols) === true)
		{
			if(($iaxidentity = $this->_service->get_identity('trunkiax')) === false)
				return(false);
			else if(($staticiaxclass = &$this->_service->get_realstatic('staticiax')) === false)
				return(false);
		}

		if($protocols === null
		|| in_array(XIVO_SRE_IPBX_AST_PROTO_CUSTOM,$protocols) === true)
		{
			if(($customidentity = $this->_service->get_identity('trunkcustom')) === false)
				return(false);
		}

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$rs[$i];

			$trunkfeatures = xivo_array_startswith_key($ref,'trunkfeatures.',true);

			switch($trunkfeatures['protocol'])
			{
				case XIVO_SRE_IPBX_AST_PROTO_SIP:
					$sip = xivo_array_startswith_key($ref,'usersip.',true);
					$sip['commented'] = (bool) $sip['commented'];

					if(empty($sip['calllimit']) === true)
						$sip['call-limit'] = '';

					$sip['register'] = $staticsipclass->parse_register_uri($ref['register']);

					if(is_array($sip['register']) === true)
						$sip['register']['commented'] = (bool) $trunkfeatures['registercommented'];

					unset($trunkfeatures['registercommented']);

					$sip['identity'] = $sipidentity->mk_identity($sip);

					$r[$trunkfeatures['id']] = array_merge($sip,$trunkfeatures);
					break;
				case XIVO_SRE_IPBX_AST_PROTO_IAX:
					$iax = xivo_array_startswith_key($ref,'useriax.',true);
					$iax['commented'] = (bool) $iax['commented'];

					$iax['register'] = $staticiaxclass->parse_register_uri($ref['register']);

					if(is_array($iax['register']) === true)
						$iax['register']['commented'] = (bool) $trunkfeatures['registercommented'];

					unset($trunkfeatures['registercommented']);

					$iax['identity'] = $iaxidentity->mk_identity($iax);

					$r[$trunkfeatures['id']] = array_merge($iax,$trunkfeatures);
					break;
				case XIVO_SRE_IPBX_AST_PROTO_CUSTOM:
					$custom = xivo_array_startswith_key($ref,'usercustom.',true);
					$custom['commented'] = (bool) $custom['commented'];
					$custom['identity'] = $customidentity->mk_identity($custom);

					unset($trunkfeatures['registercommented']);

					$r[$trunkfeatures['id']] = array_merge($custom,$trunkfeatures);
					break;
				default:
					$this->_cnt = 0;
					return(false);
			}
		}

		if(empty($r) === true)
			return(false);
		else if((bool) $assoc === false)
			return(array_values($r));

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
		|| xivo_issa('trunkfeatures',$arr) === false
		|| xivo_issa('protocol',$arr) === false)
			return(false);

		$this->set_trunkfeatures($arr['trunkfeatures']);

		$this->set_protocol($arr['protocol']);

		if(xivo_issa('register',$arr) === true)
			$this->set_register($arr['register']);

		$this->set_contextmember();

		return(($this->get_errnb() < 1));
	}

	function set_trunkfeatures($arr)
	{
		if(is_array($arr) === false)
		{
			$this->_set_result('trunkfeatures',null);
			$this->_set_error('trunkfeatures','invalid data');
			return(false);
		}

		$arr['protocol'] = $this->_param['protocol'];
		$arr['protocolid'] = 0;
		$arr['registerid'] = 0;
		$arr['registercommented'] = true;

		if(($rs = $this->_trunkfeatures->chk_values($arr)) === false)
		{
			$this->_set_result('trunkfeatures',$this->_trunkfeatures->get_filter_result());
			$this->_set_error('trunkfeatures',$this->_trunkfeatures->get_filter_error());
			return(false);
		}

		$this->_set_result('trunkfeatures',$rs);

		return(true);
	}

	function set_protocol($arr)
	{
		if(is_object($this->_protocol) === false)
		{
			$this->_set_result('protocol',null);
			$this->_set_error('protocol','invalid protocol');
			return(false);
		}
		else if(is_array($arr) === false)
		{
			$this->_set_result('protocol',null);
			$this->_set_error('protocol','invalid data');
			return(false);
		}
		else if($this->_param['protocol'] !== XIVO_SRE_IPBX_AST_PROTO_CUSTOM)
		{
			if(isset($arr['type']) === true && $arr['type'] === 'user')
				$arr['host'] = 'dynamic';
			else
			{
				if(isset($arr['host-type']) === true)
					$arr['host'] = $arr['host-type'];
				else if(isset($arr['host']) === false)
					$arr['host'] = '';

				if(isset($arr['host-static']) === true && $arr['host'] === 'static')
					$arr['host'] = $arr['host-static'];
			}

			if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_IAX
			&& xivo_haslen($arr,'timezone') === true)
			{
				$this->get_timezones();

				if(isset($this->_timezones[$arr['timezone']]) === false)
					$arr['timezone'] = '';
			}
		}

		$arr['commented'] = false;

		if(($rs = $this->_protocol->chk_values($arr)) === false)
		{
			$this->_set_result('protocol',$this->_protocol->get_filter_result());
			$this->_set_error('protocol',$this->_protocol->get_filter_error());
			return(false);
		}
		else if($rs['protocol'] !== $this->_param['protocol'])
		{
			$rs['protocol'] = '';
			$this->_set_result('protocol',$rs['protocol']);
			$this->_set_error('protocol','wrong protocol');
			return(false);
		}
		else if($this->_param['protocol'] === XIVO_SRE_IPBX_AST_PROTO_IAX
		&& $rs['type'] === 'friend')
			$rs['username'] = $rs['name'];

		if(xivo_issa('allow',$rs) === true)
			$rs['allow'] = implode(',',$rs['allow']);

		$this->_set_result('protocol',$rs);

		return(true);
	}

	function set_register($arr)
	{
		if(is_object($this->_protocol) === false)
			return(false);
		else if(is_object($this->_register) === false
		|| is_array($arr) === false
		|| empty($arr) === true)
			return(null);
		else if(($r = $this->_register->set_register($arr)) === false)
			$this->_set_error('register',$this->_register->get_error('register'));

		$this->_set_result('register',$this->_register->get_result('register'));

		return($r);
	}

	function set_contextmember()
	{
		$rs = array();
		$rs['trunk-context']['context'] = (string) $this->get_result_var('protocol','context');
		$rs['trunk-context']['typeval'] = 0;
		$rs['trunk-context']['varname'] = 'context';

		return(parent::set_contextmember($rs));
	}

	function add()
	{
		$r = true;

		$this->_status = 'add';
		$this->_return = array();
		$protocolid = $registerid = null;

		if($this->get_errnb() > 0
		|| ($protocolid = $this->add_protocol()) === false
		|| ($registerid = $this->add_register()) === false
		|| $this->add_trunkfeatures($protocolid,$registerid) === false
		|| $this->add_contextmember($protocolid) === false)
		{
			$r = false;
			if($protocolid !== null)
				$this->_reverse_add();
		}

		$this->_status = '';

		return($r);
	}

	function add_trunkfeatures($protocolid,$registerid=null)
	{
		if(($protocolid = xivo_ulongint($protocolid)) === 0
		|| ($rs = $this->get_result('trunkfeatures')) === null
		|| $rs === false)
			return(false);
		else if($registerid !== null)
		{
			if(($rs['registerid'] = xivo_ulongint($registerid)) === 0)
				return(false);

			$rs['registercommented'] = false;
		}

		$rs['protocolid'] = $protocolid;

		return(($this->_return['trunkfeatures'] = $this->_trunkfeatures->add($rs)));
	}

	function add_protocol()
	{
		if(is_object($this->_protocol) === false
		|| ($rs = $this->get_result('protocol')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['protocol'] = $this->_protocol->add($rs)));
	}

	function add_register()
	{
		if(is_object($this->_protocol) === false)
			return(false);
		else if(is_object($this->_register) === false)
			return(null);
		else if(($rs = $this->_register->add_register()) === null
		|| $rs === false)
			return($rs);

		return(($this->_return['register'] = $this->_register->get_return()));
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
				case 'trunkfeatures':
					$this->_trunkfeatures->delete($val);
					break;
				case 'protocol':
					if(is_object($this->_protocol) === true)
						$this->_protocol->delete($val);
					break;
				case 'register':
					if(is_object($this->_register) === true)
						$this->_register->delete_register($this->_status,$val);
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
		|| $this->edit_protocol() === false
		|| ($registerid = $this->edit_register()) === false
		|| $this->edit_trunkfeatures($registerid) === false
		|| $this->edit_contextmember() === false)
		{
			$r = false;
			$this->_reverse_edit();
		}

		$this->_status = '';

		return($r);
	}

	function edit_trunkfeatures($registerid)
	{
		if(xivo_issa('trunkfeatures',$this->_info) === false
		|| ($rs = $this->get_result('trunkfeatures')) === null
		|| $rs === false)
			return(false);

		$rs['registercommented'] = false;

		if($registerid === true || $registerid === 'disable')
		{
			if((int) $this->_info['trunkfeatures']['registerid'] === 0)
				return(false);

			$rs['registercommented'] = $registerid === 'disable';
			$registerid = $this->_info['trunkfeatures']['registerid'];
		}
		else if(($registerid = xivo_ulongint($registerid)) !== 0
		&& (int) $this->_info['trunkfeatures']['registerid'] !== 0)
			return(false);

		$rs['registerid'] = $registerid;
		$rs['protocolid'] = $this->_info['trunkfeatures']['protocolid'];

		return(($this->_return['trunkfeatures'] = $this->_trunkfeatures->edit($this->_info['trunkfeatures']['id'],$rs)));
	}

	function edit_protocol()
	{
		if(is_object($this->_protocol) === false
		|| xivo_issa('protocol',$this->_info) === false
		|| ($rs = $this->get_result('protocol')) === null
		|| $rs === false)
			return(false);

		return(($this->_return['protocol'] = $this->_protocol->edit($this->_info['protocol']['id'],$rs)));
	}

	function edit_register()
	{
		if(is_object($this->_register) === false)
			return(null);
		else if(xivo_issa('register',$this->_info) === false)
			return($this->add_register());
		else if(($r = $this->_register->edit_register($this->_info['register']['id'])) === null)
		{
			if($this->disable_register() !== false)
				return(($this->_return['register'] = 'disable'));

			return(false);
		}
		else if($r === false)
			return(false);
		else if(($r = $this->_register->get_return()) !== false)
			$r = true;

		return(($this->_return['register'] = $r));
	}

	function edit_contextmember()
	{
		if(xivo_issa('protocol',$this->_info) === false)
			return(false);

		return(parent::edit_contextmember($this->_info['protocol']['id']));
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
				case 'trunkfeatures':
					if(isset($this->_origin['trunkfeatures']) === false
					|| $this->_origin['trunkfeatures'] === false)
						$this->delete_trunkfeatures();
					else
						$this->_trunkfeatures->edit_origin($this->_origin['trunkfeatures']);
					break;
				case 'protocol':
					if(is_object($this->_protocol) === false)
						continue;
					else if(isset($this->_origin['protocol']) === false
					|| $this->_origin['protocol'] === false)
						$this->delete_protocol();
					else
						$this->_protocol->edit_origin($this->_origin['protocol']);
					break;
				case 'register':
					if(is_object($this->_register) === false)
						return(false);
					else if($val === 'disable')
						$this->enable_register();
					else if(isset($this->_origin['register']) === false
					|| $this->_origin['register'] === false)
						$this->delete_register();
					else
						$this->_register->edit_origin($this->_origin['register']);
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

		if($this->delete_protocol() === false
		|| $this->delete_register() === false
		|| $this->delete_trunkfeatures() === false
		|| $this->delete_contextmember() === false)
		{
			$r = false;
			$this->_reverse_delete();
		}

		$this->_status = '';

		return($r);
	}

	function delete_trunkfeatures()
	{
		if(xivo_issa('trunkfeatures',$this->_info) === false)
			return(false);

		$r = $this->_trunkfeatures->delete($this->_info['trunkfeatures']['id']);

		if($this->_status === 'delete')
			$this->_return['trunkfeatures'] = $r;

		return($r);
	}

	function delete_protocol()
	{
		if(is_object($this->_protocol) === false
		|| xivo_issa('protocol',$this->_info) === false)
			return(false);

		$r = $this->_protocol->delete($this->_info['protocol']['id']);

		if($this->_status === 'delete')
			$this->_return['protocol'] = $r;

		return($r);
	}

	function delete_register()
	{
		if(is_object($this->_register) === false)
			return(null);
		else if(xivo_issa('trunkfeatures',$this->_info) === false)
			return(false);
		else if(xivo_issa('register',$this->_info) === false)
		{
			if(xivo_ulongint($this->_info['trunkfeatures']['registerid']) === 0)
				return(null);

			return(false);
		}

		$r = $this->_register->delete_register($this->_status,$this->_info['register']['id']);

		if($this->_status === 'delete')
			$this->_return['register'] = $r;

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
				case 'trunkfeatures':
					if(isset($this->_origin['trunkfeatures']) === true
					&& $this->_origin['trunkfeatures'] !== false)
						$this->_trunkfeatures->add_origin($this->_origin['trunkfeatures']);
					break;
				case 'protocol':
					if(isset($this->_origin['protocol']) === true
					&& $this->_origin['protocol'] !== false
					&& is_object($this->_protocol) === true)
						$this->_protocol->add_origin($this->_origin['protocol']);
					break;
				case 'register':
					if(isset($this->_origin['register']) === false
					&& $this->_origin['register'] !== false)
						$this->_register->add_origin($this->_origin['register']);
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

		if($this->enable_protocol() === false
		|| (xivo_issa('trunkfeatures',$this->_info) === true
		   && xivo_ulongint($this->_info['trunkfeatures']['registerid']) !== 0
		   && $this->_info['trunkfeatures']['registercommented'] === false
		   && $this->enable_register() === false) === true)
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

		if($this->disable_protocol() === false
		|| (xivo_issa('trunkfeatures',$this->_info) === true
		   && xivo_ulongint($this->_info['trunkfeatures']['registerid']) !== 0
		   && $this->disable_register() === false) === true)
		{
			$r = false;
			$this->_reverse_enable_disable();
		}

		$this->_status = '';

		return($r);
	}

	function enable_protocol()
	{
		return($this->_enable_disable_protocol(false));
	}

	function disable_protocol()
	{
		return($this->_enable_disable_protocol(true));
	}

	function _enable_disable_protocol($disable=false)
	{
		if(is_object($this->_protocol) === false
		|| xivo_issa('protocol',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_protocol->enable($this->_info['protocol']['id']);
		else
			$r = $this->_protocol->disable($this->_info['protocol']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['protocol'] = $r;

		return($r);
	}

	function _enable_disable_register($disable=false)
	{
		if(is_object($this->_register) === false)
			return(null);
		else if(xivo_issa('register',$this->_info) === false)
			return(false);
		else if((bool) $disable === false)
			$r = $this->_register->enable_register($this->_status,
							       $this->_info['register']['id']);
		else
			$r = $this->_register->disable_register($this->_status,
								$this->_info['register']['id']);

		if($this->_status === 'enable' || $this->_status === 'disable')
			$this->_return['register'] = $r;

		return($r);
	}

	function enable_register()
	{
		return($this->_enable_disable_register(false));
	}

	function disable_register()
	{
		return($this->_enable_disable_register(true));
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
				case 'protocol':
					$this->_enable_disable_protocol($disable);
					break;
				case 'register':
					$this->_enable_disable_register($disable);
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
}

?>
