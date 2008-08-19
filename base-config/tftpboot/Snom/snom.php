<?php

/*
 * XIVO Configuration
 * Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

if(isset($_GET['mac']) === true
&& preg_match('/^[A-F0-9]{12}$/',strval($_GET['mac']),$match) === 1)
	$macaddr = $match[0];
else
	$macaddr = '';

if(isset($_SERVER['HTTP_USER_AGENT']) === false
|| preg_match('/(snom3[026]0)-/',$_SERVER['HTTP_USER_AGENT'],$match) !== 1)
	snom_get_config('snom',$macaddr);
else
	snom_get_config($match[1],$macaddr);

die();

function snom_get_config($type,$macaddr)
{
	$filename = $type.'.htm';

	if(is_file($filename) === true)
		include($filename);

	if(isset($macaddr{0}) === false)
		return(null);

	$filename = $type.'-'.$macaddr.'.htm';

	if(is_file($filename) === true)
		include($filename);
}

?>
