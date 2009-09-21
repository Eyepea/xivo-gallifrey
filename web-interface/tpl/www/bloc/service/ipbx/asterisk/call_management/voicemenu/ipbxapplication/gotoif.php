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

?>
<div id="fd-ipbxapplication-gotoif" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_gotoif = $this->get_var('apparg_gotoif');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_gotoif-condition'),
			  'name'	=> 'ipbxapplication[gotoif][condition]',
			  'labelid'	=> 'ipbxapplication-gotoif-condition',
			  'size'	=> 15,
			  'default'	=> $apparg_gotoif['condition']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_gotoif-iftrue'),
			  'name'	=> 'ipbxapplication[gotoif][iftrue]',
			  'labelid'	=> 'ipbxapplication-gotoif-iftrue',
			  'size'	=> 15,
			  'default'	=> $apparg_gotoif['iftrue']['default'])),

	$form->text(array('desc'	=> $this->bbf('fmipbxapplication_gotoif-iffalse'),
			  'name'	=> 'ipbxapplication[gotoif][iffalse]',
			  'labelid'	=> 'ipbxapplication-gotoif-iffalse',
			  'size'	=> 15,
			  'default'	=> $apparg_gotoif['iffalse']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-gotoif',
			    'id'	=> 'it-add-ipbxapplication-gotoif',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_gotoif();"');

?>
</div>
