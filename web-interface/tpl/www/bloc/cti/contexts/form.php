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

$yesno = array($this->bbf('no'), $this->bbf('yes'));

if($this->get_var('fm_save') === false):
	$dhtml = &$this->get_module('dhtml');
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;
?>

<div id="sb-part-first">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_contexts_name'),
				  'name'	=> 'contexts-name',
				  'id'		=> false,
				  'label'	=> false,
				  'key'		=> false,
				  'selected'	=> $info['cticontexts']['name']
				  ),
				  $info['displays']['pbxctx']);

	echo	$form->select(array('desc'	=> $this->bbf('fm_contexts_display'),
				  'name'	=> 'contexts-display',
				  'id'		=> false,
				  'label'	=> false,
				  'key'		=> false,
				  'selected'	=> $info['cticontexts']['display']
				  ),
				  $info['displays']['list']);
?>
	<div class="fm-paragraph fm-description">
		<fieldset id="cti-contexts_services">
			<legend><?=$this->bbf('cti-contexts-directories');?></legend>
			<div id="contexts_services" class="fm-paragraph fm-multilist">
				<div class="slt-outlist">
<?php
				echo    $form->select(array('name'  => 'directorieslist',
							'label' => false,
							'id'    => 'it-directorieslist',
							'key'   => 'name',
							'altkey'    => 'id',
							'multiple'  => true,
							'size'  => 5,
							'paragraph' => false),
							$info['directories']['list']);
?>
				</div>
				<div class="inout-list">
					<a href="#"
					onclick="dwho.form.move_selected('it-directorieslist','it-directories');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_inaccess_contexts');?>">
					<?=$url->img_html('img/site/button/arrow-left.gif',
							$this->bbf('bt_inaccess_contexts'),
							'class="bt-inlist" id="bt-inaccess_contexts" border="0"');?></a><br />

					<a href="#"
					onclick="dwho.form.move_selected('it-directories','it-directorieslist');
					return(dwho.dom.free_focus());"
					title="<?=$this->bbf('bt_outaccess_contexts');?>">
					<?=$url->img_html('img/site/button/arrow-right.gif',
							$this->bbf('bt_outaccess_contexts'),
							'class="bt-outlist" id="bt-outaccess_contexts" border="0"');?></a>
				</div>
				<div class="slt-inlist">
<?php
				echo    $form->select(array('name'  => 'directories[]',
						'label' => false,
						'id'    => 'it-directories',
						'key'	=> 'name',
						'altkey'    => 'id',
						'multiple'  => true,
						'size'  => 5,
						'paragraph' => false),
					$info['directories']['slt']);
?>
				</div>
			</div>
		</fieldset>
		<div class="clearboth"></div>
	   <div class="fm-paragraph fm-description">
			<p>
				<label id="lb-contexts-description" for="it-contexts-description"><?=$this->bbf('fm_contexts_description');?></label>
			</p>
			<?=$form->textarea(array('paragraph'    => false,
						 'label'    => false,
						 'name'     => 'contexts[description]',
						 'id'       => 'it-contexts-description',
						 'cols'     => 60,
						 'rows'     => 5,
						 'default'  => $element['cticontexts']['description']['default']),
					   $info['cticontexts']['description']);?>
		</div>

	</div>
</div>

