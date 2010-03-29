<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

$form	 = &$this->get_module('form');
$dhtml 	 = &$this->get_module('dhtml');

$element = $this->get_var('element');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

?>

<div class="b-infos b-form">
<h3 class="sb-top xspan">
	<span class="span-left">&nbsp;</span>
	<span class="span-center"><?=$this->bbf('title_content_name');?></span>
	<span class="span-right">&nbsp;</span>
</h3>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<div id="sb-part-first">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1)),

		$form->text(array('desc'	=> $this->bbf('fm_mydomain'),
				  'name'	=> 'xivo-smtp-mydomain',
				  'labelid'	=> 'mydomain',
				  'size'	=> 30,
				  'default'	=> $element['mail']['mydomain']['default'],
				  'value'	=> $this->get_var('info','xivo.smtp.mydomain'),
				  'error'	=> $this->bbf_args('error',
						   $this->get_var('error', 'xivo-smtp-mydomain')) )),

		$form->text(array('desc'	=> $this->bbf('fm_origin'),
				  'name'	=> 'xivo-smtp-origin',
				  'labelid'	=> 'origin',
				  'size'	=> 30,
				  'default'	=> $element['mail']['origin']['default'],
				  'value'	=> $this->get_var('info','xivo.smtp.origin'),
				  'error'	=> $this->bbf_args('error',
						   $this->get_var('error', 'xivo-smtp-origin')) )),

		$form->text(array('desc'	=> $this->bbf('fm_relayhost'),
				  'name'	=> 'xivo-smtp-relayhost',
				  'labelid'	=> 'relayhost',
				  'size'	=> 30,
				  'default'	=> $element['mail']['relayhost']['default'],
				  'value'	=> $this->get_var('info','xivo.smtp.relayhost'),
				  'error'	=> $this->bbf_args('error',
						   $this->get_var('error', 'xivo-smtp-relayhost')) )),

		$form->text(array('desc'	=> $this->bbf('fm_fallback_relayhost'),
				  'name'	=> 'xivo-smtp-fallback_relayhost',
				  'labelid'	=> 'fallback_relayhost',
				  'size'	=> 30,
				  'default'	=> $element['mail']['fallback_relayhost']['default'],
				  'value'	=> $this->get_var('info','xivo.smtp.fallback_relayhost'),
				  'error'	=> $this->bbf_args('error',
						   $this->get_var('error', 'xivo-smtp-fallback_relayhost')) ));
?>
	<div class="fm-paragraph fm-description">
		<p>
			<label id="lb-description" for="it-description"><?=$this->bbf('fm_canonical');?></label>
		</p>
		<?=$form->textarea(array('paragraph'	=> false,
					 'label'	=> false,
					 'name'		=> 'xivo-smtp-canonical',
					 'id'		=> 'it-canonical',
					 'cols'		=> 60,
					 'rows'		=> 5,
					 'default'	=> $element['mail']['canonical']['default']),
				   $this->get_var('info','xivo.smtp.canonical'));?>
	</div>
</div>
<?php

echo	$form->submit(array('name'	=> 'submit',
			    'id'	=> 'it-submit',
			    'value'	=> $this->bbf('fm_bt-save')));

?>
</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
