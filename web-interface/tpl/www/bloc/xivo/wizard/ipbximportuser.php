<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$error = $this->get_var('error','ipbximportuser');

echo	$form->hidden(array('name'	=> 'max_file_size',
			    'value'	=> $this->get_var('max_file_size')));

if(dwho_issa('lines',$error) === true
&& dwho_issa('total',$error) === true
&& isset($error['total']['error']) === true
&& $error['total']['error'] > 0):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('dwho.dom.set_onload(xivo_wizard_ipbximportuser_error,
					      \''.$dhtml->escape($this->bbf('ipbximportuser_view_error',
									    $error['total']['error'])).'\');');
?>
<div class="b-nodisplay">
<dl id="ipbximportuser-lines-status">
	<dt><?=$this->bbf('ipbximportuser_lines_status');?></dt>
<?php
	foreach($error['lines'] as $line => $status):
		echo	'<dd class="ipbximportuser-line-',$status,'">',
			$this->bbf('ipbximportuser_lines_status',
				   array('line' => $line,
					 $status)),
			'</dd>';
	endforeach;
?>
</dl>
</div>
<?php
endif;
?>
<div id="ipbximportuser-instruction">
	<?=nl2br($this->bbf('ipbximportuser_instruction'));?>
</div>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-example" for="it-example"><?=$this->bbf('fm_example');?></label>
	</p>
<?php
echo	$form->textarea(array('paragraph'	=> false,
			      'label'		=> false,
			      'notag'		=> false,
			      'name'		=> 'example',
			      'id'		=> 'it-example',
			      'cols'		=> 60,
			      'rows'		=> 4,
			      'readonly'	=> true),
			$this->bbf('fm_example-content'));
?>
</div>
<div class="fm-paragraph fm-desc-inline">
<?php
echo	$form->file(array('paragraph'	=> false,
			  'desc'	=> $this->bbf('fm_import'),
			  'name'	=> 'import',
			  'labelid'	=> 'import',
			  'size'	=> 15,
			  'error'	=> $this->bbf_args('error_fm_import',
							   $this->get_var('error','ipbximportuser','file'))));
?>
</div>
