<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'http','1.1','abstract.inc'));

class dwho_http_1_1_response extends dwho_http_1_1_abstract
{
	var $_status_line	= null;

	function dwho_http_1_1_response()
	{
		$this->dwho_http_1_1_abstract();
	}

	function set_status_line($code)
	{
		$code = intval($code);

		if(isset($this->_status_code[$code]) === false)
			return(false);

		$this->_code = $code;
		$this->_status_line = sprintf('HTTP/%s %d %s',
					      $this->_version,
					      $this->_code,
					      $this->_status_code[$code]);

		return(true);
	}

	function cache_control($parameters)
	{
		if(is_scalar($parameters) === true)
			$parameters = array($parameters => null);

		if(is_array($parameters) === false || empty($parameters) === true)
			return(false);

		$params = array();

		foreach($parameters as $attribute => $value)
		{
			$attrlower = strtolower($attribute);

			switch($attrlower)
			{
				case 'public':
				case 'no-store':
				case 'no-transform':
				case 'must-revalidate':
				case 'proxy-revalidate':
					$attribute = $attrlower;
					$value = null;
					break;
				case 'private':
				case 'no-cache':
					$attribute = $attrlower;

					if($value === null)
						break;
					else if($this->valid_token($value) === false)
						return(false);

					$value = '"'.$value.'"';
					break;
				case 'max-age':
				case 's-maxage':
					$attribute = $attrlower;

					if(dwho_is_uint($value) === false)
						return(false);
					break;
				default:
					if($this->valid_token($attribute) === false)
						return(false);
					else if($value !== null)
						$value = $this->quote_word($value);
			}

			if($value !== null)
				$params[$attribute] = $attribute.'='.$value;
			else
				$params[$attribute] = $attribute;
		}

		$this->_headers['cache-control'] = sprintf('Cache-Control: %s',implode(', ',$params));

		return(true);
	}

	function content_disposition($type,$parameters=null)
	{
		if(is_scalar($type) === false)
			return(false);
		else if(strcasecmp('attachment',$type) === 0)
			$type = 'attachment';
		else if(strcasecmp('inline',$type) === 0)
			$type = 'inline';
		else if($this->valid_token($type) === false)
			return(false);

		if(is_array($parameters) === false || empty($parameters) === true)
		{
			$this->_headers['content-disposition'] = sprintf('Content-Disposition: %s',$type);
			return(true);
		}

		$params = array();

		foreach($parameters as $attribute => $value)
		{
			$attrlower = strtolower($attribute);

			switch($attrlower)
			{
				case 'filename':
					$attribute = $attrlower;

					if(($value = $this->quote_word($value)) === false)
						return(false);
					break;
				case 'creation-date':
				case 'modification-date':
				case 'read-date':
					$attribute = $attrlower;

					if($this->valid_rfc2822_date($value) === false
					|| ($value = $this->quote_word($value)) === false)
						return(false);
					break;
				case 'size':
					$attribute = $attrlower;

					if(dwho_is_digit($value) === false)
						return(false);

					$value = dwho_ulongint($value);
					break;
				default:
					if($this->valid_token($attribute) === false
					|| ($value = $this->quote_word($value)) === false)
						return(false);
			}

			$params[$attribute] = $attribute.'='.$value;
		}

		$this->_headers['content-disposition'] = sprintf('Content-Disposition: %s; %s',
								 $type,
								 implode('; ',$params));

		return(false);
	}

	function authent_basic($realm)
	{
		if(($realm = $this->quote_word($realm)) === false)
			return(false);

		$this->_headers['www-authenticate'] = sprintf('WWW-Authenticate: Basic realm=%s',
							      $realm);

		return(true);
	}

	function send($die=false)
	{
		if(isset($this->_status_line) === true)
			header($this->_status_line);

		foreach($this->_headers as $value)
			header($value);

		if((bool) $die === true)
			die();
	}
}

?>
