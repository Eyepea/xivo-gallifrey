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
<div id="fd-ipbxapplication-goto" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_goto = $this->get_var('apparg_goto');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_goto-context'),
			  'name'	=> 'ipbxapplication[goto][context]',
			  'labelid'	=> 'ipbxapplication-goto-context',
			  'size'	=> 15,
			  'default'	=> $apparg_goto['context']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_goto-exten'),
			  'name'	=> 'ipbxapplication[goto][exten]',
			  'labelid'	=> 'ipbxapplication-goto-exten',
			  'size'	=> 15,
			  'default'	=> $apparg_goto['exten']['default'])),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_goto-priority'),
			  'name'	=> 'ipbxapplication[goto][priority]',
			  'labelid'	=> 'ipbxapplication-goto-priority',
			  'size'	=> 15,
			  'default'	=> $apparg_goto['priority']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-goto',
			    'id'	=> 'it-add-ipbxapplication-goto',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_goto();"');

?>
</div>
