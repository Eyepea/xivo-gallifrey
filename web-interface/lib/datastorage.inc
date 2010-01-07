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

class dwho_datastorage
{
	function factory($type,$param=array())
	{
		$type = strtolower($type);
		$name = 'dwho_datastorage_adapter_'.$type;

		if(dwho_datastorage::is_valid($type) === false)
			trigger_error('Invalid datastorage type',E_USER_ERROR);
		else if(dwho_datastorage::chk_extension_loaded($type,true) === false)
			trigger_error('Datastorage '.$type.' extension is not loaded',E_USER_ERROR);
		else if(dwho::load_class($name,array('datastorage','adapter'),$type) === false)
			return(false);

		return(new $name($param));
	}

	function is_valid($type)
	{
		$type = (string) $type;

		$datastorage = &dwho_gat::get('datastorage');

		return(isset($datastorage[$type]));
	}

	function has_ssl($type)
	{
		$type = (string) $type;

		$datastorage = &dwho_gat::get('datastorage');

		if(dwho_datastorage::is_valid($type) === true
		&& (bool) $datastorage[$type]['ssl'] === true)
			return($type);

		return(dwho_datastorage::is_ssl_scheme($type));
	}

	function is_ssl_scheme($type)
	{
		$type = (string) $type;

		$datastorage = &dwho_gat::get('datastorage');

		if(substr($type,-1) === 's'
		&& ($type = substr($type,0,-1)) !== false
		&& dwho_datastorage::is_valid($type) === true
		&& (bool) $datastorage[$type]['ssl'] === true)
			return($type);

		return(false);
	}

	function set_param_from_uri($str)
	{
		dwho::load_class('dwho_uri');
		$uri = new dwho_uri();

		if(($arr = $uri->parse_uri($str)) === false || isset($arr['scheme']) === false)
			return(false);
		else if(($ssl = dwho_datastorage::is_ssl_scheme($arr['scheme'])) !== false)
			$arr['scheme'] = $ssl;
		else if(dwho_datastorage::is_valid($arr['scheme']) === false)
			return(false);

		$r = array();
		$r['type'] = $arr['scheme'];

		if(isset($arr['authority']) === true)
		{
			if(isset($arr['authority']['user']) === true)
				$r['user'] = $arr['authority']['user'];

			if(isset($arr['authority']['passwd']) === true)
				$r['pass'] = $arr['authority']['passwd'];

			if(isset($arr['authority']['host']) === true)
				$r['host'] = $arr['authority']['host'];

			if(isset($arr['authority']['port']) === true)
				$r['port'] = $arr['authority']['port'];
		}

		if(isset($arr['path']) === true)
		{
			if($arr['path']{0} === '/' && isset($arr['authority']) === true)
				$r['db'] = substr($arr['path'],1);
			else
				$r['db'] = $arr['path'];
		}

		if(isset($arr['query']) === true)
		{
			$nb = count($arr['query']);

			for($i = 0;$i < $nb;$i++)
			{
				if(dwho_has_len($arr['query'][$i],0) === false)
					continue;
				else if(isset($arr['query'][$i][1]) === true)
					$r[$arr['query'][$i][0]] = $arr['query'][$i][1];
				else
					$r[$arr['query'][$i][0]] = '';
			}
		}

		$r['ssl'] = $ssl !== false;

		return($r);
	}

	function get_extension_name($type)
	{
		$type = (string) $type;

		if(dwho_datastorage::chk_extension_exists($type) === false)
			return(false);

		$datastorage = &dwho_gat::get('datastorage');

		return($datastorage[$type]['extension']);
	}

	function chk_extension_loaded($type,$load=false)
	{
		$type = (string) $type;

		if(dwho_datastorage::chk_extension_exists($type) === false)
			return(false);

		$datastorage = &dwho_gat::get('datastorage');

		if((bool) $load === true)
			dwho::load_extension($datastorage[$type]['extension']);

		return(dwho::chk_extension_loaded($datastorage[$type]['extension']));
	}

	function chk_extension_exists($type)
	{
		$type = (string) $type;

		if(dwho_datastorage::is_valid($type) === false)
			return(false);

		$datastorage = &dwho_gat::get('datastorage');

		return(dwho_ak('extension',$datastorage[$type]));
	}

	function get_meta($type)
	{
		$type = (string) $type;

		if(dwho_datastorage::is_valid($type) === false)
			return(false);

		$datastorage = &dwho_gat::get('datastorage');

		return(dwho_ak('meta',$datastorage[$type],true));
	}
}

?>
