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

class dwho_file_fileinfo
{
	var $_finfo	= null;
	var $info	= array(
				'path'		=> '',
				'mime'		=> false,
				'charset'	=> false);

	function open($define=FILEINFO_MIME)
	{
		$this->close();
		$this->_finfo = finfo_open($define);
	}

	function get_info($file)
	{
		if(($file = dwho_file::is_f($file)) === false)
			return(false);
		else if(is_resource($this->_finfo) === false)
			$this->open(FILEINFO_MIME);

		$info = finfo_file($this->_finfo,$file);

		$this->info['path'] = $file;

		$arr = explode(';',$info);

		if(isset($arr[1]) === true)
			$this->info['charset'] = trim($arr[1]);

		$this->info['mime'] = trim($arr[0]);

		return($this->info);
	}

	function get_charset($file)
	{
		if($this->get_info($file) === false)
			return(false);

		return($this->info['charset']);
	}

	function get_mime($file)
	{
		if($this->get_info($file) === false)
			return(false);

		return($this->info['mime']);
	}

	function close()
	{
		if(is_resource($this->_finfo) === true)
			finfo_close($this->_finfo);
	}
}

?>
