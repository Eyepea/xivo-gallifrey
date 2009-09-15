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

define('XIVO_TPL_SPACE','json');

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');

$application = $_TPL->get_application('service/ipbx/'.$ipbx->get_name().'/ajs/',2);

if($application === false)
{
	$dhtml = &$_TPL->get_module('dhtml');
	$dhtml->ajs_die('Error/404');
}

die(include($application));

?>
