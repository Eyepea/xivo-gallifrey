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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'http','1.0','abstract.inc'));

class dwho_http_1_0_response extends dwho_http_1_0_abstract
{
	var $_status_line	= null;

	function dwho_http_1_0_response()
	{
		$this->dwho_http_1_0_abstract();
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
