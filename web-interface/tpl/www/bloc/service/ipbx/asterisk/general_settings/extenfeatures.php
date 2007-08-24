<?php
	$form = &$this->get_module('form');
	$dhtml = &$this->get_module('dhtml');

	$element = $this->vars('element');
	$error = $this->vars('error');

	if($this->vars('fm_save') === true):
		$dhtml->write_js('xivo_form_success(\''.xivo_stript($this->bbf('fm_success-save')).'\');');
	endif;

	$nb = count($error);

	$extenfeatures_css = array();

	for($i = 0;$i < $nb;$i++):
		$extenfeatures_css[] = '#it-'.$error[$i].' {background-color: #ffcbcb};';
	endfor;

	$dhtml->write_css($extenfeatures_css);
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => 1));?>

<?=$form->text(array('desc' => $this->bbf('fm_voicemsg'),'name' => 'voicemsg','id' => 'it-voicemsg','size' => 15,'value' => $this->varra('info','voicemsg')));?>

<?=$form->text(array('desc' => $this->bbf('fm_fwdundoall'),'name' => 'fwdundoall','id' => 'it-fwdundoall','size' => 15,'value' => $this->varra('info','fwdundoall')));?>

<?=$form->text(array('desc' => $this->bbf('fm_fwdundounc'),'name' => 'fwdundounc','labelid' => 'fwdundounc','size' => 15,'value' => $this->varra('info','fwdundounc')));?>

<?=$form->text(array('desc' => $this->bbf('fm_fwdundorna'),'name' => 'fwdundorna','id' => 'it-fwdundorna','size' => 15,'value' => $this->varra('info','fwdundorna')));?>

<?=$form->text(array('desc' => $this->bbf('fm_fwdundobusy'),'name' => 'fwdundobusy','id' => 'it-fwdundobusy','size' => 15,'value' => $this->varra('info','fwdundobusy')));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_fwdunc'),'name' => 'fwdunc','field' => false,'id' => 'it-fwdunc','size' => 15,'value' => $this->varra('info','fwdunc')));?>
<?=$form->select(array('field' => false,'name' => 'list-fwdunc','id' => 'it-list-fwdunc','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-fwdunc\',this.value);"');?>
</div>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_fwdrna'),'name' => 'fwdrna','field' => false,'id' => 'it-fwdrna','size' => 15,'value' => $this->varra('info','fwdrna')));?>
<?=$form->select(array('field' => false,'name' => 'list-fwdrna','id' => 'it-list-fwdrna','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-fwdrna\',this.value);"');?>
</div>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_fwdbusy'),'name' => 'fwdbusy','field' => false,'id' => 'it-fwdbusy','size' => 15,'value' => $this->varra('info','fwdbusy')));?>
<?=$form->select(array('field' => false,'name' => 'list-fwdbusy','id' => 'it-list-fwdbusy','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-fwdbusy\',this.value);"');?>
</div>

<?=$form->text(array('desc' => $this->bbf('fm_recsnd'),'name' => 'recsnd','id' => 'it-recsnd','size' => 15,'value' => $this->varra('info','recsnd')));?>

<?=$form->text(array('desc' => $this->bbf('fm_phonestatus'),'name' => 'phonestatus','id' => 'it-phonestatus','size' => 15,'value' => $this->varra('info','phonestatus')));?>

<?=$form->text(array('desc' => $this->bbf('fm_enablevm'),'name' => 'enablevm','id' => 'it-enablevm','size' => 15,'value' => $this->varra('info','enablevm')));?>

<?=$form->text(array('desc' => $this->bbf('fm_enablednd'),'name' => 'enablednd','id' => 'it-enablednd','size' => 15,'value' => $this->varra('info','enablednd')));?>

<?=$form->text(array('desc' => $this->bbf('fm_incallrec'),'name' => 'incallrec','id' => 'it-incallrec','size' => 15,'value' => $this->varra('info','incallrec')));?>

<?=$form->text(array('desc' => $this->bbf('fm_incallfilter'),'name' => 'incallfilter','id' => 'it-incallfilter','size' => 15,'value' => $this->varra('info','incallfilter')));?>

<div class="fm-field">
<?=$form->text(array('desc' => $this->bbf('fm_pickup'),'name' => 'pickup','field' => false,'id' => 'it-pickup','size' => 15,'value' => $this->varra('info','pickup')));?>
<?=$form->select(array('field' => false,'name' => 'list-pickup','id' => 'it-list-pickup','key' => false,'empty' => true),array('*',range(3,11)),'onchange="xivo_exten_pattern(\'it-pickup\',this.value);"');?>
</div>

	<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
