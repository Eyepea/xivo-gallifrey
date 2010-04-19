<?php

/*
 * XIVO Configuration
 * Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

define('AASTRA_CONF_FILENAME','aastra.cfg');

$modelstandardize = array('51i'		=> '6751i',
			  '53i'		=> '6753i',
			  '55i' 	=> '6755i',
			  '57i'		=> '6757i',
			  '57i CT'	=> '6757i CT');

if(isset($_SERVER['HTTP_USER_AGENT']) === false
|| preg_match('/^Aastra((?:(?:67)?5[1357]|673[019])i(?: CT)?) /',
	      $_SERVER['HTTP_USER_AGENT'],
	      $match) !== 1)
	die();
else if(isset($modelstandardize[$match[1]]) === true)
	$modelfilename = $modelstandardize[$match[1]].'.cfg';
else
	$modelfilename = $match[1].'.cfg';

if(is_file(AASTRA_CONF_FILENAME) === true)
	readfile(AASTRA_CONF_FILENAME);

if(is_file($modelfilename) === true)
	readfile($modelfilename);

die();

?>
