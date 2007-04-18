<?php

# Copyright (C) 2006 Adrien DELLE CAVE <adellecave@proformatique.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$array = array();

$array['ini'] = array();

$array['sys'] = array();

$array['sys']['general'] = array(
				'soft_name'	=> 'xivo',
				'soft_label'	=> 'XIVO',
				'soft_version'	=> '0.1',
				'soft_url'	=> 'www.xivo.fr',
				'corp_address'	=> '67 rue Voltaire',
				'corp_zipcode'	=> '92800',
				'corp_city'	=> 'Puteaux',
				'corp_country'	=> 'France',
				'corp_phone'	=> '+33 (0) 1 41 38 99 60',
				'corp_fax'	=> '+33 (0) 1 41 38 99 70',
				'corp_name'	=> 'proformatique',
				'corp_label'	=> 'Proformatique',
				'corp_email'	=> 'contact@proformatique.com',
				'corp_url'	=> 'www.proformatique.com');

$array['sys']['php_version'] = array(
				'min'		=> '4.4',
				'max'		=> '5.0');

$array['sys']['extension'] = array(
				'prefix'	=> '',
				'suffix'	=> '',
				'const'		=> 'XIVO_EXT__');

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
