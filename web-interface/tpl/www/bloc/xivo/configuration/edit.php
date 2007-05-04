<?php
	$form = &$this->get_module('form');
	$info = $this->vars('info');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'cat','value' => 'edit'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'id','value' => $info['id']));?>
<p class="fm-field txt-left"><span class="fm-desc"><?=$this->bbf('fm_login')?></span>&nbsp;<?=$info['login']?></p>
<p class="fm-field txt-left"><span class="fm-desc"><?=$this->bbf('fm_type')?></span>&nbsp;<?=$info['meta']?></p>
<?=$form->text(array('desc' => $this->bbf('fm_password'),'name' => 'passwd','labelid' => 'passwd','size' => 25,'value' => $info['passwd']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
<?php
	if(xivo_user::chk_authorize('admin',$info['meta']) === true):
		echo $form->checkbox(array('desc' => $this->bbf('fm_valid'),'name' => 'valid','labelid' => 'valid','default' => true,'checked' => $info['valid']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');
	endif;
?>
<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
