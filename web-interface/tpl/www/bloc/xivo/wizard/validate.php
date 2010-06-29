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
$dhtml = &$this->get_module('dhtml');

$info = $this->get_var('info');
$netiface = $this->get_var('info','netiface');
$resolvconf = $this->get_var('info','resolvconf');

$error_ipbximportuser = $this->get_var('error','ipbximportuser');

if(dwho_issa('lines',$error_ipbximportuser) === true
&& dwho_issa('total',$error_ipbximportuser) === true
&& isset($error_ipbximportuser['total']['error']) === true
&& $error_ipbximportuser['total']['error'] > 0):
	$dhtml->write_js('dwho.dom.set_onload(xivo_wizard_ipbximportuser_error,
					      \''.$dhtml->escape($this->bbf('ipbximportuser_view_error',
									    $error_ipbximportuser['total']['error'])).'\');');
?>
<div class="b-nodisplay">
	<dl id="ipbximportuser-lines-status">
		<dt><?=$this->bbf('ipbximportuser_lines_status');?></dt>
<?php
	foreach($error_ipbximportuser['lines'] as $line => $status):
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
<div id="validate-instruction">
	<?=nl2br($this->bbf('validate_instruction'));?>
</div>
<fieldset id="fld-general-information">
	<legend><?=$this->bbf('fld-general-information');?></legend>
	<dl>
		<dt><?=$this->bbf('info_language');?></dt>
		<dd><?=dwho_htmlen($this->bbf('language_'.$info['welcome']['language']));?></dd>
		<dt><?=$this->bbf('info_ipbxengine');?></dt>
		<dd><?=dwho_htmlen($this->get_var('ipbxengine','engines',$info['ipbxengine']['engine'],'label'));?></dd>
		<dt><?=$this->bbf('info_dbbackend');?></dt>
		<dd><?=dwho_htmlen($this->get_var('dbbackend',$info['dbconfig']['result']['backend'],'label')),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_dbconfig_backends',
					        $this->get_var('error','dbconfig','backends'))));?></dd>
	</dl>
</fieldset>
<fieldset id="fld-server-information">
	<legend><?=$this->bbf('fld-server-information');?></legend>
	<dl>
		<dt><?=$this->bbf('info_servername');?></dt>
		<dd><?=dwho_htmlen($info['mainconfig']['hostname']),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_mainconfig_hosts',
					        $this->get_var('error','mainconfig','hosts'))));?></dd>
		<dt><?=$this->bbf('info_serverdomain');?></dt>
		<dd><?=dwho_htmlen($info['mainconfig']['domain']);?></dd>
		<dt><?=$this->bbf('info_adminpasswd');?></dt>
		<dd><?=dwho_htmlen($info['mainconfig']['adminpasswd']),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_mainconfig_adminpasswd',
					        $this->get_var('error','mainconfig','adminpasswd'))));?></dd>
<?php
	if(is_array($netiface) === true && empty($netiface) === false):	
?>
		<dt><?=$this->bbf('info_netiface_address');?></dt>
		<dd><?=dwho_htmlen($this->get_var('info','netiface','address')),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_netiface',
					        $this->get_var('error','netiface'))));?></dd>
		<dt><?=$this->bbf('info_netiface_netmask');?></dt>
		<dd><?=dwho_htmlen($this->get_var('info','netiface','netmask'));?></dd>
<?php
		if(dwho_has_len($netiface,'gateway') === true):
?>
		<dt><?=$this->bbf('info_netiface_gateway');?></dt>
		<dd><?=dwho_htmlen($netiface['gateway']);?></dd>
<?php
		endif;
	endif;

	if(is_array($resolvconf) === true && empty($resolvconf) === false):
		if(dwho_has_len($resolvconf,'nameserver1') === true):
?>
		<dt><?=$this->bbf('info_resolvconf_nameserver1');?></dt>
		<dd><?=dwho_htmlen($resolvconf['nameserver1']),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_resolvconf_nameservers',
					        $this->get_var('error','resolvconf','nameservers'))));?></dd>
<?php
		endif;
		if(dwho_has_len($resolvconf,'nameserver2') === true):
?>
		<dt><?=$this->bbf('info_resolvconf_nameserver2');?></dt>
		<dd><?=dwho_htmlen($resolvconf['nameserver2']);?></dd>
<?php
		endif;
	endif;
?>
	</dl>
</fieldset>
<fieldset id="fld-entity">
	<legend><?=$this->bbf('fld-entity');?></legend>
	<dl>
		<dt><?= $this->bbf('entity_name'); ?></dt>
		<dd><?=dwho_htmlen($info['entity']['name']),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_entity',
					        $this->get_var('error','entity'))));?></dd>
		<dt><?= $this->bbf('entity_displayname'); ?></dt>
		<dd><?=dwho_htmlen($info['entity']['displayname']);?></dd>
	</dl>
</fieldset>
<fieldset id="fld-context">
	<legend><?=$this->bbf('fld-context');?></legend>
	<dl>
		<dt><?= $this->bbf('internal_calls'); ?></dt>
		<dd><?=dwho_htmlen($info['context_internal']['context']['displayname'].
				   ' ('.$info['context_internal']['context']['name'].')'),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_context_internal',
					        $this->get_var('error','context_internal'))));?></dd>
		<dt><?= $this->bbf('incoming_calls'); ?></dt>
		<dd><?=dwho_htmlen($info['context_incall']['context']['displayname'].
				   ' ('.$info['context_incall']['context']['name'].')'),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_context_incall',
					        $this->get_var('error','context_incall'))));?></dd>
		<dt>Appels sortants :</dt>
		<dd><?=dwho_htmlen($info['context_outcall']['context']['displayname'].
				   ' ('.$info['context_outcall']['context']['name'].')'),
		       $dhtml->message_error(
				nl2br($this->bbf_args('error_context_outcall',
					        $this->get_var('error','context_outcall'))));?></dd>
	</dl>
</fieldset>
<?php

echo $form->submit(array('name'		=> 'validate',
			 'value'	=> $this->bbf('fm_bt-validate')));

?>
