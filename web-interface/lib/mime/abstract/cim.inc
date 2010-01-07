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

/* Common Internet Message */

class dwho_mime_cim
{
	var $_date		= array('time'		=> '((?:[01][0-9]|2[0-3]):(?:[0-5][0-9]):(?:[0-5][0-9]))',
					'day'		=> '(0[1-9]|[12][0-9]|3[01])',
					'wkday'		=> '(Mon|Tue|Wed|Thu|Fri|Sat|Sun)',
					'weekday'	=> '((?:Mon|Tues|Wednes|Thurs|Fri|Satur|Sun)day)',
					'month'		=> '(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)',
					'year'		=> '(19[0-9]{2}|[2-9][0-9]{3})',
					'zone'		=> '([\+\-](?:[01][0-9]|2[0-3])(?:[0-5][0-9]))');

	var $_date_format	= array('asctime'	=> array('D M j H:i:s Y','D M  j H:i:s Y'),
					'rfc850'	=> 'l, d-M-y H:i:s T',
					'rfc1123'	=> 'D, d M Y H:i:s T',
					'rfc2822'	=> 'r');

	var $_regexp		= array();
	var $_rule		= array('ctl'	=> '\x00-\x1F\x7F',
					'crlf'	=> '\x0D\x0A',
					'wsp'	=> '\x20\x09');

	function dwho_mime_cim()
	{
		if(isset($this->_rule['tspecials']) === false)
			$this->_rule['tspecials'] = '\(\)\<\>@,;:\\\"\/\[\]\?=';

		if(isset($this->_rule['token']) === false)
			$this->_rule['token'] = '\!#\$%\&\'\*\+\-\.0-9A-Z\^_`a-z\{\|\}~';

		if(isset($this->_rule['qtext']) === false)
			$this->_rule['qtext'] = $this->_rule['token'].'\(\)\<\>@,;:\\/\[\]\?=';

		$this->_regexp['token']		= '['.$this->_rule['token'].']+';

		$this->_regexp['word']		= '(?:['.$this->_rule['token'].']+|'.
						     '"['.$this->_rule['qtext'].']+")';

		$this->_regexp['parameter']	= '(['.$this->_rule['token'].']+)\s*=\s*'.
						  '(?:(['.$this->_rule['token'].']+)|'.
						     '"(['.$this->_rule['qtext'].']+)")';

		$this->_date['date1']		= $this->_date['day'].' '.
						  $this->_date['month'].' '.
						  $this->_date['year'];
		$this->_date['date2']		= $this->_date['day'].'-'.
						  $this->_date['month'].'-'.
						  '([0-9]{2})';
		$this->_date['date3']		= $this->_date['month'].' '.
						  '( ?[1-9]|[12][0-9]|3[01])';

		$this->_regexp['asctime-date']	= $this->_date['wkday'].' '.
						  $this->_date['date3'].' '.
						  $this->_date['time'].' '.
						  $this->_date['year'];

		$this->_regexp['rfc850-date']	= $this->_date['weekday'].', '.
						  $this->_date['date2'].' '.
						  $this->_date['time'].' GMT';

		$this->_regexp['rfc1123-date']	= $this->_date['wkday'].', '.
						  $this->_date['date1'].' '.
						  $this->_date['time'].' GMT';

		$this->_regexp['rfc2822-date']	= $this->_date['wkday'].', '.
						  $this->_date['day'].' '.
						  $this->_date['month'].' '.
						  $this->_date['year'].' '.
						  $this->_date['time'].' '.
						  $this->_date['zone'];
	}

	function quote_word($str)
	{
		$str = strval($str);

		if(preg_match('/^'.$this->_regexp['token'].'$/',$str) === 1)
			return('"'.$str.'"');
		else if(preg_match('/^'.$this->_regexp['word'].'$/',$str) === 1)
			return($str);

		return(false);
	}

	function _generic_valid_date($date,$format)
	{
		if(is_string($date) === true
		&& isset($this->_date_format[$format],$this->_regexp[$format.'-date']) === true
		&& preg_match('/^'.$this->_regexp[$format.'-date'].'$/',$date) === 1
		&& gmdate($this->_date_format[$format],strtotime($date)) === $date)
			return(true);

		return(false);
	}

	function valid_asctime_date($date)
	{
		if(is_string($date) === false
		|| preg_match('/^'.$this->_regexp['asctime-date'].'$/',$date,$match) === 0)
			return(false);

		$format = $this->_date_format['asctime'][(int) ($match[3]{0} === ' ')];

		return((gmdate($format,strtotime($date.' GMT')) === $date));
	}

	function valid_rfc850_date($date)
	{
		return($this->_generic_valid_date($date,'rfc850'));
	}

	function valid_rfc1123_date($date)
	{
		return($this->_generic_valid_date($date,'rfc1123'));
	}

	function valid_rfc2822_date($date)
	{
		return($this->_generic_valid_date($date,'rfc2822'));
	}

	function valid_date($date,$format='rfc2822')
	{
		if(isset($this->_date_format[$format]) === false)
			return(false);

		return($this->{'valid_'.$format.'_date'}($date));
	}

	function _generic_build_date($timestamp,$format)
	{
		if(dwho_is_digit($timestamp) === true 
		&& isset($this->_date_format[$format]) === true)
			return(gmdate($this->_date_format[$format],$timestamp));

		return(false);
	}

	function build_asctime_date($timestamp)
	{
		if(dwho_is_digit($timestamp) === false)
			return(false);

		$date = gmdate($this->_date_format['asctime'][0],$timestamp);

		if(strlen($date) !== 24)
			$date = gmdate($this->_date_format['asctime'][1],$timestamp);

		return($date);
	}

	function build_rfc850_date($timestamp)
	{
		return($this->_generic_build_date($timestamp,'rfc850'));
	}

	function build_rfc1123_date($timestamp)
	{
		return($this->_generic_build_date($timestamp,'rfc1123'));
	}

	function build_rfc2822_date($timestamp)
	{
		return($this->_generic_build_date($timestamp,'rfc2822'));
	}

	function build_date($timestamp,$format='rfc2822')
	{
		if(isset($this->_date_format[$format]) === false)
			return(false);

		return($this->{'build_'.$format.'_date'}($timestamp));
	}

	function valid_token($str)
	{
		if(is_scalar($str) === false)
			return(false);

		return((bool) preg_match('/^'.$this->_regexp['token'].'$/',$str));
	}

	function valid_word($text)
	{
		if(is_scalar($text) === false)
			return(false);

		return((bool) preg_match('/^'.$this->_regexp['word'].'$/',$text));
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

		return($this->valid_word($exp[1]));
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

	function parse_parameters($parameters)
	{
		if(is_scalar($parameters) === false)
			return(false);

		$parameters = trim($parameters);

		if(substr($parameters,-1) !== ';')
			$parameters .= ';';

		if(($nb = preg_match_all('/\s*'.$this->_regexp['parameter'].';\s*/',
					 $parameters,
					 $match,
					 PREG_SET_ORDER)) === 0)
			return(false);

		$r = array();

		for($i = 0;$i < $nb;$i++)
		{
			if(isset($match[$i][2]{0}) === true)
				$r[$match[$i][1]] = $match[$i][2];
			else
				$r[$match[$i][1]] = stripslashes($match[$i][3]);
		}

		return($r);
	}

	function parse_content_type($data)
	{
		if(is_string($data) === false)
			return(false);

		$exptype = explode('/',$data,2);

		if(isset($exptype[1]) === false
		|| $this->valid_token($exptype[0]) === false)
			return(false);

		$expsubtype = explode(';',$exptype[1],2);

		if($this->valid_token($expsubtype[0]) === false)
			return(false);

		$r = array('type'	=> $exptype[0],
			   'subtype'	=> $expsubtype[0]);
	
		if(isset($expsubtype[1]) === true
		&& ($r['parameters'] = $this->parse_parameters($expsubtype[1])) === false)
			return(false);

		return($r);
	}
}

?>
