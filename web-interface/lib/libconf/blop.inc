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

$array = array();

$array['ini'] = array();

$array['sys'] = array();

$array['sys']['php_version'] = array(
				'min'		=> '4.4',
				'max'		=> '5.0');

$array['sys']['separator'] = array(
				'dir'		=> '/',
				'path'		=> ':',
				'arg_in'	=> '&',
				'arg_out'	=> '&');

$array['sys']['session'] = array(
				'name'		=> '_eid',
				'time'		=> 7200,
				'path'		=> '/',
				'max_per_user'	=> 1);

$array['sys']['locale'] = array(
				'language'	=> 'fr',
				'territory'	=> 'FR',
				'charset'	=> 'UTF-8');

?>
