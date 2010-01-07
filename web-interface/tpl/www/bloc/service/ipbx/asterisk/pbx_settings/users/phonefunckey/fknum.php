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

$form = &$this->get_module('form');

$fkdata = $this->get_var('fkdata');
$fknumelem = $this->get_var('fknumelem');

$select = array();
$select['paragraph'] = false;
$select['name'] = 'phonefunckey[fknum][]';
$select['id'] = 'it-phonefunckey-fknum';
$select['label'] = false;
$select['key'] = false;
$select['default'] = $fknumelem['default'];

if($fkdata['ex'] === false):
	$select['id'] .= '-'.dwho_uint($fkdata['incr']);
	$select['selected'] = $fknumelem['value'];
else:
	$select['disabled'] = true;
endif;

echo	$form->select($select,$fknumelem['options']);

?>
