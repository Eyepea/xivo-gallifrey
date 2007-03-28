<?php
	$form = &$this->get_module('form');
	$info = $this->vars('info');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'act','value' => 'edit'));?>
<?=$form->hidden(array('name' => 'id','value' => $info['name']));?>

<p class="fm-field txt-center width-auto"><?=$this->bbf('fm_filename')?> <?=$info['name']?></p>

<div class="fm-field txt-center width-auto"><p><label id="lb-content" for="it-content"><?=$this->bbf('fm_content')?></label></p>
<?=$form->textarea(array('field' => false,'name' => 'content','label' => false,'notag' => false,'id' => 'it-content','cols' => 90,'rows' => 30),$info['content'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>

