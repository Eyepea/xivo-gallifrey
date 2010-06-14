<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

dwho::load_class('dwho_network');
dwho::load_class('dwho_uri');
dwho::load_class('dwho_filter');
dwho::load_class('dwho_http');
dwho::load_class('dwho::file::csv');

dwho_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk','config','config.inc'),true);
dwho_file::required(array(XIVO_PATH_OBJECT,'service','ipbx','asterisk.inc',),true);
dwho_file::required(array(XIVO_PATH_OBJECT,'application','xivo.inc'),true);

class xivo_application_xivo_object_wizard extends xivo_application_xivo_object_abstract
{
	var $_conf		= array();
	var $_sysconfd		= false;
	var $_filter		= false;
	var $_uri		= false;
	var $_http_response	= false;
	var $_wizsess		= null;
	var $_ipbx		= null;
	var $_xivo_dso_loaded	= false;

	function xivo_application_xivo_object_wizard(&$xobj,$param=null,$components=true)
	{
		$this->xivo_application_xivo_object_abstract(&$xobj);

		if(($this->_sysconfd   = &$this->_xobj->get_module('sysconfd'))   === false)
			trigger_error('Unable to load sysconfd module',E_USER_ERROR);

		$this->_uri = new dwho_uri();
		$this->_filter = new dwho_filter();
		$this->_http_response = dwho_http::factory('response');

		if(dwho_issa('_wizard',$_SESSION) === false)
			$_SESSION['_wizard'] = array();

		$this->_load_config(dirname(__FILE__));

		$this->_wizsess = &$_SESSION['_wizard'];
	}

	function _load_xivo_dso()
	{
		if($this->_xivo_dso_loaded === true)
			return(true);
		else if(dwho_issa('dbconfig',$this->_wizsess) === false
		|| dwho_issa('save',$this->_wizsess['dbconfig']) === false
		|| isset($this->_wizsess['dbconfig']['save']['xivo']) === false)
			return(false);

		$_XIVO = &dwho_gct::get('xivo');
		$_XIVO->load_datastorage($this->_wizsess['dbconfig']['save']['xivo']);
		$this->_xivo_dso_loaded = true;

		return(true);
	}

	function _load_ipbx_service()
	{
		if(is_object($this->_ipbx) === true)
			return(true);
		else if(dwho_issa('ipbxengine',$this->_wizsess) === false
		|| isset($this->_wizsess['ipbxengine']['engine']) === false
		|| dwho_issa('dbconfig',$this->_wizsess) === false
		|| dwho_issa('save',$this->_wizsess['dbconfig']) === false
		|| isset($this->_wizsess['dbconfig']['save']['ipbx']) === false)
			return(false);

		$_SRE = &dwho_gct::get('xivo_service');
		$engines = $_SRE->get_engines('ipbx');

		if(defined('XIVO_SRE_IPBX_LABEL') === false)
			define('XIVO_SRE_IPBX_LABEL',
			       $engines[$this->_wizsess['ipbxengine']['engine']]['label']);

		$config['general'] = array();
		$config['general']['datastorage'] = $this->_wizsess['dbconfig']['save']['ipbx'];

		$_SRE->set_engine_config('ipbx',$this->_wizsess['ipbxengine']['engine'],$config);

		$this->_ipbx = &$_SRE->get('ipbx',$this->_wizsess['ipbxengine']['engine']);

		return(true);
	}

	function reset_wizard()
	{
		$_SESSION['_wizard'] = array();
	}

	function get_first_step()
	{
		reset($this->_conf['step']);
		return(key($this->_conf['step']));
	}

	function get_last_step()
	{
		end($this->_conf['step']);
		return(key($this->_conf['step']));
	}

	function get_current_step()
	{
		if(isset($this->_wizsess['step']) === false)
			$this->_wizsess['step'] = $this->get_first_step();

		return($this->_wizsess['step']);
	}

	function get_previous_sess_step()
	{
		if(isset($this->_wizsess['prevstep']) === false)
			return(false);

		return($this->_wizsess['prevstep']);
	}

	function get_previous_step($step=null)
	{
		if($step === null && isset($this->_wizsess['step']) === true)
			$step = $this->_wizsess['step'];

		if(isset($this->_conf['step'][$step]) === false)
			return(false);

		if($step === $this->get_last_step())
		{
			end($this->_conf['step']);
			prev($this->_conf['step']);
		}
		else
		{
			reset($this->_conf['step']);
			while((list($key) = each($this->_conf['step']))
			&& $key !== $step);
			prev($this->_conf['step']);
			prev($this->_conf['step']);
		}

		return(key($this->_conf['step']));
	}

	function can_previous_step($step=null)
	{
		if($step === null && isset($this->_wizsess['step']) === true)
			$step = $this->_wizsess['step'];

		if($step === $this->get_first_step())
			return(false);

		$prevstep = $this->get_previous_step($step);

		if(dwho_issa('commit',$this->_wizsess) === true
		&& dwho_issa('step',$this->_wizsess['commit']) === true
		&& isset($this->_wizsess['commit']['step'][$prevstep]) === true)
			return(false);
		else if($step === $this->get_last_step())
			return($this->get_previous_step($step));
		else if(isset($this->_conf['step'][$prevstep]) === false)
			return(false);

		return($prevstep);
	}

	function set_previous_step()
	{
		$step = $this->get_current_step();

		if(($step = $this->can_previous_step($step)) === false)
			return(false);

		$this->_wizsess['step'] = $step;
	}

	function get_next_step($step=null)
	{
		if($step === null && isset($this->_wizsess['step']) === true)
			$step = $this->_wizsess['step'];

		if(isset($this->_conf['step'][$step]) === false)
			return(false);

		reset($this->_conf['step']);
		while((list($key) = each($this->_conf['step']))
		&& $key !== $step);

		return(key($this->_conf['step']));
	}

	function can_next_step($step=null)
	{
		if($step === null && isset($this->_wizsess['step']) === true)
			$step = $this->_wizsess['step'];

		if($step === $this->get_last_step())
			return(false);

		return($step);
	}

	function get_step_element()
	{
		if(isset($this->_wizsess['step']) === true)
			$step = $this->_wizsess['step'];
		else
			$step = 'welcome';

		switch($step)
		{
			case 'dbconfig':
				return($this->_conf['element'][$step]);
			case 'mainconfig':
				// read system dns
				$resolvconf = $this->_conf['element']['resolvconf'];
				if(($dns = $this->_sysconfd->request_get('/dns')) != false)
				{
					$resolvconf['nameserver1'] = $dns['nameservers'][0];
					$resolvconf['nameserver2'] = $dns['nameservers'][1];
				}

				return(array('mainconfig'	=> $this->_conf['element']['mainconfig'],
					     'netiface'		=> $this->_conf['element']['netiface'],
					     'resolvconf'	=> $resolvconf));
			case 'entitycontext':
				return(array('entity'	=> $this->_conf['element']['entity'],
					     'context'	=> $this->_conf['element']['context']));
			default:
				return(null);
		}
	}

	function discover_finish_uri()
	{
		if(isset($_SERVER['SERVER_ADDR'],$_SERVER['SERVER_PORT']) === false)
			return(false);
		else if($_SERVER['SERVER_ADDR'] !== '127.0.0.1'
		&& $_SERVER['REMOTE_ADDR'] !== '::1'
		&& dwho_issa('netiface',$this->_wizsess) === true
		&& isset($this->_wizsess['netiface']['address']) === true)
			$uri = $this->_wizsess['netiface']['address'];
		else
			$uri = $_SERVER['SERVER_ADDR'];

		$uri .= ':'.$_SERVER['SERVER_PORT'];

		$scheme = 'http';

		if(isset($_SERVER['HTTPS']) === true
		&& (bool) $_SERVER['HTTPS'] === true)
			$scheme .= 's';

		return($scheme.'://'.$uri);
	}

	function step_ipbxengine()
	{
		$r = array();

		if(dwho_issa('ipbxengine',$this->_wizsess) === true
		&& isset($this->_wizsess['engine']) === true)
			$r['selected'] = $this->_wizsess['engine'];
		else
			$r['selected'] = null;

		$_SRE = &dwho_gct::get('xivo_service');
		$r['engines'] = $_SRE->get_engines('ipbx');

		return($r);
	}

	function step_dbconfig()
	{
		if(dwho_issa('dbconfig',$this->_wizsess) === true
		&& dwho_issa('result',$this->_wizsess['dbconfig']) === true)
			return($this->_wizsess['dbconfig']['result']);

		return(null);
	}

	function _build_lshw($o)
	{
		if(is_array($o) === true)
		{
			$a = array();
			foreach($o as $k => $v)
			{
				if(is_object($v) === false)
				{
					$a[$k] = $v;
					continue;
				}
				else if(isset($v->id) === true)
					$attrname = $v->id;
				else if(isset($v->type) === true)
					$attrname = $v->type;
				else
				{
					$a[$k] = $v;
					continue;
				}

				if(isset($a[$attrname]) === true)
				{
					$hold = $a[$attrname];
					$a[$attrname] = array();
					$a[$attrname][] = $hold;
					$a[$attrname][] = $this->_build_lshw($v);
				}
				else if(isset($v->value) === true
				&& is_scalar($v->value) === true)
					$a[$attrname] = $v->value;
				else if(isset($v->__cdata__) === true
				&& is_scalar($v->__cdata__) === true)
					$a[$attrname] = $v->__cdata__;
				else if(count((array) $v) === 1)
					$a[$attrname] = '';
				else if(($value = $this->_build_lshw($v)) !== null)
					$a[$attrname] = $value;
			}

			return($a);
		}
		else if(is_object($o) === true)
		{
			$a = array();

			foreach($o as $k => $v)
			{
				if($k === 'disabled' && dwho_bool($v) === true)
					return(null);

				$a[$k] = $this->_build_lshw($v);
			}

			return($a);
		}

		return($o);
	}

	function lshw_infos()
	{
		$r = array();
		$r['network'] = array();
		$r['network']['vendor'] = '';
		$r['network']['driver'] = '';
		$r['network']['driverversion'] = '';
		$r['network']['macaddress'] = '';
		$r['network']['iface'] = '';
		$r['network']['ipaddress'] = '';
		$r['network']['speed'] = '';
		$r['network']['duplex'] = '';
		$r['network']['autonegotiation'] = '';

		if(($lshw = $this->_build_lshw($this->_sysconfd->request_get('/lshw',
							array(array('class[]','network')),
							false))) === false
		|| dwho_issa('lshw',$lshw) === false
		|| dwho_issa('node',$lshw['lshw']) === false)
			return($r);

		$ref = &$lshw['lshw']['node'];

		if(isset($ref['network:0']) === true)
			$refnet = &$ref['network:0'];
		else if(isset($ref['network']) === true)
			$refnet = &$ref['network'];
		else
			$refnet = &$ref;

		if(is_array($refnet) === false)
			return($r);

		if(isset($refnet['vendor']) === true)
			$r['network']['vendor'] = $refnet['vendor'];

		if(isset($refnet['product']) === true)
			$r['network']['product'] = $refnet['product'];

		if(isset($refnet['logicalname']) === true)
			$r['network']['iface'] = $refnet['logicalname'];

		if(isset($refnet['serial']) === true)
			$r['network']['macaddress'] = $refnet['serial'];

		if(dwho_issa('configuration',$refnet) === false
		|| dwho_issa('setting',$refnet['configuration']) === false)
			return($r);

		$refsetting = &$refnet['configuration']['setting'];

		if(isset($refsetting['driver']) === true)
			$r['network']['driver'] = $refsetting['driver'];

		if(isset($refsetting['driverversion']) === true)
			$r['network']['driverversion'] = $refsetting['driverversion'];

		if(isset($refsetting['ip']) === true)
			$r['network']['ipaddress'] = $refsetting['ip'];

		if(isset($refsetting['speed']) === true)
			$r['network']['speed'] = $refsetting['speed'];

		if(isset($refsetting['duplex']) === true)
			$r['network']['duplex'] = $refsetting['duplex'];

		if(isset($refsetting['autonegotiation']) === true)
			$r['network']['autonegotiation'] = $refsetting['autonegotiation'];

		return($r);
	}

	function packages_infos($levels=null,$force_aptcache_update=false)
	{
		if(dwho_issa('ipbxengine',$this->_wizsess) === false
		|| isset($this->_wizsess['ipbxengine']['engine']) === false
		|| dwho_issa('dbconfig',$this->_wizsess) === false
		|| dwho_issa('result',$this->_wizsess['dbconfig']) === false
		|| isset($this->_wizsess['dbconfig']['result']['backend']) === false)
			return(false);

		$ipbxengine = $this->_wizsess['ipbxengine']['engine'];
		$dbbackend = $this->_wizsess['dbconfig']['result']['backend'];

		if(isset($this->_wizsess['aptcache_update']) === false
		|| (bool) $force_aptcache_update === true)
			$this->_wizsess['aptcache_update'] = $this->_sysconfd->request_get('/aptcache_update');

		$query = array();

		if(is_array($levels) === true)
		{
			foreach($levels as $value)
				$query[] = array('level[]',$value);
		}

		$r = array();
		$r['ipbx'] = $this->_sysconfd->request_get('/dependencies_'.$ipbxengine,$query);
		$r['db'] = $this->_sysconfd->request_get('/dependencies_'.$dbbackend,$query);
		$r['xivo'] = $this->_sysconfd->request_get('/dependencies_xivo',$query);

		return($r);
	}

	function step_checkcomponents($force_aptcache_update=false)
	{
		$r = array();
		$r['hardware'] = $this->lshw_infos();
		$r['packages'] = $this->packages_infos(null,$force_aptcache_update);

		return($r);
	}

	function get_eth_iface()
	{
		if(($res = $this->_sysconfd->request_get('/discover_netifaces')) === false
		|| is_array($res) === false)
			return(false);

		$r = array();

		foreach($res as $ifname => $iface)
		{
			if(dwho_ak('family',$iface,true) !== 'inet'
			|| dwho_ak('type',$iface,true) !== 'eth'
			|| dwho_has_len($iface,'address') === false
			|| dwho_has_len($iface,'netmask') === false)
				continue;
			else if(isset($_SERVER['SERVER_ADDR']) === true
			&& $_SERVER['SERVER_ADDR'] !== '127.0.0.1'
			&& $_SERVER['SERVER_ADDR'] !== '::1'
			&& dwho_network::ipv4_in_subnet($_SERVER['SERVER_ADDR'],
							$iface['address'].'/'.$iface['netmask']) === false)
				continue;
			if($_SERVER['SERVER_ADDR'] !== $iface['address'])
				array_push($r,$iface);
			else
			{
				array_unshift($r,$iface);
				break;
			}
		}

		return(array_shift($r));
	}

	function step_mainconfig()
	{
		$r = array();

		if(dwho_issa('mainconfig',$this->_wizsess) === true)
			$r['mainconfig'] = $this->_wizsess['mainconfig'];
		else
			$r['mainconfig'] = null;

		if(dwho_issa('netiface',$this->_wizsess) === true)
			$r['netiface'] = $this->_wizsess['netiface'];
		else
			$r['netiface'] = $this->get_eth_iface();

		if(dwho_issa('resolvconf',$this->_wizsess) === true)
			$r['resolvconf'] = $this->_wizsess['resolvconf'];
		else
			$r['resolvconf'] = null;

		return($r);
	}

	function step_entitycontext()
	{
		$r = array();

		if(dwho_issa('entity',$this->_wizsess) === true)
			$r['entity'] = $this->_wizsess['entity'];
		else
			$r['entity'] = null;

		if(dwho_issa('context_internal',$this->_wizsess) === true)
			$r['context_internal'] = $this->_wizsess['context_internal'];
		else
			$r['context_internal'] = null;

		if(dwho_issa('context_incall',$this->_wizsess) === true)
			$r['context_incall'] = $this->_wizsess['context_incall'];
		else
			$r['context_incall'] = null;

		if(dwho_issa('context_outcall',$this->_wizsess) === true)
			$r['context_outcall'] = $this->_wizsess['context_outcall'];
		else
			$r['context_outcall'] = null;

		return($r);
	}

	function step_validate()
	{
		return($this->_wizsess);
	}

	function save_step_welcome($data)
	{
		$this->_wizsess['prevstep'] = $this->get_current_step();

		if(is_array($data) === false
		|| isset($data['hl']) === false)
		{
			$this->_set_error('welcome','language','missing');
			return(false);
		}

		$this->_wizsess['welcome'] = array();

		$_LANG = &dwho_gat::load_get('language',XIVO_PATH_OBJECTCONF);
		$language_available = dwho_array_intersect_key($_LANG,dwho_i18n::get_language_translated_list());

		if(isset($language_available[$data['hl']]) === false)
		{
			$this->_set_error('welcome','language','invalid');
			return(false);
		}

		$_SESSION['_i18n']['user'] = array();
		$_SESSION['_i18n']['user']['language'] = $data['hl'];

		$this->_wizsess['welcome']['language'] = $data['hl'];
		$this->_wizsess['step'] = $this->get_next_step('welcome');

		return(true);
	}

	function save_step_license($data)
	{
		$this->_wizsess['prevstep'] = $this->get_current_step();

		if(is_array($data) === false
		|| isset($data['license-agree']) === false
		|| (bool) $data['license-agree'] === false)
		{
			$this->_set_error('license','disagree');
			return(false);
		}

		$this->_wizsess['license'] = array();
		$this->_wizsess['license']['agree'] = true;

		$this->_wizsess['step'] = $this->get_next_step('license');

		return(true);
	}

	function save_step_ipbxengine($data)
	{
		$this->_wizsess['prevstep'] = $this->get_current_step();

		if(is_array($data) === false
		|| isset($data['ipbxengine']) === false)
		{
			$this->_set_error('ipbxengine','missing');
			return(false);
		}

		$_SRE = &dwho_gct::get('xivo_service');
		$engines = $_SRE->get_engines('ipbx');

		if(isset($engines[$data['ipbxengine']]) === false)
		{
			$this->_set_error('ipbxengine','invalid');
			return(false);
		}

		$this->_wizsess['ipbxengine'] = array();
		$this->_wizsess['ipbxengine']['engine'] = $data['ipbxengine'];

		$this->_wizsess['step'] = $this->get_next_step('ipbxengine');

		return(true);
	}

	function save_step_dbconfig($data)
	{
		$this->_wizsess['prevstep'] = $this->get_current_step();

		if(dwho_issa('dbconfig',$data) === false
		|| isset($data['dbconfig']['backend']) === false)
		{
			$this->_set_error('dbconfig','backend','missing');
			return(false);
		}

		$this->_filter->reset_all();
		$this->_filter->set_validator(array('backend' => $this->_conf['filter']['dbconfig']['backend']));

		if(($backend = $this->_filter->chk_value('backend',$data['dbconfig']['backend'])) === false
		|| isset($this->_conf['filter']['dbconfig'][$backend],
			 $data['dbconfig'][$backend]) === false)
			$this->_set_error('dbconfig','backend','invalid');

		$result = array();
		$result['backend'] = $backend;

		if($this->get_errnb() > 0)
		{
			$this->_set_result('dbconfig',$result);
			return(false);
		}

		$this->_filter->set_validator($this->_conf['filter']['dbconfig'][$backend]);

		if($this->_filter->chk($data['dbconfig'][$backend]) === false)
			$this->_set_errors('dbconfig',$backend,$this->_filter->get_error());

		$result[$backend] = $this->_filter->get_result();

		if($this->get_errnb() > 0)
		{
			$this->_set_result('dbconfig',$result);
			return(false);
		}

		$ipbxdburi = $xivodburi = array('scheme' => $result['backend']);

		$ref = &$result[$result['backend']];

		switch($result['backend'])
		{
			case 'sqlite':
				$xivodburi['path'] = $ref['xivodb'];
				$ipbxdburi['path'] = $ref['ipbxdb'];
				break;
			case 'mysql':
				$xivodburi['host'] = $ref['host'];
				$xivodburi['port'] = $ref['port'];
				$xivodburi['authority'] = array();
				$xivodburi['authority']['user'] = $ref['xivouser'];
				$xivodburi['authority']['passwd'] = $ref['xivopass'];
				$xivodburi['path'] = '/'.$ref['xivodbname'];

				$ipbxdburi['host'] = $ref['host'];
				$ipbxdburi['port'] = $ref['port'];
				$ipbxdburi['authority'] = array();
				$ipbxdburi['authority']['user'] = $ref['ipbxuser'];
				$ipbxdburi['authority']['passwd'] = $ref['ipbxpass'];
				$ipbxdburi['path'] = '/'.$ref['ipbxdbname'];
				break;
			default:
				$this->_set_error('dbconfig','backend','invalid');
		}

		if(($xivodburistr = $this->_uri->build_uri($xivodburi)) === false)
			$this->_set_error('dbconfig',$backend,'xivo','invalid');

		if(($ipbxdburistr = $this->_uri->build_uri($ipbxdburi)) === false)
			$this->_set_error('dbconfig',$backend,'ipbx','invalid');

		$this->_set_result('dbconfig',$result);

		if($this->get_errnb() > 0)
			return(false);

		dwho::load_class('dwho_datastorage');

		if(($xivoparamdso = dwho_datastorage::get_param_from_uri($xivodburistr)) === false
		|| ($xivodso = @dwho_datastorage::factory($xivoparamdso['type'],$xivoparamdso)) === false)
			$this->_set_error('dbconfig',$backend,'xivo','invalid');

		if(($ipbxparamdso = dwho_datastorage::get_param_from_uri($ipbxdburistr)) === false
		|| ($ipbxdso = @dwho_datastorage::factory($ipbxparamdso['type'],$ipbxparamdso)) === false)
			$this->_set_error('dbconfig',$backend,'ipbx','invalid');

		if($this->get_errnb() > 0)
			return(false);

		if($xivodso->is_open() === false)
			$this->_set_error('dbconfig',$backend,'xivo','connection');
		else
			$xivodso->close();

		if($ipbxdso->is_open() === false)
			$this->_set_error('dbconfig',$backend,'ipbx','connection');
		else
			$ipbxdso->close();

		if($this->get_errnb() > 0)
			return(false);

		$this->_wizsess['dbconfig'] = array();
		$this->_wizsess['dbconfig']['save'] = array();
		$this->_wizsess['dbconfig']['save']['xivo'] = $xivodburistr;
		$this->_wizsess['dbconfig']['save']['ipbx'] = $ipbxdburistr;
		$this->_wizsess['dbconfig']['result'] = $result;

		$this->_wizsess['step'] = $this->get_next_step('dbconfig');

		return(true);
	}

	function save_step_checkcomponents()
	{
		$this->_wizsess['prevstep'] = $this->get_current_step();

		if(($packages = $this->packages_infos(array('depends'),true)) === false)
			return(false);

		$error = array();

		foreach($packages as $category => $levels)
		{
			if(dwho_issa('depends',$levels) === false)
				continue;

			foreach($levels['depends'] as $package => $value)
			{
				if(isset($value['status']) === true
				&& $value['status'] !== 'installed')
				{
					if(isset($error[$category]) === false)
						$error[$category] = array();

					$error[$category][$package] = $value['status'];
				}
			}
		}

		if(empty($error) === false)
		{
			$this->_set_error('packages',$error);
			return(false);
		}

		$this->_wizsess['checkcomponents'] = true;
		$this->_wizsess['step'] = $this->get_next_step('checkcomponents');

		return(false);
	}

	function save_mainconfig($data)
	{
		if(dwho_issa('mainconfig',$data) === false)
		{
			$this->_set_error('mainconfig','hostname','missing');
			return(false);
		}

		$this->_filter->set_validator($this->_conf['filter']['mainconfig']);

		if(($r = (bool) $this->_filter->chk($data['mainconfig'])) === false)
			$this->_set_error('mainconfig',$this->_filter->get_error());

		$this->_set_result('mainconfig',$this->_filter->get_result());

		return($r);
	}

	function save_netiface($data)
	{
		if(($iface = $this->get_eth_iface()) === null)
			return(null);
		else if(dwho_issa('netiface',$data) === false)
		{
			$this->_set_error('netiface','address','missing');
			return(false);
		}

		$this->_filter->set_validator($this->_conf['filter']['netiface']);

		if(($r = (bool) $this->_filter->chk($data['netiface'])) === false)
			$this->_set_error('netiface',$this->_filter->get_error());

		$resnetiface = $this->_filter->get_result();
		$resnetiface['name'] = $iface['name'];
		$this->_set_result('netiface',$resnetiface);

		if($r === true
		&& dwho_has_len($resnetiface,'gateway') === true
		&& dwho_network::ipv4_in_subnet($resnetiface['gateway'],
						$resnetiface['address'].
							'/'.$resnetiface['netmask']) === false)
		{
			$r = false;
			$this->_set_error('netiface','gateway','invalid');
		}

		return($r);
	}

	function save_resolvconf($data)
	{
		if(dwho_issa('resolvconf',$data) === false)
		{
			$this->_set_error('resolvconf','nameserver1','invalid');
			return(false);
		}

		$this->_filter->set_validator($this->_conf['filter']['resolvconf']);

		if(($r = (bool) $this->_filter->chk($data['resolvconf'])) === false)
			$this->_set_error('resolvconf',$this->_filter->get_error());

		$result = $this->_filter->get_result();

		if($r === true)
		{
			$result = dwho_array_combine(array_keys($result),
						     explode(' ',trim(dwho_implode(' ',$result))),
						     true,
						     '');
		}

		$this->_set_result('resolvconf',$result);

		return($r);
	}

	function save_step_mainconfig($data)
	{
		$this->_wizsess['prevstep'] = $this->get_current_step();

		$this->save_mainconfig($data);
		$this->save_netiface($data);
		$this->save_resolvconf($data);

		if($this->get_errnb() > 0)
			return(false);

		$this->_wizsess['mainconfig'] = $this->get_result('mainconfig');
		$this->_wizsess['netiface'] = $this->get_result('netiface');
		$this->_wizsess['resolvconf'] = $this->get_result('resolvconf');

		$this->_wizsess['step'] = $this->get_next_step('mainconfig');

		return(true);
	}

	function save_entity($data)
	{
		if(dwho_issa('entity',$data) === false)
		{
			$this->_set_error('entity','name','missing');
			return(false);
		}

		$appentity = &$this->_xobj->get_application('entity');

		if(($r = (bool) $appentity->set_add($data['entity'])) === false)
			$this->_set_error('entity',$appentity->get_error('entity'));

		$this->_set_result('entity',$appentity->get_result('entity'));

		return($r);
	}

	function _prepare_context($data,$type=null,$entity=null)
	{
		$r = array();
		$r['context']['name'] = dwho_ak('name',$data,true);
		$r['context']['displayname'] = dwho_ak('displayname',$data,true);
		$r['context']['entity'] = strval($entity);

		if($type === null)
			return($r);

		$r['contextnumbers'] = array();
		$r['contextnumbers'][$type] = array();
		$r['contextnumbers'][$type][0] = array();
		$r['contextnumbers'][$type][0]['numberbeg'] = dwho_ak('numberbeg',$data,true);
		$r['contextnumbers'][$type][0]['numberend'] = dwho_ak('numberend',$data,true);

		if($type === 'incall')
			$r['contextnumbers'][$type][0]['didlength'] = dwho_ak('didlength',$data,true);

		return($r);
	}

	function save_context($data)
	{
		if(dwho_issa('context',$data) === false)
		{
			$this->_set_error('context_internal','name','missing');
			$this->_set_error('context_incall','name','missing');
			$this->_set_error('context_outcall','name','missing');
			return(false);
		}

		$entity = $this->get_result_var('entity','name');

		$appcontext = &$this->_ipbx->get_application('context');

		$outcall = null;
		$resoutcall = false;

		if(dwho_issa('outcall',$data['context']) === false)
			$this->_set_error('context_outcall','missing_context');
		else
		{
			$outcall = $this->_prepare_context($data['context']['outcall']);

			if(($resoutcall = (bool) $appcontext->set_add($outcall)) === false)
				$this->_set_error('context_outcall',$appcontext->get_error());

			$this->_set_result('context_outcall',$appcontext->get_result());
		}

		$internal = null;
		$resinternal = false;

		if(dwho_issa('internal',$data['context']) === false)
			$this->_set_error('context_internal','missing_context');
		else
		{
			$internal = $this->_prepare_context($data['context']['internal'],'user',$entity);

			if(isset($outcall['context']['name']) === true)
				$internal['contextinclude'] = array($outcall['context']['name']);

			if(($resinternal = (bool) $appcontext->set_add($internal)) === false)
				$this->_set_error('context_internal',$appcontext->get_error());
			else if(dwho_has_len($data['context']['internal'],'numberbeg') === false)
			{
				$resinternal = false;
				$this->_set_error('context_internal','missing_numberbeg');
			}

			$this->_set_result('context_internal',$appcontext->get_result());
		}

		$incall = null;
		$resincall = false;

		if(dwho_issa('incall',$data['context']) === false)
			$this->_set_error('context_incall','name','missing');
		else
		{
			$incall = $this->_prepare_context($data['context']['incall'],'incall',$entity);

			if(dwho_has_len($incall['contextnumbers']['incall'][0]['numberbeg']) === false
			&& dwho_has_len($incall['contextnumbers']['incall'][0]['numberend']) === false)
				unset($incall['contextnumbers']);

			if(($resincall = (bool) $appcontext->set_add($incall)) === false)
				$this->_set_error('context_incall',$appcontext->get_error());

			$this->_set_result('context_incall',$appcontext->get_result());
		}

		if($resoutcall === false
		|| $resinternal === false
		|| $resincall === false)
			return(false);

		return(array('outcall'	=> $outcall,
			     'internal'	=> $internal,
			     'incall'	=> $incall));
	}

	function save_step_entitycontext($data)
	{
		$this->_load_xivo_dso();
		$this->_load_ipbx_service();

		$this->_wizsess['prevstep'] = $this->get_current_step();

		$this->save_entity($data);
		$rescontext = $this->save_context($data);

		if($this->get_errnb() > 0 | is_array($rescontext) === false)
			return(false);

		$this->_wizsess['entity'] = $this->get_result('entity');
		$this->_wizsess['context'] = $rescontext;
		$this->_wizsess['context_internal'] = $this->get_result('context_internal');
		$this->_wizsess['context_incall'] = $this->get_result('context_incall');
		$this->_wizsess['context_outcall'] = $this->get_result('context_outcall');

		$this->_wizsess['step'] = $this->get_next_step('entitycontext');

		return(true);
	}

	function save_step_ipbximportuser($data)
	{
		if(dwho_issa('import',$_FILES) === false
		|| isset($_FILES['import']['error']) === false
		|| $_FILES['import']['error'] === UPLOAD_ERR_NO_FILE)
		{
			$this->_wizsess['ipbximportuser'] = null;
			$this->_wizsess['step'] = $this->get_next_step('ipbximportuser');
			return(null);
		}
		else if(($fileinfo = $this->_http_response->upload_file('import',
									$this->_conf['ipbximportuser']['file'])) === false)
		{
			$this->_set_error('ipbximportuser','file','invalid_name');
			return(false);
		}
		else if(dwho_issa('context_internal',$this->_wizsess) === false
		|| dwho_issa('context_incall',$this->_wizsess) === false
		|| isset($this->_wizsess['context_internal']['context']['name'],
			 $this->_wizsess['context_incall']['context']['name']) === false)
		{
			$this->_set_error('ipbximportuser','missing_contexts');
			return(false);
		}

		$internalcontext = $this->_wizsess['context_internal']['context']['name'];
		$internalnumberbeg = null;
		$internalnumberend = null;

		if(dwho_issa('contextnumbers',$this->_wizsess['context_internal']) === true
		&& dwho_issa('user',$this->_wizsess['context_internal']['contextnumbers']) === true
		&& dwho_issa(0,$this->_wizsess['context_internal']['contextnumbers']['user']) === true)
		{
			if(isset($this->_wizsess['context_internal']['contextnumbers']['user'][0]['numberbeg']) === true)
				$internalnumberbeg = $this->_wizsess['context_internal']['contextnumbers']['user'][0]['numberbeg'];

			if(isset($this->_wizsess['context_internal']['contextnumbers']['user'][0]['numberend']) === true)
				$internalnumberend = $this->_wizsess['context_internal']['contextnumbers']['user'][0]['numberend'];
		}

		$incallcontext = $this->_wizsess['context_incall']['context']['name'];
		$incallnumberbeg = null;
		$incallnumberend = null;
		$incalldidlength = 0;

		if(dwho_issa('contextnumbers',$this->_wizsess['context_incall']) === true
		&& dwho_issa('incall',$this->_wizsess['context_incall']['contextnumbers']) === true
		&& dwho_issa(0,$this->_wizsess['context_incall']['contextnumbers']['incall']) === true)
		{
			if(isset($this->_wizsess['context_incall']['contextnumbers']['incall'][0]['numberbeg']) === true)
				$incallnumberbeg = $this->_wizsess['context_incall']['contextnumbers']['incall'][0]['numberbeg'];

			if(isset($this->_wizsess['context_incall']['contextnumbers']['incall'][0]['numberend']) === true)
				$incallnumberend = $this->_wizsess['context_incall']['contextnumbers']['incall'][0]['numberend'];

			if(isset($this->_wizsess['context_incall']['contextnumbers']['incall'][0]['didlength']) === true)
				$incalldidlength = $this->_wizsess['context_incall']['contextnumbers']['incall'][0]['didlength'];
		}

		$didlength = -($incalldidlength);
		$numberbeg = substr($incallnumberbeg,$didlength);
		$numberend = substr($incallnumberend,$didlength);

		$this->_load_ipbx_service();

		$filecsv = new dwho_file_csv();

		if(($lines = $filecsv->parse($fileinfo['tmp_name'],
					     0,
					     '|',
					     null,
					     true,
					     $this->_conf['ipbximportuser']['field'])) === false
		|| ($nb = count($lines)) === 0)
		{
			dwho_file::rm($fileinfo['tmp_name']);
			$this->_set_error('ipbximportuser','file','invalid_content');
			return(false);
		}

		$reallines = $lines;

		$r = array();
		$r['lines'] = array();
		$r['total'] = array();
		$r['total']['success'] = 0;
		$r['total']['error'] = 0;
		$r['total']['count'] = $nb;

		for($i = 0,$row = 1;$i < $nb;$i++,$row++)
		{
			$ref = &$lines[$i];
			$refreal = &$reallines[$i];
			$ref['protocol']['context'] = $internalcontext;
			$ref['protocol']['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
			$refreal['protocol']['context'] = $internalcontext;
			$refreal['protocol']['protocol'] = XIVO_SRE_IPBX_AST_PROTO_SIP;

			if(dwho_has_len($ref['userfeatures'],'number') === true
			&& dwho_has_len($internalnumberbeg) === true
			&& xivo_service_asterisk::valid_exten_number_in_interval(
							$internalnumberbeg,
							$ref['userfeatures']['number'],
							$internalnumberend) === false)
			{
				$r['lines'][$row] = 'error';
				$r['total']['error']++;
				continue;
			}

			unset($ref['userfeatures']['number']);

			if(dwho_issa('incall',$ref) === false
			|| dwho_has_len($ref['incall'],'exten') === false)
				continue;
			else if(dwho_has_len($numberbeg) === false
			|| xivo_service_asterisk::valid_exten_in_interval(
						$numberbeg,
						$ref['incall']['exten'],
						$numberend) === false)
			{
				$r['lines'][$row] = 'error';
				$r['total']['error']++;
				continue;
			}

			$refreal['incall']['context'] = $incallcontext;
			unset($ref['incall']);

			$r['lines'][$row] = 'success';
			$r['total']['success']++;
		}

		if($r['total']['error'] > 0)
		{
			$this->_set_error('ipbximportuser',$r);
			return(false);
		}

		$appuser = &$this->_ipbx->get_application('user');
		$r = $appuser->import_from_array($lines,false,false);

		if($r['total']['error'] > 0)
		{
			$this->_set_error('ipbximportuser',$r);
			return(false);
		}

		$this->_wizsess['ipbximportuser'] = $reallines;
		$this->_wizsess['step'] = $this->get_next_step('ipbximportuser');

		return(true);
	}

	function _commit_adminpasswd()
	{
		if(dwho_issa('mainconfig',$this->_wizsess) === false
		|| isset($this->_wizsess['mainconfig']['adminpasswd']) === false)
			return(false);

		$this->_load_xivo_dso();
		xivo::load_class('xivo_user',XIVO_PATH_OBJECT);
		xivo::load_class('xivo_user_root',
				 array(XIVO_PATH_OBJECT,'user'),
				 'root');

		$root = new xivo_user_root(false);

		if(($info = $root->get_where(array('meta' => 'root'))) === false
		|| $root->edit('root',
			       array('id'	=> $info['id'],
				     'passwd'	=> $this->_wizsess['mainconfig']['adminpasswd'])) === false)
		{
			$this->_set_error('mainconfig','adminpasswd','save');
			return(false);
		}

		return(true);
	}

	function _commit_hosts()
	{
		if(dwho_issa('mainconfig',$this->_wizsess) === false
		|| isset($this->_wizsess['mainconfig']['hostname'],
			 $this->_wizsess['mainconfig']['domain']) === false)
			return(false);

		$hosts = array();
		$hosts['hostname'] = $this->_wizsess['mainconfig']['hostname'];
		$hosts['domain'] = $this->_wizsess['mainconfig']['domain'];

		if($this->_sysconfd->request_post('/hosts',$hosts) === false)
		{
			$this->_set_error('mainconfig','hosts','save');
			return(false);
		}

		return(true);
	}

	function _commit_resolvconf()
	{
		if(dwho_issa('resolvconf',$this->_wizsess) === false)
			return(null);

		$resolvconf = array();
		$resolvconf['nameservers'] = array_values(
						array_filter($this->_wizsess['resolvconf'],
							     'dwho_has_len'));

		if(empty($resolvconf['nameservers']) === true)
			return(null);

		if(dwho_issa('mainconfig',$this->_wizsess) === true
		&& isset($this->_wizsess['mainconfig']['domain']) === true)
			$resolvconf['search'] = $this->_wizsess['mainconfig']['domain'];

		if($this->_sysconfd->request_post('/resolv_conf',$resolvconf) === false)
		{
			$this->_set_error('resolvconf','nameservers','save');
			return(false);
		}

		// save to database
		$this->_load_xivo_dso();
		$this->_load_ipbx_service();
		$module = &$this->_xobj->get_module('resolvconf');

		$data = array(
			'hostname'    => $this->_wizsess['mainconfig']['hostname'],
			'domain'      => $this->_wizsess['mainconfig']['domain'],
			'nameserver1' => $resolvconf['nameservers'][0],
			'nameserver2' => $resolvconf['nameservers'][1],
			'nameserver3' => '',
			'search'      => $resolvconf['search'],
			'description' => 'wizard configuration'
		);

		if($module->save($data) === false)
			return false;

		return(true);
	}

	function _commit_entity()
	{
		if(dwho_issa('entity',$this->_wizsess) === false)
			return(false);

		$this->_load_xivo_dso();
		$this->_load_ipbx_service();

		$appentity = &$this->_xobj->get_application('entity');

		// allow to reexecute the process several times
		if($appentity->get(1) !== false)
			$appentity->delete_entity();

		if($appentity->set_add($this->_wizsess['entity']) === false
		|| $appentity->add() === false)
		{
			$this->_set_error('entity','save');
			return(false);
		}

		return(true);
	}

	function _commit_context()
	{
		if(dwho_issa('context',$this->_wizsess) === false
		|| dwho_issa('internal',$this->_wizsess['context']) === false
		|| dwho_issa('incall',$this->_wizsess['context']) === false
		|| dwho_issa('outcall',$this->_wizsess['context']) === false)
			return(false);

		$this->_load_xivo_dso();
		$this->_load_ipbx_service();

		$context_internal = true;
		$context_incall = true;
		$context_outcall = true;

		$appcontext = &$this->_ipbx->get_application('context');

		// allow to reexecute the process several times
		foreach($appcontext->get_contexts_list() as $context)
		{
			if($appcontext->get($context['context']['name']) !== false)
			{
				$appcontext->_info['deletable']       = true;
				$appcontext->_info['deletablenumber'] = true;
				$appcontext->_info['deletablemember'] = true;

				$appcontext->delete();
			}
		}


		if($appcontext->set_add($this->_wizsess['context']['internal']) === false
		|| $appcontext->add() === false)
		{
			$context_internal = false;
			$this->_set_error('context_internal','save');
		}

		if($appcontext->set_add($this->_wizsess['context']['incall']) === false
		|| $appcontext->add() === false)
		{
			$context_incall = false;
			$this->_set_error('context_incall','save');
		}

		if($appcontext->set_add($this->_wizsess['context']['outcall']) === false
		|| $appcontext->add() === false)
		{
			$context_outcall = false;
			$this->_set_error('context_outcall','save');
		}

		if($context_internal === false
		|| $context_incall === false
		|| $context_outcall === false)
			return(false);

		return(true);
	}

	function _commit_ipbximportuser()
	{
		if(dwho_issa('ipbximportuser',$this->_wizsess) === false
		|| empty($this->_wizsess['ipbximportuser']) === true)
			return(null);

		$this->_load_ipbx_service();
		$appuser = &$this->_ipbx->get_application('user');
		$r = $appuser->import_from_array($this->_wizsess['ipbximportuser']);

		if($r['total']['error'] > 0)
		{
			$this->_set_error('ipbximportuser',$r);
			return(false);
		}

		return(true);
	}

	function _commit_dbconfig()
	{
		$dbconfig = array();
		$dbconfig['xivo'] = $this->_wizsess['dbconfig']['save']['xivo'];
		$dbconfig['ipbx'] = $this->_wizsess['dbconfig']['save']['ipbx'];
		$dbconfig['ipbxengine'] = $this->_wizsess['ipbxengine']['engine'];

		if($this->_sysconfd->request_post('/set_db_backends',$dbconfig) === false)
		{
			$this->_set_error('dbconfig','backends','save');
			return(false);
		}

		return(true);
	}

	function _commit_netiface()
	{
		if(dwho_issa('netiface',$this->_wizsess) === false
		|| isset($this->_wizsess['netiface']['name']) === false)
			return(false);

		$netiface = $this->_wizsess['netiface'];
		$name = $netiface['name'];
		unset($netiface['name']);

		if(dwho_has_len($netiface,'gateway') === false)
			unset($netiface['gateway']);

		if($this->_sysconfd->request_post('/modify_eth_ipv4',
						  $netiface,
						  array(array('ifname',$name))) === false)
		{
			$this->_set_error('netiface','save');
			return(false);
		}

		return true;
	}
	
	function _prepare_netiface()
	{
		if(dwho_issa('netiface',$this->_wizsess) === false
		|| isset($this->_wizsess['netiface']['name']) === false)
			return(false);

		$netiface = $this->_wizsess['netiface'];
		$name = $netiface['name'];

		// save to database
		$this->_load_xivo_dso();
		
		$this->_load_ipbx_service();
		$module = &$this->_xobj->get_module('netiface');

		//computing broadcast address
		$ip = split('\.', $netiface['address']);
		$nm = split('\.', $netiface['netmask']);
	
		$bcast  = array();
		for($i = 0; $i < 4; $i++)
			$bcast[]  = intval($ip[$i]) | (intval($nm[$i]) ^ 255);
	
		$bcast  = implode('.', $bcast);
	
	
		$data = array(
			'name'        => $name,
			'ifname'      => $name,
			'hwtypeid'    => 1,
			'networktype' => 'voip',
			'type'        => 'iface',
			'method'      => 'static',
			'address'     => $netiface['address'],
			'netmask'     => $netiface['netmask'],
			'broadcast'   => $bcast,
			'gateway'     => $netiface['gateway'],
			'mtu'         => 1500,
			'description' => 'wizard configuration'
		);

		if(($data = $module->chk_values($data)) === false)
			return false;

		if($module->save($data) === false)
			return false;

		return(true);
	}

	function send_redirect($tpl)
	{
		$uri = $this->discover_finish_uri();
		$tpl->load_i18n_file('tpl/www/struct/page/redirect.i18n', 'global');


?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html lang="<?=DWHO_I18N_BABELFISH_LANGUAGE?>">
<head>
	<title><?= dwho_htmlsc($_TPL->bbf('page_title',php_uname('n'))); ?></title>
	<script> 
		function redirect()
		{
			window.location = "<?= $url->href($uri, $uri, true, null, true, false); ?>";
		}
		
		setInterval("redirect()", 10000);
	</script>
</head>
<body>
	<p><?=nl2br($_TPL->bbf('redirect_message',$seconds));?></p>
</body>
</html>
<?php

		ob_flush();
		flush();
		ob_end_flush();
	}

	function save_step_validate($data, $tpl)
	{
		if(isset($this->_wizsess['commit']) === false)
			$this->_wizsess['commit'] = array();

		if(isset($this->_wizsess['commit']['components']) === false)
			$this->_wizsess['commit']['components'] = array();

		if(isset($this->_wizsess['commit']['step']) === false)
			$this->_wizsess['commit']['step'] = array();

		$ref_components = $this->_wizsess['commit']['components'];
		if(array_key_exists('adminpasswd',$ref_components) === false
		|| $ref_components['adminpasswd'] === false)
			$ref_components['adminpasswd'] = $this->_commit_adminpasswd();

		if($ref_components['adminpasswd'] === false)
			return(false);

		if(array_key_exists('hosts',$ref_components) === false
		|| $ref_components['hosts'] === false)
			$ref_components['hosts'] = $this->_commit_hosts();

		if(array_key_exists('resolvconf',$ref_components) === false
		|| $ref_components['resolvconf'] === false)
			$ref_components['resolvconf'] = $this->_commit_resolvconf();

		$this->_wizsess['commit']['step']['welcome'] = true;
		$this->_wizsess['commit']['step']['license'] = true;
		$this->_wizsess['commit']['step']['ipbxengine'] = true;
		$this->_wizsess['commit']['step']['dbconfig'] = true;
		$this->_wizsess['commit']['step']['checkcomponents'] = true;
		$this->_wizsess['commit']['step']['mainconfig'] = true;

		if(array_key_exists('entity',$ref_components) === false)
			$ref_components['entity'] = $this->_commit_entity();
		else if($ref_components['entity'] === false)
		{
			$this->_commit_adminpasswd();
			$ref_components['entity'] = $this->_commit_entity();
		}

		if($ref_components['entity'] === false)
			return(false);

		if(array_key_exists('context',$ref_components) === false)
			$ref_components['context'] = $this->_commit_context();
		else if($ref_components['context'] === false)
		{
			$this->_commit_adminpasswd();
			$this->_commit_entity();
			$ref_components['context'] = $this->_commit_context();
		}

		if($ref_components['context'] === false)
			return(false);

		$this->_wizsess['commit']['step']['entitycontext'] = true;

		if(array_key_exists('ipbximportuser',$ref_components) === false)
			$ref_components['ipbximportuser'] = $this->_commit_ipbximportuser();
		else if($ref_components['ipbximportuser'] === false)
		{
			$this->_commit_adminpasswd();
			$this->_commit_entity();
			$this->_commit_context();
			$ref_components['ipbximportuser'] = $this->_commit_ipbximportuser();
		}

		if($ref_components['ipbximportuser'] === false)
			return(false);

		$this->_wizsess['commit']['step']['ipbximportuser'] = true;

		if(array_key_exists('dbconfig',$ref_components) === false
		|| $ref_components['dbconfig'] === false)
			$ref_components['dbconfig'] = $this->_commit_dbconfig();

		if($ref_components['dbconfig'] === false)
			return(false);

		if(array_key_exists('netiface',$ref_components) === false
		|| $ref_components['netiface'] === false)
			$ref_components['netiface'] = $this->_prepare_netiface();

		$this->_wizsess['commit']['step']['validate'] = true;

		// Generate commonconf file
		$this->_load_xivo_dso();
		$this->_load_ipbx_service();

		if(($commonconf = &$this->_xobj->get_module('commonconf')) === false)
			return(false);
		if($commonconf->generate() === false)
			return(false);

		//page redirection must be sent to client BEFORE changing server ip address
		$this->send_redirect($tpl);


		if($this->_commit_netiface() === false
		|| $commonconf->apply()      === false)
			return(false);

		// restart provisioning service
		if($this->_sysconfd->request_post('/services', array('pf-xivo-provisioning' => 'restart')) === false)
			return(false);


		return(true);
	}

	function save($data, $tpl)
	{
		$step = $this->get_current_step();

		if(isset($this->_conf['step'][$step]) === false)
			return(null);

		$save_step_function_name = 'save_step_'.$step;

		return($this->$save_step_function_name($data, $tpl));
	}
}

?>
