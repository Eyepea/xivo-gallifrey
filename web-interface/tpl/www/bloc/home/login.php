<?php
	$form = &$this->get_module('form');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">

<form action="#" method="post" accept-charset="utf-8">
<div class="b-field">
<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>

<?=$form->text(array('name' => 'login','id' => 'it-login','size' => 25,'value' => $this->bbf('fm_login')),'onfocus="this.value = this.value == \''.xivo_stript($this->bbf('fm_login')).'\' ? \'\' : this.value; this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->password(array('name' => 'password','id' => 'it-password','size' => 25,'value' => $this->bbf('fm_password')),'onfocus="this.value = this.value == \''.xivo_stript($this->bbf('fm_password')).'\' ? \'\' : this.value; this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->select(array('desc' => $this->bbf('fm_language'),'name' => 'language','id' => 'it-language'),array('fr' => 'Français'));?>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-connection')));?>
</div>
</form>

	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
