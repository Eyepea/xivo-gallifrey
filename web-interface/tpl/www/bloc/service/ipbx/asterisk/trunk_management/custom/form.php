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

$form = &$this->get_module('form');

$info = $this->get_var('info');
$element = $this->get_var('element');
$context_list = $this->get_var('context_list');

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

echo	$form->text(array('desc'	=> $this->bbf('fm_protocol_name'),
			  'name'	=> 'protocol[name]',
			  'labelid'	=> 'protocol-name',
			  'size'	=> 15,
			  'default'	=> $element['protocol']['interface'],
			  'value'	=> $info['protocol']['name'])),

	$form->text(array('desc'	=> $this->bbf('fm_protocol_interface'),
			  'name'	=> 'protocol[interface]',
			  'labelid'	=> 'protocol-interface',
			  'size'	=> 15,
			  'default'	=> $element['protocol']['interface'],
			  'value'	=> $info['protocol']['interface'])),

	$form->text(array('desc'	=> $this->bbf('fm_protocol_intfsuffix'),
			  'name'	=> 'protocol[intfsuffix]',
			  'labelid'	=> 'protocol-intfsuffix',
			  'size'	=> 15,
			  'default'	=> $element['protocol']['intfsuffix'],
			  'value'	=> $info['protocol']['intfsuffix']));

if($context_list !== false):
	echo	$form->select(array('desc'	=> $this->bbf('fm_protocol_context'),
				    'name'	=> 'protocol[context]',
				    'labelid'	=> 'protocol-context',
				    'empty'	=> true,
				    'key'	=> 'identity',
				    'altkey'	=> 'name',
				    'default'	=> $element['protocol']['context']['default'],
				    'selected'	=> $info['protocol']['context']),
			       $context_list);
endif;

?>
<div class="fm-paragraph fm-description">
	<p>
		<label id="lb-trunkfeatures-description" for="it-trunkfeatures-description">
			<?=$this->bbf('fm_trunkfeatures_description');?>
		</label>
	</p>
	<?=$form->textarea(array('paragraph'	=> false,
				 'label'	=> false,
				 'name'		=> 'trunkfeatures[description]',
				 'id'		=> 'it-trunkfeatures-description',
				 'cols'		=> 60,
				 'rows'		=> 5,
				 'default'	=> $element['trunkfeatures']['description']['default']),
			   $info['trunkfeatures']['description']);?>
</div>
