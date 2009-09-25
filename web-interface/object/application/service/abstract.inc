<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

require_once(dwho_file::joinpath(XIVO_PATH_OBJECT,'application','abstract.inc'));

class xivo_application_service_abstract extends xivo_application_abstract
{
	function _load_abstract_class($abstract)
	{
		$abstract = (string) $abstract;

		$file = $abstract.'.inc';

		$dir = dwho_file::joinpath(XIVO_PATH_OBJECT,
					   'application',
					   'service',
					   $this->_service_type,
					   $this->_service_name,
					   'abstract');

		$class = 'xivo_application_service_'.$this->_service_name.'_'.$abstract.'_abstract';

		if(dwho_file::load_file($file,$dir,true) !== false && class_exists($class) === true)
			return($class);

		return(false);
	}

	function _load_abstract($name,$param)
	{
		$name = (string) $name;

		if(isset($this->_abstract[$name]) === true)
		{
			if(is_array($param) === true)
				$this->_abstract[$name]->load_init($param);
			return(true);
		}
		else if(($class = $this->_load_abstract_class($name)) === false)
		{
			trigger_error('Abstract Application '.$name.' does not exist in '.$this->_service_name,E_USER_ERROR);
			return(false);
		}

		$this->_abstract[$name] = new $class($this->_service,$param);

		return(true);
	}
}

?>
