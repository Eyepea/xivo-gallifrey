<?php
	$form = &$this->get_module('form');

	$mfeatures_elt = $this->vars('mfeatures_elt');
?>
<div class="b-infos b-form">
	<h3 class="sb-top xspan"><span class="span-left">&nbsp;</span><span class="span-center"><?=$this->bbf('title_content_name');?></span><span class="span-right">&nbsp;</span></h3>
	<div class="sb-content">
<form action="#" method="post" accept-charset="utf-8">

<?=$form->hidden(array('name' => XIVO_SESS_NAME,'value' => XIVO_SESS_ID));?>
<?=$form->hidden(array('name' => 'act','value' => 'add'));?>
<?=$form->hidden(array('name' => 'fm_send','value' => '1'));?>

<?=$form->text(array('desc' => $this->bbf('fm_meetmefeatures_name'),'name' => 'mfeatures[name]','labelid' => 'mfeatures-pin','size' => 25),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_meetme_number'),'name' => 'meetme[number]','labelid' => 'meetme-number','label' => 'lb-meetme-number','size' => 25),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_meetme_pin'),'name' => 'meetme[pin]','labelid' => 'meetme-pin','size' => 25),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->text(array('desc' => $this->bbf('fm_meetme_admin-pin'),'name' => 'meetme[admpin]','labelid' => 'meetme-admpin','size' => 25),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->slt(array('desc' => $this->bbf('fm_meetmefeatures_mode'),'name' => 'mfeatures[mode]','labelid' => 'mfeatures-mode','bbf' => 'fm_meetmefeatures_mode-','key' => false,'default' => $mfeatures_elt['mode']['default']),$mfeatures_elt['mode']['value'],'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_exit'),'name' => 'mfeatures[exit]','labelid' => 'mfeatures-exit','default' => $mfeatures_elt['exit']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_musiconhold'),'name' => 'mfeatures[musiconhold]','labelid' => 'mfeatures-musiconhold','default' => $mfeatures_elt['musiconhold']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_quiet'),'name' => 'mfeatures[quiet]','labelid' => 'mfeatures-quiet','default' => $mfeatures_elt['quiet']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_record'),'name' => 'mfeatures[record]','labelid' => 'mfeatures-record','default' => $mfeatures_elt['record']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->checkbox(array('desc' => $this->bbf('fm_meetmefeatures_video'),'name' => 'mfeatures[video]','labelid' => 'mfeatures-video','default' => $mfeatures_elt['video']['default']),'onfocus="this.className=\'it-mfocus\';" onblur="this.className=\'it-mblur\';"');?>

<?=$form->submit(array('name' => 'submit','id' => 'it-submit','value' => $this->bbf('fm_bt-save')));?>
</form>
	</div>
	<div class="sb-foot xspan"><span class="span-left">&nbsp;</span><span class="span-center">&nbsp;</span><span class="span-right">&nbsp;</span></div>
</div>
