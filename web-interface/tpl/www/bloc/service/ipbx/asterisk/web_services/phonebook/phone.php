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

$xmlphone = &$this->get_module('xmlphone',
			       array('vendor'	=> $this->get_var('vendor')));

if(($vendor = $directory = $xmlphone->get_vendor()) === false)
	xivo_die('Error/Invalid Vendor and User-Agent');

header($xmlphone->get_header_contenttype());

$param = array();

switch($vendor)
{
	case 'thomson':
	case 'snom':
		$directory = 'genericxml';
		break;
}

$this->file_include($this->get_var('path').'/'.$directory.'/'.$this->get_var('act'),
		    $param);

?>
