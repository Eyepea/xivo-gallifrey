<?php

$form = &$this->get_module('form');
$dhtml = &$this->get_module('dhtml');

$element = $this->get_var('element');
$error = $this->get_var('error');

if($this->get_var('fm_save') === true):
	$dhtml->write_js('xivo_form_success(\''.$dhtml->escape($this->bbf('fm_success-save')).'\');');
endif;

$error_js = array();
$error_nb = count($error['generalmeetme']);

for($i = 0;$i < $error_nb;$i++):
	$error_js[] = 'xivo_fm_error[\'it-generalmeetme-'.$error['generalmeetme'][$i].'\'] = true;';
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
		<li id="smenu-tab-1"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-first');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_userinternal');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-agent');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_agents');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3"
		    class="moo"
		    onclick="xivo_smenu_click(this,'moc','sb-part-queue');"
		    onmouseout="xivo_smenu_out(this,'moo');"
		    onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_queues');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-4"
		    class="moo-last"
		    onclick="xivo_smenu_click(this,'moc','sb-part-last',1);"
		    onmouseout="xivo_smenu_out(this,'moo',1);"
		    onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab">
				<span class="span-center">
					<a href="#" onclick="return(false);"><?=$this->bbf('smenu_meetme');?></a>
				</span>
			</div>
			<span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?php
	echo	$form->hidden(array('name'	=> XIVO_SESS_NAME,
				    'value'	=> XIVO_SESS_ID)),
		$form->hidden(array('name'	=> 'fm_send',
				    'value'	=> 1));
?>

<div id="sb-part-first">
	<?=$form->checkbox(array('desc'		=> $this->bbf('fm_userinternal_guest'),
				 'name'		=> 'userinternal[guest]',
				 'labelid'	=> 'userinternal-guest',
				 'checked'	=> ($this->get_varra('userinternal',
				 				     array('guest','ufeatures','commented')) === false)));?>
</div>

<div id="sb-part-agent" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_generalagents_persistentagents'),
				      'name'	=> 'generalagents[persistentagents]',
				      'labelid'	=> 'generalagents-persistentagents',
				      'default'	=> $element['generalagents']['persistentagents']['default'],
				      'checked'	=> $this->get_varra('generalagents',array('persistentagents','var_val')))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalagents_multiplelogin'),
				      'name'	=> 'generalagents[multiplelogin]',
				      'labelid'	=> 'generalagents-multiplelogin',
				      'default'	=> $element['generalagents']['multiplelogin']['default'],
				      'checked'	=> $this->get_varra('generalagents',array('multiplelogin','var_val'))));
?>
</div>

<div id="sb-part-queue" class="b-nodisplay">
<?php
	echo	$form->checkbox(array('desc'	=> $this->bbf('fm_generalqueue_persistentmembers'),
				      'name'	=> 'generalqueue[persistentmembers]',
				      'labelid'	=> 'generalqueue-persistentmembers',
				      'default'	=> $element['generalqueue']['persistentmembers']['default'],
				      'checked'	=> $this->get_varra('generalqueue',array('persistentmembers','var_val')))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalqueue_autofill'),
				      'name'	=> 'generalqueue[autofill]',
				      'labelid'	=> 'generalqueue-autofill',
				      'default'	=> $element['generalqueue']['autofill']['default'],
				      'checked'	=> $this->get_varra('generalqueue',array('autofill','var_val')))),

		$form->checkbox(array('desc'	=> $this->bbf('fm_generalqueue_monitor-type'),
				      'name'	=> 'generalqueue[monitor-type]',
				      'labelid'	=> 'generalqueue-monitor-type',
				      'default'	=> $element['generalqueue']['monitor-type']['default'],
				      'checked'	=> $this->get_varra('generalqueue',array('monitor-type','var_val'))));
?>
</div>

<div id="sb-part-last" class="b-nodisplay">
	<?=$form->select(array('desc'		=> $this->bbf('fm_generalmeetme_audiobuffers'),
			       'name'		=> 'generalmeetme[audiobuffers]',
			       'labelid'	=> 'generalmeetme-audiobuffers',
			       'key'		=> false,
			       'default'	=> $element['generalmeetme']['audiobuffers']['default'],
			       'value'		=> $this->get_varra('generalmeetme',array('audiobuffers','var_val'))),
			 $element['generalmeetme']['audiobuffers']['value']);?>
</div>
<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>

	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
