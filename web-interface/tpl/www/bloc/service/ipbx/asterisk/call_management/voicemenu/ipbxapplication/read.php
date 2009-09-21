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
<div id="fd-ipbxapplication-read" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_read = $this->get_var('apparg_read');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_read-variable'),
			  'name'	=> 'ipbxapplication[read][variable]',
			  'labelid'	=> 'ipbxapplication-read-variable',
			  'size'	=> 15,
			  'default'	=> $apparg_read['variable']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_read-filename'),
			  'name'	=> 'ipbxapplication[read][filename]',
			  'labelid'	=> 'ipbxapplication-read-filename',
			  'size'	=> 15,
			  'default'	=> $apparg_read['filename']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_read-maxdigits'),
			  'name'	=> 'ipbxapplication[read][maxdigits]',
			  'labelid'	=> 'ipbxapplication-read-maxdigits',
			  'size'	=> 10,
			  'default'	=> $apparg_read['maxdigits']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_read-s'),
			      'name'	=> 'ipbxapplication[read][s]',
			      'labelid'	=> 'ipbxapplication-read-s',
			      'default'	=> $apparg_read['s']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_read-i'),
			      'name'	=> 'ipbxapplication[read][i]',
			      'labelid'	=> 'ipbxapplication-read-i',
			      'default'	=> $apparg_read['i']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_read-n'),
			      'name'	=> 'ipbxapplication[read][n]',
			      'labelid'	=> 'ipbxapplication-read-n',
			      'default'	=> $apparg_read['n']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_read-attempts'),
			  'name'	=> 'ipbxapplication[read][attempts]',
			  'labelid'	=> 'ipbxapplication-read-attempts',
			  'size'	=> 10,
			  'default'	=> $apparg_read['attempts']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_read-timeout'),
			  'name'	=> 'ipbxapplication[read][timeout]',
			  'labelid'	=> 'ipbxapplication-read-timeout',
			  'size'	=> 10,
			  'default'	=> $apparg_read['timeout']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-read',
			    'id'	=> 'it-add-ipbxapplication-read',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_read();"');

?>
</div>
