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
<div id="fd-ipbxapplication-authenticate" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_authenticate = $this->get_var('apparg_authenticate');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-password'),
			  'name'	=> 'ipbxapplication[authenticate][password]',
			  'labelid'	=> 'ipbxapplication-authenticate-password',
			  'size'	=> 15,
			  'default'	=> $apparg_authenticate['password']['default'])),

	$form->select(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-passwordinterpreter'),
			    'name'	=> 'ipbxapplication[authenticate][passwordinterpreter]',
			    'labelid'	=> 'ipbxapplication-authenticate-passwordinterpreter',
			    'empty'	=> true,
			    'key'	=> false,
			    'bbf'	=> array('paramvalue','fm_ipbxapplication_authenticate-passwordinterpreter-opt'),
			    'default'	=> $apparg_authenticate['passwordinterpreter']['default']),
		      $apparg_authenticate['passwordinterpreter']['value']),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-a'),
			      'name'	=> 'ipbxapplication[authenticate][a]',
			      'labelid'	=> 'ipbxapplication-authenticate-a',
			      'default'	=> $apparg_authenticate['a']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-j'),
			      'name'	=> 'ipbxapplication[authenticate][j]',
			      'labelid'	=> 'ipbxapplication-authenticate-j',
			      'default'	=> $apparg_authenticate['j']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_authenticate-r'),
			      'name'	=> 'ipbxapplication[authenticate][r]',
			      'labelid'	=> 'ipbxapplication-authenticate-r',
			      'default'	=> $apparg_authenticate['r']['default'])),

	$form->button(array('name'	=> 'ipbxapplication-authenticate',
			    'id'	=> 'it-add-ipbxapplication-authenticate',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_authenticate();"');

?>
</div>
