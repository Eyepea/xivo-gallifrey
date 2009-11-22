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

class dwho_http_1_0_abstract extends dwho_http_abstract
{
	var $_version		= '1.0';
	var $_status_code	= array(
					200	=> 'OK',
					201	=> 'Created',
					202	=> 'Accepted',
					204	=> 'No Content',
					301	=> 'Moved Permanently',
					302	=> 'Found',
					304	=> 'Not Modified',
					400	=> 'Bad Request',
					401	=> 'Unauthorized',
					403	=> 'Forbidden',
					404	=> 'Not Found',
					500	=> 'Internal Server Error',
					501	=> 'Not Implemented',
					502	=> 'Bad Gateway',
					503	=> 'Service Unavailable');

	function dwho_http_1_0_abstract()
	{
		$this->dwho_http_abstract();

		$separators	= '\(\)\<\>@,;:\/\\"\\\[\]\?=\{\} \t';
		$ctls		= '\x00-\x1F\x7F';

		$this->_regexp['token']		= '/^[^'.$ctls.$separators.']+$/';
		$this->_regexp['quoted-string']	= '/(?:^[^'.$ctls.$separators.']+$'.
						  '|^"[^"\x00-\x08\x0B\x0C\x0E-\x1F]+"$)/';
	}

	function quote_word($str)
	{
		$str = strval($str);

		if(preg_match($this->_regexp['token'],$str) === 1)
			return('"'.$str.'"');
		else if(preg_match($this->_regexp['quoted-string'],$str) === 1)
			return($str);

		return(false);
	}
}

?>
