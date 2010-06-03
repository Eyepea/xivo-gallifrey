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
$url = &$this->get_module('url');

$element = $this->get_var('element');
$info = $this->get_var('info');

$presence = $this->get_var('presences');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div id="sb-part-first">
<?php
	echo	$form->text(array('desc'	=> $this->bbf('fm_presences_name'),
				  'name'	=> 'presences[name]',
				  'labelid'	=> 'presences-name',
				  'size'	=> 15,
				  'default'	=> $element['presences']['name']['default'],
				  'value'	=> $info['presences']['name']));

?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-presences-description" for="it-presences-description"><?=$this->bbf('fm_presences_description');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'presences[description]',
					 'id'		=> 'it-presences-description',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['presences']['description']['default']),
				   $info['presences']['description']);?>
	</div>
</div>

