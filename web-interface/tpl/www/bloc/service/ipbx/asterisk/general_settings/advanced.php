<?php
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$element = $this->vars('element');
	$error = $this->vars('error');

	if($this->vars('fm_save') === true):
		$dhtml->write_js('xivo_form_success(\''.xivo_stript($this->bbf('fm_success-save')).'\');');
	endif;

	$error_js = array();
	$error_nb = count($error['generalagents']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-generalagents-'.$error['generalagents'][$i].'\'] = true;';
		$element['generalagents'][$error['generalagents'][$i]]['default'] = '';
	endfor;

	$error_nb = count($error['generalqueue']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-generalqueue-'.$error['generalqueue'][$i].'\'] = true;';
		$element['generalqueue'][$error['generalqueue'][$i]]['default'] = '';
	endfor;

	$error_nb = count($error['generalmeetme']);

	for($i = 0;$i < $error_nb;$i++):
		$error_js[] = 'xivo_fm_error[\'it-generalmeetme-'.$error['generalmeetme'][$i].'\'] = true;';
		$element['generalmeetme'][$error['generalmeetme'][$i]]['default'] = '';
	endfor;

	if($error_nb > 0)
		$dhtml->write_js($error_js);
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>

<div class="sb-smenu">
	<ul>
		<li id="smenu-tab-1" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-first');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_agents');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-2" class="moo" onclick="xivo_smenu_click(this,'moc','sb-part-queue');" onmouseout="xivo_smenu_out(this,'moo');" onmouseover="xivo_smenu_over(this,'mov');">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_queues');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
		<li id="smenu-tab-3" class="moo-last" onclick="xivo_smenu_click(this,'moc','sb-part-last',1);" onmouseout="xivo_smenu_out(this,'moo',1);" onmouseover="xivo_smenu_over(this,'mov',1);">
			<div class="tab"><span class="span-center"><a href="#" onclick="return(false);"><?=$this->bbf('smenu_meetme');?></a></span></div><span class="span-right">&nbsp;</span>
		</li>
	</ul>
</div>

	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<div id="sb-part-first">

<?=$form->checkbox(array('desc' => $this->bbf('fm_generalagents_persistentagents'),'name' => 'generalagents[persistentagents]','labelid' => 'generalagents-persistentagents','default' => $element['generalagents']['persistentagents']['default'],'checked' => $this->varra('generalagents',array('persistentagents','var_val'))));?>

</div>

<div id="sb-part-queue" class="b-nodisplay">

<?=$form->checkbox(array('desc' => $this->bbf('fm_generalqueue_persistentmembers'),'name' => 'generalqueue[persistentmembers]','labelid' => 'generalqueue-persistentmembers','default' => $element['generalqueue']['persistentmembers']['default'],'checked' => $this->varra('generalqueue',array('persistentmembers','var_val'))));?>

</div>

<div id="sb-part-last" class="b-nodisplay">

<?=$form->select(array('desc' => $this->bbf('fm_generalmeetme_audiobuffer'),'name' => 'generalmeetme[audiobuffer]','labelid' => 'generalmeetme-audiobuffer','key' => false,'default' => $element['generalmeetme']['audiobuffer']['default'],'value' => $this->varra('generalmeetme',array('audiobuffer','var_val'))),$element['generalmeetme']['audiobuffer']['value']);?>

</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
