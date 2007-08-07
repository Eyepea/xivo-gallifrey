<?php
	$form = &$this->get_module('form');
	$element = $this->vars('element');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?php echo $this->bbf('fm_md_properties'); ?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<?=$form->select(array('desc' => $this->bbf('fm_md_pbx_ael'),'name' => 'pbx_ael.so','id' => 'it-pbx_ael','value' => '','key' => true),$this->vars('pbx_ael'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('desc' => $this->bbf('fm_md_pbx_ael'),'name' => 'pbx_ael.so','id' => 'it-pbx_ael','value' => '','key' => true),$this->vars('pbx_ael'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('desc' => $this->bbf('fm_md_pbx_ael'),'name' => 'pbx_ael.so','id' => 'it-pbx_ael','value' => '','key' => true),$this->vars('pbx_ael'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('desc' => $this->bbf('fm_md_pbx_ael'),'name' => 'pbx_ael.so','id' => 'it-pbx_ael','value' => '','key' => true),$this->vars('pbx_ael'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('desc' => $this->bbf('fm_md_pbx_ael'),'name' => 'pbx_ael.so','id' => 'it-pbx_ael','value' => '','key' => true),$this->vars('pbx_ael'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('desc' => $this->bbf('fm_md_pbx_ael'),'name' => 'pbx_ael.so','id' => 'it-pbx_ael','value' => '','key' => true),$this->vars('pbx_ael'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
<?=$form->select(array('desc' => $this->bbf('fm_md_pbx_ael'),'name' => 'pbx_ael.so','id' => 'it-pbx_ael','value' => '','key' => true),$this->vars('pbx_ael'),'onfocus="this.className=\'it-mfocus\'" onblur="this.className=\'it-mblur\';"');?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
