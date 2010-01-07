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

class dwho_http
{
	function factory($mode,$version='1.1')
	{
		if(($httparr = &dwho_gat::load_get('http',DWHO_PATH_CONF)) === false)
			trigger_error('Failed to load HTTP configuration',E_USER_ERROR);

		$version = strval($version);

		if(isset($httparr['version'][$version]) === false)
			$version = '1.1';

		if(isset($httparr['version'][$version]['mode'][$mode]) === false)
			return(false);

		$version_name = $httparr['version'][$version]['name'];
		$name = 'dwho_http_'.$version_name.'_'.$mode;

		if(dwho::load_class($name,
				    array('http',$version),
				    $mode) === false)
			return(false);

		return(new $name());
	}
}

?>
