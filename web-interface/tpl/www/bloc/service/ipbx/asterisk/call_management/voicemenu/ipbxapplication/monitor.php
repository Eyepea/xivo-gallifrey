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
<div id="fd-ipbxapplication-monitor" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_monitor = $this->get_var('apparg_monitor');

echo	$form->select(array('desc'	=> $this->bbf('fm_ipbxapplication_monitor-fileformat'),
			    'name'	=> 'ipbxapplication[monitor][fileformat]',
			    'labelid'	=> 'ipbxapplication-monitor-fileformat',
			    'key'	=> false,
			    'bbf'	=> array('paramvalue','ast_format_name_info'),
			    'default'	=> $apparg_monitor['fileformat']['default']),
		      $apparg_monitor['fileformat']['value']),

	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_monitor-basename'),
			  'name'	=> 'ipbxapplication[monitor][basename]',
			  'labelid'	=> 'ipbxapplication-monitor-basename',
			  'size'	=> 15,
			  'default'	=> $apparg_monitor['basename']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_monitor-m'),
			      'name'	=> 'ipbxapplication[monitor][m]',
			      'labelid'	=> 'ipbxapplication-monitor-m',
			      'default'	=> $apparg_monitor['m']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_monitor-b'),
			      'name'	=> 'ipbxapplication[monitor][b]',
			      'labelid'	=> 'ipbxapplication-monitor-b',
			      'default'	=> $apparg_monitor['b']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-monitor',
			    'id'	=> 'it-add-ipbxapplication-monitor',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_monitor();"');

?>
</div>
