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

xivo_service_asterisk::required(array('realstatic','realstatic.inc'),true);

class xivo_service_asterisk_realstatic_staticsip extends xivo_service_asterisk_realstatic
{
	var $_filename	= 'sip.conf';
	var $_uri	= null;

	function _prepare($data)
	{
		if($data['category'] === 'general'
		&& $data['var_name'] === 'register'
		&& is_scalar($data['var_val']) === true
		&& ($register = $this->parse_register_uri($data['var_val'])) !== false)
			$data = array_merge($register,$data);

		return(parent::_prepare($data));
	}

	function _load_uri()
	{
		if(is_object($this->_uri) === true)
			return(true);
		else if(xivo::load_class('xivo_uri') === false)
			return(false);

		$this->_uri = new xivo_uri();

		return(true);
	}

	function build_register_uri($arr)
	{
		$r = array('str' => '','arr' => array());

		if($this->_load_uri() === false
		|| xivo_has_len($arr,'username') === false
		|| xivo_has_len($arr,'host') === false)
			return(false);

		$uri['user'] = $arr['username'];
		$uri['host'] = $arr['host'];

		if(xivo_has_len($arr,'password') === true)
		{
			$uri['passwd'] = $arr['password'];

			if(xivo_has_len($arr,'authuser') === true)
				$uri['passwd'] .= ':'.$arr['authuser'];
		}

		if(xivo_has_len($arr,'port') === true)
			$uri['port'] = $arr['port'];

		if(xivo_has_len($arr,'contact') === true)
		{
			$arr['contact'] = (string) $arr['contact'];

			if($arr['contact']{0} !== '/')
				$uri['path'] = '/'.$arr['contact'];
			else
				$uri['path'] = $arr['contact'];
		}

		if(($r['str'] = $this->_uri->build_uri($uri)) === false
		|| ($r['arr'] = $this->parse_register_uri($r['str'],false)) === false)
			return(false);

		if(substr($r['str'],0,2) === '//')
			$r['str'] = substr($r['str'],2);

		return($r);
	}

	function parse_register_uri($val,$decode=true)
	{
		if(is_scalar($val) === true && substr($val,0,2) !== '//')
			$val = '//'.$val;

		if($this->_load_uri() === false
		|| ($parse_uri = $this->_uri->parse_uri($val,$decode)) === false)
			return(false);

		$r = array(
			'username'	=> '',
			'password'	=> '',
			'authuser'	=> '',
			'host'		=> '',
			'port'		=> '',
			'contact'	=> '');

		if(xivo_issa('authority',$parse_uri) === true)
		{
			if(isset($parse_uri['authority']['user']) === true)
				$r['username'] = $parse_uri['authority']['user'];

			if(isset($parse_uri['authority']['passwd']) === true)
			{
				$r['password'] = $parse_uri['authority']['passwd'];

				if(($pos = strrpos($parse_uri['authority']['passwd'],':')) !== false)
				{
					$r['password'] = substr($parse_uri['authority']['passwd'],0,$pos);
					$r['authuser'] = substr($parse_uri['authority']['passwd'],$pos + 1);
				}
			}

			if(isset($parse_uri['authority']['host']) === true)
				$r['host'] = $parse_uri['authority']['host'];

			if(isset($parse_uri['authority']['port']) === true)
				$r['port'] = $parse_uri['authority']['port'];
		}

		if(isset($parse_uri['path']) === true)
		{
			if($parse_uri['path']{0} === '/')
				$r['contact'] = substr($parse_uri['path'],1);
			else
				$r['contact'] = $parse_uri['path'];
		}

		return($r);
	}
}

?>
