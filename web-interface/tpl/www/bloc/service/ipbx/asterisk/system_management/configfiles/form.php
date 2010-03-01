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

$form = &$this->get_module('form');

$info = $this->get_var('info');
$element = $this->get_var('element');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;
?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_filename'),
				  'name'	=> 'configfile[filename]',
				  'labelid'	=> 'configfile-filename',
				  'size'	=> 15,
				  'default'	=> $element['configfile']['filename']['default'],
				  'value'	=> $info['configfile']['filename'],
			  	  'error'	=> $this->bbf_args('error_fm_filename', 
					$this->get_var('error', 'filename'))));

?>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-configfile-description" for="it-configfile-description">
			<?=$this->bbf('fm_content');?>
		</label>
	</p>
<?php

	echo	$form->textarea(array('paragraph'	=> false,
				      'label'		=> false,
			 	      'name'		=> 'configfile[content]',
			 	      'id'		=> 'it-context-description',
			 	      'cols'		=> 90,
			 	      'rows'		=> 30,
			 	      'default'		=> $element['configfile']['content']['default'],
				      'error'		=> $this->bbf_args('error_fm_content', 
					$this->get_var('error', 'content'))),
		   		$info['configfile']['content']);
?>
</div>
</div>
