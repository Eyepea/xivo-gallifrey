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
$dhtml = &$this->get_module('dhtml');

?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center"><?=$this->bbf('title_content_name');?></span>
		<span class="span-right">&nbsp;</span>
	</h3>
	<div class="sb-content">
		<form action="#" method="post" accept-charset="utf-8">
			<div class="b-field">
<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),

		$form->text(array('name'	=> 'login',
				  'id'		=> 'it-login',
				  'size'	=> 20,
				  'value'	=> $this->bbf('fm_login'))),

		$form->password(array('name'	=> 'password',
				      'id'	=> 'it-password',
				      'size'	=> 20,
				      'value'	=> $this->bbf('fm_password'))),

		$form->select(array('desc'	=> $this->bbf('fm_language'),
				    'name'	=> 'language',
				    'id'	=> 'it-language',
				    'selected'	=> DWHO_I18N_BABELFISH_LANGUAGE),
			      $this->get_var('language')),

		$form->submit(array('name'	=> 'submit',
				    'id'	=> 'it-submit',
				    'value'	=> $this->bbf('fm_bt-connection')));
?>
			</div>
		</form>
	</div>
	<div class="sb-foot xspan">
		<span class="span-left">&nbsp;</span>
		<span class="span-center">&nbsp;</span>
		<span class="span-right">&nbsp;</span>
	</div>
</div>
<script type="text/javascript">
dwho.dom.set_onload(function ()
{
	dwho.form.set_events_text_helper('it-login');
	dwho.form.set_events_text_helper('it-password');
});
</script>
