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
<div id="fd-ipbxapplication-waitexten" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_waitexten = $this->get_var('apparg_waitexten');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_waitexten-seconds'),
			  'name'	=> 'ipbxapplication[waitexten][seconds]',
			  'labelid'	=> 'ipbxapplication-waitexten-seconds',
			  'size'	=> 10,
			  'default'	=> $apparg_waitexten['seconds']['default'])),

	$form->checkbox(array('desc'	=> $this->bbf('fm_ipbxapplication_waitexten-m'),
			      'name'	=> 'ipbxapplication[waitexten][m]',
			      'labelid'	=> 'ipbxapplication-waitexten-m',
			      'default'	=> $apparg_waitexten['m']['default']));

if(($moh_list = $this->get_var('moh_list')) !== false):
	echo $form->select(array('desc'		=> $this->bbf('fm_ipbxapplication_waitexten-musiconhold'),
				 'name'		=> 'ipbxapplication[waitexten][musiconhold]',
				 'labelid'	=> 'ipbxapplication-waitexten-musiconhold',
				 'empty'	=> true,
				 'key'		=> 'category',
				 'default'	=> $apparg_waitexten['musiconhold']['default']),
			   $moh_list);
endif;

echo $form->button(array('name'		=> 'add-ipbxapplication-waitexten',
			 'id'		=> 'it-add-ipbxapplication-waitexten',
			 'value'	=> $this->bbf('fm_bt-add')),
		   'onclick="xivo_ast_application_waitexten();"');

?>
</div>
