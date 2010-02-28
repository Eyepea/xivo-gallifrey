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
dwho::load_class('dwho_curl');
dwho::load_class('dwho_json');

xivo_object::required(array('abstract','abstract.inc'),true);

class xivo_object_sysconfd extends xivo_object_abstract
{
	var $_name		= 'sysconfd';
	var $_curl		= null;
	var $_uri		= null;
	var $_url		= array();
	var $_status_code	= null;

	function xivo_object_sysconfd(&$xobj)
	{
		$this->xivo_object_abstract(&$xobj);
		$this->_load_config();
		$this->_set_configuration();

		$this->_url = array('scheme'	=> 'http',
				    'host'	=> $this->_conf['sysconfd']['host'],
				    'port'	=> $this->_conf['sysconfd']['port']);

		$this->_uri = new dwho_uri();
		$this->_curl = new dwho_curl();
	}

	function _set_configuration()
	{
		$_CF = &dwho_gat::get('_CF');

		if(dwho_issa($this->_name,$_CF['xivo']['ini']) === false)
			return(null);

		$ref = &$_CF['xivo']['ini'][$this->_name];

		if(isset($ref['host']) === true)
		{
			if(dwho_network::valid_host($ref['host'],
						    (DWHO_NETWORK_HOST_IPV4_DOTDEC | DWHO_NETWORK_HOST_HOSTNAME)) === true)
				$this->_conf['sysconfd']['host'] = $ref['host'];
			else
				trigger_error('Invalid option host for section sysconfd in XIVO init file',E_USER_WARNING);
		}

		if(isset($ref['port']) === true)
		{
			if(dwho_network::valid_port_number($ref['port']) === true)
				$this->_conf['sysconfd']['port'] = $ref['port'];
			else
				trigger_error('Invalid option port for section sysconfd in XIVO init file',E_USER_WARNING);
		}

		if(isset($ref['connect_timeout']) === true)
		{
			if(dwho_is_uint($ref['connect_timeout']) === true)
				$this->_conf['connect_timeout'] = $ref['connect_timeout'];
			else
				trigger_error('Invalid option connect_timeout for sysconfd in XIVO init file',E_USER_WARNING);
		}

		if(isset($ref['timeout']) === true)
		{
			if(dwho_is_uint($ref['timeout']) === true)
				$this->_conf['timeout'] = $ref['timeout'];
			else
				trigger_error('Invalid option timeout for sysconfd in XIVO init file',E_USER_WARNING);
		}

		return(true);
	}

	function _create()
	{
		return($this->_curl->load(null,
					  array('connect_timeout'	=> $this->_conf['sysconfd']['connect_timeout'],
					        'timeout'		=> $this->_conf['sysconfd']['timeout'],
					        'httpheader'		=> array(dwho_json::get_header())),
				          false));
	}

	function _connect($path,$query=null)
	{
		if(dwho_has_len($path) === false)
			return(false);

		$url = $this->_url;
		$url['path'] = $path;

		if(is_array($query) === true)
			$url['query'] = $query;

		if(($uri = $this->_uri->build_uri($url)) === false
		|| $this->_curl->set_option('url',$uri) === false)
			return(false);

		return($this->_curl->connect());
	}

	function last_status_code()
	{
		return($this->_status_code);
	}

	function request_get($path,$query=null,$decode_assoc=true)
	{
		$this->_status_code = null;

		if($this->_create() === false)
			return(false);

		$this->_curl->set_option('get',true);

		if(($r = $this->_connect($path,$query)) !== false)
			$r = dwho_json::decode(rtrim($r),$decode_assoc);

		$this->_status_code = $this->_curl->get_info('http_code');

		$this->_curl->close();

		return($r);
	}

	function request_post($path,$data,$query=null,$decode_assoc=true)
	{
		$this->_status_code = null;

		if($this->_create() === false)
			return(false);

		$this->_curl->set_option('post',true);
		$this->_curl->set_option('postfields',dwho_json::encode($data));

		if(($r = $this->_connect($path,$query)) !== false)
			$r = dwho_json::decode(rtrim($r),$decode_assoc);

		$this->_status_code = $this->_curl->get_info('http_code');

		$this->_curl->close();

		return($r);
	}
}

?>
