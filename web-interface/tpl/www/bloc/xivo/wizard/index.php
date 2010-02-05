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

$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$step = $this->get_var('step');

?>
<form action="#" method="post" enctype="multipart/form-data" accept-charset="utf-8">
<div id="xivo-wizard-step-<?=$step?>" class="b-infos">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">
			<?=$this->bbf('title_content_name',$step);?>
		</span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-snav">
		<span class="span-left">
<?php
	if((bool) $this->get_var('can_previous_step') === true):
		echo $form->button(array('name'		=> 'previous',
					 'value'	=> $this->bbf('fm_bt-previous'),
					 'id'		=> 'it-previous',
					 'paragraph'	=> false));
	else:
		echo '&nbsp;';
	endif;
?>
		</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">
<?php
	if((bool) $this->get_var('can_next_step') === true):
		echo $form->submit(array('name'		=> 'next',
					 'value'	=> $this->bbf('fm_bt-next'),
					 'id'		=> 'it-next',
					 'paragraph'	=> false));
	else:
		echo '&nbsp;';
	endif;
?>
		</span>
	</div>
	<div class="sb-content">
<?php

		echo	$form->hidden(array('name'	=> 'fm_send',
					    'value'	=> 1)),

			$form->hidden(array('name'	=> 'step',
					    'value'	=> $step)),
	
			$form->hidden(array('name'	=> 'verify',
					    'value'	=> 0)),

			$form->hidden(array('name'	=> 'refresh',
					    'value'	=> 0)),

			$form->hidden(array('name'	=> 'hl',
					    'value'	=> DWHO_I18N_BABELFISH_LANGUAGE));
?>
		<div class="clearboth"></div>
<?php
		$this->file_include('bloc/xivo/wizard/'.$step);
?>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
</form>
<script type="text/javascript">
dwho.dom.set_onload(function ()
{
	dwho.dom.add_cssclass(dwho_eid('mn-wizard--<?=$dhtml->escape($step);?>'),
			      'mn-active');
});
</script>
