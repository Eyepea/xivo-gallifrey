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

require_once(xivo_file::joinpath(XIVO_PATH_LIB,'datastorage','adapter','abstract','abstract.inc'));

if(defined('LDAP_SCOPE_BASE') === false)
	define('LDAP_SCOPE_BASE',0);

if(defined('LDAP_SCOPE_ONELEVEL') === false)
	define('LDAP_SCOPE_ONELEVEL',1);

if(defined('LDAP_SCOPE_SUBTREE') === false)
	define('LDAP_SCOPE_SUBTREE',2);

class xivo_datastorage_adapter_ldap extends xivo_datastorage_adapter_abstract
{
	var $_param		= array(
					'host'		=> 'localhost',
					'user'		=> '',
					'pass'		=> '',
					'port'		=> 0,
					'ssl'		=> false,
					'tls'		=> false,
					'protocol'	=> 0);

	var $_type		= 'ldap';
	var $_meta		= 'ldap';
	var $_free		= true;
	var $_ber		= null;
	var $_entry		= null;
	var $_colall		= false;
	var $_attrib		= array();
	var $_ordersort		= 0;
	var $_ordercount	= 0;
	var $_limitoffset	= 0;
	var $_limitlength	= 0;
	var $_wherecond		= true;
	var $_aggwherelast	= '';

	function xivo_datastorage_adapter_ldap($param=array())
	{
		$param = (array) $param;

		$open = true;

		if(isset($param['open']) === true)
			$open = false;

		if(isset($param['host']) === true)
			$this->_param['host'] = (string) $param['host'];

		if(isset($param['user']) === true)
			$this->_param['user'] = (string) $param['user'];
		else if(isset($param['binddn']) === true)
			$this->_param['user'] = (string) $param['binddn'];

		if(isset($param['pass']) === true)
			$this->_param['pass'] = (string) $param['pass'];
		else if(isset($param['passwd']) === true)
			$this->_param['pass'] = (string) $param['passwd'];
		else if(isset($param['passdn']) === true)
			$this->_param['pass'] = (string) $param['passdn'];

		if(isset($param['port']) === true)
			$this->_param['port'] = xivo_uint($param['port']);

		if(isset($param['tls']) === true)
			$this->_param['tls'] = (bool) $param['tls'];

		if((bool) $this->_param['tls'] === true)
			$this->_param['ssl'] = false;
		else if(isset($param['ssl']) === true)
			$this->_param['ssl'] = (bool) $param['ssl'];

		if(isset($param['protocol']) === true)
			$this->_param['protocol'] = xivo_uint($param['protocol'],3);
		else if(isset($param['protocolversion']) === true)
			$this->_param['protocol'] = xivo_uint($param['protocolversion'],3);

		if(isset($param['debug']) === true)
			$this->_debug = xivo_uint($param['debug']);

		if($open === true && $this->open() !== false)
			$this->bind();
	}

	function _chk_option($func,$option)
	{
		$option = strtolower($option);

		switch($option)
		{
			case 'deref':
			case LDAP_OPT_DEREF:
				return(LDAP_OPT_DEREF);
			case 'sizelimit':
			case LDAP_OPT_SIZELIMIT:
				return(LDAP_OPT_SIZELIMIT);
			case 'timelimit':
			case LDAP_OPT_TIMELIMIT:
				return(LDAP_OPT_TIMELIMIT);
			case 'protocol':
			case 'protocol_version':
			case LDAP_OPT_PROTOCOL_VERSION:
				return(LDAP_OPT_PROTOCOL_VERSION);
			case 'error_number':
			case LDAP_OPT_ERROR_NUMBER:
				return(LDAP_OPT_ERROR_NUMBER);
			case 'debug':
			case 'debug_level':
			case LDAP_OPT_DEBUG_LEVEL:
				if($func !== 'set')
					return(false);

				return(LDAP_OPT_DEBUG_LEVEL);
			case 'error_string':
			case LDAP_OPT_ERROR_STRING:
				return(LDAP_OPT_ERROR_STRING);
			case 'hostname':
			case 'host_name':
			case LDAP_OPT_HOST_NAME:
				return(LDAP_OPT_HOST_NAME);
			case 'matched_dn':
			case LDAP_OPT_MATCHED_DN:
				return(LDAP_OPT_MATCHED_DN);
			case 'referrals':
			case LDAP_OPT_REFERRALS:
				return(LDAP_OPT_REFERRALS);
			case 'restart':
			case LDAP_OPT_RESTART:
				return(LDAP_OPT_RESTART);
			case 'server_controls':
			case LDAP_OPT_SERVER_CONTROLS:
				if($func !== 'set')
					return(false);

				return(LDAP_OPT_SERVER_CONTROLS);
			case 'client_controls':
			case LDAP_OPT_CLIENT_CONTROLS:
				if($func !== 'set')
					return(false);

				return(LDAP_OPT_CLIENT_CONTROLS);
		}

		return(false);
	}

	function set_option($option,$value)
	{
		$link = &$this->_link;

		if(($option = $this->_chk_option('set',$option)) === false)
			return(false);
		else if($option === LDAP_OPT_DEBUG_LEVEL && is_resource($link) === false)
			$link = null;
		else if(is_resource($link) === false)
			return(false);

		switch($option)
		{
			case LDAP_OPT_RESTART:
			case LDAP_OPT_REFERRALS:
				$value = (bool) $value;
				break;
			case LDAP_OPT_SERVER_CONTROLS:
			case LDAP_OPT_CLIENT_CONTROLS:
				if(is_array($value) === false)
					return(false);
				break;
			case LDAP_OPT_PROTOCOL_VERSION:
				if(($value = xivo_uint($value)) === 0
				|| ($value !== 2 && $value !== 3) === true)
					return(false);
				break;
			default:
				if(ctype_digit(strval($value)) === false)
					return(false);
		}

		return(ldap_set_option($link,$option,$value));
	}

	function get_option($option)
	{
		$link = &$this->_link;

		if(($option = $this->_chk_option('get',$option)) === false)
			return(false);
		else if($option === LDAP_OPT_DEBUG_LEVEL && is_resource($link) === false)
			$link = null;
		else if(is_resource($link) === false)
			return(false);

		if(ldap_get_option($link,$option,$value) === false)
			return(false);

		return($value);
	}

	function _set_scope($scope)
	{
		$scope = strtolower($scope);

		switch($scope)
		{
			case 'base':
			case LDAP_SCOPE_BASE:
				return(LDAP_SCOPE_BASE);
			case 'onelevel':
			case LDAP_SCOPE_ONELEVEL:
				return(LDAP_SCOPE_ONELEVEL);
		}

		return(LDAP_SCOPE_SUBTREE);
	}

	function set_debug($debug=0)
	{
		$this->_debug = xivo_uint($debug);
	}

	function explode_dn($dn,$attrib=false,$empty=false,$count=true)
	{
		if(xivo_has_len($dn) === false
		|| ($r = ldap_explode_dn($dn,intval((bool) $attrib))) === false
		|| is_array($r) === false
		|| ($r['count'] === 0 && (bool) $empty === false) === true)
			return(false);
		else if((bool) $count === false)
			unset($r['count']);

		return($r);
	}

	function open()
	{
		if(is_resource($this->_link) === true)
			return($this->_link);
		else if($this->_param['host'] !== '')
			$host = $this->_param['host'];
		else
			$host = 'localhost';

		if((bool) $this->_param['ssl'] === false)
			$host = 'ldap://'.$host;
		else
			$host = 'ldaps://'.$host;

		if($this->_param['port'] !== 0)
			$host .= ':'.$this->_param['port'];

		if($this->_debug !== 0)
			$this->set_option('debug',$this->_debug);

		if(($this->_link = @ldap_connect($host)) === false)
		{
			$this->halt('Can\'t connect to LDAP server');
			$this->close();
			return(false);
		}

		$this->set_option('protocol',$this->_param['protocol']);

		if((bool) $this->_param['tls'] === true
		&& $this->get_option('protocol') > 2
		&& @ldap_start_tls($this->_link) === false)
		{
			$this->halt('Unable to start TLS: Can\'t contact LDAP server');
			$this->close();
			return(false);
		}

		return($this->_link);
	}

	function bind()
	{
		if(is_resource($this->_link) === true
		&& ldap_bind($this->_link,$this->_param['user'],$this->_param['pass']) === true)
			return(true);

		$this->close();

		return(false);
	}

	function search($basedn,$scope=LDAP_SCOPE_SUBTREE,$filter='',$attrib='',$attrsonly=0,$sizelimit=0,$timelimit=0,$deref=0)
	{
		$scope = $this->_set_scope($scope);
		$attrsonly = intval((bool) $attrsonly);
		$sizelimit = xivo_uint($sizelimit);
		$timelimit = xivo_uint($timelimit);
		$deref = xivo_uint($deref);

		if(is_resource($this->_link) === false)
			return(false);
		else if(xivo_has_len($basedn) === true)
		{
			$filter = xivo_has_len($filter) === true ? $filter : '(objectclass=*)';

			if(xivo_has_len($attrib) === true)
				$this->_attrib = array($attrib);
			else if(is_array($attrib) === true && empty($attrib) === false)
				$this->_attrib = array_values($attrib);
			else
				$this->_attrib = array('*');
		}
		else if(xivo_has_len($basedn,'from') === true)
		{
			$filter = xivo_has_len($basedn,'where') === true ? $basedn['where'] : '(objectclass=*)';
			$this->_attrib = xivo_issa('column',$basedn) === true ? array_values($basedn['column']) : array('*');
			$basedn = $basedn['from'];
		}
		else
			return(false);

		if($attrsonly === 0 && ($order = $this->get_order()) !== null && in_array('*',$this->_attrib) === false)
			$attrib = array_merge($this->_attrib,array_keys($order));
		else
		{
			$attrib = &$this->_attrib;
			$order = null;
		}

		switch($scope)
		{
			case LDAP_SCOPE_BASE:
				if($sizelimit === 0)
					$this->_result = ldap_read($this->_link,
								   $basedn,
								   $filter,
								   $attrib,
								   $attrsonly,
								   $sizelimit,
								   $timelimit,
								   $deref);
				else
					$this->_result = @ldap_read($this->_link,
								    $basedn,
								    $filter,
								    $attrib,
								    $attrsonly,
								    $sizelimit,
								    $timelimit,
								    $deref);
				break;
			case LDAP_SCOPE_ONELEVEL:
				if($sizelimit === 0)
					$this->_result = ldap_list($this->_link,
								   $basedn,
								   $filter,
								   $attrib,
								   $attrsonly,
								   $sizelimit,
								   $timelimit,
								   $deref);
				else
					$this->_result = @ldap_list($this->_link,
								    $basedn,
								    $filter,
								    $attrib,
								    $attrsonly,
								    $sizelimit,
								    $timelimit,
								    $deref);
				break;
			case LDAP_SCOPE_SUBTREE:
			default:
				if($sizelimit === 0)
					$this->_result = ldap_search($this->_link,
								     $basedn,
								     $filter,
								     $attrib,
								     $attrsonly,
								     $sizelimit,
								     $timelimit,
								     $deref);
				else
					$this->_result = @ldap_search($this->_link,
								      $basedn,
								      $filter,
								      $attrib,
								      $attrsonly,
								      $sizelimit,
								      $timelimit,
								      $deref);
		}

		if($this->_result !== false
		&& $order !== null
		&& $this->_ordercount === 1)
			ldap_sort($this->_link,$this->_result,key($order));
		else if((bool) $this->_debug === true)
			$this->halt();

		return($this->_result);
	}

	function clean_result(&$result,$lower=false,$recursive=false)
	{
		$recursive = (bool) $recursive;

		if(is_array($result) === false
		|| empty($result) === true
		|| isset($result['count']) === false
		|| $result['count'] === 0)
			return(false);
		else if($recursive === false)
			$return = array();

		unset($result['count'],$result['dn']);

		reset($result);

		while(list($key) = each($result))
		{
			$val = &$result[$key];

			if(is_array($val) === true
			&& $this->clean_result($val,false,true) === false)
				unset($result[$key]);
			else if(is_int($key) === true
			&& xivo_issa($val,$result) === true)
				unset($result[$key]);
			else if($recursive === false && is_int($key) === true)
				$return[] = $result[$key];
		}

		if($recursive === true)
			return($result);
		else if(is_int($key) === true)
			$result = (bool) $lower === false ? $return : array_change_key_case($return,CASE_LOWER);
		else if((bool) $lower === true)
			$result = array_change_key_case($result,CASE_LOWER);

		return($result);
	}

	function errno()
	{
		if(is_resource($this->_link) === true)
			return(ldap_errno($this->_link));

		return(0);
	}

	function error()
	{
		if(is_resource($this->_link) === true)
			return(ldap_error($this->_link));

		return('');
	}

	function close()
	{
		$r = null;

		if(is_resource($this->_link) === true)
			$r = ldap_close($this->_link);

		$this->_link = null;

		return($r);
	}

	function free()
	{
		if(is_resource($this->_result) === true
		&& ldap_free_result($this->_result) === true)
		{
			$this->_ber = $this->_entry = $this->_result = null;
			return(true);
		}

		return(false);
	}

	function num_rows()
	{
		if(is_resource($this->_link) === true && is_resource($this->_result) === true)
			return(ldap_count_entries($this->_link,$this->_result));

		return(false);
	}

	function first_attribute()
	{
		if(is_resource($this->_link) === true
		&& is_resource($this->_result) === true
		&& (is_resource($this->_entry) === true
		   || ($this->_entry = ldap_first_entry($this->_link,$this->_result)) !== false) === true
		&& ($r = ldap_first_attribute($this->_link,$this->_entry,&$this->_ber)) !== false)
			return(strtolower($r));

		return(false);
	}

	function next_attribute()
	{
		if(is_resource($this->_link) === true
		&& is_resource($this->_result) === true
		&& is_resource($this->_entry) === true
		&& ($r = ldap_next_attribute($this->_link,$this->_entry,&$this->_ber)) !== false)
			return(strtolower($r));

		return(false);
	}

	function first_entry($attributes=true,$clean=false)
	{
		if(is_resource($this->_link) === true
		&& is_resource($this->_result) === true
		&& (is_resource($this->_entry) === true
		   || ($this->_entry = ldap_first_entry($this->_link,$this->_result)) !== false) === true)
		{
			if((bool) $attributes === false)
				return(true);
			else if(($r = ldap_get_attributes($this->_link,$this->_entry)) !== false
			&& $clean === true)
				return($this->clean_result($r,true));

			return($r);
		}

		return(false);
	}

	function next_entry($attributes=true,$clean=false)
	{
		if(is_resource($this->_link) === true
		&& is_resource($this->_entry) === true
		&& ($this->_entry = ldap_next_entry($this->_link,$this->_entry)) !== false)
		{
			if((bool) $attributes === true)
				return(ldap_get_attributes($this->_link,$this->_entry));

			return(true);
		}

		return(false);
	}

	function get_entries()
	{
		if(is_resource($this->_link) === true && is_resource($this->_result) === true)
			return(ldap_get_entries($this->_link,$this->_result));

		return(false);
	}

	function get_value_first_entry($attrib,$clean=false,$lower=false)
	{
		if(xivo_has_len($attrib) === true
		&& is_resource($this->_link) === true
		&& is_resource($this->_result) === true
		&& (is_resource($this->_entry) === true
		   || ($this->_entry = ldap_first_entry($this->_link,$this->_result)) !== false) === true
		&& ($r = @ldap_get_values($this->_link,$this->_entry,$attrib)) !== false
		&& $r['count'] !== 0)
		{
			if((bool) $clean === true)
				$this->clean_result($r,$lower);

			return($r);
		}

		return(false);
	}

	function get_value_next_entry($attrib,$clean=false,$lower=false)
	{
		if(xivo_has_len($attrib) === true
		&& is_resource($this->_link) === true
		&& is_resource($this->_result) === true
		&& ($this->_entry = ldap_next_entry($this->_link,$this->_result)) !== false
		&& ($r = @ldap_get_values($this->_link,$this->_entry,$attrib)) !== false
		&& $r['count'] !== 0)
		{
			if((bool) $clean === true)
				$this->clean_result($r,$lower);

			return($r);
		}

		return(false);
	}

	function get_values($attrib,$clean=false,$lower=false)
	{
		if(xivo_has_len($attrib) === true
		&& is_resource($this->_link) === true
		&& is_resource($this->_entry) === true
		&& ($r = @ldap_get_values($this->_link,$this->_entry,$attrib)) !== false
		&& $r['count'] !== 0)
		{
			if((bool) $clean === true)
				$this->clean_result($r,$lower);

			return($r);
		}

		return(false);
	}

	function escape_string($str)
	{
		$pat = array("\x00",'\\',"\x20",'*','(',')');
		$rep = array('\\00','\\5c','\\20','\\2a','\\28','\\29');

		return(str_replace($pat,$rep,$str));
	}

	function from($table,$column='*')
	{
		if(($table = $this->_mk_table($table)) === false)
			return(false);

		$this->_from = implode(',',$table);

		return($this->_mk_column($column));
	}

	function _mk_select()
	{
		$r = array();

		if(($r['from'] = $this->get_from()) === null
		|| ($r['column'] = $this->get_column()) === null)
			return(false);

		$r['where'] = $this->get_where();
		$r['order'] = $this->get_order();

		return($r);
	}

	function reset_where()
	{
		parent::reset_where();
		$this->_aggwherelast = '';
	}

	function get_where()
	{
		$r = '';

		if(xivo_has_len($this->_aggwherelast) === true)
		{
			if(xivo_has_len($this->_where) === false)
				$this->_aggwhere[] = $this->_aggwherelast;
			else
				$this->_aggwhere[] = '('.((bool) $this->_wherecond === true ? '&' : '|').
						     $this->_aggwherelast.
						     $this->_where.')';

			$this->_where = '';
		}

		if(is_array($this->_aggwhere) === true && isset($this->_aggwhere[0]) === true)
			$r = implode('',$this->_aggwhere);

		if(xivo_has_len($this->_where) === true)
		{
			if($r !== '')
				$r = '('.((bool) $this->_wherecond === true ? '&' : '|').$r.$this->_where.')';
			else
				$r .= $this->_where;
		}

		if($r === '')
			$r = null;

		return($r);
	}

	function _mk_table($table)
	{
		if((xivo_has_len($table) === true
		   && ($table = $this->explode_dn($table,false,false,false)) === false) === true
		|| is_array($table) === false)
			return(false);

		$r = array();

		foreach($table as $val)
		{
			if(xivo_has_len($val) === false)
				return(false);

			$r[] = strtolower($val);
		}

		if(isset($r[0]) === false)
			return(false);

		$this->_table = $r;

		return($r);
	}

	function reset_column()
	{
		parent::reset_column();
		$this->_colall = false;
	}

	function get_column()
	{
		if(is_array($this->_column) === true && empty($this->_column) === false)
			return(array_values($this->_column));
		else if((bool) $this->_colall === true)
			return(array('*'));

		return(null);
	}

	function _mk_column($column)
	{
		if(is_scalar($column) === true)
		{
			$column = trim($column);

			if($column === '*')
				return(($this->_colall = true));
			else if(xivo_has_len($column) === true)
				$column = (array) $column;
			else
				return(null);
		}

		if(is_array($column) === false || empty($column) === true)
			return(null);

		foreach($column as $val)
		{
			if($this->chk_attribute($val) === 1)
			{
				$val = strtolower($val);
				$this->_column[$val] = $val;
			}
		}

		return(true);
	}

	function chk_attribute($attr)
	{
		if(is_scalar($attr) === false)
			return(0);
		else if(preg_match('/^(?:[a-zA-Z0-9-]+|[0-9]+(?:\.[0-9]+)*)$/',$attr) === 1)
			return(1);
		else if(preg_match('/^(?:[a-zA-Z0-9-]+\:)+$/',$attr) === 1
		|| preg_match('/^(?:\:?[a-zA-Z0-9-]+)*\:[0-9]+(?:\.[0-9]+)*\:$/',$attr) === 1)
			return(2);

		return(0);
	}

	function _mk_sep(&$data)
	{
		$data = (array) $data;

		if(empty($data) === true)
			return(false);

		$r = '';

		foreach($data as $key => $val)
		{
			if($this->chk_attribute($key) !== 0)
				$r .= '('.strtolower($key).'='.$this->escape($val).')';
		}

		if($r === '')
			return(false);

		return($r);
	}

	function aggregate_where()
	{
		if($this->_where === '')
			return(false);

		if($this->_aggwherelast !== '')
		{
			$this->_aggwhere[] = '('.((bool) $this->_wherecond === true ? '&' : '|').$this->_aggwherelast.$this->_where.')';
			$this->_aggwherelast = '';
		}
		else
			$this->_aggwherelast = $this->_where;

		$this->_where = '';
	}

	function _set_where($cond,$val='',$condand=true,$and=true,$not=false)
	{
		if(($where = $this->_mk_where($cond,$val,$condand)) === false)
			return(false);
		else if((bool) $not === true)
		{
			if($where{0} !== '(')
				$where = '('.$where;

			if(substr($where,-1) !== ')')
				$where .= ')';

			$where = '(!'.$where.')';
		}

		if($this->_where === '')
		{
			$this->_where = $where;
			$this->_wherecond = (bool) $and;
		}
		else
			$this->_where = '('.((bool) $and === true ? '&' : '|').$this->_where.$where.')';

		return(true);
	}

	function _mk_where($cond,$val='',$condand=true)
	{
		if(is_array($cond) === true)
		{
			if(($condsep = $this->_mk_sep($cond)) === false)
				return(false);
			else if(is_array($cond) === true && count($cond) > 1)
				return('('.((bool) $condand === true ? '&' : '|').$condsep.')');

			return($condsep);
		}
		else if(is_array($val) === true || xivo_has_len($val) === true)
		{
			$val = (array) $val;
			$r = vsprintf(xivo_printf_escape($cond,count($val)),
				      array_map(array(&$this,'escape'),$val));
		}
		else if(xivo_has_len($cond) === true)
			$r = (string) $cond;
		else
			return(false);

		if(trim($r) === '')
			return(false);

		return($r);
	}

	function _set_wherenull($column,$table='',$and=true,$not=false)
	{
		if(($where = $this->_mk_wherenull($column)) === false)
			return(false);
		else if((bool) $not === false)
			$where = '(!'.$where.')';

		if($this->_where === '')
		{
			$this->_where = $where;
			$this->_wherecond = (bool) $and;
		}
		else
			$this->_where = '('.((bool) $and === true ? '&' : '|').$this->_where.$where.')';

		return(true);
	}

	function _mk_wherenull($column)
	{
		if($this->chk_attribute($column) === 1)
			return('('.$column.'=*)');

		return(false);
	}

	function _set_wherebetween($column,$beg,$end,$table='',$and=true,$not=false)
	{
		if(($between = $this->_mk_wherebetween($column,$beg,$end)) === false)
			return(false);
		else if((bool) $not === true)
			$where = '(!'.$between.')';

		if($this->_where === '')
		{
			$this->_where = $between;
			$this->_wherecond = (bool) $and;
		}
		else
			$this->_where = '('.((bool) $and === true ? '&' : '|').$this->_where.$between.')';

		return(true);
	}

	function _mk_wherebetween($column,$beg,$end)
	{
		if($this->chk_attribute($column) === 0)
			return(false);

		return(sprintf('(&(%s>=%s)(%s<=%s))',$column,$this->escape($beg),$column,$this->escape($end)));
	}

	function _set_wherein($column,&$list,$table='',$keys=false,$and=true,$not=false,$join=false)
	{
		if(($in = $this->_mk_wherein($column,$list,$keys)) === false)
			return(false);
		else if((bool) $not === true)
			$where = '(!'.$in.')';

		if($this->_where === '')
		{
			$this->_where = $in;
			$this->_wherecond = (bool) $and;
		}
		else
			$this->_where = '('.((bool) $and === true ? '&' : '|').$this->_where.$in.')';

		return(true);
	}

	function _mk_wherein($column,$list,$keys=false,$not=false)
	{
		if($this->chk_attribute($column) === 0 || is_array($list) === false || empty($list) === true)
			return(false);
		else if((bool) $keys === true)
			$list = array_keys($list);

		$strlist = implode(')('.$column.'=',array_map(array(&$this,'escape'),$list));

		if(trim($strlist) === '')
			return(false);

		return(sprintf('(|(%s=%s))',$column,$strlist));
	}

	function _set_wherelike($column,$pattern,$table='',$format='',$condand=true,$and=true,$not=false)
	{
		if(($like = $this->_mk_wherelike($column,$pattern,$format,$condand)) === false)
			return(false);
		else if((bool) $not === true)
			$where = '(!'.$like.')';

		if($this->_where === '')
		{
			$this->_where = $like;
			$this->_wherecond = (bool) $and;
		}
		else
			$this->_where = '('.((bool) $and === true ? '&' : '|').$this->_where.$like.')';

		return(true);
	}

	function _mk_wherelike($column,$pattern,$format='',$condand=true)
	{
		$format = $format !== '' ? strtolower($format) : 'exact';

		if($this->chk_attribute($column) === 0)
			return(false);
		else if(is_scalar($pattern) === true || is_null($pattern) === true)
			$pattern = array((string) $pattern);
		else if(is_array($pattern) === false || empty($pattern) === true)
			return(false);

		switch($format)
		{
			case 'begin':
				$like_format = '%s*';
				break;
			case 'contain':
				$like_format = '*%s*';
				break;
			case 'end':
				$like_format = '*%s';
				break;
			default:
				$like_format = '%s';
		}

		$i = 0;

		$r = '';

		foreach($pattern as $value)
		{
			$r .= sprintf('(%s='.$like_format.')',$column,$this->escape_string($value));
			$i++;
		}

		if($r === '')
			$r = false;
		else if($i > 1)
			$r = '('.((bool) $condand === true ? '&' : '|').$r.')';

		return($r);
	}

	function reset_order()
	{
		parent::reset_order();
		$this->_ordercount = $this->_ordersort = 0;
	}

	function _set_order($column,$sort='')
	{
		$r = array();

		if(is_array($column) === true)
		{
			if(empty($column) === true)
				return(false);

			foreach($column as $key => $val)
			{
				if($this->chk_attribute($key) === 1)
				{
					$sort = $this->_set_sort($val);
					$this->_ordercount++;
					$this->_ordersort |= $sort;

					$r[strtolower($key)] = $sort;
				}
			}

			if(empty($r) === true)
				$r = false;

			return($r);
		}
		else if($this->chk_attribute($column) !== 1)
			return(false);

		$sort = $this->_set_sort($sort);
		$this->_ordercount++;
		$this->_ordersort |= $sort;

		$r[strtolower($column)] = $sort;

		return($r);
	}

	function _mk_order()
	{
		if(empty($this->_order) === false)
			return($this->_order);

		return(false);
	}

	function _order_result(&$result)
	{
		if(is_array($result) === false
		|| $result['count'] === 0
		|| ($order = $this->get_order()) === null)
			return(false);

		$nb = $result['count'];
		unset($result['count']);

		$rs = $sort = array();

		for($i = 0;$i < $nb;$i++)
		{
			$ref = &$result[$i];

			foreach($order as $key => $val)
			{
				if(isset($rs[$key]) === false)
				{
					$rs[$key] = array();
					$rs[] = $val;
				}

				if(isset($ref[$key]) === true
				&& is_array($ref[$key]) === true
				&& isset($ref[$key][0]) === true)
					$rs[$key][$i] = &$ref[$key][0];
				else
					$rs[$key][$i] = '';
			}
		}

		if(empty($rs) === true)
		{
			$result['count'] = $nb;
			return(false);
		}

		$rs[] = &$result;

		call_user_func_array('array_multisort',$rs);
		$result['count'] = $nb;

		return(true);
	}

	function reset_limit()
	{
		parent::reset_limit();
		$this->_limitoffset = $this->_limitlength = 0;
	}

	function _mk_limit()
	{
		if(empty($this->_limit) === true)
			return(false);
		else if(isset($this->_limit[1]) === true)
		{
			$this->_limitoffset = $this->_limit[0];
			$this->_limitlength = $this->_limit[1];
		}
		else
		{
			$this->_limitoffset = 0;
			$this->_limitlength = $this->_limit[0];
		}

		return($this->_limit);
	}

	function select_singlecolumn($search=array(),$free=true,$scope=LDAP_SCOPE_SUBTREE)
	{
		if($this->_ordercount > 1 || ($this->_ordersort & SORT_DESC) !== 0)
		{
			if(($rs = $this->select_all($search,false,false,$scope,false)) === false
			|| $rs['count'] === 0)
			{
				if($this->_free === true && (bool) $free === true)
					$this->free();
				return(false);
			}
			else if(in_array('*',$this->_attrib) === true)
				$attrib = $this->first_attribute();
			else
				$attrib = xivo_ak(0,$this->_attrib,true);

			if($attrib === false)
				$r = false;
			else
			{
				$r = array();

				for($i = $j = 0;$i < $rs['count'];$i++)
				{
					if(isset($rs[$i][$attrib]) === true)
						$r[$j++] = $rs[$i][$attrib][0];
				}
			}
		}
		else if((empty($search) === true
		   && ($search = $this->get_select()) === null) === true
		|| $this->search($search,$scope) === false)
			return(false);
		else
		{
			if(in_array('*',$this->_attrib) === true)
				$attrib = $this->first_attribute();
			else
				$attrib = xivo_ak(0,$this->_attrib,true);

			if($attrib === false)
				$r = false;
			else
			{
				$r = array();

				for($i = 0,$entry = $this->first_entry(false);$entry === true;$entry = $this->next_entry(false))
				{
					if(($first = $this->get_values($attrib,false,true)) !== false)
						$r[$i++] = $first[0];
				}
			}
		}

		if(array_key_exists(0,$r) === false)
			$r = false;

		if($this->_free === true && (bool) $free === true)
			$this->free();

		return($r);
	}

	function select_columnpair($search=array(),$free=true,$scope=LDAP_SCOPE_SUBTREE)
	{
		if($this->_ordercount > 1 || ($this->_ordersort & SORT_DESC) !== 0)
		{
			if(($rs = $this->select_all($search,false,false,$scope,false)) === false
			|| $rs['count'] === 0)
			{
				if($this->_free === true && (bool) $free === true)
					$this->free();
				return(false);
			}
			else if(in_array('*',$this->_attrib) === true)
			{
				$attrib1 = $this->first_attribute();
				$attrib2 = $this->next_attribute();
			}
			else
			{
				$attrib1 = xivo_ak(0,$this->_attrib,true);
				$attrib2 = xivo_ak(1,$this->_attrib,true);
			}

			if($attrib1 === false || $attrib2 === false)
				$r = false;
			else if($this->get_limit() !== null)
			{
				$i = 0;

				if($this->_limitoffset > 0)
				{
					for($j = 0;$j < $this->_limitoffset && $i < $rs['count'];$i++)
					{
						if(isset($rs[$i][$attrib1],$rs[$i][$attrib2]) === true)
							$j++;
					}
				}

				$r = array();

				for($j = 0;$j < $this->_limitlength && $i < $rs['count'];$i++)
				{
					if(isset($rs[$i][$attrib1],$rs[$i][$attrib2]) === true)
					{
						$r[$rs[$i][$attrib1][0]] = $rs[$i][$attrib2][0];
						$j++;
					}
				}
			}
			else
			{
				$r = array();

				for($i = 0;$i < $rs['count'];$i++)
				{
					if(isset($rs[$i][$attrib1],$rs[$i][$attrib2]) === true)
						$r[$rs[$i][$attrib1][0]] = $rs[$i][$attrib2][0];
				}
			}
		}
		else if((empty($search) === true
		   && ($search = $this->get_select()) === null) === true
		|| $this->search($search,$scope) === false)
			return(false);
		else
		{
			if(in_array('*',$this->_attrib) === true)
			{
				$attrib1 = $this->first_attribute();
				$attrib2 = $this->next_attribute();
			}
			else
			{
				$attrib1 = xivo_ak(0,$this->_attrib,true);
				$attrib2 = xivo_ak(1,$this->_attrib,true);
			}

			if($attrib1 === false || $attrib2 === false)
				$r = false;
			else if($this->get_limit() !== null)
			{
				$entry = $this->first_entry(false);

				if($this->_limitoffset > 0)
				{
					for($i = 0;$entry === true && $i < $this->_limitoffset;$entry = $this->next_entry(false))
					{
						if($this->get_values($attrib1) !== false
						&& $this->get_values($attrib2) !== false)
							$i++;
					}
				}

				$r = array();

				for($i = 0;$entry === true && $i < $this->_limitlength;$entry = $this->next_entry(false))
				{
					if(($first = $this->get_values($attrib1,false,true)) !== false
					&& ($next = $this->get_values($attrib2,false,true)) !== false)
						$r[$first[0]] = $next[0];
				}
			}
			else
			{
				$r = array();

				for($entry = $this->first_entry(false);$entry === true;$entry = $this->next_entry(false))
				{
					if(($first = $this->get_values($attrib1,false,true)) !== false
					&& ($next = $this->get_values($attrib2,false,true)) !== false)
						$r[$first[0]] = $next[0];
				}
			}
		}

		if(empty($r) === true)
			$r = false;

		if($this->_free === true && (bool) $free === true)
			$this->free();

		return($r);
	}

	function select_count($search=array(),$free=true,$scope=LDAP_SCOPE_SUBTREE)
	{
		$free = (bool) $free;

		if(empty($search) === true
		&& ($search = $this->get_select()) === null)
		   	return(false);
		else if($free === true)
		{
			if(xivo_has_len($search) === true)
				$search = array('from' => $search,'column' => array('*'));
			else if(is_array($search) === true)
				$search['column'] = array('*');
		}

		if($this->search($search,$scope,null,null,(int) $free) === false)
			return(false);
		else if(($r = $this->num_rows()) !== false)
			$r = xivo_ulongint($r);

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_one($search=array(),$free=true,$scope=LDAP_SCOPE_SUBTREE)
	{
		if($this->_ordercount > 1 || ($this->_ordersort & SORT_DESC) !== 0)
		{
			if(($r = $this->select_all($search,false,false,$scope,false)) === false)
			{
				if($this->_free === true && (bool) $free === true)
					$this->free();
				return(false);
			}
			else if(in_array('*',$this->_attrib) === true)
				$attrib = $this->first_attribute();
			else
				$attrib = xivo_ak(0,$this->_attrib,true);

			if($attrib === false)
				$r = false;
			else if(isset($r[0][$attrib]) === false || is_array($r[0][$attrib]) === false)
				$r = null;
			else
				$r = $r[0][$attrib];
		}
		else if((empty($search) === true
		   && ($search = $this->get_select()) === null) === true
		|| $this->search($search,$scope) === false)
			return(false);
		else if(($r = $this->first_entry()) !== false)
		{
			if(in_array('*',$this->_attrib) === true)
				$attrib = $this->first_attribute();
			else
				$attrib = xivo_ak(0,$this->_attrib,true);

			if($attrib === false)
				$r = false;
			else if(isset($r[$attrib]) === false || is_array($r[$attrib]) === false)
				$r = null;
			else
				$r = $r[$attrib];
		}

		unset($r['count']);

		if($this->_free === true && $free === true)
			$this->free();

		return($r);
	}

	function select_single($search=array(),$free=true,$clean=false,$scope=LDAP_SCOPE_SUBTREE)
	{
		if($this->_ordercount > 1 || ($this->_ordersort & SORT_DESC) !== 0)
		{
			if(($r = $this->select_all($search,$free,$clean,$scope,false)) === false)
				return(false);

			return($r[0]);
		}
		else if((empty($search) === true
		   && ($search = $this->get_select()) === null) === true
		|| $this->search($search,$scope) === false)
			return(false);
		else if(($r = $this->first_entry()) !== false && $r['count'] === 0)
			$r = false;

		if($r !== false && (bool) $clean === true)
			$this->clean_result($r,true);

		if($this->_free === true && (bool) $free === true)
			$this->free();

		return($r);
	}

	function select_all($search=array(),$free=true,$clean=false,$scope=LDAP_SCOPE_SUBTREE,$limit=true)
	{
		if((empty($search) === true
		   && ($search = $this->get_select()) === null) === true
		|| $this->search($search,$scope) === false)
			return(false);
		else if(($r = $this->get_entries()) !== false && $r['count'] === 0)
			$r = false;
		else if($this->_ordercount === 1 && $this->_ordersort === SORT_DESC)
			$r = array_reverse($r);
		else if($this->_ordercount > 1 || ($this->_ordersort & SORT_DESC) !== 0)
			$this->_order_result($r);

		if($r !== false)
		{
			if((bool) $clean === true)
				$this->clean_result($r,true);

			if((bool) $limit === true && $this->get_limit() !== null)
				$r = array_slice($r,$this->_limitoffset,$this->_limitlength);

			if(empty($r) === true)
				$r = false;
		}

		if($this->_free === true && (bool) $free === true)
			$this->free();

		return($r);
	}
}

?>
