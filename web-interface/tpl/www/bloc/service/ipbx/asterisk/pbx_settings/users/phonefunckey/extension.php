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

$inputtxt = array();
$inputtxt['paragraph'] = false;
$inputtxt['name'] = 'phonefunckey[typeval][]';
$inputtxt['label'] = false;
$inputtxt['id'] = 'it-phonefunckey-extension-typeval';
$inputtxt['size'] = 15;

if($fkdata['ex'] === false):
	$incr = dwho_uint($fkdata['incr']);
	$inputtxt['id'] = 'it-phonefunckey-'.$this->get_var('fktype').'-typeval-'.$incr;
	$inputtxt['value'] = $this->get_var('fktypeval');

	$inputtxtoptattr = '';
else:
	$inputtxt['disabled'] = true;
	$inputtxtoptattr = 'style="display: none;"';
endif;

echo	$form->text($inputtxt,$inputtxtoptattr);

?>
