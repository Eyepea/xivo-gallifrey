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

require_once(dwho_file::joinpath(DWHO_PATH_ROOT,'http','1.1','abstract.inc'));

class dwho_http_1_1_request extends dwho_http_1_1_abstract
{
	function dwho_http_1_1_request()
	{
		$this->dwho_http_1_1_abstract();
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
				case 'no-cache':
				case 'no-store':
				case 'no-transform':
				case 'only-if-cached':
					$attribute = $attrlower;
					$value = null;
					break;
				case 'max-age':
				case 'min-fresh':
					$attribute = $attrlower;

					if(dwho_is_uint($value) === false)
						return(false);
					break;
				case 'max-stale':
					$attribute = $attrlower;

					if($value === null)
						break;
					else if(dwho_is_uint($value) === false)
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
}

?>
