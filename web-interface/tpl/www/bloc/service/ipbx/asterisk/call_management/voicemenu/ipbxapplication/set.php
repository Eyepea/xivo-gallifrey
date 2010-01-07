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

?>
<div id="fd-ipbxapplication-set" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_set = $this->get_var('apparg_set');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_set-name'),
			  'name'	=> 'ipbxapplication[set][name]',
			  'labelid'	=> 'ipbxapplication-set-name',
			  'size'	=> 15,
			  'default'	=> $apparg_set['name']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_set-value'),
			  'name'	=> 'ipbxapplication[set][value]',
			  'labelid'	=> 'ipbxapplication-set-value',
			  'size'	=> 15,
			  'default'	=> $apparg_set['value']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_set-g'),
			      'name'	=> 'ipbxapplication[set][g]',
			      'labelid'	=> 'ipbxapplication-set-g',
			      'default'	=> $apparg_set['g']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-set',
			    'id'	=> 'it-add-ipbxapplication-set',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_set();"');

?>
</div>
