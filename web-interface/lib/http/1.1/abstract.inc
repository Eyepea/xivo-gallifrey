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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'http','abstract.inc'));

class dwho_http_1_1_abstract extends dwho_http_abstract
{
	var $_version		= '1.1';
	var $_status_code	= array(
					100	=> 'Continue',
					101	=> 'Switching Protocols',
					200	=> 'OK',
					201	=> 'Created',
					202	=> 'Accepted',
					203	=> 'Non-Authoritative Information',
					204	=> 'No Content',
					205	=> 'Reset Content',
					206	=> 'Partial Content',
					300	=> 'Multiple Choices',
					301	=> 'Moved Permanently',
					302	=> 'Found',
					303	=> 'See Other',
					304	=> 'Not Modified',
					305	=> 'Use Proxy',
					307	=> 'Temporary Redirect',
					400	=> 'Bad Request',
					401	=> 'Unauthorized',
					402	=> 'Payment Required',
					403	=> 'Forbidden',
					404	=> 'Not Found',
					405	=> 'Method Not Allowed',
					406	=> 'Not Acceptable',
					407	=> 'Proxy Authentication Required',
					408	=> 'Request Time-out',
					409	=> 'Conflict',
					410	=> 'Gone',
					411	=> 'Length Required',
					412	=> 'Precondition Failed',
					413	=> 'Request Entity Too Large',
					414	=> 'Request-URI Too Large',
					415	=> 'Unsupported Media Type',
					416	=> 'Requested range not satisfiable',
					417	=> 'Expectation Failed',
					500	=> 'Internal Server Error',
					501	=> 'Not Implemented',
					502	=> 'Bad Gateway',
					503	=> 'Service Unavailable',
					504	=> 'Gateway Time-out',
					505	=> 'HTTP Version not supported');

	function dwho_http_1_1_abstract()
	{
		$this->dwho_http_abstract();

		$this->_rule['quoted']		= $this->_rule['lws'].'\x21\x23-\x5B\x5D-\x7E\x80-\xFF';
		$this->_rule['quoted-text']	= '\x01-\x09\x0B\x0C\x0E-\xFF';

		$this->_regexp['quoted-string']	= '(?:['.$this->_rule['token'].']+|'.
						     '"(?:['.$this->_rule['quoted'].']|'.
							 '\\\['.$this->_rule['quoted-text'].'])+")';

		$this->_regexp['parameter']	= '(['.$this->_rule['token'].']+)\s*=\s*'.
						  '(?:(['.$this->_rule['quoted'].']+)|'.
						      '"((?:['.$this->_rule['quoted'].']|'.
							   '\\\['.$this->_rule['quoted-text'].'])+)")';
	}

	function quote_word($str)
	{
		$str = strval($str);

		if(preg_match('/^'.$this->_regexp['token'].'$/',$str) === 1)
			return('"'.$str.'"');
		else if(preg_match('/^'.$this->_regexp['quoted-string'].'$/',$str) === 1)
			return($str);

		return('"'.preg_replace('/(?<!\\\)"/','\"',$str).'"');
	}
}

?>
