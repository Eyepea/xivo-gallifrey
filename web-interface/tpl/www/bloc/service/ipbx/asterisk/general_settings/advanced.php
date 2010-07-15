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
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$error = $this->get_var('error');

if(($fm_save = $this->get_var('fm_save')) === true):
	$dhtml->write_js('xivo_form_result(true,\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
elseif($fm_save === false):
	$dhtml->write_js('xivo_form_result(false,\''.$dhtml->escape($this->bbf('fm_error-save')).'\');');
endif;

$error_js = array();
$error_nb = count($error['generalmeetme']);

for($i = 0;$i < $error_nb;$i++):
	$error_js[] = 'dwho.form.error[\'it-generalmeetme-'.$error['generalmeetme'][$i].'\'] = true;';
endfor;

if(isset($error_js[0]) === true)
	$dhtml->write_js($error_js);

?>
<div class="b-infos b-form">
<h3 class="sb-top xspan">
	<span class="span-left">&nbsp;</span>
	<span class="span-center"><?=$this->bbf('title_content_name');?></span>
	<span class="span-right">&nbsp;</span>
</h3>
<div class="sb-smenu">
	<ul>
		<li id="dwsm-tab-1"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-first');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_userinternal');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-2"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-agent');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_agents');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-3"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-queue');"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_queues');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-4"
		    class="dwsm-blur"
		    onclick="dwho_submenu.select(this,'sb-part-meetme',1);"
		    onmouseout="dwho_submenu.blur(this);"
		    onmouseover="dwho_submenu.focus(this);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_meetme');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="dwsm-tab-5"
		    class="dwsm-blur-last"
		    onclick="dwho_submenu.select(this,'sb-part-last',1);"
		    onmouseout="dwho_submenu.blur(this,1);"
		    onmouseover="dwho_submenu.focus(this,1);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_timezone');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?php
	echo	$form->hidden(array('name'	=> DWHO_SESS_NAME,
				    'value'	=> DWHO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">
	<?=$form->checkbox(array('desc'		=> $this->bbf('fm_userinternal_guest'),
				 'name'		=> 'userinternal[guest]',
				 'labelid'	=> 'userinternal-guest',
				 'help'	=> $this->bbf('hlp_fm_userinternal_guest'),
				 'checked'	=> ($this->get_var('userinternal','guest','userfeatures','commented'))?'no':'yes'));?>
</div>

<div id="sb-part-agent" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_generalagents_persistentagents'),
				      'name'	=> 'generalagents[persistentagents]',
				      'labelid'	=> 'generalagents-persistentagents',
				      'default'	=> $element['generalagents']['persistentagents']['default'],
				      'help'	=> $this->bbf('hlp_fm_generalagents_persistentagents'),
				      'checked'	=> $this->get_var('generalagents','persistentagents','var_val'))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalagents_multiplelogin'),
				      'name'	=> 'generalagents[multiplelogin]',
				      'labelid'	=> 'generalagents-multiplelogin',
				      'default'	=> $element['generalagents']['multiplelogin']['default'],
				      'help'	=> $this->bbf('hlp_fm_generalagents_multiplelogin'),
				      'checked'	=> $this->get_var('generalagents','multiplelogin','var_val'))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalagents_recordagentcalls'),
				      'name'	=> 'generalagents[recordagentcalls]',
				      'labelid'	=> 'generalagents-recordagentcalls',
				      'default'	=> $element['generalagents']['recordagentcalls']['default'],
				      'help'	=> $this->bbf('hlp_fm_generalagents_recordagentcalls'),
				      'checked'	=> $this->get_var('generalagents','recordagentcalls','var_val'))),

		$form->select(array('desc'	=> $this->bbf('fm_generalagents_recordformat'),
				    'name'	=> 'generalagents[recordformat]',
				    'labelid'	=> 'generalagents-recordformat',
				    'key'	=> false,
				    'bbf'	=> 'ast_format_name_info',
				    'bbfopt'	=> array('argmode' => 'paramvalue'),
				    'default'	=> $element['generalagents']['recordformat']['default'],
				    'selected'	=> $info['generalagents']['recordformat']),
			      $element['generalagents']['recordformat']['value']);

?>
</div>

<div id="sb-part-queue" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_generalqueues_persistentmembers'),
				      'name'	=> 'generalqueues[persistentmembers]',
				      'labelid'	=> 'generalqueues-persistentmembers',
				      'default'	=> $element['generalqueues']['persistentmembers']['default'],
				      'help'	=> $this->bbf('hlp_fm_generalqueues_persistentmembers'),
				      'checked'	=> $this->get_var('generalqueues','persistentmembers','var_val'))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalqueues_autofill'),
				      'name'	=> 'generalqueues[autofill]',
				      'labelid'	=> 'generalqueues-autofill',
				      'default'	=> $element['generalqueues']['autofill']['default'],
				      'help'	=> $this->bbf('hlp_fm_generalqueues_autofill'),
				      'checked'	=> $this->get_var('generalqueues','autofill','var_val'))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalqueues_monitor-type'),
				      'name'	=> 'generalqueues[monitor-type]',
				      'labelid'	=> 'generalqueues-monitor-type',
				      'default'	=> $element['generalqueues']['monitor-type']['default'],
				      'help'	=> $this->bbf('hlp_fm_generalqueues_monitor-type'),
				      'checked'	=> $this->get_var('generalqueues','monitor-type','var_val')));
?>
</div>

<div id="sb-part-meetme" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_generalmeetme_audiobuffers'),
				    'name'	=> 'generalmeetme[audiobuffers]',
				    'labelid'	=> 'generalmeetme-audiobuffers',
				    'key'	=> false,
				    'default'	=> $element['generalmeetme']['audiobuffers']['default'],
				    'help'	=> $this->bbf('hlp_fm_generalmeetme_audiobuffers'),
				    'selected'	=> $this->get_var('generalmeetme','audiobuffers','var_val')),
			      $element['generalmeetme']['audiobuffers']['value']);
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
<?php
	echo	$form->select(array('desc'	=> $this->bbf('fm_general_timezone'),
				    'name'     => 'general[timezone]',
				    'labelid'  => 'general-timezone',
				    'key'      => false,
				    'default'  => $element['general']['timezone']['default'],
				    'help'     => $this->bbf('hlp_fm_general_timezone'),
				    'selected' => $this->get_var('general','timezone')),
			      $element['general']);
?>
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
