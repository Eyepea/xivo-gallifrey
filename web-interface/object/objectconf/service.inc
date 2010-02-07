<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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

define('XIVO_SRE_CLASS_PRE','_sre_');

$array = array();

$array['ipbx'] = array();

if(($asterisk_ini = dwho_gat::get_file(
			array(XIVO_PATH_OBJECT,
			      'service',
			      'ipbx',
			      'asterisk',
			      'initcfg'))) !== false)
{
	$array['ipbx']['asterisk'] = array();
	$array['ipbx']['asterisk']['label']	= 'Asterisk';
	$array['ipbx']['asterisk']['ini']	= $asterisk_ini;
}

$array['monitoring'] = array();

if(($monit_ini = dwho_gat::get_file(
			array(XIVO_PATH_OBJECT,
			      'service',
			      'monitoring',
			      'monit',
			      'initcfg'))) !== false)
{
	$array['monitoring']['monit'] = array();
	$array['monitoring']['monit']['label']	= 'Monit';
	$array['monitoring']['monit']['ini']	= $monit_ini;
}

?>
