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
<div id="fd-ipbxapplication-setcallerid" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_setcallerid = $this->get_var('apparg_setcallerid');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_setcallerid-callerid'),
			  'name'	=> 'ipbxapplication[setcallerid][callerid]',
			  'labelid'	=> 'ipbxapplication-setcallerid-callerid',
			  'size'	=> 15,
			  'default'	=> $apparg_setcallerid['callerid']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-setcallerid',
			    'id'	=> 'it-add-ipbxapplication-setcallerid',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_setcallerid();"');

?>
</div>
