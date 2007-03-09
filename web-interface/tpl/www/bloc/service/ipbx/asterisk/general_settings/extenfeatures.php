<?php
	$form = &$this->get_module('form');
	$element = $this->vars('element');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?php echo $this->bbf('fm_vm_propertiesvm'); ?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<?=$form->text(array('desc' => $this->bbf('fm_vm_port'),'name' => 'name','id' => 'it-name','size' => 25,'value' => ''),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->text(array('desc' => $this->bbf('fm_vm_bind'),'name' => 'tel','id' => 'it-tel','size' => 25,'value' => ''),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->text(array('desc' => $this->bbf('fm_vm_chan_lang'),'name' => 'password','id' => 'it-password','size' => 25,'value' => ''),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->text(array('desc' => $this->bbf('fm_vm_realm'),'name' => 'mailbox','id' => 'it-mailbox','size' => 25,'value' => ''),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_srvlookup'),'name' => 'protocol','id' => 'it-protocol','value' => '','key' => true),$this->vars('protocol'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->text(array('desc' => $this->bbf('fm_vm_maxexpirey'),'name' => 'maxexpirey','id' => 'it-maxexpirey','size' => 25,'value' => ''),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_defaultexpirey'),'name' => 'defaultexpirey','id' => 'it-defaultexpirey','value' => '','key' => true),$this->vars('protocol'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->text(array('desc' => $this->bbf('fm_vm_useragent'),'name' => 'useragent','id' => 'it-useragent','value' => '','key' => true),$this->vars('useragent'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_nat'),'name' => 'nat','id' => 'it-nat','value' => '','key' => true),$this->vars('nat'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_acceptnotauthcall'),'name' => 'acceptnotauthcall','id' => 'it-acceptnotauthcall','value' => '','key' => true),$this->vars('acceptnotauthcall'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_tos'),'name' => 'tos','id' => 'it-tos','value' => '','key' => true),$this->vars('tos'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->text(array('desc' => $this->bbf('fm_vm_externip'),'name' => 'externip','id' => 'it-externip','value' => '','key' => true),$this->vars('externip'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_context'),'name' => 'context','id' => 'it-context','value' => '','key' => true),$this->vars('context'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_moh'),'name' => 'moh','id' => 'it-moh','value' => '','key' => true),$this->vars('moh'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->text(array('desc' => $this->bbf('fm_vm_checkmwi'),'name' => 'checkmwi','id' => 'it-checkmwi','value' => '','key' => true),$this->vars('checkmwi'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->slt(array('desc' => $this->bbf('fm_vm_defaultcodec'),'name' => 'defaultcodec','id' => 'it-defaultcodec','value' => '','key' => true),$this->vars('defaultcodec'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
