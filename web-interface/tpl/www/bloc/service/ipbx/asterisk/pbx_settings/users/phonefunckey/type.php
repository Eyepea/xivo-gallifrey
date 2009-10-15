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
$list = $this->get_var('fktype_list');
$select = array();
$select['paragraph'] = false;
$select['name'] = 'phonefunckey[type][]';
$select['label'] = false;
$select['key'] = 'name';
$select['altkey'] = 'name';
$select['bbf'] = 'fm_phonefunckey_type-opt';
$select['bbfopt'] = array('argmode' => 'paramvalue');
$select['id'] = 'it-phonefunckey-type';

$select['optgroup'] = array('key'	=> 'category',
			    'unique'	=> true,
			    'bbf'	=> 'fm_phonefunckey_type-optgroup',
			    'bbfopt'	=> array('argmode' => 'paramvalue'));

$selectoptattr = 'onchange="xivo_phonefunckey_chg_type(this);"';

if($fkdata['ex'] === false):
	$select['id'] .= '-'.dwho_uint($fkdata['incr']);
	$select['selected'] = $fkdata['type'];
else:
	$select['disabled'] = true;
endif;

echo	$form->select($select,$list,$selectoptattr);

?>
