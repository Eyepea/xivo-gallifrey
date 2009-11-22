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

class dwho_http_abstract
{
	var $_format	= array();
	var $_headers	= array();
	var $_regexp	= array();
	var $_code	= null;

	function dwho_http_abstract()
	{
		$this->_format['rfc1123-date']	= 'D, d M Y H:i:s T';
		$this->_format['rfc850-date']	= 'l, d-M-y H:i:s T';
		$this->_format['asctime-date']	= 'D M j H:i:s Y';
		$this->_format['asctime-date2']	= 'D M  j H:i:s Y';
		$this->_format['rfc2822-date']	= 'r';

		$this->_regexp['time']		= '((?:[01][0-9]|2[0-3]):(?:[0-5][0-9]):(?:[0-5][0-9]))';
		$this->_regexp['day']		= '(0[1-9]|[12][0-9]|3[01])';
		$this->_regexp['wkday']		= '(Mon|Tue|Wed|Thu|Fri|Sat|Sun)';
		$this->_regexp['weekday']	= '((?:Mon|Tues|Wednes|Thurs|Fri|Satur|Sun)day)';
		$this->_regexp['month']		= '(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)';
		$this->_regexp['year']		= '(19[7-9][0-9]|[2-9][0-9]{3})';
		$this->_regexp['numtz']		= '([\+\-](?:[01][0-9]|2[0-3])(?:[0-5][0-9]))';

		$this->_regexp['date1']		= $this->_regexp['day'].' '.
						  $this->_regexp['month'].' '.
						  $this->_regexp['year'];
		$this->_regexp['date2']		= $this->_regexp['day'].'-'.
						  $this->_regexp['month'].'-'.
						  '([0-9]{2})';
		$this->_regexp['date3']		= $this->_regexp['month'].' '.
						  '( [1-9]|[12][0-9]|3[01])';

		$this->_regexp['rfc1123-date']	= $this->_regexp['wkday'].', '.
						  $this->_regexp['date1'].' '.
						  $this->_regexp['time'].' GMT';

		$this->_regexp['rfc850-date']	= $this->_regexp['weekday'].', '.
						  $this->_regexp['date2'].' '.
						  $this->_regexp['time'].' GMT';

		$this->_regexp['asctime-date']	= $this->_regexp['wkday'].' '.
						  $this->_regexp['date3'].' '.
						  $this->_regexp['time'].' '.
						  $this->_regexp['year'];

		$this->_regexp['rfc2822-date']	= $this->_regexp['wkday'].', '.
						  $this->_regexp['day'].' '.
						  $this->_regexp['month'].' '.
						  $this->_regexp['year'].' '.
						  $this->_regexp['time'].' '.
						  $this->_regexp['numtz'];
	}

	function valid_rfc1123_date($date)
	{
		if(is_string($date) === true
		&& preg_match('/^'.$this->_regexp['rfc1123-date'].'$/',$date) === 1
		&& gmdate($this->_format['rfc1123-date'],strtotime($date)) === $date)
			return(true);

		return(false);
	}

	function valid_rfc850_date($date)
	{
		if(is_string($date) === true
		&& preg_match('/^'.$this->_regexp['rfc850-date'].'$/',$date) === 1
		&& gmdate($this->_format['rfc850-date'],strtotime($date)) === $date)
			return(true);

		return(false);
	}

	function valid_asctime_date($date)
	{
		if(is_string($date) === false
		|| preg_match('/^'.$this->_regexp['asctime-date'].'$/',$date,$match) === 0)
			return(false);
		else if($match[3]{0} !== ' ')
			$format = $this->_format['asctime-date'];
		else
			$format = $this->_format['asctime-date2'];

		return((gmdate($format,strtotime($date.' GMT')) === $date));
	}

	function valid_date()
	{
		if($this->valid_rfc1123_date($date) === true
		|| $this->valid_rfc850_date($date) === true
		|| $this->valid_asctime_date($date) === true)
			return(true);

		return(false);
	}

	function valid_rfc2822_date($date)
	{
		if(is_string($date) === true
		&& preg_match('/^'.$this->_regexp['rfc2822-date'].'$/',$date) === 1
		&& gmdate($this->_format['rfc2822-date'],strtotime($date)) === $date)
			return(true);

		return(false);
	}

	function valid_token($str)
	{
		if(is_scalar($str) === false)
			return(false);

		return((bool) preg_match($this->_regexp['token'],$str));
	}

	function valid_quotedstring($text)
	{
		if(is_scalar($text) === false)
			return(false);

		return((bool) preg_match($this->_regexp['quoted-string'],$text));
	}

	function valid_token_word($text,$word_optional=true)
	{
		if(is_scalar($text) === false)
			return(false);

		$exp = explode('=',$text,2);

		if($this->valid_token($exp[0]) === false)
			return(false);
		else if(isset($exp[1]) === false)
			return((bool) $word_optional);

		return($this->valid_quotedstring($exp[1]));
	}

	function build_parameters($parameters)
	{
		if(is_array($parameters) === false)
			return(false);

		$r = array();

		foreach($parameters as $attribute => $value)
		{
			if($this->valid_token($attribute) === false
			|| ($value = $this->quote_word($value)) === false)
				return(false);

			$r[] = $attribute.'='.$value;
		}

		return(implode('; ',$r));
	}

	function build_date($timestamp,$format='rfc1123')
	{
		if(dwho_digit($timestamp) === false
		|| is_string($format) === false
		|| isset($this->_format[$format.'-date']) === false)
			return(false);

		$date = gmdate($this->_format[$format.'-date'],$timestamp);

		if($format === 'asctime' && strlen($date) !== 24)
			$date = gmdate($this->_format[$format.'-date2'],$timestamp);

		return($date);
	}

	function content_length($length)
	{
		if(dwho_is_digit($length) === false)
			return(false);

		return(dwho_ulongint($length));
	}

	function content_type($type,$subtype,$parameters=null)
	{
		if($this->valid_token($type) === false
		|| $this->valid_token($subtype) === false)
			return(false);

		if(is_array($parameters) === false || empty($parameters) === true)
		{
			$this->_headers['content-type'] = sprintf('Content-Type: %s/%s',$type,$subtype);
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

	function expires($date,$strict=true)
	{
		if((bool) $strict === true)
		{
			if($this->valid_date($date) === false)
				return(false);
		}
		else if($date !== -1
		&& $date !== 0
		&& $this->valid_date($date) === false)
			return(false);

		$this->_headers['expires'] = sprintf('Expires: %s',$date);

		return(true);
	}

	function pragma($pragma,$value=null)
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

	function last_modified($date)
	{
		if($this->valid_date($date) === false)
			return(false);

		$this->_headers['last-modified'] = sprintf('Last-Modified: %s',$date);

		return(true);
	}

	function get_status_code()
	{
		return($this->_code);
	}

	function get_status_type()
	{
		if(isset($this->_status_code[$this->_code]) === false)
			return('unknown');

		switch(substr($this->_code,0,1))
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
