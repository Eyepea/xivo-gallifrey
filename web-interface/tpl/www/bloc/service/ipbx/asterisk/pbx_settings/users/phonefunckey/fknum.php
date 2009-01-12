<?php

#
# XiVO Web-Interface
# Copyright (C) 2006, 2007, 2008  Proformatique <technique@proformatique.com>
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
$select['field'] = false;
$select['name'] = 'phonefunckey[fknum][]';
$select['id'] = false;
$select['label'] = false;
$select['key'] = false;
$select['default'] = $fknumelem['default'];

if($fkdata['ex'] === false):
	$selectoptattr = '';
	$select['value'] = $fknumelem['value'];
else:
	$select['disabled'] = true;

	$selectoptattr = 'onfocus="xivo_fm_set_onfocus(this);" '.
			 'onblur="xivo_fm_set_onblur(this);"';
endif;

echo	$form->select($select,$fknumelem['options'],$selectoptattr);

?>
