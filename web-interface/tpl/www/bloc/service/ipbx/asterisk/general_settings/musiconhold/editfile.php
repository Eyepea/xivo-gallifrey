<?php
	$form = &$this->get_module('form');
	$element = $this->vars('element');
	$info = $this->vars('info');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>
<?=$form->hidden(array('name' => 'act','value' => 'editfile'));?>
<?=$form->hidden(array('name' => 'id','value' => $this->vars('id')));?>
<?=$form->hidden(array('name' => 'cat','value' => $this->vars('cat')));?>

<?=$form->text(array('desc' => $this->bbf('fm_filename'),'name' => 'filename','labelid' => 'filename','size' => 15,'value' => $info['filename']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_category'),'name' => 'category','labelid' => 'it-category','key' => true,'overkey' => 'category','value' => $info['category']),$this->vars('list_cats'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
