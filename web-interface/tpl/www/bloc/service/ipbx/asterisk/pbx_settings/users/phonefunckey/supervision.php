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

$form = &$this->get_module('form');

$fkdata = $this->get_var('fkdata');
$supelem = $this->get_var('supelem');

$select = array();
$select['field'] = false;
$select['name'] = 'phonefunckey[supervision][]';
$select['label'] = false;
$select['bbf'] = array('concatkey','fm_phonefunckey_supervision-opt-');
$select['id'] = 'it-phonefunckey-supervision';
$select['default'] = $supelem['default'];

if($fkdata['ex'] === false):
	$select['id'] .= '-'.xivo_uint($fkdata['incr']);
	$select['value'] = $supelem['value'];
else:
	$select['disabled'] = true;
endif;

echo	$form->select($select,$supelem['options']);

?>
