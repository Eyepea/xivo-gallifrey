<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'mime','abstract','cim.inc'));

class dwho_http_abstract extends dwho_mime_cim
{
	var $_headers	= array();
	var $_code	= null;

	function dwho_http_abstract()
	{
		$this->_rule['tspecials']	= '\t \(\)\<\>@,;:\\\"\/\[\]\?=\{\}';
		$this->_rule['token']		= '\!#\$%\&\'\*\+\-\.0-9A-Z\^_`a-z\|~';
		$this->_rule['qtext']		= $this->_rule['crlf'].
						  $this->_rule['wsp'].
						  '\x21\x23-\x7E';

		$this->dwho_mime_cim();
	}

	function valid_http_date($date)
	{
		if($this->valid_rfc1123_date($date) === true
		|| $this->valid_rfc850_date($date) === true
		|| $this->valid_asctime_date($date) === true)
			return(true);

		return(false);
	}

	function set_content_length($length)
	{
		if(dwho_is_digit($length) === false)
			return(false);

		$this->headers['content-length'] = sprintf('Content-Length: %d',
							   dwho_ulongint($length));

		return(true);
	}

	function set_content_type($type,$subtype,$parameters=null)
	{
		if($this->valid_token($type) === false
		|| $this->valid_token($subtype) === false)
			return(false);
		else if(is_array($parameters) === false || empty($parameters) === true)
		{
			$this->_headers['content-type'] = sprintf('Content-Type: %s/%s',
								  $type,
								  $subtype);
			return(true);
		}
		else if(($parameters = $this->build_parameters($parameters)) !== false)
		{
			$this->_headers['content-type'] = sprintf('Content-Type: %s/%s; %s',
								  $type,
								  $subtype,
								  $parameters);
			return(true);
		}
	
		return(false);
	}

	function set_expires($date,$strict=true)
	{
		if((bool) $strict === true)
		{
			if($this->valid_http_date($date) === false)
				return(false);
		}
		else if($date !== -1
		&& $date !== 0
		&& $this->valid_http_date($date) === false)
			return(false);

		$this->_headers['expires'] = sprintf('Expires: %s',$date);

		return(true);
	}

	function set_pragma($pragma,$value=null)
	{
		if(is_scalar($pragma) === false)
			return(false);
		else if(strcasecmp('no-cache',$pragma) === 0)
		{
			$pragma = 'no-cache';
			$value = null;
		}
		else if($this->valid_token($pragma) === false)
			return(false);

		if($value === null)
			$this->_headers['pragma'] = sprintf('Pragma: %s',$pragma);
		else if(($value = $this->quote_word($value)) !== false)
			$this->_headers['pragma'] = sprintf('Pragma: %s=%s',$pragma,$value);
		else
			return(false);

		return(true);
	}

	function set_last_modified($date)
	{
		if($this->valid_http_date($date) === false)
			return(false);

		$this->_headers['last-modified'] = sprintf('Last-Modified: %s',$date);

		return(true);
	}

	function get_status_code()
	{
		return($this->_code);
	}

	function get_status_type($code=null)
	{
		if(is_scalar($code) === false)
			$code = $this->_code;

		if(isset($this->_status_code[$code]) === false)
			return('unknown');

		switch(substr($code,0,1))
		{
			case '1':
				return('informational');
			case '2':
				return('successful');
			case '3':
				return('redirection');
			case '4':
				return('client-error');
			case '5':
				return('server-error');
		}

		return('unknown');
	}
}

?>
