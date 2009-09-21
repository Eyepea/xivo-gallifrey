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
<div id="fd-ipbxapplication-noop" class="b-nodisplay">
<?php

$form = &$this->get_module('form');
$apparg_noop = $this->get_var('apparg_noop');

echo	$form->text(array('desc'	=> $this->bbf('fm_ipbxapplication_noop-data'),
			  'name'	=> 'ipbxapplication[noop][data]',
			  'labelid'	=> 'ipbxapplication-noop-data',
			  'size'	=> 15,
			  'default'	=> $apparg_noop['data']['default'])),

	$form->button(array('name'	=> 'add-ipbxapplication-noop',
			    'id'	=> 'it-add-ipbxapplication-noop',
			    'value'	=> $this->bbf('fm_bt-add')),
		      'onclick="xivo_ast_application_noop();"');

?>
</div>
