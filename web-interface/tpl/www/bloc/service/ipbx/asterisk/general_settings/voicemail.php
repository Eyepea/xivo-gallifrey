<?php
	$form = &$this->get_module('form');
	$element = $this->vars('element');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<?=$form->text(array('desc' => $this->bbf('fm_maxmessage'),'name' => 'maxmessage','id' => 'it-maxmessage','default' => $element['maxmessage']['default'],'value' => $this->varra('info','maxmessage')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_minmessage'),'name' => 'minmessage','id' => 'it-minmessage','default' => $element['minmessage']['default'],'value' => $this->varra('info','minmessage')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_maxsilence'),'name' => 'maxsilence','id' => 'it-maxsilence','default' => $element['maxsilence']['default'],'value' => $this->varra('info','maxsilence')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_review'),'name' => 'review','id' => 'it-review','default' => $element['review']['default'],'checked' => $this->varra('info','review')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_serveremail'),'name' => 'serveremail','id' => 'it-serveremail','default' => $element['serveremail']['default'],'size' => 15,'value' => $this->varra('info','serveremail')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_fromstring'),'name' => 'fromstring','id' => 'it-fromstring','default' => $element['fromstring']['default'],'size' => 15,'value' => $this->varra('info','fromstring')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_maxmsg'),'name' => 'maxmsg','id' => 'it-maxmsg','default' => $element['maxmsg']['default'],'size' => 15,'value' => $this->varra('info','maxmsg')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_emailsubject'),'name' => 'emailsubject','id' => 'it-subject','size' => 25,'value' => $this->varra('info','emailsubject')),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<div id="emailbody" class="fm-field"><p><label id="lb-emailbody" for="it-emailbody"><?=$this->bbf('fm_emailbody')?></label></p>
<?=$form->textarea(array('field' => false,'name' => 'emailbody','id' => 'it-emailbody','cols' => 60,'rows' => 5),$this->varra('info','emailbody'),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>
</div>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>

</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
